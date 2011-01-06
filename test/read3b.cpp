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

int main()
{
    ifstream file ("../data/volumes/piedra-alta-res.3b", ios::in|ios::binary|ios::ate);
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
        //byte* p = (byte*)memblock;
        size_t p = 0;
        // Magic signature 'MESH'
        const byte mesh_dword[] = { 'H', 'S', 'E', 'M' };
        //byte* mesh_dword = (byte*)malloc(sizeof(dword));
        //*mesh_dword++ = 'M'; *mesh_dword++ = 'E'; *mesh_dword++ = 'S'; *mesh_dword++ = 'H';
        if( memcmp(&memblock[p], mesh_dword, 4) == 0 )
        {
            cout << "File seems to be of the 3b format" << endl;
        }
        p += sizeof(dword);

        // Test integer = 1
        const int test_integer = 1;
        if( memcmp(&memblock[p], &test_integer, 4) == 0 )
        {
            cout << "Test byte passed" << endl;
        }
        p += sizeof(dword);

        const byte vol3_dword[] = { '3', 'L', 'O', 'V' };
        while(p < file_size)
        {
            // read the voxel chunk
            if( memcmp(&memblock[p], vol3_dword, 4) == 0 )
            {
                cout << endl << "--> VOLUME FOUND" << endl;
                p += sizeof(dword);

                size_t voxel_chunk_size;
                voxel_chunk_size = *((size_t*)&memblock[p]);
                p += sizeof(dword);
                printf("Volume version: %i\n", *((int*)&memblock[p]));

                // we only read one chunk, so break from while
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
