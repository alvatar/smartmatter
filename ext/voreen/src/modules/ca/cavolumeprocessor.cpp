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

const std::string CAVolumeProcessor::loggerCat_("voreen.CAVolumeProcessor");

CAVolumeProcessor::CAVolumeProcessor()
	: VolumeProcessor()
	  , outport_(Port::OUTPORT, "volumehandle.output", 0)
	  , dimension_("dimension", "Dimension", 64, 2, 1024, Processor::VALID) 
      , timer_(0)
      , eventHandler_()
	  , target_(0)
	  , ipcvolume_(0)
{
	addPort(outport_);

	addProperty(dimension_);

    eventHandler_.addListenerToBack(this);
    timer_ = VoreenApplication::app()->createTimer(&eventHandler_);
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

    if (timer_) {
		timer_->start(100);
    } else {
        LWARNING("No timer.");
        return;
	}
}

void CAVolumeProcessor::deinitialize() throw (VoreenException) {
	outport_.deleteVolume();

	VolumeProcessor::deinitialize();
}

void CAVolumeProcessor::timerEvent(tgt::TimeEvent* te) {
	using namespace boost::interprocess;
	try {
		shared_memory_object shm_obj(open_only, MEM_NAME, read_write);
		mapped_region region(shm_obj, read_write);
        std::cout << "Memory read correctly" << std::endl;

		ipcvolume_ = static_cast<IPCVolumeUInt8*>(region.get_address());
		scoped_lock<interprocess_mutex> lock(ipcvolume_->header.mutex);
        if(!ipcvolume_->header.fresh_data) {
            //ipcvolume_->header.cond_processing_ca.wait(lock);
            return;
        }

		target_ = new VolumeUInt8(ivec3(40,40,40));

		uint8_t *p = ipcvolume_->data;
		for(int k=0; k<IPCVolumeUInt8::size_z; k++) {
			for(int j=0; j<IPCVolumeUInt8::size_y; j++) {
				for(int i=0; i<IPCVolumeUInt8::size_x; i++) {
					target_->voxel(i,j,k) = *p++;
				}
			}
		}
        ipcvolume_->header.fresh_data = false;
        ipcvolume_->header.cond_processing_visuals.notify_one();

		if (target_){
			outport_.setData(new VolumeHandle(target_), true);
		} else  {
			outport_.setData(0, true);
		}
		invalidate();
	} catch(interprocess_exception &e) {
		std::cout << "Unexpected exception: " << e.what() << std::endl;
		outport_.setData(0, true);
	}
}

void CAVolumeProcessor::process() {
	/*
	Volume* outputVolume = 0;

	ivec3 dimensions;
	dimensions.x = dimension_.get();
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
		outport_.setData(new VolumeHandle(outputVolume), true);
	else
		outport_.setData(0, true);
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
