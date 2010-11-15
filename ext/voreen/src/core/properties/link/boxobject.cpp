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

#include "voreen/core/properties/link/boxobject.h"
#include "voreen/core/properties/shaderproperty.h"
#include "voreen/core/datastructures/transfunc/transfunc.h"
#include "voreen/core/datastructures/transfunc/transfuncintensity.h"
#include "voreen/core/properties/volumehandleproperty.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/datastructures/volume/volumehandle.h"

#include <sstream>

#define VP(a) (*(a*)value_)

namespace voreen {

using std::string;
using std::vector;
using std::pair;
using std::stringstream;
using tgt::vec2;
using tgt::vec3;
using tgt::vec4;
using tgt::ivec2;
using tgt::ivec3;
using tgt::ivec4;
using tgt::dvec2;
using tgt::dvec3;
using tgt::dvec4;
using tgt::mat2;
using tgt::mat3;
using tgt::mat4;
using tgt::Camera;

template<class T>
void BoxObject::set(const T& value, BoxObjectType type) {
    if (type != currentType_) {
        deleteValue();
        currentType_ = type;
    }
    if (value_ == 0) {
        value_ = new T;
    }
    *(T*)value_ = value;
}

void BoxObject::set(const TransFunc& value, BoxObjectType type) {
    if (type != currentType_) {
        deleteValue();
        currentType_ = type;
    }
    value_ = (void*)&value;
}

void BoxObject::set(const Camera& value, BoxObjectType type) {
    if (type != currentType_) {
        deleteValue();
        currentType_ = type;
    }
    value_ = (void*)&value;
}

void BoxObject::set(const VolumeHandle& value, BoxObjectType type) {
    if (type != currentType_) {
        deleteValue();
        currentType_ = type;
    }
    value_ = (void*)&value;
}

void BoxObject::set(const VolumeCollection& value, BoxObjectType type) {
    if (type != currentType_) {
        deleteValue();
        currentType_ = type;
    }
    value_ = (void*)&value;
}

template<class T>
const string BoxObject::toString(const T& value) const {
    stringstream s;
    string result;
    s << value;
    s >> result;
    return result;
}


BoxObject::BoxObject()
    : Serializable()
    , value_(0)
    , currentType_(NIL)
{}

BoxObject::BoxObject(const BoxObject& obj)
    : Serializable()
    , value_(0) {
    switch (obj.getType()) {
    case NIL:
        break;
    case BOOL:
        set<bool>(obj.getBool(), BOOL);
        break;
    case COLORMAP:
        set<ColorMap>(obj.getColorMap(), COLORMAP);
        break;
    case DOUBLE:
        set<double>(obj.getDouble(), DOUBLE);
        break;
    case FLOAT:
        set<float>(obj.getFloat(), FLOAT);
        break;
    case INTEGER:
        set<int>(obj.getInt(), INTEGER);
        break;
    case PLOTENTITYSETTINGSVEC:
        set<vector<PlotEntitySettings> >(obj.getPlotEntitySettingsVec(), PLOTENTITYSETTINGSVEC);
        break;
    case LONG:
        set<long>(obj.getLong(), LONG);
        break;
    case STRING:
        set<string>(obj.getString(), STRING);
        break;
    case STRINGVEC:
        set<vector<string> >(obj.getStringVec(), STRINGVEC);
        break;
    case IVEC2:
        set<ivec2>(obj.getIVec2(), IVEC2);
        break;
    case IVEC3:
        set<ivec3>(obj.getIVec3(), IVEC3);
        break;
    case IVEC4:
        set<ivec4>(obj.getIVec4(), IVEC4);
        break;
    case VEC2:
        set<vec2>(obj.getVec2(), VEC2);
        break;
    case VEC3:
        set<vec3>(obj.getVec3(), VEC3);
        break;
    case VEC4:
        set<vec4>(obj.getVec4(), VEC4);
        break;
    case DVEC2:
        set<dvec2>(obj.getDVec2(), DVEC2);
        break;
    case DVEC3:
        set<dvec3>(obj.getDVec3(), DVEC3);
        break;
    case DVEC4:
        set<dvec4>(obj.getDVec4(), DVEC4);
        break;
    case MAT2:
        set<mat2>(obj.getMat2(), MAT2);
        break;
    case MAT3:
        set<mat3>(obj.getMat3(), MAT3);
        break;
    case MAT4:
        set<mat4>(obj.getMat4(), MAT4);
        break;
    case PLOTPREDICATEVEC:
        set<std::vector<std::pair<int, PlotPredicate*> > >(obj.getPlotPredicateVec(), PLOTPREDICATEVEC);
        break;
    case PLOTSELECTIONENTRYVEC:
        set< std::vector< PlotSelectionEntry > >(obj.getPlotSelectionEntryVec(), PLOTSELECTIONENTRYVEC);
        break;
    case SHADER:
        set<ShaderSource>(obj.getShader(), SHADER);
        break;
    case TRANSFUNC:
        set(*obj.getTransFunc(), TRANSFUNC);
        break;
    case CAMERA:
        set(obj.getCamera(), CAMERA);
        break;
    case VOLUMEHANDLE:
        set(*obj.getVolumeHandle(), VOLUMEHANDLE);
        break;
    case VOLUMECOLLECTION:
        set(*obj.getVolumeCollection(), VOLUMECOLLECTION);
        break;
    default:
        LWARNINGC("voreen.BoxObject", "Unknown data type (missing case in switch-statement)");
    }
}

BoxObject::BoxObject(int value) : value_(0), currentType_(INTEGER) {
    set<int>(value, INTEGER);
}

BoxObject::BoxObject(float value) : value_(0), currentType_(FLOAT) {
    set<float>(value, FLOAT);
}

BoxObject::BoxObject(double value) : value_(0), currentType_(DOUBLE) {
    set<double>(value, DOUBLE);
}

BoxObject::BoxObject(bool value) : value_(0), currentType_(BOOL) {
    set<bool>(value, BOOL);
}

BoxObject::BoxObject(const string& value) : value_(0), currentType_(STRING) {
    set<string>(value, STRING);
}

BoxObject::BoxObject(const vector<string>& value) : value_(0), currentType_(STRINGVEC) {
    set<vector<string> >(value, STRINGVEC);
}

BoxObject::BoxObject(const vector<PlotEntitySettings>& value) : value_(0), currentType_(PLOTENTITYSETTINGSVEC) {
    set<vector<PlotEntitySettings> >(value, PLOTENTITYSETTINGSVEC);
}

BoxObject::BoxObject(long value) : value_(0), currentType_(LONG) {
    set<long>(value, LONG);
}

BoxObject::BoxObject(const vec2& value) : value_(0), currentType_(VEC2) {
    set<vec2>(value, VEC2);
}

BoxObject::BoxObject(const vec3& value) : value_(0), currentType_(VEC3) {
    set<vec3>(value, VEC3);
}

BoxObject::BoxObject(const vec4& value) : value_(0), currentType_(VEC4) {
    set<vec4>(value, VEC4);
}

BoxObject::BoxObject(const dvec2& value) : value_(0), currentType_(DVEC2) {
    set<dvec2>(value, DVEC2);
}

BoxObject::BoxObject(const dvec3& value) : value_(0), currentType_(DVEC3) {
    set<dvec3>(value, DVEC3);
}

BoxObject::BoxObject(const dvec4& value) : value_(0), currentType_(DVEC4) {
    set<dvec4>(value, DVEC4);
}

BoxObject::BoxObject(const ivec2& value) : value_(0), currentType_(IVEC2) {
    set<ivec2>(value, IVEC2);
}

BoxObject::BoxObject(const ivec3& value) : value_(0), currentType_(IVEC3) {
    set<ivec3>(value, IVEC3);
}

BoxObject::BoxObject(const ivec4& value) : value_(0), currentType_(IVEC4) {
    set<ivec4>(value, IVEC4);
}

BoxObject::BoxObject(const mat2& value) : value_(0), currentType_(MAT2) {
    set<mat2>(value, MAT2);
}

BoxObject::BoxObject(const mat3& value) : value_(0), currentType_(MAT3) {
    set<mat3>(value, MAT3);
}

BoxObject::BoxObject(const mat4& value) : value_(0), currentType_(MAT4) {
    set<mat4>(value, MAT4);
}

BoxObject::BoxObject(const ShaderSource& value) : value_(0), currentType_(SHADER) {
    set<ShaderSource>(value, SHADER);
}

BoxObject::BoxObject(const ColorMap& value) : value_(0), currentType_(COLORMAP) {
    set<ColorMap>(value, COLORMAP);
}

BoxObject::BoxObject(const TransFunc* value) : value_(0), currentType_(TRANSFUNC) {
    set(*value, TRANSFUNC);
}

BoxObject::BoxObject(const Camera value) : value_(0), currentType_(CAMERA) {
    set(value, CAMERA);
}

BoxObject::BoxObject(const VolumeHandle* value) : value_(0), currentType_(VOLUMEHANDLE) {
    set(*value, VOLUMEHANDLE);
}

BoxObject::BoxObject(const VolumeCollection* value) : value_(0), currentType_(VOLUMECOLLECTION) {
    set(*value, VOLUMECOLLECTION);
}

BoxObject::BoxObject(const std::vector< std::pair< int, voreen::PlotPredicate* > >& value) : value_(0), currentType_(PLOTPREDICATEVEC) {
    set(value, PLOTPREDICATEVEC);
}

BoxObject::BoxObject(const std::vector< PlotSelectionEntry >& value) : value_(0), currentType_(PLOTSELECTIONENTRYVEC) {
    set(value, PLOTSELECTIONENTRYVEC);
}

BoxObject BoxObject::deepCopy() const {
    switch (currentType_) {
    case BOOL:
    case COLORMAP:
    case DOUBLE:
    case FLOAT:
    case INTEGER:
    case LONG:
    case STRING:
    case STRINGVEC:
    case IVEC2:
    case IVEC3:
    case IVEC4:
    case VEC2:
    case VEC3:
    case VEC4:
    case DVEC2:
    case DVEC3:
    case DVEC4:
    case MAT2:
    case MAT3:
    case MAT4:
    case VOLUMEHANDLE:
        return BoxObject(*this);
        break;
    case TRANSFUNC:
        {
            const TransFunc* trans = getTransFunc();
            const TransFuncIntensity* transIntensity = dynamic_cast<const TransFuncIntensity*>(trans);
            tgtAssert(transIntensity, "only TransFuncIntensity are supported for now");
            TransFuncIntensity* result = new TransFuncIntensity(*transIntensity);
            return BoxObject(result);
        }
    case CAMERA:
        {
            const Camera* camera = getCamera();
            Camera* result = camera->clone();
            return BoxObject(result);
        }
    default:
        tgtAssert(false, "shouldn't get here");
        return BoxObject();
    }

}

BoxObject::~BoxObject() {
    deleteValue();
}

void BoxObject::deleteValue() {
    if (value_ != 0) {
        switch (currentType_) {
        case NIL:
            break;
        case BOOL:
            delete static_cast<bool*>(value_);
            break;
        case COLORMAP:
            delete static_cast<ColorMap*>(value_);
            break;
        case DOUBLE:
            delete static_cast<double*>(value_);
            break;
        case FLOAT:
            delete static_cast<float*>(value_);
            break;
        case INTEGER:
            delete static_cast<int*>(value_);
            break;
        case PLOTENTITYSETTINGSVEC:
            delete static_cast<vector<PlotEntitySettings>*>(value_);
            break;
        case LONG:
            delete static_cast<long*>(value_);
            break;
        case STRING:
            delete static_cast<string*>(value_);
            break;
        case STRINGVEC:
            delete static_cast<vector<string>*>(value_);
            break;
        case IVEC2:
            delete static_cast<ivec2*>(value_);
            break;
        case IVEC3:
            delete static_cast<ivec3*>(value_);
            break;
        case IVEC4:
            delete static_cast<ivec4*>(value_);
            break;
        case VEC2:
            delete static_cast<vec2*>(value_);
            break;
        case VEC3:
            delete static_cast<vec3*>(value_);
            break;
        case VEC4:
            delete static_cast<vec4*>(value_);
            break;
        case DVEC2:
            delete static_cast<dvec2*>(value_);
            break;
        case DVEC3:
            delete static_cast<dvec3*>(value_);
            break;
        case DVEC4:
            delete static_cast<dvec4*>(value_);
            break;
        case MAT2:
            delete static_cast<mat2*>(value_);
            break;
        case MAT3:
            delete static_cast<mat3*>(value_);
            break;
        case MAT4:
            delete static_cast<mat4*>(value_);
            break;
        case PLOTPREDICATEVEC:
            delete static_cast<vector<pair<int, PlotPredicate* > >* >(value_);
        case PLOTSELECTIONENTRYVEC:
            delete static_cast<vector< PlotSelectionEntry >* >(value_);
        case TRANSFUNC:
            //delete static_cast<TransFuncIntensity*>(value_);
            break;
        case CAMERA:
            //delete static_cast<Camera*>(value_);
            break;
        case VOLUMEHANDLE:
            //delete static_cast<VolumeHandle*>(value_);
            break;
        case VOLUMECOLLECTION:
            //delete static_cast<VolumeCollection*>(value_);
            break;
        case SHADER:
            delete static_cast<ShaderSource*>(value_);
            break;
        default:
            LWARNINGC("voreen.BoxObject", "Unable to delete value due to unknown data type (missing case in switch-statement)");
        }
        value_ = 0;
    }
}

BoxObject::BoxObjectType BoxObject::getType() const {
    return currentType_;
}

std::string BoxObject::getTypeName(BoxObjectType type) {
    switch (type) {
    case NIL:
        return "<empty>";
        break;
    case BOOL:
        return "bool";
        break;
    case COLORMAP:
        return "colormap";
        break;
    case DOUBLE:
        return "double";
        break;
    case FLOAT:
        return "float";
        break;
    case INTEGER:
        return "integer";
        break;
    case PLOTENTITYSETTINGSVEC:
        return "plotentitysettings vector";
        break;
    case LONG:
        return "long";
        break;
    case STRING:
        return "string";
        break;
    case STRINGVEC:
        return "string vector";
        break;
    case IVEC2:
        return "ivec2";
        break;
    case IVEC3:
        return "ivec3";
        break;
    case IVEC4:
        return "ivec4";
        break;
    case VEC2:
        return "vec2";
        break;
    case VEC3:
        return "vec3";
        break;
    case VEC4:
        return "vec4";
        break;
    case DVEC2:
        return "dvec2";
        break;
    case DVEC3:
        return "dvec3";
        break;
    case DVEC4:
        return "dvec4";
        break;
    case MAT2:
        return "mat2";
        break;
    case MAT3:
        return "mat3";
        break;
    case MAT4:
        return "mat4";
        break;
    case PLOTPREDICATEVEC:
        return "plot predicate vector";
        break;
    case PLOTSELECTIONENTRYVEC:
        return "plot selection entry vector";
        break;
    case SHADER:
        return "shader";
        break;
    case TRANSFUNC:
        return "transfer function";
        break;
    case CAMERA:
        return "camera";
        break;
    case VOLUMEHANDLE:
        return "volume handle";
        break;
    case VOLUMECOLLECTION:
        return "volume collection";
        break;
    default:
        return "<unknown>";
    }
}

BoxObject::BoxObjectType BoxObject::getType(const std::string& typeName) {
    if (typeName == "<empty>")
        return NIL;
    else if (typeName == "bool")
        return BOOL;
    else if (typeName == "colormap")
        return COLORMAP;
    else if (typeName == "double")
        return DOUBLE;
    else if (typeName == "float")
        return FLOAT;
    else if (typeName == "integer")
        return INTEGER;
    else if (typeName == "plotentitysettings vector")
        return PLOTENTITYSETTINGSVEC;
    else if (typeName == "long")
        return LONG;
    else if (typeName == "string")
        return STRING;
    else if (typeName == "string vector")
        return STRINGVEC;
    else if (typeName == "ivec2")
        return IVEC2;
    else if (typeName == "ivec3")
        return IVEC3;
    else if (typeName == "ivec4")
        return IVEC4;
    else if (typeName == "vec2")
        return VEC2;
    else if (typeName == "vec3")
        return VEC3;
    else if (typeName == "vec4")
        return VEC4;
    else if (typeName == "dvec2")
        return DVEC2;
    else if (typeName == "dvec3")
        return DVEC3;
    else if (typeName == "dvec4")
        return DVEC4;
    else if (typeName == "mat2")
        return MAT2;
    else if (typeName == "mat3")
        return MAT3;
    else if (typeName == "mat4")
        return MAT4;
    else if (typeName == "plot predicate vector")
        return PLOTPREDICATEVEC;
    else if (typeName == "plot zoom")
        return PLOTSELECTIONENTRYVEC;
    else if (typeName == "shader")
        return SHADER;
    else if (typeName == "transfer function")
        return TRANSFUNC;
    else if (typeName == "camera")
        return CAMERA;
    else if (typeName == "volume handle")
        return VOLUMEHANDLE;
    else if (typeName == "volume collection")
        return VOLUMECOLLECTION;
    else
        return NIL;
}

bool BoxObject::getBool() const throw (VoreenException) {
    switch (currentType_) {
    case BOOL:
        return VP(bool);
        break;
    case DOUBLE:
        return static_cast<bool>(VP(double));
        break;
    case INTEGER:
        return static_cast<bool>(VP(int));
        break;
    case FLOAT:
        return static_cast<bool>(VP(float));
        break;
    case STRING: {
        string s = VP(string);
        for (unsigned int i = 0; i < s.length(); ++i)
            s[i] = tolower(s[i]);

        if (s == "1" || s == "true") {
            return true;
        }
        if (s == "0" || s == "false") {
            return false;
        }
        throw VoreenException("String->Bool conversion failed");
        break;
    }
    case LONG:
        return static_cast<bool>(VP(long));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to bool not implemented");
    }
    return true; //never reached...fixes warning
}

ColorMap BoxObject::getColorMap() const throw (VoreenException) {
    switch (currentType_) {
    case COLORMAP:
        return VP(ColorMap);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to colormap not implemented");
    }
}

double BoxObject::getDouble() const throw (VoreenException) {
    switch (currentType_) {
    case BOOL:
        return static_cast<double>(VP(bool));
        break;
    case DOUBLE:
        return VP(double);
        break;
    case FLOAT:
        return static_cast<double>(VP(float));
        break;
    case INTEGER:
        return static_cast<double>(VP(int));
        break;
    case LONG:
        return static_cast<double>(VP(long));
        break;
    case STRING: {
            stringstream s(VP(string));
            double result;
            if ( (s >> result).fail() )
                throw VoreenException("String->Double conversion failed");
            return result;
            break;
        }
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to double not implemented");
    }
    return 0.0; //never reached...fixes warning
}

float BoxObject::getFloat() const throw (VoreenException) {
    switch (currentType_) {
    case BOOL:
        return static_cast<float>(VP(bool));
        break;
    case DOUBLE:
        return static_cast<float>(VP(double));
        break;
    case FLOAT:
        return VP(float);
        break;
    case INTEGER:
        return static_cast<float>(VP(int));
        break;
    case LONG:
        return static_cast<float>(VP(long));
        break;
    case STRING: {
        stringstream s(VP(string));
        float result;
        if ( (s >> result).fail() )
            throw VoreenException("String->Double conversion failed");
        return result;
        break;
     }
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to float not implemented");
    }
    return 0.0f; //never reached...fixes warning
}

int BoxObject::getInt() const throw (VoreenException) {
    switch (currentType_) {
    case BOOL:
        return static_cast<int>(VP(bool));
        break;
    case DOUBLE:
        return static_cast<int>(VP(double));
        break;
    case FLOAT:
        return static_cast<int>(VP(float));
        break;
    case INTEGER:
        return VP(int);
        break;
    case LONG:
        return static_cast<int>(VP(long));
        break;
    case STRING: {
            stringstream s(VP(string));
            int result;
            if ( (s >> result).fail() )
                throw VoreenException("String->Int conversion failed");
            return result;
            break;
        }
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to integer not implemented");
    }
    return 0; //never reached...fixes warning
}

vector<PlotEntitySettings> BoxObject::getPlotEntitySettingsVec() const throw (VoreenException) {
    switch (currentType_) {
    case PLOTENTITYSETTINGSVEC:
        return VP(vector<PlotEntitySettings>);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion not implemented");
    }
}

long BoxObject::getLong() const throw (VoreenException) {
    switch (currentType_) {
    case BOOL:
        return static_cast<long>(VP(bool));
        break;
    case DOUBLE:
        return static_cast<long>(VP(double));
        break;
    case FLOAT:
        return static_cast<long>(VP(float));
        break;
    case INTEGER:
        return static_cast<long>(VP(int));
        break;
    case LONG:
        return VP(long);
        break;
    case STRING: {
            stringstream s(VP(string));
            long result;
            if ( (s >> result).fail() )
                throw VoreenException("String->Int conversion failed");
            return result;
            break;
        }
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to long not implemented");
    }
    return 0; //never reached...fixes warning
}

string BoxObject::getString() const throw (VoreenException) {
    switch (currentType_) {
    case BOOL:
        return VP(bool) ? "true" : "false";
        break;
    case DOUBLE:
        return toString<double>(VP(double));
        break;
    case FLOAT:
        return toString<float>(VP(float));
        break;
    case INTEGER:
        return toString<int>(VP(int));
        break;
    case LONG:
        return toString<long>(VP(long));
        break;
    case STRING:
        return VP(string);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to string not implemented");
    }
}

vector<string> BoxObject::getStringVec() const throw (VoreenException) {
    switch (currentType_) {
    case STRINGVEC:
        return VP(vector<string>);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion not implemented");
    }
}

ivec2 BoxObject::getIVec2() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return VP(ivec2);
        break;
    case IVEC3:
        return VP(ivec3).xy();
        break;
    case IVEC4:
        return VP(ivec4).xy();
        break;
    case VEC2:
        return ivec2(static_cast<int>(VP(vec2).x), static_cast<int>(VP(vec2).y));
        break;
    case VEC3:
        return ivec2(static_cast<int>(VP(vec3).x), static_cast<int>(VP(vec3).y));
        break;
    case VEC4:
        return ivec2(static_cast<int>(VP(vec4).x), static_cast<int>(VP(vec4).y));
        break;
    case DVEC2:
        return ivec2(static_cast<int>(VP(dvec2).x), static_cast<int>(VP(dvec2).y));
        break;
    case DVEC3:
        return ivec2(static_cast<int>(VP(dvec3).x), static_cast<int>(VP(dvec3).y));
        break;
    case DVEC4:
        return ivec2(static_cast<int>(VP(dvec4).x), static_cast<int>(VP(dvec4).y));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to ivec2 not implemented");
    }
}

ivec3 BoxObject::getIVec3() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return ivec3(VP(ivec2).x, VP(ivec2).y, 0);
        break;
    case IVEC3:
        return VP(ivec3);
        break;
    case IVEC4:
        return VP(ivec4).xyz();
        break;
    case VEC2:
        return ivec3(static_cast<int>(VP(vec2).x), static_cast<int>(VP(vec2).y), 0);
        break;
    case VEC3:
        return ivec3(static_cast<int>(VP(vec3).x), static_cast<int>(VP(vec3).y), static_cast<int>(VP(vec3).z));
        break;
    case VEC4:
        return ivec3(static_cast<int>(VP(vec4).x), static_cast<int>(VP(vec4).y), static_cast<int>(VP(vec4).z));
        break;
    case DVEC2:
        return ivec3(static_cast<int>(VP(dvec2).x), static_cast<int>(VP(dvec2).y), 0);
        break;
    case DVEC3:
        return ivec3(static_cast<int>(VP(dvec3).x), static_cast<int>(VP(dvec3).y), static_cast<int>(VP(dvec3).z));
        break;
    case DVEC4:
        return ivec3(static_cast<int>(VP(dvec4).x), static_cast<int>(VP(dvec4).y), static_cast<int>(VP(dvec4).z));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to ivec3 not implemented");
    }
}

ivec4 BoxObject::getIVec4() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return ivec4(VP(ivec2).x, VP(ivec2).y, 0, 0);
        break;
    case IVEC3:
        return ivec4(VP(ivec3).x, VP(ivec3).y, VP(ivec3).z, 0);
        break;
    case IVEC4:
        return VP(ivec4);
        break;
    case VEC2:
        return ivec4(static_cast<int>(VP(vec2).x), static_cast<int>(VP(vec2).y), 0, 0);
        break;
    case VEC3:
        return ivec4(static_cast<int>(VP(vec3).x), static_cast<int>(VP(vec3).y), static_cast<int>(VP(vec3).z), 0);
        break;
    case VEC4:
        return ivec4(static_cast<int>(VP(vec4).x), static_cast<int>(VP(vec4).y), static_cast<int>(VP(vec4).z), static_cast<int>(VP(vec4).w));
        break;
    case DVEC2:
        return ivec4(static_cast<int>(VP(dvec2).x), static_cast<int>(VP(dvec2).y), 0, 0);
        break;
    case DVEC3:
        return ivec4(static_cast<int>(VP(dvec3).x), static_cast<int>(VP(dvec3).y), static_cast<int>(VP(dvec3).z), 0);
        break;
    case DVEC4:
        return ivec4(static_cast<int>(VP(dvec4).x), static_cast<int>(VP(dvec4).y), static_cast<int>(VP(dvec4).z), static_cast<int>(VP(dvec4).w));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to ivec4 not implemented");
    }
}

vec2 BoxObject::getVec2() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return vec2(static_cast<float>(VP(ivec2).x), static_cast<float>(VP(ivec2).y));
        break;
    case IVEC3:
        return vec2(static_cast<float>(VP(ivec3).x), static_cast<float>(VP(ivec3).y));
        break;
    case IVEC4:
        return vec2(static_cast<float>(VP(ivec4).x), static_cast<float>(VP(ivec4).y));
        break;
    case VEC2:
        return VP(vec2);
        break;
    case VEC3:
        return VP(vec3).xy();
        break;
    case VEC4:
        return VP(vec4).xy();
        break;
    case DVEC2:
        return vec2(static_cast<float>(VP(dvec2).x), static_cast<float>(VP(dvec2).y));
        break;
    case DVEC3:
        return vec2(static_cast<float>(VP(dvec3).x), static_cast<float>(VP(dvec3).y));
        break;
    case DVEC4:
        return vec2(static_cast<float>(VP(dvec4).x), static_cast<float>(VP(dvec4).y));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to vec2 not implemented");
    }
}

vec3 BoxObject::getVec3() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return vec3(static_cast<float>(VP(ivec2).x), static_cast<float>(VP(ivec2).y), 0);
        break;
    case IVEC3:
        return vec3(static_cast<float>(VP(ivec3).x), static_cast<float>(VP(ivec3).y), static_cast<float>(VP(ivec3).z));
        break;
    case IVEC4:
        return vec3(static_cast<float>(VP(ivec4).x), static_cast<float>(VP(ivec4).y), static_cast<float>(VP(ivec4).z));
        break;
    case VEC2:
        return vec3(VP(vec2).x, VP(vec2).y, 0);
        break;
    case VEC3:
        return VP(vec3);
        break;
    case VEC4:
        return VP(vec4).xyz();
        break;
    case DVEC2:
        return vec3(static_cast<float>(VP(dvec2).x), static_cast<float>(VP(dvec2).y), 0);
        break;
    case DVEC3:
        return vec3(static_cast<float>(VP(dvec3).x), static_cast<float>(VP(dvec3).y), static_cast<float>(VP(dvec3).z));
        break;
    case DVEC4:
        return vec3(static_cast<float>(VP(dvec4).x), static_cast<float>(VP(dvec4).y), static_cast<float>(VP(dvec4).z));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to vec3 not implemented");
    }
}

vec4 BoxObject::getVec4() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return vec4(static_cast<float>(VP(ivec2).x), static_cast<float>(VP(ivec2).y), 0, 0);
        break;
    case IVEC3:
        return vec4(static_cast<float>(VP(ivec3).x), static_cast<float>(VP(ivec3).y), static_cast<float>(VP(ivec3).z), 0);
        break;
    case IVEC4:
        return vec4(static_cast<float>(VP(ivec4).x), static_cast<float>(VP(ivec4).y), static_cast<float>(VP(ivec4).z), static_cast<float>(VP(ivec4).w));
        break;
    case VEC2:
        return vec4(VP(vec2).x, VP(vec2).y, 0, 0);
        break;
    case VEC3:
        return vec4(VP(vec3).x, VP(vec3).y, VP(vec3).z, 0);
        break;
    case VEC4:
        return VP(vec4);
        break;
    case DVEC2:
        return vec4(static_cast<float>(VP(dvec2).x), static_cast<float>(VP(dvec2).y), 0, 0);
        break;
    case DVEC3:
        return vec4(static_cast<float>(VP(dvec3).x), static_cast<float>(VP(dvec3).y), static_cast<float>(VP(dvec3).z), 0);
        break;
    case DVEC4:
        return vec4(static_cast<float>(VP(dvec4).x), static_cast<float>(VP(vec4).y), static_cast<float>(VP(dvec4).z), static_cast<float>(VP(dvec4).w));
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to vec4 not implemented");
    }
}

dvec2 BoxObject::getDVec2() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return dvec2(static_cast<double>(VP(ivec2).x), static_cast<double>(VP(ivec2).y));
        break;
    case IVEC3:
        return dvec2(static_cast<double>(VP(ivec3).x), static_cast<double>(VP(ivec3).y));
        break;
    case IVEC4:
        return dvec2(static_cast<double>(VP(ivec4).x), static_cast<double>(VP(ivec4).y));
        break;
    case VEC2:
        return dvec2(static_cast<double>(VP(vec2).x), static_cast<double>(VP(vec2).y));
        break;
    case VEC3:
        return dvec2(static_cast<double>(VP(vec3).x), static_cast<double>(VP(vec3).y));
        break;
    case VEC4:
        return dvec2(static_cast<double>(VP(vec4).x), static_cast<double>(VP(vec4).y));
        break;
    case DVEC2:
        return VP(dvec2);
        break;
    case DVEC3:
        return VP(dvec3).xy();
        break;
    case DVEC4:
        return VP(dvec4).xy();
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to dvec2 not implemented");
    }
}

dvec3 BoxObject::getDVec3() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return dvec3(static_cast<double>(VP(ivec2).x), static_cast<double>(VP(ivec2).y), 0);
        break;
    case IVEC3:
        return dvec3(static_cast<double>(VP(ivec3).x), static_cast<double>(VP(ivec3).y), static_cast<double>(VP(ivec3).z));
        break;
    case IVEC4:
        return dvec3(static_cast<double>(VP(ivec4).x), static_cast<double>(VP(ivec4).y), static_cast<double>(VP(ivec4).z));
        break;
    case VEC2:
        return dvec3(static_cast<double>(VP(vec2).x), static_cast<double>(VP(vec2).y), 0);
        break;
    case VEC3:
        return dvec3(static_cast<double>(VP(vec3).x), static_cast<double>(VP(vec3).y), static_cast<double>(VP(vec3).z));
        break;
    case VEC4:
        return dvec3(static_cast<double>(VP(vec4).x), static_cast<double>(VP(vec4).y), static_cast<double>(VP(vec4).z));
        break;
    case DVEC2:
        return dvec3(VP(dvec2).x, VP(dvec2).y, 0);
        break;
    case DVEC3:
        return VP(dvec3);
        break;
    case DVEC4:
        return VP(dvec4).xyz();
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to dvec3 not implemented");
    }
}

dvec4 BoxObject::getDVec4() const throw (VoreenException) {
    switch (currentType_) {
    case IVEC2:
        return dvec4(static_cast<double>(VP(ivec2).x), static_cast<double>(VP(ivec2).y), 0, 0);
        break;
    case IVEC3:
        return dvec4(static_cast<double>(VP(ivec3).x), static_cast<double>(VP(ivec3).y), static_cast<double>(VP(ivec3).z), 0);
        break;
    case IVEC4:
        return dvec4(static_cast<double>(VP(ivec4).x), static_cast<double>(VP(ivec4).y), static_cast<double>(VP(ivec4).z), static_cast<double>(VP(ivec4).w));
        break;
    case VEC2:
        return dvec4(static_cast<double>(VP(vec2).x), static_cast<double>(VP(vec2).y), 0, 0);
        break;
    case VEC3:
        return dvec4(static_cast<double>(VP(vec3).x), static_cast<double>(VP(vec3).y), static_cast<double>(VP(vec3).z), 0);
        break;
    case VEC4:
        return dvec4(static_cast<double>(VP(vec4).x), static_cast<double>(VP(vec4).y), static_cast<double>(VP(vec4).z), static_cast<double>(VP(vec4).w));
        break;
    case DVEC2:
        return dvec4(VP(dvec2).x, VP(dvec2).y, 0, 0);
        break;
    case DVEC3:
        return dvec4(VP(dvec3).x, VP(dvec3).y, VP(dvec3).z, 0);
        break;
    case DVEC4:
        return VP(dvec4);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to dvec4 not implemented");
    }
}

mat2 BoxObject::getMat2() const throw (VoreenException) {
    switch (currentType_) {
    case MAT2:
        return VP(mat2);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to mat2 not implemented");
    }
}

mat3 BoxObject::getMat3() const throw (VoreenException) {
    switch (currentType_) {
    case MAT3:
        return VP(mat3);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to mat3 not implemented");
    }
}

mat4 BoxObject::getMat4() const throw (VoreenException) {
    switch (currentType_) {
    case MAT4:
        return VP(mat4);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to mat4 not implemented");
    }
}

vector<pair<int, PlotPredicate*> > BoxObject::getPlotPredicateVec() const throw (VoreenException) {
    switch (currentType_) {
        case PLOTPREDICATEVEC:
            // don't use VP macro here because of comma in template arguments
            return (*(vector<pair<int, PlotPredicate*> >*)value_);
            break;
        case NIL:
            throw VoreenException("Conversion tried on empty boxobject");
        default:
            throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to PlotPredicateVector not implemented");
    }
}

vector< PlotSelectionEntry > BoxObject::getPlotSelectionEntryVec() const throw (VoreenException) {
    switch (currentType_) {
        case PLOTSELECTIONENTRYVEC:
            // don't use VP macro here because of comma in template arguments
            return VP(vector< PlotSelectionEntry >);
            break;
        case NIL:
            throw VoreenException("Conversion tried on empty boxobject");
        default:
            throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to PlotZoom not implemented");
    }
}

const TransFunc* BoxObject::getTransFunc() const throw (VoreenException) {
    switch (currentType_) {
    case TRANSFUNC:
        return &VP(TransFunc);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to transfer function not implemented");
    }
}

const Camera* BoxObject::getCamera() const throw (VoreenException) {
    switch (currentType_) {
    case CAMERA:
        return &VP(Camera);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to camera not implemented");
    }
}

const VolumeHandle* BoxObject::getVolumeHandle() const throw (VoreenException) {
    switch (currentType_) {
    case VOLUMEHANDLE:
        return &VP(VolumeHandle);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to volume handle not implemented");
    }
}

const VolumeCollection* BoxObject::getVolumeCollection() const throw (VoreenException) {
    switch (currentType_) {
    case VOLUMECOLLECTION:
        return &VP(VolumeCollection);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to volume collection not implemented");
    }
}

ShaderSource BoxObject::getShader() const throw (VoreenException) {
    switch (currentType_) {
    case SHADER:
        return VP(ShaderSource);
        break;
    case NIL:
        throw VoreenException("Conversion tried on empty boxobject");
    default:
        throw VoreenException("BoxObject: Conversion from " + getTypeName(currentType_) + " to shader not implemented");
    }
}

void BoxObject::setBool(const bool& value) {
    set<bool>(value, BOOL);
}

void BoxObject::setColorMap(const ColorMap& value) {
    set<ColorMap>(value, COLORMAP);
}

void BoxObject::setDouble(const double& value) {
    set<double>(value, DOUBLE);
}

void BoxObject::setFloat(const float& value) {
    set<float>(value, FLOAT);
}

void BoxObject::setInt(const int& value) {
    set<int>(value, INTEGER);
}

void BoxObject::setLong(const long& value) {
    set<long>(value, LONG);
}

void BoxObject::setString(const std::string& value) {
    set<std::string>(value, STRING);
}

void BoxObject::setStringVec(const std::vector<std::string>& value) {
    set<std::vector<std::string> >(value, STRINGVEC);
}

void BoxObject::setPlotEntitySettingsVec(const std::vector<PlotEntitySettings>& value) {
    set<std::vector<PlotEntitySettings> >(value, PLOTENTITYSETTINGSVEC);
}

void BoxObject::setIVec2(const tgt::ivec2& value) {
    set<tgt::ivec2>(value, IVEC2);
}

void BoxObject::setIVec3(const tgt::ivec3& value) {
    set<tgt::ivec3>(value, IVEC3);
}

void BoxObject::setIVec4(const tgt::ivec4& value) {
    set<tgt::ivec4>(value, IVEC4);
}

void BoxObject::setVec2(const tgt::vec2& value) {
    set<tgt::vec2>(value, VEC2);
}

void BoxObject::setVec3(const tgt::vec3& value) {
    set<tgt::vec3>(value, VEC3);
}

void BoxObject::setVec4(const tgt::vec4& value) {
    set<tgt::vec4>(value, VEC4);
}

void BoxObject::setDVec2(const tgt::dvec2& value) {
    set<tgt::dvec2>(value, DVEC2);
}

void BoxObject::setDVec3(const tgt::dvec3& value) {
    set<tgt::dvec3>(value, DVEC3);
}

void BoxObject::setDVec4(const tgt::dvec4& value) {
    set<tgt::dvec4>(value, DVEC4);
}

void BoxObject::setMat2(const tgt::mat2& value) {
    set<tgt::mat2>(value, MAT2);
}

void BoxObject::setMat3(const tgt::mat3& value) {
    set<tgt::mat3>(value, MAT3);
}

void BoxObject::setMat4(const tgt::mat4& value) {
    set<tgt::mat4>(value, MAT4);
}

void BoxObject::setPlotPredicateVec(const vector<pair<int, PlotPredicate*> >& value) {
    set<vector<pair<int, PlotPredicate*> > >(value, PLOTPREDICATEVEC);
}

void BoxObject::setPlotSelectionEntryVec(const vector< PlotSelectionEntry >& value) {
    set<vector< PlotSelectionEntry > >(value, PLOTSELECTIONENTRYVEC);
}

void BoxObject::setShader(const ShaderSource* value) {
    set(*value, SHADER);
}

void BoxObject::setTransFunc(const TransFunc* value) {
    set(*value, TRANSFUNC);
}

void BoxObject::setCamera(const tgt::Camera* value) {
    set(*value, CAMERA);
}

void BoxObject::setVolumeHandle(const VolumeHandle* value) {
    set(*value, VOLUMEHANDLE);
}

void BoxObject::setVolumeCollection(const VolumeCollection* value) {
    set(*value, VOLUMECOLLECTION);
}

void BoxObject::serialize(XmlSerializer& s) const {
    s.serialize("BoxObjectType", getTypeName(currentType_));

    switch (currentType_) {
    case BOOL:
        s.serialize("value", getBool());
        break;
    case COLORMAP:
        s.serialize("value", getColorMap());
        break;
    case DOUBLE:
        s.serialize("value", getDouble());
        break;
    case FLOAT:
        s.serialize("value", getFloat());
        break;
    case INTEGER:
        s.serialize("value", getInt());
        break;
    case LONG:
        s.serialize("value", getLong());
        break;
    case STRING:
        s.serialize("value", getString());
        break;
    case STRINGVEC:
        s.serialize("value", getStringVec());
        break;
    case PLOTENTITYSETTINGSVEC:
        s.serialize("value", getPlotEntitySettingsVec());
        break;
    case IVEC2:
        s.serialize("value", getIVec2());
        break;
    case IVEC3:
        s.serialize("value", getIVec3());
        break;
    case IVEC4:
        s.serialize("value", getIVec4());
        break;
    case VEC2:
        s.serialize("value", getVec2());
        break;
    case VEC3:
        s.serialize("value", getVec3());
        break;
    case VEC4:
        s.serialize("value", getVec4());
        break;
    case DVEC2:
        s.serialize("value", getDVec2());
        break;
    case DVEC3:
        s.serialize("value", getDVec3());
        break;
    case DVEC4:
        s.serialize("value", getDVec4());
        break;
    case MAT2:
        s.serialize("value", getMat2());
        break;
    case MAT3:
        s.serialize("value", getMat3());
        break;
    case MAT4:
        s.serialize("value", getMat4());
        break;
    case PLOTPREDICATEVEC:
        //s.serialize("value", getPlotPredicateVector());
        break;
    case PLOTSELECTIONENTRYVEC:
        s.serialize("value", getPlotSelectionEntryVec());
        break;
    case SHADER:
        s.serialize("value", getShader());
        break;
    case TRANSFUNC:
        s.serialize("value", getTransFunc());
        break;
    case CAMERA:
        // TODO make Camera serializable
        //s.serialize("value", getCamera());
        break;
    case VOLUMEHANDLE:
        s.serialize("value", getVolumeHandle());
        break;
    case VOLUMECOLLECTION:
        s.serialize("value", getVolumeCollection());
        break;
    default:
        tgtAssert(false, "shouldn't get here");
    }
}

void BoxObject::deserialize(XmlDeserializer& d) {
    std::string typeString;
    d.deserialize("BoxObjectType", typeString);
    currentType_ = getType(typeString);

    switch (currentType_) {
    case BOOL:
        {
            bool value;
            d.deserialize("value", value);
            setBool(value);
            break;
        }
    case COLORMAP:
        {
            ColorMap value;
            d.deserialize("value", value);
            setColorMap(value);
            break;
        }
    case DOUBLE:
        {
            double value;
            d.deserialize("value", value);
            setDouble(value);
            break;
        }
    case FLOAT:
        {
            float value;
            d.deserialize("value", value);
            setFloat(value);
            break;
        }
    case INTEGER:
        {
            int value;
            d.deserialize("value", value);
            setInt(value);
            break;
        }
    case LONG:
        {
            long value;
            d.deserialize("value", value);
            setLong(value);
            break;
        }
    case STRING:
        {
            std::string value;
            d.deserialize("value", value);
            setString(value);
            break;
        }
    case STRINGVEC:
        {
            std::vector<std::string> value;
            d.deserialize("value", value);
            setStringVec(value);
            break;
        }
    case PLOTENTITYSETTINGSVEC:
        {
            std::vector<PlotEntitySettings> value;
            d.deserialize("value", value);
            setPlotEntitySettingsVec(value);
            break;
        }
    case IVEC2:
        {
            ivec2 value;
            d.deserialize("value", value);
            setIVec2(value);
            break;
        }
    case IVEC3:
        {
            ivec3 value;
            d.deserialize("value", value);
            setIVec3(value);
            break;
        }
    case IVEC4:
        {
            ivec4 value;
            d.deserialize("value", value);
            setIVec4(value);
            break;
        }
    case VEC2:
        {
            vec2 value;
            d.deserialize("value", value);
            setVec2(value);
            break;
        }
    case VEC3:
        {
            vec3 value;
            d.deserialize("value", value);
            setVec3(value);
            break;
        }
    case VEC4:
        {
            vec4 value;
            d.deserialize("value", value);
            setVec4(value);
            break;
        }
    case DVEC2:
        {
            dvec2 value;
            d.deserialize("value", value);
            setDVec2(value);
            break;
        }
    case DVEC3:
        {
            dvec3 value;
            d.deserialize("value", value);
            setDVec3(value);
            break;
        }
    case DVEC4:
        {
            dvec4 value;
            d.deserialize("value", value);
            setDVec4(value);
            break;
        }
    case MAT2:
        {
            mat2 value;
            d.deserialize("value", value);
            setMat2(value);
            break;
        }
    case MAT3:
        {
            mat3 value;
            d.deserialize("value", value);
            setMat3(value);
            break;
        }
    case MAT4:
        {
            mat4 value;
            d.deserialize("value", value);
            setMat4(value);
            break;
        }
    case PLOTPREDICATEVEC:
        {
            tgtAssert(false, "tried to deserialize PlotPredicateVector, but it isn't serializable");
            vector<pair<int, PlotPredicate*> > value;
            //d.deserialize("value", value);
            setPlotPredicateVec(value);
            break;
        }
    case PLOTSELECTIONENTRYVEC:
        {
            vector< PlotSelectionEntry > value;
            d.deserialize("value", value);
            setPlotSelectionEntryVec(value);
            break;
        }
    case SHADER:
        {
            ShaderSource* value = new ShaderSource;
            d.deserialize("value", *value);
            setShader(value);
            break;
        }
    case TRANSFUNC:
        {
            TransFuncIntensity* value = new TransFuncIntensity;
            d.deserialize("value", *value);
            setTransFunc(value);
            break;
        }
    case CAMERA:
        {
            Camera* value = new Camera;
            //d.deserialize("value", value);
            setCamera(value);
            break;
        }
    case VOLUMEHANDLE:
        {
            VolumeHandle* value = new VolumeHandle;
            d.deserialize("value", value);
            setVolumeHandle(value);
            break;
        }
    case VOLUMECOLLECTION:
        {
            VolumeCollection* value = new VolumeCollection;
            d.deserialize("value", *value);
            setVolumeCollection(value);
            break;
        }
    default:
        tgtAssert(false, "shouldn't get here");
    }
}

BoxObject& BoxObject::operator= (const BoxObject& rhs) {
    if (this != &rhs) {
        switch (rhs.getType()) {
        case NIL:
            break;
        case BOOL:
            set<bool>(rhs.getBool(), BOOL);
            break;
        case DOUBLE:
            set<double>(rhs.getDouble(), DOUBLE);
            break;
        case FLOAT:
            set<float>(rhs.getFloat(), FLOAT);
            break;
        case INTEGER:
            set<int>(rhs.getInt(), INTEGER);
            break;
        case PLOTENTITYSETTINGSVEC:
            set<vector<PlotEntitySettings> >(rhs.getPlotEntitySettingsVec(), PLOTENTITYSETTINGSVEC);
            break;
        case LONG:
            set<long>(rhs.getLong(), LONG);
            break;
        case STRING:
            set<string>(rhs.getString(), STRING);
            break;
        case STRINGVEC:
            set<vector<string> >(rhs.getStringVec(), STRINGVEC);
            break;
        case IVEC2:
            set<ivec2>(rhs.getIVec2(), IVEC2);
            break;
        case IVEC3:
            set<ivec3>(rhs.getIVec3(), IVEC3);
            break;
        case IVEC4:
            set<ivec4>(rhs.getIVec4(), IVEC4);
            break;
        case VEC2:
            set<vec2>(rhs.getVec2(), VEC2);
            break;
        case VEC3:
            set<vec3>(rhs.getVec3(), VEC3);
            break;
        case VEC4:
            set<vec4>(rhs.getVec4(), VEC4);
            break;
        case DVEC2:
            set<dvec2>(rhs.getDVec2(), DVEC2);
            break;
        case DVEC3:
            set<dvec3>(rhs.getDVec3(), DVEC3);
            break;
        case DVEC4:
            set<dvec4>(rhs.getDVec4(), DVEC4);
            break;
        case MAT2:
            set<mat2>(rhs.getMat2(), MAT2);
            break;
        case MAT3:
            set<mat3>(rhs.getMat3(), MAT3);
            break;
        case MAT4:
            set<mat4>(rhs.getMat4(), MAT4);
            break;
        case PLOTPREDICATEVEC:
            set<vector<pair<int, PlotPredicate*> > >(rhs.getPlotPredicateVec(), PLOTPREDICATEVEC);
            break;
        case PLOTSELECTIONENTRYVEC:
            set<vector< PlotSelectionEntry > >(rhs.getPlotSelectionEntryVec(), PLOTSELECTIONENTRYVEC);
            break;
        case TRANSFUNC:
            set(*rhs.getTransFunc(), TRANSFUNC);
            break;
        case CAMERA:
            set(*rhs.getCamera(), CAMERA);
            break;
        case VOLUMEHANDLE:
            set(*rhs.getVolumeHandle(), VOLUMEHANDLE);
            break;
        case VOLUMECOLLECTION:
            set(*rhs.getVolumeCollection(), VOLUMECOLLECTION);
            break;
        case SHADER:
            set<ShaderSource>(rhs.getShader(), SHADER);
            break;
        default:
            throw VoreenException("BoxObject: Conversion not implemented");
        }
    }
    return *this;
}

BoxObject& BoxObject::operator= (const bool& rhs) {
    set<bool>(rhs, BOOL);
    return *this;
}

BoxObject& BoxObject::operator= (const ColorMap& rhs) {
    set<ColorMap>(rhs, COLORMAP);
    return *this;
}

BoxObject& BoxObject::operator= (const double& rhs) {
    set<double>(rhs, DOUBLE);
    return *this;
}

BoxObject& BoxObject::operator= (const float& rhs) {
    set<float>(rhs, FLOAT);
    return *this;
}

BoxObject& BoxObject::operator= (const int& rhs) {
    set<int>(rhs, INTEGER);
    return *this;
}

BoxObject& BoxObject::operator= (const vector<PlotEntitySettings>& rhs) {
    set<vector<PlotEntitySettings> >(rhs, PLOTENTITYSETTINGSVEC);
    return *this;
}

BoxObject& BoxObject::operator= (const long& rhs) {
    set<long>(rhs, LONG);
    return *this;
}

BoxObject& BoxObject::operator= (const string& rhs) {
    set<string>(rhs, STRING);
    return *this;
}

BoxObject& BoxObject::operator= (const vector<string>& rhs) {
    set<vector<string> >(rhs, STRINGVEC);
    return *this;
}

BoxObject& BoxObject::operator= (const ivec2& rhs) {
    set<ivec2>(rhs, IVEC2);
    return *this;
}

BoxObject& BoxObject::operator= (const ivec3& rhs) {
    set<ivec3>(rhs, IVEC3);
    return *this;
}

BoxObject& BoxObject::operator= (const ivec4& rhs) {
    set<ivec4>(rhs, IVEC4);
    return *this;
}

BoxObject& BoxObject::operator= (const vec2& rhs) {
    set<vec2>(rhs, VEC2);
    return *this;
}

BoxObject& BoxObject::operator= (const vec3& rhs) {
    set<vec3>(rhs, VEC3);
    return *this;
}

BoxObject& BoxObject::operator= (const vec4& rhs) {
    set<vec4>(rhs, VEC4);
    return *this;
}

BoxObject& BoxObject::operator= (const mat2& rhs) {
    set<mat2>(rhs, MAT2);
    return *this;
}

BoxObject& BoxObject::operator= (const mat3& rhs) {
    set<mat3>(rhs, MAT3);
    return *this;
}

BoxObject& BoxObject::operator= (const mat4& rhs) {
    set<mat4>(rhs, MAT4);
    return *this;
}

BoxObject& BoxObject::operator= (const vector<pair<int, PlotPredicate*> >& rhs) {
    set<vector<pair<int, PlotPredicate*> > >(rhs, PLOTPREDICATEVEC);
    return *this;
}

BoxObject& BoxObject::operator= (const vector< PlotSelectionEntry >& rhs) {
    set<vector< PlotSelectionEntry > >(rhs, PLOTSELECTIONENTRYVEC);
    return *this;
}

BoxObject& BoxObject::operator= (const TransFunc* rhs) {
    set(*rhs, TRANSFUNC);
    return *this;
}

BoxObject& BoxObject::operator= (const Camera* rhs) {
    set(*rhs, CAMERA);
    return *this;
}

BoxObject& BoxObject::operator= (const VolumeHandle* rhs) {
    set(*rhs, VOLUMEHANDLE);
    return *this;
}

BoxObject& BoxObject::operator= (const VolumeCollection* rhs) {
    set(*rhs, VOLUMECOLLECTION);
    return *this;
}

BoxObject& BoxObject::operator= (const ShaderSource& rhs) {
    set<ShaderSource>(rhs, SHADER);
    return *this;
}

bool BoxObject::operator== (const BoxObject& rhs) const {
    if (getType() != rhs.getType())
        return false;
    else {
        switch (currentType_) {
        case NIL:
            return false;
        case BOOL:
            return (getBool() == rhs.getBool());
        case COLORMAP:
            return (getColorMap() == rhs.getColorMap());
        case DOUBLE:
            return (getDouble() == rhs.getDouble());
        case FLOAT:
            return (getFloat() == rhs.getFloat());
        case INTEGER:
            return (getInt() == rhs.getInt());
        case LONG:
            return (getLong() == rhs.getLong());
        case STRING:
            return (getString() == rhs.getString());
        case STRINGVEC:
            return (getStringVec() == rhs.getStringVec());
        case IVEC2:
            return (getIVec2() == rhs.getIVec2());
        case IVEC3:
            return (getIVec3() == rhs.getIVec3());
        case IVEC4:
            return (getIVec4() == rhs.getIVec4());
        case VEC2:
            return (getVec2() == rhs.getVec2());
        case VEC3:
            return (getVec3() == rhs.getVec3());
        case VEC4:
            return (getVec4() == rhs.getVec4());
        case DVEC2:
            return (getDVec2() == rhs.getDVec2());
        case DVEC3:
            return (getDVec3() == rhs.getDVec3());
        case DVEC4:
            return (getDVec4() == rhs.getDVec4());
        case MAT2:
            return (getMat2() == rhs.getMat2());
        case MAT3:
            return (getMat3() == rhs.getMat3());
        case MAT4:
            return (getMat4() == rhs.getMat4());
        case PLOTENTITYSETTINGSVEC:
            return (getPlotEntitySettingsVec() == rhs.getPlotEntitySettingsVec());
        case PLOTPREDICATEVEC:
            break; // omit warning
        case PLOTSELECTIONENTRYVEC:
            return (getPlotSelectionEntryVec() == rhs.getPlotSelectionEntryVec());
        case TRANSFUNC:
            {
                const TransFuncIntensity* lhsCast = dynamic_cast<const TransFuncIntensity*>(getTransFunc());
                const TransFuncIntensity* rhsCast = dynamic_cast<const TransFuncIntensity*>(rhs.getTransFunc());

                if (lhsCast && rhsCast)
                    return lhsCast == rhsCast;
                else
                    return false;
            }
            //return (getTransFunc() == rhs.getTransFunc());
        case CAMERA:
            return (getCamera() == rhs.getCamera());
        case VOLUMEHANDLE:
            return (getVolumeHandle() == rhs.getVolumeHandle());
        case VOLUMECOLLECTION:
            return (getVolumeCollection() == rhs.getVolumeCollection());
        case SHADER:
            return (getShader() == rhs.getShader());
        default:
            return false;
        }
    }
    return false;
}

bool BoxObject::operator!= (const BoxObject& rhs) const {
    return !(*this == rhs);
}

} // namespace
