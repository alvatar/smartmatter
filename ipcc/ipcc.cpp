#include <iostream>
#include <csignal>
#include <ctime>
#ifdef _FORK_IPVR
#include <unistd.h>
#endif

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "voreen/core/datastructures/volume/volumeatomic.h"

#include "ipcvolume.hpp"

using namespace std;

bool loop = true;

void exit_program(int sig) {
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    loop=false;
}

int main(int argc, char** argv) {
    
    (void) signal(SIGINT, exit_program);
	srand ( time(NULL) );

#ifdef _FORK_IPVR
	pid_t pID = fork();
	if (pID == 0) { // child code
		execl("./ipvr", "./ipvr", (char *) 0);
    } else if (pID < 0) { // parent code
        cerr << "Failed to fork" << endl;
        exit(1);
    }
#endif

    using namespace boost::interprocess;
    using namespace tgt;
    using namespace voreen;
    try {
        shared_memory_object::remove("ipcc_shared_memory");
        shared_memory_object shm_obj(create_only , "ipcc_shared_memory", read_write);
        shm_obj.truncate(sizeof(IPCVolumeUInt8));
        mapped_region region(shm_obj, read_write);

        IPCVolumeUInt8 *mem = static_cast<IPCVolumeUInt8*>(region.get_address());
        IPCVolumeUInt8 *ipcvolume = new(mem) IPCVolumeUInt8();
		VolumeUInt8* target = new VolumeUInt8( ipcvolume->data
											   ,ivec3(IPCVolumeUInt8::size_x
													 ,IPCVolumeUInt8::size_y
													 ,IPCVolumeUInt8::size_z) );
        while(loop) {
			int counter = 0;
            scoped_lock<interprocess_mutex> lock(ipcvolume->header.mutex);
            for(int k=0; k<IPCVolumeUInt8::size_z; k++) {
                for(int j=0; j<IPCVolumeUInt8::size_y; j++) {
                    for(int i=0; i<IPCVolumeUInt8::size_x; i++) {
                        target->voxel(i,j,k) = rand()%255;
                    }
                }
            }
            // mutex is released here
        }
		shared_memory_object::remove("ipcc_shared_memory");
    } catch(interprocess_exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
