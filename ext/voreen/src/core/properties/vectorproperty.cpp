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

#include "voreen/core/properties/vectorproperty.h"
#include "voreen/core/properties/propertywidgetfactory.h"

namespace voreen {

FloatVec2Property::FloatVec2Property(const std::string& id, const std::string& guiText,
                             const tgt::vec2& value, const tgt::vec2& minValue,
                             const tgt::vec2& maxValue, Processor::InvalidationLevel invalidationLevel)
    : NumericProperty<tgt::vec2>(id, guiText, value, minValue, maxValue, tgt::vec2(0.01f),
        invalidationLevel)
{}

PropertyWidget* FloatVec2Property::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string FloatVec2Property::getTypeString() const {
    return "FloatVector2";
}

//---------------------------------------------------------------------------

FloatVec3Property::FloatVec3Property(const std::string& id, const std::string& guiText,
                             const tgt::vec3& value, const tgt::vec3& minValue,
                             const tgt::vec3& maxValue, Processor::InvalidationLevel invalidationLevel)
    : NumericProperty<tgt::vec3>(id, guiText, value, minValue, maxValue, tgt::vec3(0.01f),
        invalidationLevel)
{}


PropertyWidget* FloatVec3Property::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string FloatVec3Property::getTypeString() const {
    return "FloatVector3";
}

//---------------------------------------------------------------------------

FloatVec4Property::FloatVec4Property(const std::string& id, const std::string& guiText,
                             const tgt::vec4& value, const tgt::vec4& minValue,
                             const tgt::vec4& maxValue, Processor::InvalidationLevel invalidationLevel)
    : NumericProperty<tgt::vec4>(id, guiText, value, minValue, maxValue, tgt::vec4(0.01f),
        invalidationLevel)
{}

PropertyWidget* FloatVec4Property::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string FloatVec4Property::getTypeString() const {
    return "FloatVector4";
}

//---------------------------------------------------------------------------

IntVec2Property::IntVec2Property(const std::string& id, const std::string& guiText,
                         const tgt::ivec2& value, const tgt::ivec2& minValue,
                         const tgt::ivec2& maxValue, Processor::InvalidationLevel invalidationLevel)
    : NumericProperty<tgt::ivec2>(id, guiText, value, minValue, maxValue, tgt::ivec2(1),
            invalidationLevel)
{}

PropertyWidget* IntVec2Property::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string IntVec2Property::getTypeString() const {
    return "IntVector2";
}

//---------------------------------------------------------------------------

IntVec3Property::IntVec3Property(const std::string& id, const std::string& guiText,
                         const tgt::ivec3& value, const tgt::ivec3& minValue,
                         const tgt::ivec3& maxValue, Processor::InvalidationLevel invalidationLevel)
    : NumericProperty<tgt::ivec3>(id, guiText, value, minValue, maxValue, tgt::ivec3(1),
        invalidationLevel)
{}

PropertyWidget* IntVec3Property::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string IntVec3Property::getTypeString() const {
    return "IntVector3";
}

//---------------------------------------------------------------------------

IntVec4Property::IntVec4Property(const std::string& id, const std::string& guiText,
                         const tgt::ivec4& value, const tgt::ivec4& minValue,
                         const tgt::ivec4& maxValue, Processor::InvalidationLevel invalidationLevel)
    : NumericProperty<tgt::ivec4>(id, guiText, value, minValue, maxValue, tgt::ivec4(1),
        invalidationLevel)
{}

PropertyWidget* IntVec4Property::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string IntVec4Property::getTypeString() const {
    return "IntVector4";
}

} // namespace voreen
