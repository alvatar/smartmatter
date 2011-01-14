#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
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
	  , _dimension("dimension", "Dimension", 64, 2, 512, Processor::VALID) 
	  , _timer_interval("timer_interval", "Timer Interval", _default_timer_interval, 40, 5000, Processor::VALID) 
      , _current_shared_memory_name(SHARED_MEMORY_DEFAULT_NAME)
	  , _shared_memory_name("shared_memory_name", "Shared memory name", SHARED_MEMORY_DEFAULT_NAME, Processor::VALID)
      , _timer(0)
      , _eventHandler()
	  , _target(0)
      , _ipcvolume(0)
{
    _count_instances++;
	addPort(_outport);

	addProperty(&_dimension);
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

    shared_memory_object::remove(_current_shared_memory_name.c_str());
    if(_shm_obj) { delete _shm_obj; _shm_obj = 0; }
    if(_region) { delete _region; _region = 0; }

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
        shared_memory_object::remove(_current_shared_memory_name.c_str());
        _shm_obj = new shared_memory_object(create_only , _current_shared_memory_name.c_str(), read_write);
        _shm_obj->truncate(sizeof(ipc_volume_uint16));
        _region = new mapped_region(*_shm_obj, read_write);
        ipc_volume_uint16 *mem = static_cast<ipc_volume_uint16*>(_region->get_address());
        _ipcvolume = new(mem) ipc_volume_uint16();
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
    shared_memory_object::remove(_current_shared_memory_name.c_str());
    if(_shm_obj) { delete _shm_obj; _shm_obj = 0; }
    if(_region) { delete _region; _region = 0; }

    // Create new shared memory
    _current_shared_memory_name = _shared_memory_name.get();
    shared_memory_object::remove(_current_shared_memory_name.c_str());
    _shm_obj = new shared_memory_object(create_only , _current_shared_memory_name.c_str(), read_write);
    _shm_obj->truncate(sizeof(ipc_volume_uint16));
    _region = new mapped_region(*_shm_obj, read_write);
    ipc_volume_uint16 *mem = static_cast<ipc_volume_uint16*>(_region->get_address());
    _ipcvolume = new(mem) ipc_volume_uint16();
}

void IPCVolumeSource::timerEvent(tgt::TimeEvent* te)
{
	try
    {
        ipc_volume_uint16 *ipcvolume = static_cast<ipc_volume_uint16*>(_region->get_address());

		scoped_lock<interprocess_mutex> lock(ipcvolume->header.mutex);
        if(!ipcvolume->header.fresh_data)
        {
            return;
        }

		_target = new VolumeUInt16( ivec3(ipc_volume_uint16::size_x,
                                          ipc_volume_uint16::size_y,
                                          ipc_volume_uint16::size_z) );

		uint16_t *p = ipcvolume->data;
		for(int k=0; k<ipc_volume_uint16::size_z; k++)
			for(int j=0; j<ipc_volume_uint16::size_y; j++)
				for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
					_target->voxel(i,j,k) = *p++;
				}

        ipcvolume->header.fresh_data = false;
        ipcvolume->header.cond_processing_visuals.notify_one();

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
	/*
	Volume* outputVolume = 0;

	ivec3 dimensions;
	dimensions.x = _dimension.get();
	dimensions.y = dimensions.x;
	dimensions.z = dimensions.x;

	VolumeUInt16* target = new VolumeUInt16(dimensions);

	outputVolume = target;

	vec3 center;
	center = dimensions / 2;

	int s = dimensions.x;
	int thickness = s / 15;
	int border = 3;

	uint16_t white = 200;

	LINFO("Cellular Automata with dimensions: " << dimensions);

	fillBox(target, ivec3(0,0,0), dimensions, 0); // clear dataset
	fillBox(target, ivec3(0,0,0), dimensions/2, white);

	// assign computed volume to outport
	if (outputVolume)
		_outport.setData(new VolumeHandle(outputVolume), true);
	else
		_outport.setData(0, true);
		*/
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
