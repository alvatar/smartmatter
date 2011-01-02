#include <iostream>
#include <csignal>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "ipcvolume.hpp"

using namespace std;

bool loop = true;
//typedef Lattice<char> LatticeType;
//typedef LatticeType::VoxelType VoxelType;

void exit_program(int sig) {
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    loop = false;
}

int main() {

    (void) signal(SIGINT, exit_program);

    using namespace boost::interprocess;
    try {
        shared_memory_object shm_obj(open_only, "ipcc_shared_memory", read_write);
        mapped_region region(shm_obj, read_write);
        IPCVolumeUInt8 *lattice = static_cast<IPCVolumeUInt8*>(region.get_address());

        while(loop) {
			unsigned int counter = 0;
            scoped_lock<interprocess_mutex> lock(lattice->header.mutex);
            for(int i=0; i<lattice->size_x; i++) {
                for(int j=0; j<lattice->size_y; j++) {
                    for(int k=0; k<lattice->size_z; k++) {
						IPCVolumeUInt8::VoxelType v = lattice->data[counter++];
                        //VoxelType v = lattice->voxels[i][j][k];
                        //printf("r: %i, g: %i, b: %i\n", v.r, v.g, v.b);
                    }
                }
            }
            cout << "Done reading from client process" << endl;
            // mutex is released here
        }
    } catch(interprocess_exception &e) {
        std::cout << "Unexpected exception: " << e.what() << std::endl;
        shared_memory_object::remove("ipcc_shared_memory");
        return 1;
    }

    return 0;
}
