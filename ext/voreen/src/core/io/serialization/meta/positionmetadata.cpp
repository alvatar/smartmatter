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

#include "voreen/core/io/serialization/meta/positionmetadata.h"

namespace voreen {

PositionMetaData::PositionMetaData(const int& x, const int& y)
    : x_(x)
    , y_(y)
{}

PositionMetaData::~PositionMetaData() {}

void PositionMetaData::serialize(XmlSerializer& s) const {
    s.serialize("x", x_);
    s.serialize("y", y_);
}

void PositionMetaData::deserialize(XmlDeserializer& s) {
    s.deserialize("x", x_);
    s.deserialize("y", y_);
}

const std::string PositionMetaData::getTypeString(const std::type_info& type) const {
    if (type == typeid(PositionMetaData))
        return "PositionMetaData";
    else
        return "";
}

Serializable* PositionMetaData::createType(const std::string &typeString) {
    if (typeString == "PositionMetaData")
        return new PositionMetaData();
    else
        return 0;
}

void PositionMetaData::setX(const int& value) {
    x_ = value;
}

int PositionMetaData::getX() const {
    return x_;
}

void PositionMetaData::setY(const int& value) {
    y_ = value;
}

int PositionMetaData::getY() const {
    return y_;
}

} // namespace
