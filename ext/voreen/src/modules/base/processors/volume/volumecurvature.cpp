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

#include "voreen/modules/base/processors/volume/volumecurvature.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumehandle.h"
#include "voreen/core/datastructures/volume/gradient.h"

namespace voreen {

VolumeCurvature::VolumeCurvature()
    : VolumeProcessor()
    , curvatureType_("curvatureType", "Curvature Type")
    , processedVolumeHandle_(0)
    , inport_(Port::INPORT, "volumehandle.input")
    , outport_(Port::OUTPORT, "volumehandle.output", 0)
{
    curvatureType_.addOption("first", "First Principle");
    curvatureType_.addOption("second", "Second Principle");
    curvatureType_.addOption("mean", "Mean");
    curvatureType_.addOption("gauss", "Gaussian");
    addProperty(curvatureType_);

    addPort(inport_);
    addPort(outport_);
}

Processor* VolumeCurvature::create() const {
    return new VolumeCurvature();
}

std::string VolumeCurvature::getProcessorInfo() const {
    return "Computes surface curvatures for each voxel from its Hessian matrix.";
}

void VolumeCurvature::deinitialize() throw (VoreenException) {
    outport_.setData(0);
    delete processedVolumeHandle_;
    processedVolumeHandle_ = 0;
    VolumeProcessor::deinitialize();
}

void VolumeCurvature::process() {
    unsigned int curvatureType = -1;
    if (curvatureType_.get() == "first") curvatureType = 0;
    else if (curvatureType_.get() == "second") curvatureType = 1;
    else if (curvatureType_.get() == "mean") curvatureType = 2;
    else if (curvatureType_.get() == "gauss") curvatureType = 3;

    Volume* v = calcCurvature<float>(inport_.getData()->getVolume(), curvatureType);

    delete processedVolumeHandle_;
    processedVolumeHandle_ = 0;

    if (v)
        processedVolumeHandle_ = new VolumeHandle(v, 0.0f);

    outport_.setData(processedVolumeHandle_);
}

}   // namespace
