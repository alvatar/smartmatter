#ifndef LATTICE
#define LATTICE


#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include<boost/interprocess/offset_ptr.hpp>

#include "tgt/types.h"
#include "tgt/vector.h"

#define SHARED_MEMORY_DEFAULT_NAME "volume_shared_memory"

struct ipc_volume_info
{
    //! IPC Mutex
    boost::interprocess::interprocess_mutex mutex;

    //!Condition to wait when processing visualization
    boost::interprocess::interprocess_condition cond_processing_visuals;

    //! A pointer to the data buffer to be read (used for double-buffering)
    boost::interprocess::offset_ptr<uint16_t> offset_ptr;

    //! Double-buffered?
    bool double_buffer;

    //!Is there any message
    bool fresh_data;

    //! Dimensions
    size_t size_x;
    size_t size_y;
    size_t size_z;
};

#endif
