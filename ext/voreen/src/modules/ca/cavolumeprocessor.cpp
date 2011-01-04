#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"
#include "voreen/core/datastructures/volume/volumehandle.h"
#include "voreen/core/datastructures/volume/gradient.h"
#include "voreen/core/voreenapplication.h"

#include "voreen/modules/ca/cavolumeprocessor.h"

namespace voreen {

//! Extends VolumeAtomic to allow redefinition of the internal data, avoiding re-creation
//! of VolumeAtomics for every updateIPC call
template <class T>
class RawVolumeAtomic : public VolumeAtomic<T> {
public:
    RawVolumeAtomic(T* data
                    ,const tgt::ivec3& dimensions
                    ,const tgt::vec3& spacing = tgt::vec3(1.f)
                    ,const tgt::mat4& transformation = tgt::mat4::identity
                    ,int bitsStored = VolumeAtomic<T>::BITS_PER_VOXEL)
		: VolumeAtomic<T>(data, dimensions, spacing, transformation, bitsStored) {}

	virtual void setData(T* const data) {
		this->data_ = data;
	}


};

const std::string CAVolumeProcessor::_loggerCat("voreen.CAVolumeProcessor");

CAVolumeProcessor::CAVolumeProcessor()
	: VolumeProcessor()
	  , _outport(Port::OUTPORT, "volumehandle.output", 0)
	  , _dimension("dimension", "Dimension", 64, 2, 1024, Processor::VALID) 
      , _timer(0)
      , _eventHandler()
	  , _target(0)
	  , _ipcvolume(0)
{
	addPort(_outport);

	addProperty(_dimension);

    _eventHandler.addListenerToBack(this);
    _timer = VoreenApplication::app()->createTimer(&_eventHandler);
}

CAVolumeProcessor::~CAVolumeProcessor() {
}

Processor* CAVolumeProcessor::create() const {
	return new CAVolumeProcessor();
}

std::string CAVolumeProcessor::getProcessorInfo() const {
	return std::string("Generates an 8-bit dataset from a Cellular Automata");
}

void CAVolumeProcessor::initialize() throw (VoreenException) {
	Processor::initialize();

    if (_timer) {
		_timer->start(1000);
    } else {
        LWARNING("No timer.");
        return;
	}
}

void CAVolumeProcessor::deinitialize() throw (VoreenException) {
	_outport.deleteVolume();

	VolumeProcessor::deinitialize();
}

void CAVolumeProcessor::timerEvent(tgt::TimeEvent* te) {
	using namespace boost::interprocess;
	try {
		shared_memory_object shm_obj(open_only, SHARED_MEMORY_NAME, read_write);
		mapped_region region(shm_obj, read_write);
        std::cout << "Memory read correctly" << std::endl;

		_ipcvolume = static_cast<ipc_volume_uint8*>(region.get_address());
		scoped_lock<interprocess_mutex> lock(_ipcvolume->header.mutex);
        if(!_ipcvolume->header.fresh_data) {
            //_ipcvolume->header.cond_processing_ca.wait(lock);
            return;
        }

		_target = new VolumeUInt8(ivec3(ipc_volume_uint8::size_x
                                        ,ipc_volume_uint8::size_y
                                        ,ipc_volume_uint8::size_z));

		uint8_t *p = _ipcvolume->data;
		for(int k=0; k<ipc_volume_uint8::size_z; k++) {
			for(int j=0; j<ipc_volume_uint8::size_y; j++) {
				for(int i=0; i<ipc_volume_uint8::size_x; i++) {
					_target->voxel(i,j,k) = *p++;
				}
			}
		}
        _ipcvolume->header.fresh_data = false;
        _ipcvolume->header.cond_processing_visuals.notify_one();

		if (_target){
			_outport.setData(new VolumeHandle(_target), true);
		} else  {
			_outport.setData(0, true);
		}
		invalidate();
	} catch(interprocess_exception &e) {
		std::cout << "Unexpected exception: " << e.what() << std::endl;
		_outport.setData(0, true);
	}
}

void CAVolumeProcessor::process() {
	/*
	Volume* outputVolume = 0;

	ivec3 dimensions;
	dimensions.x = _dimension.get();
	dimensions.y = dimensions.x;
	dimensions.z = dimensions.x;

	VolumeUInt8* target = new VolumeUInt8(dimensions);

	outputVolume = target;

	vec3 center;
	center = dimensions / 2;

	int s = dimensions.x;
	int thickness = s / 15;
	int border = 3;

	uint8_t white = 200;

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

void CAVolumeProcessor::fillBox(VolumeUInt8* vds, ivec3 start, ivec3 end, uint8_t value) {
	ivec3 i;
	for (i.x = start.x; i.x < end.x; i.x++) {
		for (i.y = start.y; i.y < end.y; i.y++) {
			for (i.z = start.z; i.z < end.z; i.z++) {
				vds->voxel(i.x, i.y, i.z) = value;
			}
		}
	}
}

}   // namespace
