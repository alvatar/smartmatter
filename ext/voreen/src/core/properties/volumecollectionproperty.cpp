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

#include "voreen/core/properties/volumecollectionproperty.h"

#include "voreen/core/datastructures/volume/volumecontainer.h"
#include "voreen/core/io/volumeserializerpopulator.h"
#include "voreen/core/io/volumeserializer.h"

#include "voreen/core/properties/propertywidgetfactory.h"

namespace voreen {

VolumeCollectionProperty::VolumeCollectionProperty(const std::string& id, const std::string& guiText,
                    VolumeCollection* const value, Processor::InvalidationLevel invalidationLevel) :
                    TemplateProperty<VolumeCollection*>(id, guiText, value, invalidationLevel)
{

}

void VolumeCollectionProperty::deinitialize() throw (VoreenException) {
    delete value_;
    value_ = 0;
}

PropertyWidget* VolumeCollectionProperty::createWidget(PropertyWidgetFactory* f)     {
    return f->createWidget(this);
    //return 0;
}

void VolumeCollectionProperty::serialize(XmlSerializer& s) const {
    Property::serialize(s);

    s.serialize("VolumeCollection", value_);
}

void VolumeCollectionProperty::deserialize(XmlDeserializer& s) {
    Property::deserialize(s);

    VolumeCollection* collection = 0;
    s.deserialize("VolumeCollection", collection);
    if (collection)
        set(collection);

    invalidate();
}

std::string VolumeCollectionProperty::getTypeString() const {
    return "VolumeCollection";
}

} // namespace voreen
