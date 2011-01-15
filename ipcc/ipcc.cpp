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

using namespace std;
using namespace boost::interprocess;
using namespace tgt;
using namespace voreen;

typedef offset_ptr<uint16_t> offset_uint16_ptr;

//! Catch signals for exiting program
//!
void signal_exit_program(int sig)
{
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    exit(0);
}

//! Calculate next step of the 3d CA (double-buffered version)
//!
void ca_step_double_buffer( uint size_x
                            , uint size_y
                            , uint size_z
                            , VolumeUInt16* v1
                            , VolumeUInt16* v2
                            , void (*f)(uint, uint, uint, VolumeUInt16*, VolumeUInt16*) )
{
    f(size_x, size_y, size_z, v1, v2);
}

//! Swap buffers in IPC shared memory
//
void ca_step_swap_buffers(offset_uint16_ptr &p_current, offset_uint16_ptr p1, offset_uint16_ptr p2)
{
    if(p_current == p1)
    {
        p_current = p2;
        return;
    }
    else if (p_current == p2)
    {
        p_current = p1;
        return;
    }
    else cout << "Warning: problem swapping buffers!" << endl;
}


//! Calculate next step of the 3d CA (single-buffered version)
//!
void ca_step_single_buffer( uint size_x
                            , uint size_y
                            , uint size_z
                            , VolumeUInt16* v
                            , void (*f)(uint, uint, uint, VolumeUInt16*, VolumeUInt16*) )
{
    f(size_x, size_y, size_z, v, NULL);
}

int main(int argc, char** argv)
{
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
#ifdef _DEBUG
        assert(volumeinfo_pair.second == 1);
#endif
        ipc_volume_info* volumeinfo = volumeinfo_pair.first;

        uint size_x = volumeinfo->size_x;
        uint size_y = volumeinfo->size_y;
        uint size_z = volumeinfo->size_z;

        pair<uint16_t*,size_t> full_stream_shared = segment.find<uint16_t>(unique_instance);

        if(volumeinfo->double_buffer)
        {
            cout << "Executing Double buffer version" << endl;
#ifdef _DEBUG
            assert(full_stream_shared.second == size_x*size_y*size_z*2);
#endif

            uint16_t* stream_pos1 = full_stream_shared.first;
            uint16_t* stream_pos2 = stream_pos1 + (size_x*size_y*size_z);

            VolumeUInt16* vol1 = new VolumeUInt16( stream_pos1
                                                   ,ivec3(size_x
                                                   ,size_y
                                                   ,size_z) );
            VolumeUInt16* vol2 = new VolumeUInt16( stream_pos2
                                                   ,ivec3(size_x
                                                   ,size_y
                                                   ,size_z) );
            volumeinfo->offset_ptr = stream_pos1;
            offset_uint16_ptr offset_pos1 = stream_pos1;
            offset_uint16_ptr offset_pos2 = stream_pos2;
            while(true)
            {
                scoped_lock<interprocess_mutex> lock(volumeinfo->mutex);
                if(volumeinfo->fresh_data)
                {
                    volumeinfo->cond_processing_visuals.wait(lock);
                }

                ca_step_double_buffer(size_x, size_y, size_z, vol1, vol2, basic_algorithm);
                ca_step_swap_buffers(volumeinfo->offset_ptr, offset_pos1, offset_pos2);
            //volumeinfo->offset_ptr = stream_pos2;
                
                // notify the client that the CA processing is finished
                volumeinfo->fresh_data = true;
                // mutex is released here
            }
        }
        else
        {
            cout << "Executing Single buffer version" << endl;
#ifdef _DEBUG
            assert(full_stream_shared.second == size_x*size_y*size_z);
#endif

            uint16_t* stream_pos = full_stream_shared.first;

            VolumeUInt16* vol = new VolumeUInt16( stream_pos
                                                  ,ivec3(size_x
                                                         ,size_y
                                                         ,size_z) );
            volumeinfo->offset_ptr = stream_pos;
            while(true)
            {
                scoped_lock<interprocess_mutex> lock(volumeinfo->mutex);
                if(volumeinfo->fresh_data)
                {
                    volumeinfo->cond_processing_visuals.wait(lock);
                }

                ca_step_single_buffer(size_x, size_y, size_z, vol, basic_algorithm);

                // notify the client that the CA processing is finished
                volumeinfo->fresh_data = true;
                // mutex is released here
            }
        }
    }
    catch(interprocess_exception &e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
