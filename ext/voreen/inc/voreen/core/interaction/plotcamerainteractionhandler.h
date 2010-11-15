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

#ifndef VRN_PLOTCAMERAINTERACTIONHANDLER_H
#define VRN_PLOTCAMERAINTERACTIONHANDLER_H

#include "voreen/core/interaction/interactionhandler.h"

namespace voreen {

class CameraProperty;

/**
 * Interaction handler for camera interactions of 3d plots.
 * In opposite to the TrackballNavigation,
 * this class provides rather rigid rotations appropriate for plots.
 */
class PlotCameraInteractionHandler : public InteractionHandler {

public:

    /// Default constructor needed for serialization. Do not call it directly.
    PlotCameraInteractionHandler();

    /**
     * Constructor.
     *
     * @param id Identifier that must be unique across all interaction handlers
     *  of a processor. Must not be empty.
     * @param guiText the string that is to be displayed in the GUI
     * @param cameraProp camera property the handler will operate on. Must not be null.
     */
    PlotCameraInteractionHandler(const std::string& id, const std::string& guiText, CameraProperty* cameraProp,
        bool sharing = false, bool enabled = true);

private:

    /// @see InteractionHandler::onEvent
    virtual void onEvent(tgt::Event* e);

    // functions called by the event properties
    void rotateEvent(tgt::MouseEvent* e);
    void rotate(float horAngle, float vertAngle);

    // camera property the handler operates on
    CameraProperty* cameraProp_;

    EventProperty<PlotCameraInteractionHandler>* rotateEvent_;

    tgt::vec2 lastMousePosition_;
};

} // namespace

#endif // VRN_PLOTINTERACTIONHANDLER_H
