#ifndef VRN_IPCVOLUMESOURCE_H
#define VRN_IPCVOLUMESOURCE_H

#include <string>

#include "tgt/timer.h"
#include "tgt/event/eventhandler.h"

#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/node_allocator.hpp>

#include "ipc_volume.hpp"

namespace boost
{
    namespace interprocess
    {
        typedef node_allocator<int, managed_shared_memory::segment_manager> node_allocator_t;
    }
}

namespace voreen
{

class VolumeHandle;

class IPCVolumeSource : public VolumeProcessor
{
public:
	IPCVolumeSource();

	virtual ~IPCVolumeSource();

	virtual Processor* create() const;

	virtual std::string getClassName() const
    {
		return "IPCVolumeSource";
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

    void changeCheckTime();

    void adaptSharedSegment();

    void toggleIPCRead();

private:
    VolumePort _outport;

    static const std::string _loggerCat; // category used in logging

    //! Volume dimensions
    IntProperty _x_dimension;
    IntProperty _y_dimension;
    IntProperty _z_dimension;

    //! Enable/disable IPC reading
    BoolProperty _toggle_ipc;
    bool _enable_ipc;
    //! Timer
    IntProperty _timer_interval;
    static const uint _default_timer_interval;
	//! Timer object
    tgt::Timer* _timer;
	//! A local eventhanlde which is added to the timer
    tgt::EventHandler _eventHandler;

    //! Shared memory name
    StringProperty _shared_memory_name;
    std::string _current_shared_memory_name;
    //! Node allocator
    //typedef node_allocator<int, managed_shared_memory::segment_manager> node_allocator_t;
    boost::interprocess::node_allocator_t *_allocator;
    //! Managed shared memory object
    boost::interprocess::managed_shared_memory *_shared_segment;
    //! Volume information as a header
    ipc_volume_info *_volumeinfo;
    //! Place in shared memory where data is stored
    uint16_t *_volumedata;
	//! Structure for interpreting the shared data for visualization
    VolumeUInt16 *_target;
};

}   //namespace

#endif
