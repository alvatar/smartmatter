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

// include this before any windows headers
#include "voreen/modules/pvm/ddsbase.h"

#include "voreen/modules/pvm/pvmvolumereader.h"

#include <fstream>
#include <iostream>

#include "tgt/exception.h"
#include "tgt/texturemanager.h"

#include "voreen/core/io/rawvolumereader.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"

using std::string;
using tgt::Texture;

namespace voreen {

const std::string PVMVolumeReader::loggerCat_ = "voreen.pvm.PVMVolumeReader";

PVMVolumeReader::PVMVolumeReader(ProgressBar* progress)
  : VolumeReader(progress)
{
    extensions_.push_back("pvm");
}

namespace {

inline void endian_swap(uint16_t& x) {
    x = (x>>8) | (x<<8);
}

} // namespace

VolumeCollection* PVMVolumeReader::read(const std::string &url)
    throw (tgt::FileException, tgt::IOException, std::bad_alloc)
{
    VolumeOrigin origin(url);
    std::string fileName = origin.getPath();

    uint8_t* data;
    uint8_t* tmpData;

    unsigned int width, height, depth, components;
    float scalex, scaley, scalez;
    unsigned char *description;
    unsigned char *courtesy;
    unsigned char *parameter;
    unsigned char *comment;

    LINFO("Reading PVM volume " << fileName);

    /*
        TODO This subroutine returns an array created with malloc but it should
        be created with 'new[]' because this chunk of data will be deleted with 'delete[]'.
        This can cause hard to find errors.

        As a temporary workaround the data are copied over into a new array
        and the c-array is deleted with 'free'.
        Because of some c-pointer vodoo done in ddsbase.cpp free must be invoked
        after the use of all other returned pointers. (roland)
    */
    tmpData = readPVMvolume(const_cast<char*>(fileName.c_str()), getProgressBar(),
                            &width, &height, &depth, &components,
                            &scalex, &scaley, &scalez, &description, &courtesy,
                            &parameter, &comment);

    if (!tmpData) {
        LERROR("PVM Reading failed");
        return 0;
    }

    data = new uint8_t[width * height * depth * components];
    memcpy(data, tmpData, width * height * depth * components);

    Volume* dataset = 0;

    if (!data) {
        throw tgt::IOException();
    }
    else {
        LINFO("Size: " << width << " x " << height << " x " << depth);
        LINFO("Spacing: " << scalex << " x " << scaley << " x " << scalez);
        LINFO("Components: " << components);
        if (description)
            LINFO("Description: " << description);
        if (courtesy)
            LINFO("Courtesy: " << courtesy);
        if (parameter)
            LINFO("Parameter: " << parameter);
        if (comment)
            LINFO("Comment: " << comment);
        if (components == 1) {
            LINFO("Create 8 bit data set.");
            dataset = new VolumeUInt8(data, tgt::ivec3(width, height, depth),
                                      tgt::vec3(scalex, scaley, scalez));
        }
        else if (components == 2) {
            // the endianness conversion in ddsbase.cpp seem to be broken,
            // so we perform it here instead
            uint16_t* data16 = reinterpret_cast<uint16_t*>(data);
            int numElements = width * height * depth;
            uint16_t maxValue = 0;
            for (int i=0; i < numElements; i++) {
                endian_swap(data16[i]);
                if (data16[i] > maxValue)
                    maxValue = data16[i];
            }

            int bits;
            if (maxValue < 4096) {
                LINFO("Create 12 bit data set.");
                bits = 12;
            } else {
                LINFO("Create 16 bit data set.");
                bits = 16;
            }
            dataset = new VolumeUInt16((uint16_t*)data,
                                       tgt::ivec3(width, height, depth),
                                       tgt::vec3(scalex, scaley, scalez),
                                       tgt::mat4::identity,
                                       bits);

        }
        else LERROR("Bit depth not supported.");
    }

    // TODO now it is safe to free
    free(tmpData);

    VolumeCollection* volumeCollection = new VolumeCollection();
    if (dataset) {
        VolumeHandle* volumeHandle = new VolumeHandle(dataset, 0.0f);
        volumeHandle->setOrigin(VolumeOrigin(fileName));
        volumeCollection->add(volumeHandle);
    }

    return volumeCollection;
}

VolumeReader* PVMVolumeReader::create(ProgressBar* progress) const {
    return new PVMVolumeReader(progress);
}

} // namespace voreen
