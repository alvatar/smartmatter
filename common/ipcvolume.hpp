#ifndef LATTICE
#define LATTICE


#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include "tgt/types.h"
#include "tgt/vector.h"

#define MEM_NAME "volume_shared_memory"

struct IPCVolumeHeader {
    //! IPC Mutex
    boost::interprocess::interprocess_mutex mutex;
    
    //!Condition to wait when processing CA
    boost::interprocess::interprocess_condition cond_processing_ca;

    //!Condition to wait when processing visualization
    boost::interprocess::interprocess_condition cond_processing_visuals;

    //!Is there any message
    bool fresh_data;
};

template <class T>
struct IPCVolume {
	//! Access to the type that the IPCVolume is built upon
	typedef T VoxelType;

	enum { size_x = 40 };
	enum { size_y = 40 };
	enum { size_z = 40 };
	enum { buffer_size = size_x*size_y*size_z };

	//! Metadata and header of the shared memory
	IPCVolumeHeader header;

	//! The volume data
	T data[buffer_size];
};

typedef IPCVolume<uint8_t> IPCVolumeUInt8;

#endif
