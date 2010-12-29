#ifndef LATTICE
#define LATTICE


#include <boost/interprocess/sync/interprocess_mutex.hpp>

struct LatticeHeader {
    boost::interprocess::interprocess_mutex mutex;
};

template <class T>
struct Lattice {
    enum { size_x = 2 };
    enum { size_y = 2 };
    enum { size_z = 2 };

	struct Voxel {
		Voxel() {}
		Voxel(T r, T g, T b) : r(r)
							, g(g)
							, b(b) {}
		T r, g, b;
	};

	LatticeHeader header;
	Voxel voxels[size_x][size_y][size_z];
};

typedef Lattice<char> LatticeType;
typedef LatticeType::Voxel VoxelType;

#endif

