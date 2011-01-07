#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>


using namespace std;

ifstream::pos_type file_size;
char * memblock;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long qword;



int restore_rle2(byte* src, word* dst,int size)
{
    int pos = 1;
    int src_advance = 0;

    //word v0 = src->read_word();
    word v0 = *((word*)&src); src += sizeof(word);
    dst[0] = v0;
    if(size)
    {
        do
        {
            //byte sz = Src->ReadBYTE();
            byte sz = *src;
            src++; src_advance++;
            if(sz >= 220)
            {
                sz = sz - 220;
                for(int i = 0; i < sz; i++)
                {
                    //dst[pos]=dst[pos-1]+src->readword();
                    dst[pos] = dst[pos-1] + *((word*)&src);
                    src += sizeof(word); src_advance += sizeof(word);
                    pos++;
                }
            }
            else
            {
                for(int i=0;i<sz;i++)
                {
                    //Dst[pos]=Dst[pos-1];
                    dst[pos] = dst[pos-1];
                    pos++;
                }
            }
        }while(pos < size);
    }		
    return src_advance;
}

int main()
{
    ifstream file ("../data/volumes/reverse-engineering.3b", ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        file_size = file.tellg();
        memblock = new char [file_size];
        file.seekg (0, ios::beg);
        file.read (memblock, file_size);
        file.close();

        cout << "The file has " << file_size << " units" << endl;
    }
    else
    {
        cout << "Unable to open file";
        exit(1);
    }

    if(memblock)
    {
        size_t p = 0;
        // dword: Magic signature 'MESH'
        const byte mesh_dword[] = { 'H', 'S', 'E', 'M' };
        //byte* mesh_dword = (byte*)malloc(sizeof(dword));
        //*mesh_dword++ = 'M'; *mesh_dword++ = 'E'; *mesh_dword++ = 'S'; *mesh_dword++ = 'H';
        if( memcmp(&memblock[p], mesh_dword, 4) == 0 )
        {
            cout << "File seems to be of the 3b format" << endl;
        }
        p += sizeof(dword);

        // dword: test integer = 1
        const dword test_integer = 1;
        if( memcmp(&memblock[p], &test_integer, 4) == 0 )
        {
            cout << "Test byte passed" << endl;
        }
        p += sizeof(dword);

        bool voxel_chunk_found = false;
        const byte vol3_dword[] = { '3', 'L', 'O', 'V' };
        while(p < file_size)
        {
            // each chunk:
            //   dword : type
            //   dword : chunksize

            // read the voxel chunk
            if( memcmp(&memblock[p], vol3_dword, 4) == 0 )
            {
                if(voxel_chunk_found)
                {
                    cout << "Error: more than one voxel chunk found" << endl;
                    break;
                }
                voxel_chunk_found = true;

                cout << endl << "--> VOXEL CHUNK FOUND" << endl;
                p += sizeof(dword);

                // dword: chunk size
                dword voxel_chunk_size;
                voxel_chunk_size = *((dword*)&memblock[p]);
                p += sizeof(dword);

                // dword: save version
                dword version = *((dword*)&memblock[p]);
                cout << "Version: " << version << endl;
                p += sizeof(dword);

                // dword: number of volumes
                dword volume_number = *((dword*)&memblock[p]);
                cout << "Number of volumes: " << volume_number << endl;
                p += sizeof(dword);

                for(int i=0; i<volume_number; i++)
                {
                    // dword: uid
                    dword uid = *((dword*)&memblock[p]);
                    cout << "UID: " << uid << endl;
                    p += sizeof(dword);

                    // float 4x4: transformation matrix (unused)
                    p += 4*4*sizeof(float);

                    // dword: volume name length
                    dword volume_name_length = *((dword*)&memblock[p]);
                    cout << "Volume name length: " << volume_name_length << endl;
                    p += sizeof(dword);

                    // char*: volume name
                    char* volume_name = &memblock[p];
                    cout << volume_name << endl;
                    p += volume_name_length*sizeof(byte);

                    // dword: default color
                    p += sizeof(dword);

                    // dword: representation (-1: surface, 0: voxel)
                    dword representation = *((dword*)&memblock[p]);
                    if(representation != 0)
                    {
                        cout << "Error: representation is not in Voxel format" << endl;
                        break;
                    }
                    p += sizeof(dword);
                    
                    // dword: index of hidden volume (0 if no hidden volume)
                    dword hidden_volume = *((dword*)&memblock[p]);
                    if(hidden_volume != 0)
                    {
                        cout << "Warning: this volume has hidden parts" << endl;
                    }
                    p += sizeof(dword);

                    // dword: shader name length
                    dword shader_name_length = *((dword*)&memblock[p]);
                    cout << "Shader name length: " << shader_name_length << endl;
                    p += sizeof(dword);

                    // char*: shader name
                    char* shader_name = &memblock[p];
                    cout << shader_name << endl;
                    p += shader_name_length*sizeof(char);

                    // char*: optional shader xml (ignored)
                    //while(memblock[p] != 0) { p++; } p++;

                    // dword: number of voxel cells
                    dword number_voxel_cells = *((dword*)&memblock[p]);
                    cout << "Number of voxel cells: " << number_voxel_cells << endl;
                    p += sizeof(dword);

                    //for(int i=0; i<number_voxel_cells; i++)
                    for(int i=0; i<10; i++)
                    {
                        // int: x position
                        int x_position = *((int*)&memblock[p]);
                        cout << "x position: " << x_position << endl;
                        p += sizeof(int);

                        // int: y position
                        int y_position = *((int*)&memblock[p]);
                        cout << "y position: " << y_position << endl;
                        p += sizeof(int);
                        
                        // int: z position
                        int z_position = *((int*)&memblock[p]);
                        cout << "z position: " << z_position << endl;
                        p += sizeof(int);
                        
                        // word: cell side (should be 9)
                        int cell_side = *((int*)&memblock[p]);
                        cout << "cell side: " << cell_side << endl;
                        p += sizeof(int);
                        
                        // byte:
                        //   bit 1 - cell has at least one zero value
                        //   bit 2 - cell has at least one non-zero value
                        //   bit 3 - cell is invisible (unused)
                        //   bit 4 - cell is in surface representation and has at least one triangle
                        byte flags = memblock[p];
                        printf("flags: %i\n", flags);
                        p++;

                        // byte: data flag, 0 if array consists of same values, 1 otherwise
                        byte same_values = memblock[p];
                        printf("same value? %i\n", same_values);
                        //cout << "same values?: " << same_values << endl;
                        p++;

                        if(same_values)
                        {
                            p += sizeof(word);
                        }
                        else
                        {
                            const int vbuff_size = 9*9*9;
                            word voxels[vbuff_size];
                            int pene = restore_rle2((byte*)&memblock[p], voxels, vbuff_size);
                            printf("IJAAAAAAAAAAAAA: %i\n", pene);
                            p += pene;
                        }
                    }
                }

                // TODO: temp
                break;
            }
            // find next chunk
            else
            {
                printf("%c%c%c%c: ", memblock[p], memblock[p+1], memblock[p+2], memblock[p+3]);
                // the chunks size is saved in the next dword
                p += sizeof(dword);
                dword chunk_size;
                chunk_size = *((size_t*)&memblock[p]);
                printf("%i bytes\n", chunk_size);
                p += chunk_size + sizeof(dword);
            }
        }
        
        delete[] memblock; memblock = 0;
        //cout << "Block removed" << endl;
    }
    return 0;
}
