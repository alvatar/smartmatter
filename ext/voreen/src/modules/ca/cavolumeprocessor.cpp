#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumehandle.h"
#include "voreen/core/datastructures/volume/gradient.h"
#include "voreen/core/voreenapplication.h"

#include "lattice.hpp"

#include "voreen/modules/ca/cavolumeprocessor.h"

using tgt::vec2;

namespace voreen {

const std::string CAVolumeProcessor::loggerCat_("voreen.CAVolumeProcessor");

CAVolumeProcessor::CAVolumeProcessor()
	: VolumeProcessor()
	  , outport_(Port::OUTPORT, "volumehandle.output", 0)
	  , dimension_("dimension", "Dimension", 64, 2, 1024, Processor::VALID) 
      , timer_(0)
      , eventHandler_()
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
		timer_->start(1000);
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
        shared_memory_object shm_obj(open_only, "ipcc_shared_memory", read_write);
        mapped_region region(shm_obj, read_write);

        LatticeType *lattice = static_cast<LatticeType*>(region.get_address());
		scoped_lock<interprocess_mutex> lock(lattice->header.mutex);
		std::cout << "LOCKED INTERPROCESS MUTEX" << std::endl;

		for(int i=0; i<lattice->size_x; i++) {
			for(int j=0; j<lattice->size_y; j++) {
				for(int k=0; k<lattice->size_z; k++) {
					VoxelType v = lattice->voxels[i][j][k];
					printf("r: %i, g: %i, b: %i\n", v.r, v.g, v.b);
				}
			}
		}
		std::cout << "UNLOCKED INTERPROCESS MUTEX" << std::endl;
		// mutex is released here
    } catch(interprocess_exception &e) {
        std::cout << "Unexpected exception: " << e.what() << std::endl;
        shared_memory_object::remove("ipcc_shared_memory");
    }
}

void CAVolumeProcessor::process() {
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
