#include "ipc_volume.hpp"
#include "ca_algorithms/basic.hpp"

#include <iostream>
#include <csignal>
#include <ctime>
#ifdef _FORK_IPVR
#include <unistd.h>
#endif

//#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
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
void ca_step( uint size_x, uint size_y, uint size_z
        , void (*f)(uint, uint, uint, voreen::VolumeUInt16*)
        , voreen::VolumeUInt16* v)
{
    f(size_x, size_y, size_z, v);
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
        managed_shared_memory segment(open_only, SHARED_MEMORY_DEFAULT_NAME);
        pair<ipc_volume_info*,size_t> volumeinfo_pair = segment.find<ipc_volume_info>(unique_instance);
        assert(volumeinfo_pair.second == 1);
        ipc_volume_info* volumeinfo = volumeinfo_pair.first;

        uint size_x = volumeinfo->size_x;
        uint size_y = volumeinfo->size_y;
        uint size_z = volumeinfo->size_z;
        cout << "x size: " << size_x << endl;
        cout << "y size: " << size_y << endl;
        cout << "z size: " << size_z << endl;

        pair<uint16_t*,size_t> volumedata_pair = segment.find<uint16_t>(unique_instance);
        assert(volumedata_pair.second == (size_x * size_y * size_z));
        uint16_t* volumedata = volumedata_pair.first;
        VolumeUInt16* target = new VolumeUInt16( volumedata,
                                                 ivec3(size_x,
                                                       size_y,
                                                       size_z) );
        while(true)
        {
            scoped_lock<interprocess_mutex> lock(volumeinfo->mutex);
            if(volumeinfo->fresh_data)
            {
                volumeinfo->cond_processing_visuals.wait(lock);
            }

            ca_step(size_x, size_y, size_z, basic_algorithm, target);

            // notify the client that the CA processing is finished
            volumeinfo->fresh_data = true;
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
