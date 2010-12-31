#ifndef LATTICE
#define LATTICE


#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include "tgt/types.h"
#include "tgt/vector.h"

struct IPCVolumeHeader {
    boost::interprocess::interprocess_mutex mutex;
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
