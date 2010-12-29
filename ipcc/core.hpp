#ifndef CORE
#define CORE

#include <iostream>
#include <csignal>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "../common/lattice.hpp"

template <class T>
class Core {
    public:

	typedef Lattice<T> LatticeType;
	typedef typename LatticeType::Voxel VoxelType;

	private:

	//! Name of the shared memory space
	const char *_shmName;
	//! Shared memory to allocate
	boost::interprocess::shared_memory_object *_shm_obj;
	//! Process mapping of the memory
	boost::interprocess::mapped_region *_region;
	//! Voxel lattice
	LatticeType *_lattice;
    //! A counter for the total of steps done
    uint _step_counter;

	public:

	//! Constructor
	Core() : _shmName("ipcc_shared_memory")
            , _step_counter(0) {}

	//! Destructor
	virtual ~Core() {
        shutdown();
    }

	//! Initialize the core
	int init() {
		using namespace std;
		using namespace boost::interprocess;
		try {
			shared_memory_object::remove(_shmName);

			_shm_obj = new shared_memory_object(create_only , _shmName, read_write);
			_shm_obj->truncate(sizeof(LatticeType));

			_region = new mapped_region(*_shm_obj, read_write);

			LatticeType *mem = static_cast<LatticeType*>(_region->get_address());
			_lattice = new(mem) LatticeType;
		} catch(interprocess_exception &e) {
			shared_memory_object::remove(_shmName);
			cout << e.what() << endl;
			return 1;
		}
		return 0;
	}

	//! Shutdown the core
	int shutdown() {
		using namespace std;
		using namespace boost::interprocess;
		try {
			shared_memory_object::remove(_shmName);
		} catch(interprocess_exception &e) {
			cout << e.what() << endl;
			return 1;
		}
		return 0;
	}

	//! Set a value of the lattice
	void set(int i, int j, int k, VoxelType val) {
        _lattice->voxels[i][j][k] = val;
    }

	//! Get a value out of the lattice
	VoxelType get(int i, int j, int k) {
        return _lattice->voxels[i][j][k];
    }

    //! Get step counter
    uint getCounter() {
        return _step_counter;
    }

    //! Calculate the next iteration of the core CA
    //! Returns 0 if the computation is not over (more steps remaining)
    //! Returns an exit or error code otherwise
    int step() {
		using namespace std;
		using namespace boost::interprocess;
        cout << "--> begin step!" << endl;

        {
            scoped_lock<interprocess_mutex> lock(_lattice->header.mutex);
            for(int i=0; i<_lattice->size_x; i++) {
                for(int j=0; j<_lattice->size_y; j++) {
                    for(int k=0; k<_lattice->size_z; k++) {
                        cout << i << ", " << j << ", " << k << endl;
                    }
                }
            }
            // lock released here
        }

        cout << "--> end step!" << endl;

        _step_counter++;
        if (_step_counter >= 40000000) {
            return 99;
        } else {
            return 0;
        }
    }
};


typedef Core<char> CharCore;
typedef CharCore::LatticeType CharLattice;
typedef CharCore::VoxelType CharVoxel;

#endif
