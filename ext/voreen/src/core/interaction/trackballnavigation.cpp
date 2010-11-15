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

#include "voreen/core/interaction/trackballnavigation.h"
#include "voreen/core/properties/cameraproperty.h"

#include "voreen/core/network/networkevaluator.h"
#include "tgt/camera.h"

namespace voreen {

using tgt::vec3;
using tgt::vec2;
using tgt::ivec2;
using tgt::quat;
using tgt::MouseEvent;
using tgt::KeyEvent;

const std::string TrackballNavigation::loggerCat_ = "voreen.Trackballnavigation";

TrackballNavigation::TrackballNavigation(CameraProperty* camera, TrackballNavigation::Mode mode, float minDist, float maxDist)
    : trackball_(new VoreenTrackball(camera))
    , mode_(mode)
    , minDistance_(minDist)
    , maxDistance_(maxDist)
{

    initializeEventHandling();

    setMouseZoom(tgt::vec2(0.f, 0.5f));
    setMouseRoll(0.5f);

    setMouseWheelZoom(10.f, true);
    setMouseWheelRoll(10.f, true);
    trackball_->setSize(0.7f); // sets trackball sensitivity
    resetButton_ = tgt::MouseEvent::MOUSE_BUTTON_NONE;

    trackball_->setMoveCenter(false);
//        trackball_->setContinuousSpin(false);

    wheelCounter_   = -1;
    spinCounter_    = -1;
    moveCounter_    = -1;

    spinit_ = true;

    trackballEnabled_ = true;
    tracking_ = false;
}

TrackballNavigation::~TrackballNavigation() {
    delete trackball_;
}

void TrackballNavigation::resetTrackball() {
    wheelCounter_   = -1;
    spinCounter_    = -1;
    moveCounter_    = -1;
    spinit_ = true;
}

void TrackballNavigation::mousePressEvent(tgt::MouseEvent* e) {

    if (!trackball_)
        return;

    if (trackballEnabled_) {

        startMouseDrag( e );
        e->ignore();
    }
}

void TrackballNavigation::mouseReleaseEvent(tgt::MouseEvent* e) {

    if (!trackball_)
        return;

    if (trackballEnabled_) {
        endMouseDrag( e );
        e->ignore();

        moveCounter_ = -1;
    }
}

void TrackballNavigation::mouseMoveEvent(tgt::MouseEvent* e) {

    e->ignore();

    if (!trackball_ || !tracking_)
        return;

    if (trackballEnabled_) {

        vec2 newMouse = scaleMouse( ivec2(e->x(), e->y()), e->viewport() );

        if (mode_ == ROTATE_MODE) {
            trackball_->rotate(newMouse, lastMousePosition_);
            e->accept();
        }
        if (mode_ == SHIFT_MODE) {
            trackball_->move(newMouse, lastMousePosition_);
            e->accept();
        }
        if (mode_ == ZOOM_MODE) {
            trackball_->zoom(newMouse, lastMousePosition_, mouseZoomInDirection_);
            e->accept();
        }
        if (mode_ == ROLL_MODE) {
            rollCameraHorz((newMouse.x-lastMousePosition_.x) / mouseRollAcuteness_);
            e->accept();
        }

        lastMousePosition_ = newMouse;

        // restrict distance within specified range
        if (trackball_->getCenterDistance() < minDistance_)
            trackball_->zoomAbsolute(minDistance_);
        if (trackball_->getCenterDistance() > maxDistance_)
            trackball_->zoomAbsolute(maxDistance_);
    }
}


void TrackballNavigation::mouseDoubleClickEvent(tgt::MouseEvent* e) {

    e->ignore();

    if (!trackball_ )
        return;

    if (trackballEnabled_ && (tgt::MouseEvent::MouseButtons)e->button() == resetButton_) {
        resetTrackball();
        e->accept();
    }
}

void TrackballNavigation::wheelEvent(tgt::MouseEvent* e) {

    e->ignore();

    if (!trackball_ )
        return;

    wheelCounter_ = 0;

    if (trackballEnabled_) {

        if (mode_ == ZOOM_MODE) {
                trackball_->zoom( getZoomFactor( mouseWheelZoomAcuteness_,
                    ( mouseWheelUpZoomIn_ && (e->button()&MouseEvent::MOUSE_WHEEL_UP)) ||
                    (!mouseWheelUpZoomIn_ && (e->button()&MouseEvent::MOUSE_WHEEL_DOWN))
                    ));
                e->accept();
        }
        else if (mode_ == ROLL_MODE) {
                rollCameraHorz( getRollAngle( mouseWheelRollAcuteness_,
                    ( mouseWheelUpRollLeft_ && (e->button()&MouseEvent::MOUSE_WHEEL_UP)) ||
                    (!mouseWheelUpRollLeft_ && (e->button()&MouseEvent::MOUSE_WHEEL_DOWN))
                    ) );
                e->accept();
        }


        // restrict distance within specified range
        if (trackball_->getCenterDistance() < minDistance_)
            trackball_->zoomAbsolute(minDistance_);
        if (trackball_->getCenterDistance() > maxDistance_)
            trackball_->zoomAbsolute(maxDistance_);

    }
}

void TrackballNavigation::timerEvent(tgt::TimeEvent* e) {

    e->ignore();

    if (!trackball_)
        return;

    if (wheelCounter_ >= 0)
        wheelCounter_++;
    if (moveCounter_ >= 0)
        moveCounter_++;

    if (wheelCounter_ == 70) {
        wheelCounter_ = -1;
    }
    if (moveCounter_ == 10) {
        spinit_ = false;
        moveCounter_ = -1;
    }

    /*if (trackball_->getContinuousSpin() && spinit_ &&
        e->getTimer() == trackball_->getContinuousSpinTimer())
    {
        trackball_->timerEvent(e);
        distributeCamera(trackball_->getCamera());
    } */
}

void TrackballNavigation::keyEvent(tgt::KeyEvent* e) {

    e->ignore();
    bool accepted = false;

    if ( (keyRotateMod_ == e->modifiers() || keyRotateMod_ & e->modifiers())
         && e->pressed() == keyRotatePressed_) {
            if (e->keyCode() == keyRotateLeft_) {
                trackball_->rotate(vec3(0.f, 1.f, 0.f),  getRotationAngle(keyRotateAcuteness_));
                accepted = true;
            }
            else if (e->keyCode() == keyRotateRight_) {
                trackball_->rotate(vec3(0.f, 1.f, 0.f), -getRotationAngle(keyRotateAcuteness_));
                accepted = true;
            }
            else if (e->keyCode() == keyRotateUp_) {
                trackball_->rotate(vec3(1.f, 0.f, 0.f), -getRotationAngle(keyRotateAcuteness_));
                accepted = true;
            }
            else if (e->keyCode() == keyRotateDown_) {
                trackball_->rotate(vec3(1.f, 0.f, 0.f),  getRotationAngle(keyRotateAcuteness_));
                accepted = true;
            }
    }
    if ( (keyMoveMod_ == e->modifiers() || keyMoveMod_ & e->modifiers())
         && e->pressed() == keyMovePressed_) {
            if (e->keyCode() == keyMoveLeft_) {
                trackball_->move(getMovementLength(keyMoveAcuteness_), vec3(-1.f, 0.f, 0.f));
                accepted = true;
            }
            else if (e->keyCode() == keyMoveRight_) {
                trackball_->move(getMovementLength(keyMoveAcuteness_), vec3( 1.f, 0.f, 0.f));
                accepted = true;
            }
            else if (e->keyCode() == keyMoveUp_) {
                trackball_->move(getMovementLength(keyMoveAcuteness_), vec3(0.f,  1.f, 0.f));
                accepted = true;
            }
            else if (e->keyCode() == keyMoveDown_) {
                trackball_->move(getMovementLength(keyMoveAcuteness_), vec3(0.f, -1.f, 0.f));
                accepted = true;
            }
    }
    if ( (keyZoomMod_ == e->modifiers() || keyZoomMod_ & e->modifiers())
         && e->pressed() == keyZoomPressed_) {
            if (e->keyCode() == keyZoomIn_) {
                trackball_->zoom(getZoomFactor(keyZoomAcuteness_, true));
                accepted = true;
            }
            else if (e->keyCode() == keyZoomOut_) {
                trackball_->zoom(getZoomFactor(keyZoomAcuteness_, false));
                accepted = true;
            }
    }
    if ( (keyRollMod_ == e->modifiers() || keyRollMod_ & e->modifiers())
         && e->pressed() == keyRollPressed_) {
            if (e->keyCode() == keyRollLeft_) {
                rollCameraHorz(getRollAngle(keyRollAcuteness_, true));
                accepted = true;
            }
            else if (e->keyCode() == keyRollRight_) {
                rollCameraHorz(getRollAngle(keyRollAcuteness_, false));
                accepted = true;
            }
    }

    if (accepted)
        e->accept();

}
VoreenTrackball* TrackballNavigation::getTrackball() {
    return trackball_;
}

void TrackballNavigation::startMouseDrag(tgt::MouseEvent* e) {
    lastMousePosition_ = scaleMouse(ivec2(e->x(), e->y()), e->viewport());
    tracking_ = true;
}

void TrackballNavigation::endMouseDrag(tgt::MouseEvent* /*e*/) {
    tracking_ = false;
}

// This can be used to rotate (or "roll") the camera left and right, angle in radian measure
void TrackballNavigation::rollCameraHorz(float angle) {
    tgt::Camera cam = trackball_->getCamera()->get();
    vec3 up = normalize( quat::rotate(cam.getUpVector(), angle, cam.getLook()) );
    //cam.setUpVector(up);
    trackball_->getCamera()->setUpVector(up);
}

// This can be used to rotate (or "roll") the camera forwards and backwards, angle in radian measure
void TrackballNavigation::rollCameraVert(float angle) {
    tgt::Camera cam = trackball_->getCamera()->get();
    vec3 up = normalize( quat::rotate(cam.getUpVector(), angle, cam.getStrafe()) );
    cam.setUpVector(up);
    vec3 look = cross(up, cam.getStrafe());
    cam.setFocus(look);
    trackball_->getCamera()->setUpVector(up);
    //trackball_->getCamera()->set(cam);
    //trackball_->getCamera()->set(cam);
}


tgt::vec2 TrackballNavigation::scaleMouse(const tgt::ivec2& coords, const tgt::ivec2& viewport) const {
    return vec2( static_cast<float>(coords.x*2.f) / static_cast<float>(viewport.x) - 1.f,
                 1.f - static_cast<float>(coords.y*2.f) / static_cast<float>(viewport.y) );
}



float TrackballNavigation::getRotationAngle(float acuteness) const {
    return 1.f / acuteness;
}

float TrackballNavigation::getMovementLength(float acuteness) const {
    return trackball_->getSize() / acuteness;
}

float TrackballNavigation::getZoomFactor(float acuteness, bool zoomIn) const {
    if (zoomIn) {
        return 1.f + 1.f/acuteness;
    }
    else {
        return 1.f - 1.f/acuteness;
    }
}

float TrackballNavigation::getRollAngle(float acuteness, bool left) const {
    if (left) {
        return - 1.f / acuteness;
    }
    else {
        return 1.f / acuteness;
    }
}

void TrackballNavigation::initializeEventHandling() {
    keyRotateLeft_ = keyRotateRight_ = keyRotateUp_ = keyRotateDown_
        = keyMoveLeft_ = keyMoveRight_ = keyMoveUp_ = keyMoveDown_
        = keyZoomIn_ = keyZoomOut_
        = keyRollLeft_ = keyRollRight_
        = KeyEvent::K_LAST;
}

void TrackballNavigation::setKeyRotate(float acuteness,
                             KeyEvent::KeyCode left,
                             KeyEvent::KeyCode right,
                             KeyEvent::KeyCode up,
                             KeyEvent::KeyCode down,
                             int mod, bool pressed) {
    if (acuteness != 0.f) {
        keyRotateAcuteness_ = acuteness;
        keyRotateLeft_ = left;
        keyRotateRight_ = right;
        keyRotateUp_ = up;
        keyRotateDown_ = down;
        keyRotateMod_ = mod;
        keyRotatePressed_ = pressed;
    }
    else {
        keyRotateLeft_ = keyRotateRight_ = keyRotateUp_ = keyRotateDown_ = KeyEvent::K_LAST;
    }
}

void TrackballNavigation::setKeyMove(float acuteness,
                           KeyEvent::KeyCode left,
                           KeyEvent::KeyCode right,
                           KeyEvent::KeyCode up,
                           KeyEvent::KeyCode down,
                           int mod, bool pressed) {
    if (acuteness != 0.f) {
        keyMoveAcuteness_ = acuteness;
        keyMoveLeft_ = left;
        keyMoveRight_ = right;
        keyMoveUp_ = up;
        keyMoveDown_ = down;
        keyMoveMod_ = mod;
        keyMovePressed_ = pressed;
    }
    else {
        keyMoveLeft_ = keyMoveRight_ = keyMoveUp_ = keyMoveDown_ = KeyEvent::K_LAST;
    }
}

void TrackballNavigation::setMouseZoom(vec2 zoomInDirection) {
    mouseZoomInDirection_ = normalize(zoomInDirection);
}

void TrackballNavigation::setMouseWheelZoom(float acuteness, bool wheelUpZoomIn) {
    if (acuteness != 0.f) {
        mouseWheelZoomAcuteness_ = acuteness;
        mouseWheelUpZoomIn_ = wheelUpZoomIn;
    }

}

void TrackballNavigation::setKeyZoom(float acuteness, KeyEvent::KeyCode in, KeyEvent::KeyCode out,
                                     int mod, bool pressed) {
    if (acuteness != 0.f) {
        keyZoomAcuteness_ = acuteness;
        keyZoomIn_ = in;
        keyZoomOut_ = out;
        keyZoomMod_ = mod;
        keyZoomPressed_ = pressed;
    }
    else {
        keyZoomIn_ = keyZoomOut_ = KeyEvent::K_LAST;
    }
}

void TrackballNavigation::setMouseRoll(float acuteness) {
    mouseRollAcuteness_ = acuteness;
}

void TrackballNavigation::setMouseWheelRoll(float acuteness, bool wheelUpRollLeft) {
    if (acuteness != 0.f) {
        mouseWheelRollAcuteness_ = acuteness;
        mouseWheelUpRollLeft_ = wheelUpRollLeft;
    }
}

void TrackballNavigation::setKeyRoll(float acuteness, KeyEvent::KeyCode left, KeyEvent::KeyCode right,
                                     int mod, bool pressed) {
    if (acuteness != 0.f) {
        keyRollAcuteness_ = acuteness;
        keyRollLeft_ = left;
        keyRollRight_ = right;
        keyRollMod_ = mod;
        keyRollPressed_ = pressed;
    }
    else {
        keyRollLeft_ = keyRollRight_ = KeyEvent::K_LAST;
    }

}

void TrackballNavigation::setMode(TrackballNavigation::Mode mode) {
    mode_ = mode;
}

TrackballNavigation::Mode TrackballNavigation::getMode() const {
    return mode_;
}


} // namespace
