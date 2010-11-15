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

#include "voreen/core/ports/volumeport.h"

namespace voreen {

bool VolumePort::isReady() const {
    if (isOutport())
        return isConnected();
    else
        return (hasData() && getData()->getVolume());
}

void VolumePort::setData(VolumeHandle* handle) {
    tgtAssert(isOutport(), "called setData on inport!");

    if (portData_ != handle) {
        if (portData_)
            portData_->removeObserver(this);
        portData_ = handle;
        if (portData_)
            portData_->addObserver(this);
    }
    invalidate();
}

void VolumePort::setData(VolumeHandle* handle, bool deletePrevious) {
    tgtAssert(isOutport(), "called setData on inport!");
    if (deletePrevious && portData_ && (portData_ != handle)) {
        delete portData_;
    }
    setData(handle);
}

void VolumePort::volumeHandleDelete(const VolumeHandle* source) {

    if (getData() == source)
        setData(0);
}

void VolumePort::volumeChange(const VolumeHandle* source) {

    if (getData() == source) {
        hasChanged_ = true;
        invalidate();
    }
}

void VolumePort::deleteVolume() {
    tgtAssert(isOutport(), "deleteVolume called on inport!");
    VolumeHandle* tempVol = portData_;
    if (tempVol) {
        setData(0);
        delete tempVol;
    }
}

} // namespace
