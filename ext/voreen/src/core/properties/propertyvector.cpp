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

#include "voreen/core/properties/propertyvector.h"
#include "voreen/core/properties/condition.h"
#include "voreen/core/properties/propertywidgetfactory.h"
#include <sstream>


namespace voreen {

const std::string PropertyVector::loggerCat_("voreen.Properties.PropertyVector");

PropertyVector::PropertyVector(const std::string& id, const std::string& guiText, std::vector<Property*> properties)
    : Property(id, guiText),
      properties_(properties)
{}

PropertyVector::~PropertyVector() {
    for (size_t i=0; i<properties_.size(); ++i)
        delete properties_[i];
}

std::string PropertyVector::getName() const {
    return "PropertyVector";
}

PropertyWidget* PropertyVector::createWidget(PropertyWidgetFactory* f) {

    return f->createWidget(this);
}

const std::vector<Property*>& PropertyVector::getProperties() const {
    return properties_;
}

void PropertyVector::setOwner(PropertyOwner* owner){
    Property::setOwner(owner);
    for (size_t i=0; i < properties_.size(); ++i)
        properties_[i]->setOwner(owner);
}

void PropertyVector::addProperty(Property* prop) {
    properties_.push_back(prop);
    prop->setOwner(this);
}

void PropertyVector::serialize(XmlSerializer& s) const {
    Property::serialize(s);

    // serialize the properties of the processor
    typedef std::map<std::string, Property*> PropertyMapType;

    PropertyMapType propertyMap;
    for (std::vector<Property*>::const_iterator it = properties_.begin(); it != properties_.end(); ++it)
        propertyMap[(*it)->getID()] = *it;

    const bool usePointerContentSerialization = s.getUsePointerContentSerialization();
    s.setUsePointerContentSerialization(true);
    s.serialize("ElementProperties", propertyMap, "Property", "name");
    s.setUsePointerContentSerialization(usePointerContentSerialization);
}

void PropertyVector::deserialize(XmlDeserializer& s) {
    Property::deserialize(s);

    // deserialize the properties of the processor
    typedef std::map<std::string, Property*> PropertyMapType;

    PropertyMapType propertyMap;
    for (std::vector<Property*>::const_iterator it = properties_.begin(); it != properties_.end(); ++it) {
        propertyMap[(*it)->getID()] = *it;
    }

    const bool usePointerContentSerialization = s.getUsePointerContentSerialization();
    s.setUsePointerContentSerialization(true);
    s.deserialize("ElementProperties", propertyMap, "Property", "name");
    s.setUsePointerContentSerialization(usePointerContentSerialization);
}

int PropertyVector::size() const {
    return static_cast<int>(properties_.size());
}

void PropertyVector::initialize() throw (VoreenException) {
    for (std::vector<Property*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
        (*it)->initialize();
}

void PropertyVector::deinitialize() throw (VoreenException) {
    for (std::vector<Property*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
        (*it)->deinitialize();
}

std::string PropertyVector::getTypeString() const {
    return "PropertyVector";
}

}   // namespace
