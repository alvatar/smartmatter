#include "basic.hpp"

#include "ipc_volume.hpp"

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

void basic_algorithm(VolumeUInt8* w)
{
    for(int k=0; k<ipc_volume_uint8::size_z; k++)
        for(int j=0; j<ipc_volume_uint8::size_y; j++)
            for(int i=0; i<ipc_volume_uint8::size_x; i++)
            {
                w->voxel(i,j,k) = rand()%255;
            }
}
