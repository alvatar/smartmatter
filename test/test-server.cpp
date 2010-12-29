#include <iostream>
#include <csignal>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "lattice.hpp"

using namespace std;

bool loop = true;
typedef Lattice<char> LatticeType;
typedef LatticeType::VoxelType VoxelType;

void exit_program(int sig) {
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    loop=false;
}

int main(int argc, char** argv) {
    
    (void) signal(SIGINT, exit_program);

    using namespace boost::interprocess;
    try {
        shared_memory_object::remove("ipcc_shared_memory");
        shared_memory_object shm_obj(create_only , "ipcc_shared_memory", read_write);
        shm_obj.truncate(sizeof(LatticeType));
        mapped_region region(shm_obj, read_write);

        LatticeType *mem = static_cast<LatticeType*>(region.get_address());
        LatticeType *lattice = new(mem) LatticeType;
        while(loop) {
            scoped_lock<interprocess_mutex> lock(lattice->header.mutex);
            for(int i=0; i<lattice->size_x; i++) {
                for(int j=0; j<lattice->size_y; j++) {
                    for(int k=0; k<lattice->size_z; k++) {
                        lattice->voxels[i][j][k] = VoxelType(i,j,k);
                    }
                }
            }
            cout << "Done writing from client process" << endl;
            // mutex is released here
        }
		shared_memory_object::remove("ipcc_shared_memory");
    } catch(interprocess_exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
