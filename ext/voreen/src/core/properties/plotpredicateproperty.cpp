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

#include "voreen/core/properties/plotpredicateproperty.h"
#include "voreen/core/properties/propertywidgetfactory.h"

#include "voreen/core/plotting/plotpredicate.h"
#include <sstream>


namespace voreen {

const std::string PlotPredicateProperty::loggerCat_("voreen.Properties.PlotPredicateProperty");

PlotPredicateProperty::PlotPredicateProperty(const std::string& id, const std::string& guiText, Processor::InvalidationLevel invalidationLevel)
    : TemplateProperty<std::vector<std::pair<int, PlotPredicate*> > >(id, guiText, std::vector<std::pair<int, PlotPredicate*> >(), invalidationLevel)
{
    value_ = std::vector<std::pair<int, PlotPredicate*> >();
}

PlotPredicateProperty::~PlotPredicateProperty() {
    clear();
}

void PlotPredicateProperty::clear() {
    for (size_t i=0; i<value_.size(); ++i)
        delete value_[i].second;
    value_.clear();
    notifyAll();;
}

const std::vector<std::pair<int, PlotPredicate*> >& PlotPredicateProperty::get() const {
    return value_;
}

std::vector<std::pair<int,PlotPredicate*> > PlotPredicateProperty::getCloned() const {
    std::vector<std::pair<int,PlotPredicate*> > toReturn;
    for (size_t i = 0; i < value_.size(); ++i) {
        toReturn.push_back(std::make_pair(value_.at(i).first, value_.at(i).second->clone()));
    }
    return toReturn;
}

void PlotPredicateProperty::set(const std::vector<std::pair<int, PlotPredicate*> >& value) {
    clear();
    for (size_t i = 0; i < value.size(); ++i) {
        value_.push_back(std::make_pair(value.at(i).first, value.at(i).second->clone()));
    }
    notifyAll();
}

int PlotPredicateProperty::size() const {
    return static_cast<int>(value_.size());
}

void PlotPredicateProperty::select(const std::vector<int>& columns){
    std::vector<std::pair<int, PlotPredicate*> >::iterator predit;
    bool found;
    int pos;
    predit = value_.begin();
    while (predit < value_.end()) {
        found = false;
        pos = -1;
        for (size_t i = 0; i < columns.size(); ++i) {
            if (predit->first == columns[i]) {
                found = true;
                pos = i;
                break;
            }
        }
        if (found) {
            predit->first = pos;
            ++predit;
        }
        else {
            delete predit->second;
            predit = value_.erase(predit);
        }
    }
}

void PlotPredicateProperty::notifyAll() {
    std::vector<std::pair<int, PlotPredicate*> > cpy = value_;

    // check if conditions are met and exec actions
    for (size_t j = 0; j < conditions_.size(); ++j)
        conditions_[j]->exec();

    updateWidgets();

    // invalidate owner:
    invalidateOwner();
}


void PlotPredicateProperty::serialize(XmlSerializer& s) const {
    Property::serialize(s);

    //std::vector<SerializablePair<int, PlotPredicate*> > serializableVector;
    //for (size_t i = 0; i < value_.size(); ++i) {
    //    serializableVector.push_back(SerializablePair<int,PlotPredicate*>(value_.at(i).first,value_.at(i).second));
    //}
    //s.serialize("PredicateList",serializableVector);

    /*    // serialize the properties of the processor
        typedef std::map<std::string, Property*> PropertyMapType;

        PropertyMapType propertyMap;
        for (Properties::const_iterator it = properties_.begin(); it != properties_.end(); ++it)
            propertyMap[(*it)->getID()] = *it;

        const bool usePointerContentSerialization = s.getUsePointerContentSerialization();
        s.setUsePointerContentSerialization(true);
        s.serialize("ElementProperties", propertyMap, "Property", "name");
        s.setUsePointerContentSerialization(usePointerContentSerialization);*/
}

void PlotPredicateProperty::deserialize(XmlDeserializer& s) {
    Property::deserialize(s);

    /*    // deserialize the properties of the processor
        typedef std::map<std::string, Property*> PropertyMapType;

        PropertyMapType propertyMap;
        for (Properties::const_iterator it = properties_.begin(); it != properties_.end(); ++it)
            propertyMap[(*it)->getID()] = *it;

        const bool usePointerContentSerialization = s.getUsePointerContentSerialization();
        s.setUsePointerContentSerialization(true);
        s.deserialize("ElementProperties", propertyMap, "Property", "name");
        s.setUsePointerContentSerialization(usePointerContentSerialization);*/
}

PropertyWidget* PlotPredicateProperty::createWidget(PropertyWidgetFactory* f) {
    return f->createWidget(this);
}

std::string PlotPredicateProperty::getTypeString() const {
    return "PlotPredicate";
}

}   // namespace


