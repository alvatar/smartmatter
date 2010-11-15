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

#ifndef VRN_PICKINGMANAGER_H
#define VRN_PICKINGMANAGER_H

#include "tgt/event/mouseevent.h"

#include <vector>

namespace tgt {
class Camera;
}

namespace voreen {

class IDManager;
class PickableObject;
class RenderTarget;

class PickingManager {
public:
    PickingManager();
    ~PickingManager();

    void setRenderTarget(RenderTarget* renderTarget);
    void prepareDrawing();
    void addPickableObject(PickableObject* pickableObject);
    void addNonDynamicPickableObject(PickableObject* pickableObject);
    bool setGLColor(PickableObject* pickableObject);

    bool onMouseEvent(tgt::MouseEvent* mouseEve, tgt::Camera* sceneCamera);

    bool isInitialized() const;

protected:
    IDManager* idManager_;
    bool initialized_;

    std::vector<PickableObject*> pickableObjects_;
    std::vector<PickableObject*> nonDynamicPickableObjects_;
};

// ================================================================================================

class PickableObject {
public:
    PickableObject();
    virtual ~PickableObject();

    virtual void mousePress(tgt::MouseEvent* mouseEvent, tgt::Camera* sceneCamera) = 0;
    virtual void mouseRelease(tgt::MouseEvent* mouseEvent, tgt::Camera* sceneCamera) = 0;
    virtual void mouseMove(tgt::MouseEvent* mouseEvent, tgt::Camera* sceneCamera) = 0;

    void setRenderTargetDimensions(tgt::ivec2 renderTargetDimensions);
    bool isClicked() const;

protected:
    void refreshPositions();

    tgt::ivec2 oldScreenPosition_, newScreenPosition_;
    tgt::vec3 oldWorldPosition_, newWorldPosition_;

    tgt::ivec2 startingPosition_;
    tgt::ivec2 renderTargetDimensions_;

    bool isClicked_;
    tgt::MouseEvent::MouseButtons pressedButton_;
};

} // namespace

#endif // PICKINGMANAGER_H
