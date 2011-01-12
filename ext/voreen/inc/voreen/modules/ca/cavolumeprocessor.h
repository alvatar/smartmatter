#ifndef VRN_VOLUMECREATEPROCESSOR_H
#define VRN_VOLUMECREATEPROCESSOR_H

#include <string>

#include "tgt/timer.h"
#include "tgt/event/eventhandler.h"

#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"

#include "ipc_volume.hpp"

namespace boost
{
    namespace interprocess
    {
        class shared_memory_object;
        class mapped_region;
    }
}

namespace voreen
{

class VolumeHandle;
template <class T> class RawVolumeAtomic;
typedef RawVolumeAtomic<uint16_t> RawVolumeUInt16;

class CAVolumeProcessor : public VolumeProcessor
{
public:
	CAVolumeProcessor();

	virtual ~CAVolumeProcessor();

	virtual Processor* create() const;

	virtual std::string getClassName() const
    {
		return "CAVolumeProcessor";
	}

	virtual std::string getCategory() const
    {
        return "Volume Processing";
    }

    virtual CodeState getCodeState() const
    {
        return CODE_STATE_STABLE;
    }

    virtual std::string getProcessorInfo() const;

    virtual void timerEvent(tgt::TimeEvent* te);

protected:
    virtual void process();

    virtual void deinitialize() throw (VoreenException);

    void fillBox(VolumeUInt16* vds, tgt::ivec3 start, tgt::ivec3 end, uint16_t value);

    virtual void initialize() throw (VoreenException);

private:
    VolumePort _outport;

    static const std::string _loggerCat; // category used in logging

    IntProperty _dimension;

    ipc_volume_uint16 *_ipcvolume;

    boost::interprocess::shared_memory_object* _shm_obj;

    boost::interprocess::mapped_region* _region;

	//! Structure for interpreting the shared data for visualization
    VolumeUInt16 *_target;

	//! Timer object
    tgt::Timer* _timer;

	//! A local eventhanlde which is added to the timer
    tgt::EventHandler _eventHandler;
};

}   //namespace

#endif
