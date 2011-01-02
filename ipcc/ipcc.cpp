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

void exit_program(int sig) {
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    exit(0);
}

int main(int argc, char** argv) {

    using namespace std;
    using namespace boost::interprocess;
    using namespace tgt;
    using namespace voreen;

    //Erase previous shared memory and schedule erasure on exit
    struct shm_remove
    {
        shm_remove() { shared_memory_object::remove(MEM_NAME); }
        ~shm_remove() { shared_memory_object::remove(MEM_NAME); }
    } remover;

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

    try {
        shared_memory_object shm_obj(create_only , MEM_NAME, read_write);
        shm_obj.truncate(sizeof(IPCVolumeUInt8));
        mapped_region region(shm_obj, read_write);

        IPCVolumeUInt8 *mem = static_cast<IPCVolumeUInt8*>(region.get_address());
        IPCVolumeUInt8 *ipcvolume = new(mem) IPCVolumeUInt8();
        VolumeUInt8* target = new VolumeUInt8( ipcvolume->data
                ,ivec3(IPCVolumeUInt8::size_x
                    ,IPCVolumeUInt8::size_y
                    ,IPCVolumeUInt8::size_z) );
        while(true) {
            scoped_lock<interprocess_mutex> lock(ipcvolume->header.mutex);
            if(ipcvolume->header.fresh_data) {
                ipcvolume->header.cond_processing_visuals.wait(lock);
            }
            for(int k=0; k<IPCVolumeUInt8::size_z; k++) {
                for(int j=0; j<IPCVolumeUInt8::size_y; j++) {
                    for(int i=0; i<IPCVolumeUInt8::size_x; i++) {
                        target->voxel(i,j,k) = rand()%255;
                    }
                }
            }
            // notify the client that the CA processing is finished
            //ipcvolume->header.cond_processing_ca.notify_one();
            ipcvolume->header.fresh_data = true;
            std::cout << "Step finished!" << std::endl;
            // mutex is released here
        }
    } catch(interprocess_exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
