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

#include "voreen/core/properties/eventproperty.h"

namespace voreen {

EventPropertyBase::EventPropertyBase(const std::string& id, const std::string& guiName,
                                     bool mouseEvents, bool keyEvents,
                                     tgt::MouseEvent::MouseButtons mouseButtons,
                                     tgt::MouseEvent::MouseAction mouseAction,
                                     tgt::KeyEvent::KeyCode keyCode,
                                     tgt::Event::Modifier modifier,
                                     bool shareEvents, bool enabled)
    : Property(id, guiName)
    , receivesMouseEvents_(mouseEvents)
    , receivesKeyEvents_(keyEvents)
    , enabled_(enabled)
    , mouseButtons_(mouseButtons)
    , mouseAction_(mouseAction)
    , keyCode_(keyCode)
    , modifier_(modifier)
    , shareEvents_(shareEvents)
{
    setLevelOfDetail(Property::DEVELOPER);
}

bool EventPropertyBase::accepts(tgt::Event* e) const {

    if (!isEnabled())
        return false;

    bool accept = false;

    if (tgt::MouseEvent* mouseEvent = dynamic_cast<tgt::MouseEvent*>(e)) {
        accept = receivesMouseEvents();
        accept &= (mouseEvent->modifiers() == getModifier());
        accept &= ((mouseEvent->action() & getMouseAction())  || (mouseEvent->action() == getMouseAction()));
        accept &= ((mouseEvent->button() & getMouseButtons()) || (mouseEvent->button() == getMouseButtons()));
    }
    else if (tgt::KeyEvent* keyEvent = dynamic_cast<tgt::KeyEvent*>(e)) {
        accept = receivesKeyEvents();
        accept &= (keyEvent->modifiers() == getModifier());
        accept &= ((keyEvent->keyCode() == getKeyCode()) || (getKeyCode() == tgt::KeyEvent::K_LAST));
    }

    return accept;
}

tgt::Event::Modifier EventPropertyBase::getModifier() const {
    return modifier_;
}

void EventPropertyBase::setModifier(tgt::Event::Modifier value) {
    if (modifier_ != value) {
        modifier_ = value;
        notifyChangeListener();
    }
}

bool EventPropertyBase::isEnabled() const {
    return enabled_;
}

void EventPropertyBase::setEnabled(bool enabled) {
    if (enabled_ != enabled) {
        enabled_ = enabled;
        notifyChangeListener();
    }
}

bool EventPropertyBase::isSharing() const {
    return shareEvents_;
}

void EventPropertyBase::setSharing(bool shareEvents) {
    if (shareEvents_ != shareEvents) {
        shareEvents_ = shareEvents;
        notifyChangeListener();
    }
}


bool EventPropertyBase::receivesMouseEvents() const {
    return receivesMouseEvents_;
}

bool EventPropertyBase::receivesKeyEvents() const {
    return receivesKeyEvents_;
}

tgt::MouseEvent::MouseButtons EventPropertyBase::getMouseButtons() const {
    return mouseButtons_;
}

void EventPropertyBase::setMouseButtons(tgt::MouseEvent::MouseButtons buttons) {
    if (mouseButtons_ != buttons) {
        mouseButtons_ = buttons;
        notifyChangeListener();
    }
}

tgt::MouseEvent::MouseAction EventPropertyBase::getMouseAction() const {
    return mouseAction_;
}

void EventPropertyBase::setMouseAction(tgt::MouseEvent::MouseAction action) {
    if (mouseAction_ != action) {
        mouseAction_ = action;
        notifyChangeListener();
    }
}

tgt::KeyEvent::KeyCode EventPropertyBase::getKeyCode() const {
    return keyCode_;
}

void EventPropertyBase::setKeyCode(tgt::KeyEvent::KeyCode keyCode) {
    if (keyCode_) {
        keyCode_ = keyCode;
        notifyChangeListener();
    }
}

void EventPropertyBase::setReceivesMouseEvents(bool mouseEvents) {
    if (receivesMouseEvents_ != mouseEvents) {
        receivesMouseEvents_ = mouseEvents;
        notifyChangeListener();
    }
}

void EventPropertyBase::setReceivesKeyEvents(bool keyEvents) {
    if (receivesKeyEvents_) {
        receivesKeyEvents_ = keyEvents;
        notifyChangeListener();
    }
}

void EventPropertyBase::serialize(XmlSerializer& s) const {

    Property::serialize(s);

    s.serialize("enabled", enabled_);
    s.serialize("mouseAction", mouseAction_);
    s.serialize("mouseButtons", mouseButtons_);
    s.serialize("keyCode", keyCode_);
    s.serialize("modifier", modifier_);
    s.serialize("shareEvents", shareEvents_);
}

void EventPropertyBase::deserialize(XmlDeserializer& s) {

    Property::deserialize(s);

    try {
        bool enabled, shareEvents;
        s.deserialize("enabled", enabled);
        s.deserialize("shareEvents", shareEvents);
        enabled_ = enabled;
        shareEvents_ = shareEvents;
    }
    catch (XmlSerializationNoSuchDataException& /*e*/) {
        s.removeLastError();
    }

    try {
        int mouseAction = 0;
        int mouseButtons = 0;
        s.deserialize("mouseAction", mouseAction);
        s.deserialize("mouseButtons", mouseButtons);
        mouseAction_ = static_cast<tgt::MouseEvent::MouseAction>(mouseAction);
        mouseButtons_ = static_cast<tgt::MouseEvent::MouseButtons>(mouseButtons);
    }
    catch (XmlSerializationNoSuchDataException& /*e*/) {
        s.removeLastError();
    }

    try {
        int keyCode = 0;
        s.deserialize("keyCode", keyCode);
        keyCode_ = static_cast<tgt::KeyEvent::KeyCode>(keyCode);
    }
    catch (XmlSerializationNoSuchDataException& /*e*/) {
        s.removeLastError();
    }

    try {
        int modifier = 0;
        s.deserialize("modifier", modifier);
        modifier_ = static_cast<tgt::Event::Modifier>(modifier);
    }
    catch (XmlSerializationNoSuchDataException& /*e*/) {
        s.removeLastError();
    }

    notifyChangeListener();
}

std::string EventPropertyBase::getTypeString() const {
    return "EventProperty";
}


} // namespace
