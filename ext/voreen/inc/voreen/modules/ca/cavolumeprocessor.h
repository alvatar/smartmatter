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

#include "ipcvolume.hpp"

namespace voreen {

class VolumeHandle;
template <class T> class RawVolumeAtomic;
typedef RawVolumeAtomic<uint8_t> RawVolumeUInt8;


class CAVolumeProcessor : public VolumeProcessor {
	public:
		CAVolumeProcessor();

		virtual ~CAVolumeProcessor();

		virtual Processor* create() const;

		virtual std::string getClassName() const {
			return "CAVolumeProcessor";
		}

		virtual std::string getCategory() const {
			return "Volume Processing";
		}

		virtual CodeState getCodeState() const {
			return CODE_STATE_STABLE;
		}

		virtual std::string getProcessorInfo() const;

		virtual void timerEvent(tgt::TimeEvent* te);

	protected:
		virtual void process();

		virtual void deinitialize() throw (VoreenException);

		void fillBox(VolumeUInt8* vds, tgt::ivec3 start, tgt::ivec3 end, uint8_t value);

		virtual void initialize() throw (VoreenException);

	private:
		VolumePort outport_;

		static const std::string loggerCat_; // category used in logging

		IntProperty dimension_;

		//! Structure used for sharing data via IPC
        IPCVolumeUInt8 *ipcvolume_;

		//! Structure for interpreting the shared data for visualization
		VolumeUInt8 *target_;

		//! Timer object
		tgt::Timer* timer_;

		//! A local eventhanlde which is added to the timer
		tgt::EventHandler eventHandler_;
};

}   //namespace

#endif
