#include "ipc_volume.hpp"
#include "ca_algorithms/basic.hpp"

#include <iostream>
#include <csignal>
#include <ctime>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#ifdef _FORK_IPVR
#include <unistd.h>
#endif

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/algorithm/string.hpp>

#include "voreen/core/datastructures/volume/volumeatomic.h"

using namespace std;
using namespace boost::interprocess;
using namespace tgt;
using namespace voreen;

typedef offset_ptr<uint16_t> offset_uint16_ptr;

//! Globals
//!
char* memblock = 0;

//! Clean up global stuff
//!
void cleanup()
{
    if(memblock) free(memblock);
}

//! Exit with a message
//!
void exit_message(const string msg)
{
    cerr << msg << endl;
    cleanup();
    exit(1);
}

//! Catch signals for exiting program
//!
void signal_exit_program(const int sig)
{
    printf("Catched signal: %d ... !!\n", sig);
    // redirect the SIGINT signal to default handling
    (void) signal(SIGINT, SIG_DFL);
    cleanup();
    exit(0);
}

//! Calculate next step of the 3d CA (double-buffered version)
//!
void ca_step_double_buffer( const uint size_x
                            , const uint size_y
                            , const uint size_z
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

    // Load a seed .dat volume if given an argument
    string raw_file;
    uint file_size_x;
    uint file_size_y;
    uint file_size_z;
    uint byte_depth;
    if(argc > 1)
    {
        ifstream datfile (argv[1]);

        string line;
        if(datfile.is_open())
        {
            while(datfile.good())
            {
                getline(datfile,line);

                string object_file_name_string = "ObjectFileName:";
                string resolution_string = "Resolution:";
                string format_string = "Format:";
                if( line.find(object_file_name_string) != string::npos )
                {
                    line.erase(0,object_file_name_string.size());
                    boost::trim(line);
                    raw_file = dirname(argv[1]) + string("/") + line;
                }
                else if( line.find(resolution_string) != string::npos )
                {
                    line.erase(0,resolution_string.size());
                    vector<string> split_vec;
                    boost::trim(line);
                    boost::split( split_vec, line, boost::is_any_of(" ") );
                    if(split_vec.size()!=3) exit_message("Error: dat file has wrong number of size parameters");

                    if( !(stringstream(split_vec[0]) >> file_size_x) ) exit_message("Error parsing x size parameters");
                    if( !(stringstream(split_vec[0]) >> file_size_y) ) exit_message("Error parsing y size parameters");
                    if( !(stringstream(split_vec[0]) >> file_size_z) ) exit_message("Error parsing z size parameters");
                }
                else if( line.find(format_string) != string::npos )
                {
                    line.erase(0,format_string.size());
                    if( line.find("UCHAR") != string::npos )
                    {
                        byte_depth = 1;
                    }
                    else if( line.find("USHORT") != string::npos )
                    {
                        byte_depth = 2;
                    }
                    else exit_message("Error: unknown bit depth specified in .dat file");
                }
            }
            datfile.close();

            if(!raw_file.size()) exit_message("Error: dat file missing some basic information");

            size_t file_size;
            ifstream file (raw_file.c_str(), ios::in|ios::binary|ios::ate);
            if (file.is_open())
            {
                file_size = file.tellg();
                memblock = new char [file_size];
                file.seekg (0, ios::beg);
                file.read (memblock, file_size);
                file.close();

                cout << "The file has " << file_size << " bytes" << endl;
            }
            else exit_message("Unable to open raw file");
        }
    }

    // Enter shared segment
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

        if( memblock
            && (size_x != file_size_x
                || size_y != file_size_y
                || size_z != file_size_z) )
        {
            cerr << "Host shared volume sizes doesn't match the loaded volume sizes" << endl;
            cerr << "Host offers: "
                 << size_x << " (x), " << size_y << " (y), " << size_z << " (z) " << endl;
            cerr << "Loaded file needs: "
                 << file_size_x << " (x), " << file_size_y << " (y), " << file_size_z << " (z) " << endl;
            exit(1);
        }

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

            // Copy data of opened volume
            if(memblock)
            {
                for(int k=0; k<size_z; k++)
                    for(int j=0; j<size_y; j++)
                        for(int i=0; i<size_x; i++)
                        {
                            vol1->voxel(i,j,k) = *memblock;
                            memblock += byte_depth;
                        }
            }

            while(true)
            {
                scoped_lock<interprocess_mutex> lock(volumeinfo->mutex);
                if(volumeinfo->fresh_data)
                {
                    volumeinfo->cond_processing_visuals.wait(lock);
                }

                ca_step_double_buffer(size_x, size_y, size_z, vol1, vol2, basic_algorithm);
                ca_step_swap_buffers(volumeinfo->offset_ptr, offset_pos1, offset_pos2);
                
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

            // Copy data of opened volume
            if(memblock)
            {
                for(int k=0; k<size_z; k++)
                    for(int j=0; j<size_y; j++)
                        for(int i=0; i<size_x; i++)
                        {
                            vol->voxel(i,j,k) = *memblock;
                            memblock += byte_depth;
                        }
            }

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
        cleanup();
        return 1;
    }

    cleanup();
    return 0;
}
