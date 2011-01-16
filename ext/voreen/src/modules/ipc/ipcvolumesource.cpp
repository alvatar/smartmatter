#include <boost/interprocess/sync/scoped_lock.hpp>

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"
#include "voreen/core/datastructures/volume/volumehandle.h"
#include "voreen/core/datastructures/volume/gradient.h"
#include "voreen/core/voreenapplication.h"

#include "voreen/modules/ipc/ipcvolumesource.h"

namespace voreen
{

using namespace boost::interprocess;
using namespace std;

//! Static data
//
const string IPCVolumeSource::_loggerCat("voreen.IPCVolumeSource");

const uint IPCVolumeSource::_default_timer_interval(1000);

//! IPCVolumeSource
//
IPCVolumeSource::IPCVolumeSource()
	: VolumeProcessor()
	  , _outport(Port::OUTPORT, "volumehandle.output", 0)
	  , _x_dimension("x_dimension", "x dimension", 64, 2, 512, Processor::VALID) 
	  , _y_dimension("y_dimension", "y dimension", 64, 2, 512, Processor::VALID) 
	  , _z_dimension("z_dimension", "z dimension", 64, 2, 512, Processor::VALID) 
	  , _timer_interval("timer_interval", "New data check interval", _default_timer_interval, 40, 5000, Processor::VALID) 
      , _toggle_ipc("toggle_ipc", "Enable IPC reading", true)
      , _enable_ipc(true) // same as above
      , _create_double_buffer("create_double_buffer", "Double-buffered volume", true)
      , _double_buffer(true) // same as above
      , _current_shared_memory_name(SHARED_MEMORY_DEFAULT_NAME)
	  , _shared_memory_name("shared_memory_name", "Shared memory name", SHARED_MEMORY_DEFAULT_NAME, Processor::VALID)
      , _timer(0)
      , _eventHandler()
	  , _target(0)
{
	addPort(_outport);

	addProperty(&_toggle_ipc);
	addProperty(&_create_double_buffer);
	addProperty(&_x_dimension);
	addProperty(&_y_dimension);
	addProperty(&_z_dimension);
	addProperty(&_timer_interval);
	addProperty(&_shared_memory_name);

    _timer_interval.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::changeCheckTime));

    _toggle_ipc.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::toggleIPCRead));

    _create_double_buffer.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::toggleDoubleBuffer));
    _x_dimension.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::adaptSharedSegment));
    _y_dimension.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::adaptSharedSegment));
    _z_dimension.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::adaptSharedSegment));
    _shared_memory_name.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::adaptSharedSegment));

    _eventHandler.addListenerToBack(this);
    _timer = VoreenApplication::app()->createTimer(&_eventHandler);
}

IPCVolumeSource::~IPCVolumeSource()
{
}

Processor* IPCVolumeSource::create() const
{
	return new IPCVolumeSource();
}

std::string IPCVolumeSource::getProcessorInfo() const
{
	return string("Generates an 16-bit dataset updated periodically via IPC");
}

void IPCVolumeSource::initialize() throw (VoreenException)
{
    Processor::initialize();

    // If there is more than one instance, change wait to generate until a change in properties is made
    // TODO: also a check for same name should be performed,
    // otherwise it can break with multiple instances of this processor
    uint size_x = _x_dimension.get();
    uint size_y = _y_dimension.get();
    uint size_z = _z_dimension.get();
    shared_memory_object::remove(_current_shared_memory_name.c_str());

    size_t buffer_size = size_x * size_y * size_z * sizeof(uint16_t);
    // TODO: Why do I need to add extra allocation? [2 places]
    _double_buffer ? 
        _shared_segment = new managed_shared_memory(create_only
                                                    ,_current_shared_memory_name.c_str()
                                                    ,(buffer_size * 2) + sizeof(ipc_volume_info) + 65536):
        _shared_segment = new managed_shared_memory(create_only
                                                    ,_current_shared_memory_name.c_str()
                                                    ,buffer_size + sizeof(ipc_volume_info) + 65536);


    _allocator = new node_allocator_t(_shared_segment->get_segment_manager());

    _volumeinfo = _shared_segment->construct<ipc_volume_info>(unique_instance)();
    _volumeinfo->size_x = size_x;
    _volumeinfo->size_y = size_y;
    _volumeinfo->size_z = size_z;
    _volumeinfo->double_buffer = _double_buffer;

    _double_buffer ? 
        _volumedata = _shared_segment->construct<uint16_t>(unique_instance)[size_x*size_y*size_z * 2]():
        _volumedata = _shared_segment->construct<uint16_t>(unique_instance)[size_x*size_y*size_z]();

    if (_timer)
    {
        _timer->start(_default_timer_interval);
    }
    else
    {
        LWARNING("No timer.");
        return;
    }
}

void IPCVolumeSource::deinitialize() throw (VoreenException)
{
    VolumeProcessor::deinitialize();
    _timer->stop();
    _outport.deleteVolume();

    if(_timer) { delete _timer; _timer = 0; }

    if(_allocator) { delete _allocator; _allocator = 0; }
    if(_shared_segment) { delete _shared_segment; _shared_segment = 0; }
    shared_memory_object::remove(_current_shared_memory_name.c_str());
}

void IPCVolumeSource::changeCheckTime()
{
    _timer->setTickTime(_timer_interval.get());
}

void IPCVolumeSource::toggleIPCRead()
{
    _enable_ipc = _toggle_ipc.get();
}

void IPCVolumeSource::toggleDoubleBuffer()
{
    _double_buffer = _create_double_buffer.get();
    adaptSharedSegment();
}

void IPCVolumeSource::adaptSharedSegment()
{
    if(isInitialized())
    {
        // Remove previous shared memory
        if(_allocator) { delete _allocator; _allocator = 0; }
        if(_shared_segment) { delete _shared_segment; _shared_segment = 0; }
        shared_memory_object::remove(_current_shared_memory_name.c_str());

        // Create new shared memory
        uint size_x = _x_dimension.get();
        uint size_y = _y_dimension.get();
        uint size_z = _z_dimension.get();

        _current_shared_memory_name = _shared_memory_name.get();
        shared_memory_object::remove(_current_shared_memory_name.c_str());
        // TODO: Why do I need to add extra allocation? [2 places]
        size_t buffer_size = size_x * size_y * size_z * sizeof(uint16_t);

        _double_buffer ? 
            _shared_segment = new managed_shared_memory(create_only
                    ,_current_shared_memory_name.c_str()
                    ,(buffer_size * 2) + sizeof(ipc_volume_info) + 65536):
            _shared_segment = new managed_shared_memory(create_only
                    ,_current_shared_memory_name.c_str()
                    ,buffer_size + sizeof(ipc_volume_info) + 65536);

        _allocator = new node_allocator_t(_shared_segment->get_segment_manager());

        _volumeinfo = _shared_segment->construct<ipc_volume_info>(unique_instance)();
        _volumeinfo->size_x = size_x;
        _volumeinfo->size_y = size_y;
        _volumeinfo->size_z = size_z;
        _volumeinfo->double_buffer = _double_buffer;

        _double_buffer ? 
            _volumedata = _shared_segment->construct<uint16_t>(unique_instance)[size_x*size_y*size_z * 2]():
            _volumedata = _shared_segment->construct<uint16_t>(unique_instance)[size_x*size_y*size_z]();

        LINFO("Shared memory for IPC reallocated");
    }
}

void IPCVolumeSource::timerEvent(tgt::TimeEvent* te)
{
	try
    {
		scoped_lock<interprocess_mutex> lock(_volumeinfo->mutex);
        // Test for new data from server process
        if(!_enable_ipc || !_volumeinfo->fresh_data) return;

        uint size_x = _x_dimension.get();
        uint size_y = _y_dimension.get();
        uint size_z = _z_dimension.get();
		_target = new VolumeUInt16(ivec3(size_x,size_y,size_z));

        if(_double_buffer)
        {
            uint16_t *p = _volumeinfo->offset_ptr.get();
            for(int k=0; k<size_z; k++)
                for(int j=0; j<size_y; j++)
                    for(int i=0; i<size_x; i++)
                    {
                        _target->voxel(i,j,k) = *p++;
                    }
        }
        else
        {
            uint16_t *p = _volumedata;
            for(int k=0; k<size_z; k++)
                for(int j=0; j<size_y; j++)
                    for(int i=0; i<size_x; i++)
                    {
                        _target->voxel(i,j,k) = *p++;
                    }
        }

        _volumeinfo->fresh_data = false;
        _volumeinfo->cond_processing_visuals.notify_one();

		if (_target)
        {
			_outport.setData(new VolumeHandle(_target), true);
		}
        else
        {
			_outport.setData(0, true);
		}
		invalidate();
	}
    catch(interprocess_exception &e)
    {
		std::cout << "Unexpected exception: " << e.what() << std::endl;
        shared_memory_object::remove(_current_shared_memory_name.c_str());
		_outport.setData(0, true);
	}
}

void IPCVolumeSource::process()
{
}

void IPCVolumeSource::fillBox(VolumeUInt16* vds, ivec3 start, ivec3 end, uint16_t value) {
	ivec3 i;
	for (i.x = start.x; i.x < end.x; i.x++)
		for (i.y = start.y; i.y < end.y; i.y++)
			for (i.z = start.z; i.z < end.z; i.z++)
            {
				vds->voxel(i.x, i.y, i.z) = value;
			}
}

}   // namespace
