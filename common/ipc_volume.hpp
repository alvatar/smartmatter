#ifndef LATTICE
#define LATTICE


#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include "tgt/types.h"
#include "tgt/vector.h"

#define SHARED_MEMORY_NAME "volume_shared_memory"

struct ipc_volume_header
{
    //! IPC Mutex
    boost::interprocess::interprocess_mutex mutex;

    //!Condition to wait when processing visualization
    boost::interprocess::interprocess_condition cond_processing_visuals;

    //!Is there any message
    bool fresh_data;
};

template <class T>
struct ipc_volume
{
	//! Access to the type that the IPCVolume is built upon
	typedef T voxel_type;

	enum { size_x = 40 };
	enum { size_y = 40 };
	enum { size_z = 40 };
	enum { buffer_size = size_x*size_y*size_z };

	//! Metadata and header of the shared memory
	ipc_volume_header header;

	//! The volume data
	T data[buffer_size];
};

typedef ipc_volume<uint8_t> ipc_volume_uint8;

#endif
