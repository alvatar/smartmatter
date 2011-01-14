#include "basic.hpp"

#include "ipc_volume.hpp"

#include <iostream>

///// ALGORITMOS DE COMPORTAMIENTO
// Especialización de células madre

namespace voxel_state
{
    enum t_voxel_state
    {
        // continuous values from 0 to 0x0f
        // empty cell
        empty = 0x00,

        // soil types
        soil = 0x01,

        // architectural types
        external_surfaces = 0x02,
        main_access_space = 0x03,
        horizontal_distribution = 0x04,
        vertical_distribution = 0x05,
        fire_exit = 0x06,

        energy_generation = 0x07,
        energy_backup = 0x08,
        climate_control = 0x09,
        cooling_equipment = 0x0a,
        servers = 0x0b,
        server_irrigation = 0x0c,
        server_control = 0x0d,
        office = 0x0e,
        conference = 0x0f,
        restroom = 0x10,
        kitchen = 0x11,
        playground = 0x12,
        storage = 0x13,
        maintenance = 0x14,

        // flags (higher bits)
        is_interior = 0x80
    };
}

using namespace voreen;
using namespace std;
using namespace tgt;

inline void move_voxel(VolumeUInt16* w, ivec3 origin, ivec3 destination)
{
    w->voxel(destination) = w->voxel(origin);
    w->voxel(origin) = voxel_state::empty;
}

//!
//! Keep all the persistent data of the algorithm here
struct basic_algorithm_data
{
    unsigned int current_main_phase;
    bool entry_found;
    ivec3 conquistador;

    basic_algorithm_data() :
        current_main_phase(0),
        entry_found(false),
        conquistador(ivec3(0,0,0))
    {}
};


void basic_algorithm(VolumeUInt16* w)
{
    enum main_phases
    {
        find_entry,
        create_external_infrastructure,
        penetrate_soil,
        irrigate_soil,
        grow_internal,
        grow_external,
    };

    static basic_algorithm_data o;

    switch(o.current_main_phase)
    {
        ////////////////////////////////////////////////////////////////////////
        // Phase 1: Find entry
        case find_entry:
        cout << "Step: find entry" << endl;
        move_voxel(w, o.conquistador, ivec3(rand()%2, rand()%2, rand()%2));
        for(int k=0; k<ipc_volume_uint16::size_z; k++)
            for(int j=0; j<ipc_volume_uint16::size_y; j++)
                for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
                    w->voxel(i,j,k) = rand()%0xffff;
                }
        if(o.entry_found) o.current_main_phase++;
        break;

        ////////////////////////////////////////////////////////////////////////
        // Phase 2: Create external infrastructure
        case create_external_infrastructure:
        cout << "Step: create external infrastructure" << endl;
        for(int k=0; k<ipc_volume_uint16::size_z; k++)
            for(int j=0; j<ipc_volume_uint16::size_y; j++)
                for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
                    w->voxel(i,j,k) = rand()%0xffff;
                }
        break;

        ////////////////////////////////////////////////////////////////////////
        // Phase 3: Penetrate soil
        case penetrate_soil:
        cout << "Step: penetrate soil" << endl;
        for(int k=0; k<ipc_volume_uint16::size_z; k++)
            for(int j=0; j<ipc_volume_uint16::size_y; j++)
                for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
                    w->voxel(i,j,k) = rand()%0xffff;
                }
        break;

        ////////////////////////////////////////////////////////////////////////
        // Phase 4: Irrigate soil
        case irrigate_soil:
        cout << "Step: irrigate soil" << endl;
        for(int k=0; k<ipc_volume_uint16::size_z; k++)
            for(int j=0; j<ipc_volume_uint16::size_y; j++)
                for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
                    w->voxel(i,j,k) = rand()%0xffff;
                }
        break;

        ////////////////////////////////////////////////////////////////////////
        // Phase 5: Grow internal
        case grow_internal:
        cout << "Step: grow internal" << endl;
        for(int k=0; k<ipc_volume_uint16::size_z; k++)
            for(int j=0; j<ipc_volume_uint16::size_y; j++)
                for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
                    w->voxel(i,j,k) = rand()%0xffff;
                }
        break;

        ////////////////////////////////////////////////////////////////////////
        // Phase 6: Grow external
        case grow_external:
        cout << "Step: grow external" << endl;
        for(int k=0; k<ipc_volume_uint16::size_z; k++)
            for(int j=0; j<ipc_volume_uint16::size_y; j++)
                for(int i=0; i<ipc_volume_uint16::size_x; i++)
                {
                    w->voxel(i,j,k) = rand()%0xffff;
                }
        break;
    }
}
