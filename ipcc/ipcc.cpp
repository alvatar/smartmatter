#include "ipc_volume.hpp"
#include "ca_algorithms/basic.hpp"

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

//! Catch signals for exiting program
//!
void signal_exit_program(int sig)
{
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    exit(0);
}

//! Calculate next step of the 3d CA
//!
void ca_step( void (*f)(voreen::VolumeUInt16*), voreen::VolumeUInt16* v)
{
    f(v);
}

int main(int argc, char** argv)
{
    using namespace std;
    using namespace boost::interprocess;
    using namespace tgt;
    using namespace voreen;

    // SIGINT callback
    (void) signal(SIGINT, signal_exit_program);
    // Random seed
    srand ( time(NULL) );

#ifdef _FORK_IPVR
    pid_t pID = fork();
    if (pID == 0)
    { // child code
        execl("./ipvr", "./ipvr", (char *) 0);
    }
    else if (pID < 0)
    { // parent code
        cerr << "Failed to fork" << endl;
        exit(1);
    }
#endif

    try
    {
		shared_memory_object shm_obj(open_only, SHARED_MEMORY_NAME, read_write);
		mapped_region region(shm_obj, read_write);

        ipc_volume_uint16 *ipcvolume = static_cast<ipc_volume_uint16*>(region.get_address());
        VolumeUInt16* target = new VolumeUInt16( ipcvolume->data,
                                                 ivec3(ipc_volume_uint16::size_x,
                                                 ipc_volume_uint16::size_y,
                                                 ipc_volume_uint16::size_z) );
        while(true)
        {
            scoped_lock<interprocess_mutex> lock(ipcvolume->header.mutex);
            if(ipcvolume->header.fresh_data)
            {
                ipcvolume->header.cond_processing_visuals.wait(lock);
            }

            ca_step(basic_algorithm,target);

            // notify the client that the CA processing is finished
            ipcvolume->header.fresh_data = true;
            // mutex is released here
        }
    }
    catch(interprocess_exception &e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
