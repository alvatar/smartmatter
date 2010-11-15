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

#ifndef VRN_SERIALIZATIONFACTORIES_H
#define VRN_SERIALIZATIONFACTORIES_H

#include "voreen/core/io/serialization/serialization.h"

namespace voreen {

template <class T> class InterpolationFunction;

class PropertyTimelineFactory : public SerializableFactory {
public:
    /**
     * @see SerializableFactory::getTypeString
     */
    virtual const std::string getTypeString(const std::type_info& type) const;

    /**
     * @see SerializableFactory::createType
     */
    virtual Serializable* createType(const std::string& typeString);

    static PropertyTimelineFactory* getInstance();

private:
    static PropertyTimelineFactory* instance_;
};

class KeyValueFactory : public SerializableFactory {
public:
    virtual const std::string getTypeString(const std::type_info& type) const;

    virtual Serializable* createType(const std::string& typeString);

    static KeyValueFactory* getInstance();

private:
    static KeyValueFactory* instance_;
};

class TemplatePropertyTimelineStateFactory : public SerializableFactory {
public:
    virtual const std::string getTypeString(const std::type_info& type) const;

    virtual Serializable* createType(const std::string& typeString);

    static TemplatePropertyTimelineStateFactory* getInstance();

private:
    static TemplatePropertyTimelineStateFactory* instance_;
};

} // namespace voreen

#endif
