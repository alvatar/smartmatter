/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "voreen/modules/base/io/philipsusvolumereader.h"

#include <fstream>
#include <iostream>

#include "tgt/exception.h"
#include "tgt/vector.h"

#include "voreen/core/io/textfilereader.h"
#include "voreen/core/io/rawvolumereader.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"

using tgt::vec3;
using tgt::ivec3;
using tgt::hor;
using tgt::lessThanEqual;

namespace voreen {

const std::string PhilipsUSVolumeReader::loggerCat_ = "voreen.io.VolumeReader.philipsus";

PhilipsUSVolumeReader::PhilipsUSVolumeReader(ProgressBar* progress)
    : VolumeReader(progress)
{
    //extensions_.push_back("dcm");  //< clash with dicom file ending
    protocols_.push_back("phus");    //< use this protocol specifier for loading philips us data
}

VolumeCollection* PhilipsUSVolumeReader::read(const std::string &url)
    throw (tgt::FileException, std::bad_alloc)
{
    return readSlices(url,0,0);
}

VolumeCollection* PhilipsUSVolumeReader::readSlices(const std::string &url, size_t firstSlice, size_t lastSlice)
    throw (tgt::FileException, std::bad_alloc)
{
    VolumeOrigin origin(url);
    std::string fileName = origin.getPath();

    std::fstream fin(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!fin.good() || fin.eof() || !fin.is_open())
        throw tgt::FileNotFoundException("Unable to open Philips US file for reading", fileName);

    fin.seekg(128); // waste first 128 bytes

    // read out 4 chars containing DCIM
    char* curChar = new char[4];
    fin.read(curChar, 4);
    for (int i=0; i<4; ++i)
        std::cout << curChar[i];
    std::cout << std::endl;

    // allocate variables to be read
    unsigned short rows = 0;
    unsigned short columns = 0;
    unsigned int depth = 0;
    unsigned short numFrames = 0;
    tgt::dvec3 spacing;

    unsigned short tagA = 0;
    unsigned short tagB = 0;
    char* code = new char[2];
    unsigned short numBytes = 0;
    unsigned loopCounter = 0;
    do {
        loopCounter++;
        fin.read(reinterpret_cast<char*>(&tagA), 2);
        fin.read(reinterpret_cast<char*>(&tagB), 2);
        fin.read(reinterpret_cast<char*>(code), 2);
        fin.read(reinterpret_cast<char*>(&numBytes), 2);

        int curPos = fin.tellg();
        if (code[0] == 'O' && code[1] == 'B') {
            // waste 6 bytes when reading code OB
            fin.seekg(curPos+6);
        }
        else {
            if (tagA == 0x0018) {
                if (tagB == 0x602C) {
                    // read PhysicalDeltaX
                    fin.read(reinterpret_cast<char*>(&spacing[0]), 8);
                }
                else if (tagB == 0x602E) {
                    // read PhysicalDeltaY
                    fin.read(reinterpret_cast<char*>(&spacing[1]), 8);
                }
                else
                    fin.seekg(curPos+numBytes);
            }
            else if (tagA == 0x0028) {
                if (tagB == 0x0008) {
                    // read NumberOfFrames
                    char* tmp = new char[3];
                    fin.read(reinterpret_cast<char*>(tmp), 2);
                    tmp[2] = '\n';
                    sscanf(tmp, "%hu", &numFrames);
                }
                else if (tagB == 0x0010) {
                    // read Rows (X)
                    fin.read(reinterpret_cast<char*>(&rows), 2);
                }
                else if (tagB == 0x0011) {
                    // read Columns (Y)
                    fin.read(reinterpret_cast<char*>(&columns), 2);
                }
                else
                    fin.seekg(curPos+numBytes);
            }
            else if (tagA == 0x3001) {
                if (tagB == 0x1001) {
                    // read Depths (Z)
                    fin.read(reinterpret_cast<char*>(&depth), 4);
                }
                else if (tagB == 0x1003) {
                    // read PhysicalDeltaZ
                    fin.read(reinterpret_cast<char*>(&spacing[2]), 8);
                }
                else
                    fin.seekg(curPos+numBytes);
            }
            else
                fin.seekg(curPos+numBytes);
        }
    } while ((loopCounter < 200) && (code[0] != 'O' || code[1] != 'W'));

    int loadingOffset = fin.tellg();
    fin.close();

    if (numFrames == 0) {
        throw tgt::CorruptedFileException("Unable to extract frames from Philips US file", fileName);
    }

    RawVolumeReader rawReader(getProgressBar());
    VolumeCollection* volumeCollection = 0;
    for (unsigned short curFrame=0; curFrame<numFrames; curFrame++) {
        RawVolumeReader::ReadHints hints(ivec3(columns, rows, depth), // dimensions of the volume
                                         spacing,                     // thickness of one slice
                                         8,                           // bits per voxel
                                         "I",                         // intensity image
                                         "UCHAR",                     // one unsigned char per voxel
                                         loadingOffset+curFrame*(columns*rows*depth)); // header skip
        hints.modality_ = Modality::MODALITY_US;
        hints.timeStep_ = static_cast<float>(curFrame);
        rawReader.setReadHints(hints);

        if (curFrame==0) {
            volumeCollection = rawReader.readSlices(fileName, firstSlice, lastSlice);
        }
        else {
            VolumeCollection* curVolumeCollection = rawReader.readSlices(fileName, firstSlice, lastSlice);
            // we know that readSlices has returned only one VolumeHandle
            tgtAssert(curVolumeCollection->size() == 1, "Collection with one volume expected");
            VolumeHandle* curVolumeHandle = curVolumeCollection->first();
            curVolumeHandle->setOrigin(VolumeOrigin("phus", fileName));
            curVolumeHandle->setTimestep(static_cast<float>(curFrame));
            volumeCollection->add(curVolumeHandle);

            delete curVolumeCollection;
        }
    }
    return volumeCollection;
}

VolumeReader* PhilipsUSVolumeReader::create(ProgressBar* progress) const {
    return new PhilipsUSVolumeReader(progress);
}


} // namespace voreen
