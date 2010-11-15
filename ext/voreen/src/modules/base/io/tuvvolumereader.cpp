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

#include "voreen/modules/base/io/tuvvolumereader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <limits>

#include "tgt/exception.h"
#include "tgt/logmanager.h"
#include "tgt/vector.h"

#include "voreen/core/datastructures/volume/volumeatomic.h"

using tgt::ivec3;
using tgt::vec3;

namespace voreen {

const std::string TUVVolumeReader::loggerCat_ = "voreen.io.VolumeReader.tuv";

VolumeCollection* TUVVolumeReader::read(const std::string &url)
    throw (tgt::CorruptedFileException, tgt::IOException, std::bad_alloc)
{
    VolumeOrigin origin(url);
    std::string fileName = origin.getPath();

    LINFO("Reading file " << fileName);

    std::fstream fin(fileName.c_str(), std::ios::in | std::ios::binary);
    if (!fin.good())
        throw tgt::IOException();

    unsigned short dim[3];
    fin.read(reinterpret_cast<char*>(dim),6);
    ivec3 dimensions = ivec3(dim[0], dim[1], dim[2]);

    LINFO("Read 16 bit dataset");
    VolumeUInt16* dataset;
    try {
        dataset = new VolumeUInt16(dimensions, ivec3(1));
    } catch (std::bad_alloc&) {
        throw; // throw it to the caller
    }

    fin.read(reinterpret_cast<char*>(dataset->voxel()), dataset->getNumBytes());

    if ( fin.eof() )
        throw tgt::CorruptedFileException();

    fin.close();

    VolumeCollection* volumeCollection = new VolumeCollection();
    VolumeHandle* volumeHandle = new VolumeHandle(dataset, 0.0f);
    volumeHandle->setOrigin(fileName);
    volumeCollection->add(volumeHandle);

    return volumeCollection;
}

VolumeReader* TUVVolumeReader::create(ProgressBar* /*progress*/) const {
    return new TUVVolumeReader(/*progress*/);
}

} // namespace voreen
