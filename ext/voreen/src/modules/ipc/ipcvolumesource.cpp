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

//! Static data
//
const std::string IPCVolumeSource::_loggerCat("voreen.IPCVolumeSource");

const uint IPCVolumeSource::_default_timer_interval(1000);

uint IPCVolumeSource::_count_instances = 0;

//! IPCVolumeSource
//
IPCVolumeSource::IPCVolumeSource()
	: VolumeProcessor()
	  , _outport(Port::OUTPORT, "volumehandle.output", 0)
	  , _x_dimension("x_dimension", "x Dimension", 64, 2, 512, Processor::VALID) 
	  , _y_dimension("y_dimension", "y Dimension", 64, 2, 512, Processor::VALID) 
	  , _z_dimension("z_dimension", "z Dimension", 64, 2, 512, Processor::VALID) 
	  , _timer_interval("timer_interval", "Timer Interval", _default_timer_interval, 40, 5000, Processor::VALID) 
      , _current_shared_memory_name(SHARED_MEMORY_DEFAULT_NAME)
	  , _shared_memory_name("shared_memory_name", "Shared memory name", SHARED_MEMORY_DEFAULT_NAME, Processor::VALID)
      , _timer(0)
      , _eventHandler()
	  , _target(0)
{
    _count_instances++;
	addPort(_outport);

	addProperty(&_x_dimension);
	addProperty(&_y_dimension);
	addProperty(&_z_dimension);
	addProperty(&_timer_interval);
	addProperty(&_shared_memory_name);

    _timer_interval.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::changeCheckTime));
    _shared_memory_name.onChange(CallMemberAction<IPCVolumeSource>(this, &IPCVolumeSource::createNewSharedMemory));

    _eventHandler.addListenerToBack(this);
    _timer = VoreenApplication::app()->createTimer(&_eventHandler);
}

IPCVolumeSource::~IPCVolumeSource()
{
    if(_timer) { delete _timer; _timer = 0; }

    _shared_segment->deallocate(_volumedata);
    shared_memory_object::remove(_current_shared_memory_name.c_str());
    if(_allocator) { delete _allocator; _allocator = 0; }
    if(_shared_segment) { delete _shared_segment; _shared_segment = 0; }

    _count_instances--;
}

Processor* IPCVolumeSource::create() const
{
	return new IPCVolumeSource();
}

std::string IPCVolumeSource::getProcessorInfo() const
{
	return std::string("Generates an 16-bit dataset updated periodically via IPC");
}

void IPCVolumeSource::initialize() throw (VoreenException)
{
	Processor::initialize();

    // If there is more than one instance, change wait to generate until a change in properties is made
    // TODO: also a check for same name should be performed
    if(_count_instances)
    {
        uint size_x = _x_dimension.get();
        uint size_y = _y_dimension.get();
        uint size_z = _z_dimension.get();
        shared_memory_object::remove(_current_shared_memory_name.c_str());

        // TODO: Why do I need to add extra allocation? [2 places]
        size_t total_shared_memory_size = size_x
                                          * size_y
                                          * size_z
                                          * sizeof(uint16_t)
                                          + sizeof(ipc_volume_info)
                                          + 65536;
        _shared_segment = new managed_shared_memory(create_only, _current_shared_memory_name.c_str(), total_shared_memory_size);

        _allocator = new node_allocator_t(_shared_segment->get_segment_manager());

        _volumeinfo = _shared_segment->construct<ipc_volume_info>(unique_instance)();
        _volumeinfo->size_x = size_x;
        _volumeinfo->size_y = size_y;
        _volumeinfo->size_z = size_z;
        _volumedata = _shared_segment->construct<uint16_t>(unique_instance)[size_x*size_y*size_z]();
    }

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
    _timer->stop();
	_outport.deleteVolume();

	VolumeProcessor::deinitialize();
}

void IPCVolumeSource::changeCheckTime()
{
    _timer->setTickTime(_timer_interval.get());
}

void IPCVolumeSource::createNewSharedMemory()
{
    // Remove previous shared memory
    _shared_segment->deallocate(_volumedata);
    shared_memory_object::remove(_current_shared_memory_name.c_str());
    if(_allocator) { delete _allocator; _allocator = 0; }
    if(_shared_segment) { delete _shared_segment; _shared_segment = 0; }

    // Create new shared memory
    uint size_x = _x_dimension.get();
    uint size_y = _y_dimension.get();
    uint size_z = _z_dimension.get();

    _current_shared_memory_name = _shared_memory_name.get();
    shared_memory_object::remove(_current_shared_memory_name.c_str());
    // TODO: Why do I need to add extra allocation? [2 places]
    size_t total_shared_memory_size = size_x
                                      * size_y
                                      * size_z
                                      * sizeof(uint16_t)
                                      + sizeof(ipc_volume_info)
                                      + 65536;
    _shared_segment = new managed_shared_memory(create_only
                                                ,_current_shared_memory_name.c_str()
                                                ,total_shared_memory_size);
    _allocator = new node_allocator_t(_shared_segment->get_segment_manager());

    _volumeinfo = _shared_segment->find_or_construct<ipc_volume_info>(unique_instance)();
    _volumeinfo->size_x = size_x;
    _volumeinfo->size_y = size_y;
    _volumeinfo->size_z = size_z;
    _volumedata = _shared_segment->find_or_construct<uint16_t>(unique_instance)[size_x*size_y*size_z]();
}

void IPCVolumeSource::timerEvent(tgt::TimeEvent* te)
{
	try
    {
		scoped_lock<interprocess_mutex> lock(_volumeinfo->mutex);
        if(!_volumeinfo->fresh_data)
        {
            return;
        }

        uint size_x = _x_dimension.get();
        uint size_y = _y_dimension.get();
        uint size_z = _z_dimension.get();
		_target = new VolumeUInt16(ivec3(size_x,size_y,size_z));

		uint16_t *p = _volumedata;
		for(int k=0; k<size_z; k++)
			for(int j=0; j<size_y; j++)
				for(int i=0; i<size_x; i++)
                {
					_target->voxel(i,j,k) = *p++;
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
