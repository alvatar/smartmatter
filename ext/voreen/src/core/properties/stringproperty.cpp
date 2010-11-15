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

#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/properties/condition.h"
#include "voreen/core/properties/propertywidgetfactory.h"
#include <sstream>

namespace voreen {

StringProperty::StringProperty(const std::string& id, const std::string& guiText,
                       const std::string& value, Processor::InvalidationLevel invalidationLevel)
    : TemplateProperty<std::string>(id, guiText, value, invalidationLevel)
{}

void StringProperty::serialize(XmlSerializer& s) const {
    Property::serialize(s);

    s.serialize("value", value_);
}

void StringProperty::deserialize(XmlDeserializer& s) {
    Property::deserialize(s);

    std::string value;
    s.deserialize("value", value);
    try {
        set(value);
    }
    catch (Condition::ValidationFailed& e) {
        s.addError(e);
    }
}

PropertyWidget* StringProperty::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string StringProperty::getTypeString() const {
    return "String";
}

}   // namespace
