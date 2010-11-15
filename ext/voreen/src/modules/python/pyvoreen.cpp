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

// Ignore "dereferencing type-punned pointer will break strict-aliasing rules" warnings on gcc
// caused by Py_RETURN_TRUE and such. The problem lies in Python so we don't want the warning
// here.
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

// Do this at very first
#include "voreen/modules/python/pythonmodule.h"

#include "voreen/modules/python/pyvoreen.h"

#include "voreen/core/voreenapplication.h"
#include "voreen/core/properties/allproperties.h"
#include "voreen/core/datastructures/transfunc/transfunc.h"
#include "voreen/core/network/processornetwork.h"

#include "voreen/core/processors/processor.h"
#include "voreen/core/network/networkevaluator.h"
#include "voreen/core/processors/canvasrenderer.h"
#include "voreen/core/processors/processorwidget.h"
#ifdef VRN_MODULE_BASE
#include "voreen/modules/base/processors/datasource/volumesource.h"
#include "voreen/modules/base/processors/entryexitpoints/entryexitpoints.h"
#include "voreen/modules/base/processors/entryexitpoints/meshentryexitpoints.h"
#include "voreen/modules/base/processors/utility/clockprocessor.h"
#endif
#include "voreen/core/interaction/voreentrackball.h"


//-------------------------------------------------------------------------------------------------
// internal helper functions

namespace {

/**
 * Retrieves the current processor network.
 *
 * @param functionName name of the calling function, e.g. "setFloatProperty" (included in Python exception)
 */
voreen::ProcessorNetwork* getProcessorNetwork(const std::string& functionName);

/**
 * Retrieves the processor with the specified name from the network.
 */
voreen::Processor* getProcessor(const std::string& processorName, const std::string& functionName);

/**
 * Retrieves the processor with the specified name, if it matches the template parameter.
 */
template<typename T>
T* getTypedProcessor(const std::string& processorName, const std::string& processorTypeString,
                     const std::string& functionName);

/**
 * Retrieves a property with the specified ID of a certain processor.
 */
voreen::Property* getProperty(const std::string& processorName, const std::string& propertyID,
                              const std::string& functionName);

/**
 * Retrieves the property with the specified ID, if it matches the template parameter.
 */
template<typename T>
T* getTypedProperty(const std::string& processorName, const std::string& propertyID,
                    const std::string& propertyTypeString, const std::string& functionName);

/**
 * Assigns the passed value to the TemplateProperty with the specified ID
 * that is owned by the processor with the specified name.
 *
 * @tparam PropertyType the type of the property the value is to be assigned to
 * @tparam ValueType the type of the value to assign
 *
 * @param processorName the name of the processor that owns the property
 * @param propertyID the id of the property to modify
 * @param value the value to set
 * @param propertyTypeString the value's typename, e.g. "Float" (included in Python exception)
 * @param functionName name of the calling function, e.g. "setFloatProperty" (included in Python exception)
 *
 * Failure cases:
 *  - if processor or property do not exist, a PyExc_NameError is raised
 *  - if property type does not match, a PyExc_TypeError is raised
 *  - if property validation fails, a PyExc_ValueError with the corresponding validation message is raised
 *
 * @return true if property manipulation has been successful
 */
template<typename PropertyType, typename ValueType>
bool setPropertyValue(const std::string& processorName, const std::string& propertyID, const ValueType& value,
                      const std::string& propertyTypeString, const std::string& functionName);

/**
 * Assigns the passed value to the passed TemplateProperty.
 *
 * @tparam PropertyType the type of the property the value is to be assigned to
 * @tparam ValueType the type of the value to assign
 *
 * @param property the property to manipulate
 * @param value the value to set
 * @param functionName name of the calling function, e.g. "setFloatProperty" (included in Python exception)
 *
 * Failure cases:
 *  - if property validation fails, a PyExc_ValueError with the corresponding validation message is raised
 *
 * @return true if property manipulation has been successful
 */
template<typename PropertyType, typename ValueType>
bool setPropertyValue(PropertyType* property, const ValueType& value,
                      const std::string& functionName);

/**
 * Uses the apihelper.py script to print documentation
 * about the module's functions.
 */
static PyObject* printModuleInfo(const std::string& moduleName, bool omitFunctionName = false,
                                 int spacing = 0, bool collapse = false, bool blanklines = false);

// type conversion macros
#define VecToPyVec(VARIABLE, CONVERTER, GETVEC, SIZE, VALUEPOSTFIX) \
    VARIABLE = PyList_New(SIZE); \
    for(unsigned int i=0; i<SIZE; i++){ \
    PyList_SetItem(VARIABLE, i, CONVERTER(GETVEC[i]VALUEPOSTFIX)); \
    }

#define PyVecToVec(PYVEC, CONVERTER, VEC) \
{ \
    PyObject* pyList = PYVEC; \
    for(int i=0; i < PyList_Size(pyList); i++) { \
    VEC[i] = (CONVERTER(PyList_GetItem(pyList, i))); \
    } \
}

} // namespace anonymous


//-------------------------------------------------------------------------------------------------
// definitions of Python binding methods

using namespace voreen;

//
// Python module 'voreen.network'
//
static PyObject* voreen_setPropertyValue(PyObject* /*self*/, PyObject* args) {

    // check length of tuple
    if (PyTuple_Size(args) != 3) {
        std::ostringstream errStr;
        errStr << "setPropertyValue() takes exactly 3 arguments: processor name, property id, value";
        errStr << " (" << PyTuple_Size(args) << " given)";
        PyErr_SetString(PyExc_TypeError, errStr.str().c_str());
        return 0;
    }

    // check parameter 1 and 2, if they are strings
    if (!PyString_Check(PyTuple_GetItem(args, 0)) || !PyString_Check(PyTuple_GetItem(args, 1))) {
        PyErr_SetString(PyExc_TypeError, "setPropertyValue() arguments 1 and 2 must be strings");
        return 0;
    }

    // read processor name and property id
    char* processorName = PyString_AsString(PyTuple_GetItem(args, 0));
    char* propertyID = PyString_AsString(PyTuple_GetItem(args, 1));
    if (!processorName || !propertyID) {
        PyErr_SetString(PyExc_TypeError, "setPropertyValue() arguments 1 and 2 must be strings");
        return 0;
    }

    // fetch property
    Property* property = getProperty(std::string(processorName), std::string(propertyID), "setPropertyValue");
    if (!property)
        return 0;

    // determine property type, convert and assign value
    if (IntProperty* typedProp = dynamic_cast<IntProperty*>(property)) {
        int value;
        if (!PyArg_ParseTuple(args, "ssi:setPropertyValue", &processorName, &propertyID, &value))
            return 0;
        if (setPropertyValue<IntProperty, int>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatProperty* typedProp = dynamic_cast<FloatProperty*>(property)) {
        float value;
        if (!PyArg_ParseTuple(args, "ssf:setPropertyValue", &processorName, &propertyID, &value))
            return 0;
        if (setPropertyValue<FloatProperty, float>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (BoolProperty* typedProp = dynamic_cast<BoolProperty*>(property)) {
        char value;
        if (!PyArg_ParseTuple(args, "ssb:setPropertyValue", &processorName, &propertyID, &value))
            return 0;
        if (setPropertyValue<BoolProperty, bool>(typedProp, (bool)value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (StringProperty* typedProp = dynamic_cast<StringProperty*>(property)) {
        char* value;
        if (!PyArg_ParseTuple(args, "sss:setPropertyValue", &processorName, &propertyID, &value))
            return 0;
        if (setPropertyValue<StringProperty, std::string>(typedProp, std::string(value), "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (OptionPropertyBase* typedProp = dynamic_cast<OptionPropertyBase*>(property)) {
        char* value;
        if (!PyArg_ParseTuple(args, "sss:setPropertyValue", &processorName, &propertyID, &value))
            return 0;
        if (setPropertyValue<OptionPropertyBase, std::string>(typedProp, std::string(value), "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (IntVec2Property* typedProp = dynamic_cast<IntVec2Property*>(property)) {
        tgt::ivec2 value;
        if (!PyArg_ParseTuple(args, "ss(ii):setPropertyValue", &processorName, &propertyID,
                &value.x, &value.y))
            return 0;
        if (setPropertyValue<IntVec2Property, tgt::ivec2>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (IntVec3Property* typedProp = dynamic_cast<IntVec3Property*>(property)) {
        tgt::ivec3 value;
        if (!PyArg_ParseTuple(args, "ss(iii):setPropertyValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z))
            return 0;
        if (setPropertyValue<IntVec3Property, tgt::ivec3>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (IntVec4Property* typedProp = dynamic_cast<IntVec4Property*>(property)) {
        tgt::ivec4 value;
        if (!PyArg_ParseTuple(args, "ss(iiii):setPropertyValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z, &value.w))
            return 0;
        if (setPropertyValue<IntVec4Property, tgt::ivec4>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatVec2Property* typedProp = dynamic_cast<FloatVec2Property*>(property)) {
        tgt::vec2 value;
        if (!PyArg_ParseTuple(args, "ss(ff):setPropertyValue", &processorName, &propertyID,
                &value.x, &value.y))
            return 0;
        if (setPropertyValue<FloatVec2Property, tgt::vec2>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatVec3Property* typedProp = dynamic_cast<FloatVec3Property*>(property)) {
        tgt::vec3 value;
        if (!PyArg_ParseTuple(args, "ss(fff):setPropertyValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z))
            return 0;
        if (setPropertyValue<FloatVec3Property, tgt::vec3>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatVec4Property* typedProp = dynamic_cast<FloatVec4Property*>(property)) {
        tgt::vec4 value;
        if (!PyArg_ParseTuple(args, "ss(ffff):setPropertyValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z, &value.w))
            return 0;
        if (setPropertyValue<FloatVec4Property, tgt::vec4>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatMat2Property* typedProp = dynamic_cast<FloatMat2Property*>(property)) {
        tgt::vec2 vec0, vec1;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((ff)(ff)):setPropertyValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec1.x, &vec1.y))
            return 0;

        tgt::mat2 value(vec0, vec1);
        if (setPropertyValue<FloatMat2Property, tgt::mat2>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatMat3Property* typedProp = dynamic_cast<FloatMat3Property*>(property)) {
        tgt::vec3 vec0, vec1, vec2;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((fff)(fff)(fff)):setPropertyValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec0.z, &vec1.x, &vec1.y, &vec1.z,
                &vec2.x, &vec2.y, &vec2.z))
            return 0;

        tgt::mat3 value(vec0, vec1, vec2);
        if (setPropertyValue<FloatMat3Property, tgt::mat3>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (FloatMat4Property* typedProp = dynamic_cast<FloatMat4Property*>(property)) {
        tgt::vec4 vec0, vec1, vec2, vec3;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((ffff)(ffff)(ffff)(ffff)):setPropertyValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec0.z, &vec0.w, &vec1.x, &vec1.y, &vec1.z, &vec1.w,
                &vec2.x, &vec2.y, &vec2.z, &vec2.w, &vec3.x, &vec3.y, &vec3.z, &vec3.w))
            return 0;

        tgt::mat4 value(vec0, vec1, vec2, vec3);
        if (setPropertyValue<FloatMat4Property, tgt::mat4>(typedProp, value, "setPropertyValue"))
            Py_RETURN_NONE;
    }
    else if (CameraProperty* typedProp = dynamic_cast<CameraProperty*>(property)) {
        tgt::vec3 position, focus, up;
        if (!PyArg_ParseTuple(args, "ss((fff)(fff)(fff)):setPropertyValue",
                &processorName, &propertyID,
                &position.x, &position.y, &position.z,
                &focus.x, &focus.y, &focus.z,
                &up.x, &up.y, &up.z))
            return 0;

        typedProp->setPosition(position);
        typedProp->setFocus(focus);
        typedProp->setUpVector(up);
        Py_RETURN_NONE;
    }


    // we only get here, if property value assignment has failed or
    // the property type is not supported at all

    if (!PyErr_Occurred()) {
        // no error so far => unknown property type
        std::ostringstream errStr;
        errStr << "setPropertyValue() Property '" << property->getFullyQualifiedID() << "'";
        errStr << " has unsupported type: '" << property->getTypeString() << "'";
        PyErr_SetString(PyExc_ValueError, errStr.str().c_str());
    }

    return 0; //< indicates failure
}

static PyObject* voreen_getPropertyValue(PyObject* /*self*/, PyObject* args) {

    // Parse passed arguments: processor name, property ID
    char *processorName, *propertyID;
    PyArg_ParseTuple(args, "ss:getPropertyValue", &processorName, &propertyID);
    if (PyErr_Occurred())
        return 0;

    // fetch property
    Property* property = getProperty(std::string(processorName), std::string(propertyID), "getPropertyValue");
    if (!property)
        return 0;

    // determine property type and return value, if type compatible
    PyObject* result = (PyObject*)-1; //< to determine whether Py_BuildValue has been executed
    if (IntProperty* typedProp = dynamic_cast<IntProperty*>(property))
        result = Py_BuildValue("i", typedProp->get());
    else if (FloatProperty* typedProp = dynamic_cast<FloatProperty*>(property))
        result = Py_BuildValue("f", typedProp->get());
    else if (BoolProperty* typedProp = dynamic_cast<BoolProperty*>(property))
        result = Py_BuildValue("b", typedProp->get());
    else if (StringProperty* typedProp = dynamic_cast<StringProperty*>(property))
        result = Py_BuildValue("s", typedProp->get().c_str());
    else if (OptionPropertyBase* typedProp = dynamic_cast<OptionPropertyBase*>(property))
        result = Py_BuildValue("s", typedProp->get().c_str());
    else if (IntVec2Property* typedProp = dynamic_cast<IntVec2Property*>(property)) {
        tgt::ivec2 value = typedProp->get();
        result = Py_BuildValue("[ii]", value.x, value.y);
    }
    else if (IntVec3Property* typedProp = dynamic_cast<IntVec3Property*>(property)) {
        tgt::ivec3 value = typedProp->get();
        result = Py_BuildValue("[iii]", value.x, value.y, value.z);
    }
    else if (IntVec4Property* typedProp = dynamic_cast<IntVec4Property*>(property)) {
        tgt::ivec4 value = typedProp->get();
        result = Py_BuildValue("[iiii]", value.x, value.y, value.z, value.w);
    }
    else if (FloatVec2Property* typedProp = dynamic_cast<FloatVec2Property*>(property)) {
        tgt::vec2 value = typedProp->get();
        result = Py_BuildValue("[ff]", value.x, value.y);
    }
    else if (FloatVec3Property* typedProp = dynamic_cast<FloatVec3Property*>(property)) {
        tgt::vec3 value = typedProp->get();
        result = Py_BuildValue("[fff]", value.x, value.y, value.z);
    }
    else if (FloatVec4Property* typedProp = dynamic_cast<FloatVec4Property*>(property)) {
        tgt::vec4 value = typedProp->get();
        result = Py_BuildValue("[ffff]", value.x, value.y, value.z, value.w);
    }
    else if (FloatMat2Property* typedProp = dynamic_cast<FloatMat2Property*>(property)) {
        tgt::mat2 value = typedProp->get();
        result = Py_BuildValue("[[ff][ff]]",
                    value[0][0], value[0][1],
                    value[1][0], value[1][1]);
    }
    else if (FloatMat3Property* typedProp = dynamic_cast<FloatMat3Property*>(property)) {
        tgt::mat3 value = typedProp->get();
        result = Py_BuildValue("[[fff][fff][fff]]",
                    value[0][0], value[0][1], value[0][2],
                    value[1][0], value[1][1], value[1][2],
                    value[2][0], value[2][1], value[2][2]);
    }
    else if (FloatMat4Property* typedProp = dynamic_cast<FloatMat4Property*>(property)) {
        tgt::mat4 value = typedProp->get();
        result = Py_BuildValue("[[ffff][ffff][ffff][ffff]]",
                    value[0][0], value[0][1], value[0][2], value[0][3],
                    value[1][0], value[1][1], value[1][2], value[1][3],
                    value[2][0], value[2][1], value[2][2], value[2][3],
                    value[3][0], value[3][1], value[3][2], value[3][3]);
    }
    else if (CameraProperty* typedProp = dynamic_cast<CameraProperty*>(property)) {
        tgt::vec3 position = typedProp->get().getPosition();
        tgt::vec3 focus = typedProp->get().getFocus();
        tgt::vec3 upVector = typedProp->get().getUpVector();
        result = Py_BuildValue("([fff][fff][fff])",
                    position.x, position.y, position.z,
                    focus.x, focus.y, focus.z,
                    upVector.x, upVector.y, upVector.z);
    }

    // if result is still -1, Py_BuildValue has not been executed
    if (result == (PyObject*)-1) {
        std::ostringstream errStr;
        errStr << "getPropertyValue() Property '" << property->getFullyQualifiedID() << "'";
        errStr << " has unsupported type: '" << property->getTypeString() << "'";
        PyErr_SetString(PyExc_ValueError, errStr.str().c_str());
        return 0;
    }

    return result;
}

static PyObject* voreen_setPropertyMinValue(PyObject* /*self*/, PyObject* args) {

    // check length of tuple
    if (PyTuple_Size(args) != 3) {
        std::ostringstream errStr;
        errStr << "setPropertyMinValue() takes exactly 3 arguments: processor name, property id, value";
        errStr << " (" << PyTuple_Size(args) << " given)";
        PyErr_SetString(PyExc_TypeError, errStr.str().c_str());
        return 0;
    }

    // check parameter 1 and 2, if they are strings
    if (!PyString_Check(PyTuple_GetItem(args, 0)) || !PyString_Check(PyTuple_GetItem(args, 1))) {
        PyErr_SetString(PyExc_TypeError, "setPropertyValue() arguments 1 and 2 must be strings");
        return 0;
    }

    // read processor name and property id
    char* processorName = PyString_AsString(PyTuple_GetItem(args, 0));
    char* propertyID = PyString_AsString(PyTuple_GetItem(args, 1));
    if (!processorName || !propertyID) {
        PyErr_SetString(PyExc_TypeError, "setPropertyMinValue() arguments 1 and 2 must be strings");
        return 0;
    }

    // fetch property
    Property* property = getProperty(std::string(processorName), std::string(propertyID), "setPropertyMinValue");
    if (!property)
        return 0;

    // determine property type, convert and assign value
    if (IntProperty* typedProp = dynamic_cast<IntProperty*>(property)) {
        int value;
        if (!PyArg_ParseTuple(args, "ssi:setPropertyMinValue", &processorName, &propertyID, &value))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatProperty* typedProp = dynamic_cast<FloatProperty*>(property)) {
        float value;
        if (!PyArg_ParseTuple(args, "ssf:setPropertyMinValue", &processorName, &propertyID, &value))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (IntVec2Property* typedProp = dynamic_cast<IntVec2Property*>(property)) {
        tgt::ivec2 value;
        if (!PyArg_ParseTuple(args, "ss(ii):setPropertyMinValue", &processorName, &propertyID,
                &value.x, &value.y))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (IntVec3Property* typedProp = dynamic_cast<IntVec3Property*>(property)) {
        tgt::ivec3 value;
        if (!PyArg_ParseTuple(args, "ss(iii):setPropertyMinValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (IntVec4Property* typedProp = dynamic_cast<IntVec4Property*>(property)) {
        tgt::ivec4 value;
        if (!PyArg_ParseTuple(args, "ss(iiii):setPropertyMinValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z, &value.w))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatVec2Property* typedProp = dynamic_cast<FloatVec2Property*>(property)) {
        tgt::vec2 value;
        if (!PyArg_ParseTuple(args, "ss(ff):setPropertyMinValue", &processorName, &propertyID,
                &value.x, &value.y))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatVec3Property* typedProp = dynamic_cast<FloatVec3Property*>(property)) {
        tgt::vec3 value;
        if (!PyArg_ParseTuple(args, "ss(fff):setPropertyMinValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatVec4Property* typedProp = dynamic_cast<FloatVec4Property*>(property)) {
        tgt::vec4 value;
        if (!PyArg_ParseTuple(args, "ss(ffff):setPropertyMinValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z, &value.w))
            return 0;
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatMat2Property* typedProp = dynamic_cast<FloatMat2Property*>(property)) {
        tgt::vec2 vec0, vec1;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((ff)(ff)):setPropertyMinValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec1.x, &vec1.y))
            return 0;

        tgt::mat2 value(vec0, vec1);
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatMat3Property* typedProp = dynamic_cast<FloatMat3Property*>(property)) {
        tgt::vec3 vec0, vec1, vec2;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((fff)(fff)(fff)):setPropertyMinValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec0.z, &vec1.x, &vec1.y, &vec1.z,
                &vec2.x, &vec2.y, &vec2.z))
            return 0;

        tgt::mat3 value(vec0, vec1, vec2);
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatMat4Property* typedProp = dynamic_cast<FloatMat4Property*>(property)) {
        tgt::vec4 vec0, vec1, vec2, vec3;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((ffff)(ffff)(ffff)(ffff)):setPropertyMinValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec0.z, &vec0.w, &vec1.x, &vec1.y, &vec1.z, &vec1.w,
                &vec2.x, &vec2.y, &vec2.z, &vec2.w, &vec3.x, &vec3.y, &vec3.z, &vec3.w))
            return 0;

        tgt::mat4 value(vec0, vec1, vec2, vec3);
        typedProp->setMinValue(value);
        Py_RETURN_NONE;
    }


    // we only get here, if property value assignment has failed or
    // the property type is not supported at all

    if (!PyErr_Occurred()) {
        // no error so far => unknown property type
        std::ostringstream errStr;
        errStr << "setPropertyMinValue() Property '" << property->getFullyQualifiedID() << "'";
        errStr << " has unsupported type: '" << property->getTypeString() << "'";
        PyErr_SetString(PyExc_ValueError, errStr.str().c_str());
    }

    return 0; //< indicates failure
}

static PyObject* voreen_setPropertyMaxValue(PyObject* /*self*/, PyObject* args) {

    // check length of tuple
    if (PyTuple_Size(args) != 3) {
        std::ostringstream errStr;
        errStr << "setPropertyMaxValue() takes exactly 3 arguments: processor name, property id, value";
        errStr << " (" << PyTuple_Size(args) << " given)";
        PyErr_SetString(PyExc_TypeError, errStr.str().c_str());
        return 0;
    }

    // check parameter 1 and 2, if they are strings
    if (!PyString_Check(PyTuple_GetItem(args, 0)) || !PyString_Check(PyTuple_GetItem(args, 1))) {
        PyErr_SetString(PyExc_TypeError, "setPropertyValue() arguments 1 and 2 must be strings");
        return 0;
    }

    // read processor name and property id
    char* processorName = PyString_AsString(PyTuple_GetItem(args, 0));
    char* propertyID = PyString_AsString(PyTuple_GetItem(args, 1));
    if (!processorName || !propertyID) {
        PyErr_SetString(PyExc_TypeError, "setPropertyMaxValue() arguments 1 and 2 must be strings");
        return 0;
    }

    // fetch property
    Property* property = getProperty(std::string(processorName), std::string(propertyID), "setPropertyMaxValue");
    if (!property)
        return 0;

    // determine property type, convert and assign value
    if (IntProperty* typedProp = dynamic_cast<IntProperty*>(property)) {
        int value;
        if (!PyArg_ParseTuple(args, "ssi:setPropertyMaxValue", &processorName, &propertyID, &value))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatProperty* typedProp = dynamic_cast<FloatProperty*>(property)) {
        float value;
        if (!PyArg_ParseTuple(args, "ssf:setPropertyMaxValue", &processorName, &propertyID, &value))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (IntVec2Property* typedProp = dynamic_cast<IntVec2Property*>(property)) {
        tgt::ivec2 value;
        if (!PyArg_ParseTuple(args, "ss(ii):setPropertyMaxValue", &processorName, &propertyID,
                &value.x, &value.y))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (IntVec3Property* typedProp = dynamic_cast<IntVec3Property*>(property)) {
        tgt::ivec3 value;
        if (!PyArg_ParseTuple(args, "ss(iii):setPropertyMaxValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (IntVec4Property* typedProp = dynamic_cast<IntVec4Property*>(property)) {
        tgt::ivec4 value;
        if (!PyArg_ParseTuple(args, "ss(iiii):setPropertyMaxValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z, &value.w))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatVec2Property* typedProp = dynamic_cast<FloatVec2Property*>(property)) {
        tgt::vec2 value;
        if (!PyArg_ParseTuple(args, "ss(ff):setPropertyMaxValue", &processorName, &propertyID,
                &value.x, &value.y))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatVec3Property* typedProp = dynamic_cast<FloatVec3Property*>(property)) {
        tgt::vec3 value;
        if (!PyArg_ParseTuple(args, "ss(fff):setPropertyMaxValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatVec4Property* typedProp = dynamic_cast<FloatVec4Property*>(property)) {
        tgt::vec4 value;
        if (!PyArg_ParseTuple(args, "ss(ffff):setPropertyMaxValue", &processorName, &propertyID,
                &value.x, &value.y, &value.z, &value.w))
            return 0;
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatMat2Property* typedProp = dynamic_cast<FloatMat2Property*>(property)) {
        tgt::vec2 vec0, vec1;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((ff)(ff)):setPropertyMaxValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec1.x, &vec1.y))
            return 0;

        tgt::mat2 value(vec0, vec1);
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatMat3Property* typedProp = dynamic_cast<FloatMat3Property*>(property)) {
        tgt::vec3 vec0, vec1, vec2;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((fff)(fff)(fff)):setPropertyMaxValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec0.z, &vec1.x, &vec1.y, &vec1.z,
                &vec2.x, &vec2.y, &vec2.z))
            return 0;

        tgt::mat3 value(vec0, vec1, vec2);
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }
    else if (FloatMat4Property* typedProp = dynamic_cast<FloatMat4Property*>(property)) {
        tgt::vec4 vec0, vec1, vec2, vec3;
        char *processorName, *propertyID;
        if (!PyArg_ParseTuple(args, "ss((ffff)(ffff)(ffff)(ffff)):setPropertyMaxValue",
                &processorName, &propertyID,
                &vec0.x, &vec0.y, &vec0.z, &vec0.w, &vec1.x, &vec1.y, &vec1.z, &vec1.w,
                &vec2.x, &vec2.y, &vec2.z, &vec2.w, &vec3.x, &vec3.y, &vec3.z, &vec3.w))
            return 0;

        tgt::mat4 value(vec0, vec1, vec2, vec3);
        typedProp->setMaxValue(value);
        Py_RETURN_NONE;
    }


    // we only get here, if property value assignment has failed or
    // the property type is not supported at all

    if (!PyErr_Occurred()) {
        // no error so far => unknown property type
        std::ostringstream errStr;
        errStr << "setPropertyMaxValue() Property '" << property->getFullyQualifiedID() << "'";
        errStr << " has unsupported type: '" << property->getTypeString() << "'";
        PyErr_SetString(PyExc_ValueError, errStr.str().c_str());
    }

    return 0; //< indicates failure
}

static PyObject* voreen_getPropertyMinValue(PyObject* /*self*/, PyObject* args) {

    // Parse passed arguments: processor name, property ID
    char *processorName, *propertyID;
    PyArg_ParseTuple(args, "ss:getPropertyMinValue", &processorName, &propertyID);
    if (PyErr_Occurred())
        return 0;

    // fetch property
    Property* property = getProperty(std::string(processorName), std::string(propertyID), "getPropertyMinValue");
    if (!property)
        return 0;

    // determine property type and return value, if type compatible
    PyObject* result = (PyObject*)-1; //< to determine whether Py_BuildValue has been executed
    if (IntProperty* typedProp = dynamic_cast<IntProperty*>(property))
        result = Py_BuildValue("i", typedProp->getMinValue());
    else if (FloatProperty* typedProp = dynamic_cast<FloatProperty*>(property))
        result = Py_BuildValue("f", typedProp->getMinValue());
    else if (IntVec2Property* typedProp = dynamic_cast<IntVec2Property*>(property)) {
        tgt::ivec2 value = typedProp->getMinValue();
        result = Py_BuildValue("[ii]", value.x, value.y);
    }
    else if (IntVec3Property* typedProp = dynamic_cast<IntVec3Property*>(property)) {
        tgt::ivec3 value = typedProp->getMinValue();
        result = Py_BuildValue("[iii]", value.x, value.y, value.z);
    }
    else if (IntVec4Property* typedProp = dynamic_cast<IntVec4Property*>(property)) {
        tgt::ivec4 value = typedProp->getMinValue();
        result = Py_BuildValue("[iiii]", value.x, value.y, value.z, value.w);
    }
    else if (FloatVec2Property* typedProp = dynamic_cast<FloatVec2Property*>(property)) {
        tgt::vec2 value = typedProp->getMinValue();
        result = Py_BuildValue("[ff]", value.x, value.y);
    }
    else if (FloatVec3Property* typedProp = dynamic_cast<FloatVec3Property*>(property)) {
        tgt::vec3 value = typedProp->getMinValue();
        result = Py_BuildValue("[fff]", value.x, value.y, value.z);
    }
    else if (FloatVec4Property* typedProp = dynamic_cast<FloatVec4Property*>(property)) {
        tgt::vec4 value = typedProp->getMinValue();
        result = Py_BuildValue("[ffff]", value.x, value.y, value.z, value.w);
    }
    else if (FloatMat2Property* typedProp = dynamic_cast<FloatMat2Property*>(property)) {
        tgt::mat2 value = typedProp->getMinValue();
        result = Py_BuildValue("[[ff][ff]]",
                    value[0][0], value[0][1],
                    value[1][0], value[1][1]);
    }
    else if (FloatMat3Property* typedProp = dynamic_cast<FloatMat3Property*>(property)) {
        tgt::mat3 value = typedProp->getMinValue();
        result = Py_BuildValue("[[fff][fff][fff]]",
                    value[0][0], value[0][1], value[0][2],
                    value[1][0], value[1][1], value[1][2],
                    value[2][0], value[2][1], value[2][2]);
    }
    else if (FloatMat4Property* typedProp = dynamic_cast<FloatMat4Property*>(property)) {
        tgt::mat4 value = typedProp->getMinValue();
        result = Py_BuildValue("[[ffff][ffff][ffff][ffff]]",
                    value[0][0], value[0][1], value[0][2], value[0][3],
                    value[1][0], value[1][1], value[1][2], value[1][3],
                    value[2][0], value[2][1], value[2][2], value[2][3],
                    value[3][0], value[3][1], value[3][2], value[3][3]);
    }

    // if result is still -1, Py_BuildValue has not been executed
    if (result == (PyObject*)-1) {
        std::ostringstream errStr;
        errStr << "getPropertyMinValue() Property '" << property->getFullyQualifiedID() << "'";
        errStr << " has unsupported type: '" << property->getTypeString() << "'";
        PyErr_SetString(PyExc_ValueError, errStr.str().c_str());
        return 0;
    }

    return result;
}

static PyObject* voreen_getPropertyMaxValue(PyObject* /*self*/, PyObject* args) {

    // Parse passed arguments: processor name, property ID
    char *processorName, *propertyID;
    PyArg_ParseTuple(args, "ss:getPropertyMaxValue", &processorName, &propertyID);
    if (PyErr_Occurred())
        return 0;

    // fetch property
    Property* property = getProperty(std::string(processorName), std::string(propertyID), "getPropertyMaxValue");
    if (!property)
        return 0;

    // determine property type and return value, if type compatible
    PyObject* result = (PyObject*)-1; //< to determine whether Py_BuildValue has been executed
    if (IntProperty* typedProp = dynamic_cast<IntProperty*>(property))
        result = Py_BuildValue("i", typedProp->getMaxValue());
    else if (FloatProperty* typedProp = dynamic_cast<FloatProperty*>(property))
        result = Py_BuildValue("f", typedProp->getMaxValue());
    else if (IntVec2Property* typedProp = dynamic_cast<IntVec2Property*>(property)) {
        tgt::ivec2 value = typedProp->getMaxValue();
        result = Py_BuildValue("[ii]", value.x, value.y);
    }
    else if (IntVec3Property* typedProp = dynamic_cast<IntVec3Property*>(property)) {
        tgt::ivec3 value = typedProp->getMaxValue();
        result = Py_BuildValue("[iii]", value.x, value.y, value.z);
    }
    else if (IntVec4Property* typedProp = dynamic_cast<IntVec4Property*>(property)) {
        tgt::ivec4 value = typedProp->getMaxValue();
        result = Py_BuildValue("[iiii]", value.x, value.y, value.z, value.w);
    }
    else if (FloatVec2Property* typedProp = dynamic_cast<FloatVec2Property*>(property)) {
        tgt::vec2 value = typedProp->getMaxValue();
        result = Py_BuildValue("[ff]", value.x, value.y);
    }
    else if (FloatVec3Property* typedProp = dynamic_cast<FloatVec3Property*>(property)) {
        tgt::vec3 value = typedProp->getMaxValue();
        result = Py_BuildValue("[fff]", value.x, value.y, value.z);
    }
    else if (FloatVec4Property* typedProp = dynamic_cast<FloatVec4Property*>(property)) {
        tgt::vec4 value = typedProp->getMaxValue();
        result = Py_BuildValue("[ffff]", value.x, value.y, value.z, value.w);
    }
    else if (FloatMat2Property* typedProp = dynamic_cast<FloatMat2Property*>(property)) {
        tgt::mat2 value = typedProp->getMaxValue();
        result = Py_BuildValue("[[ff][ff]]",
                    value[0][0], value[0][1],
                    value[1][0], value[1][1]);
    }
    else if (FloatMat3Property* typedProp = dynamic_cast<FloatMat3Property*>(property)) {
        tgt::mat3 value = typedProp->getMaxValue();
        result = Py_BuildValue("[[fff][fff][fff]]",
                    value[0][0], value[0][1], value[0][2],
                    value[1][0], value[1][1], value[1][2],
                    value[2][0], value[2][1], value[2][2]);
    }
    else if (FloatMat4Property* typedProp = dynamic_cast<FloatMat4Property*>(property)) {
        tgt::mat4 value = typedProp->getMaxValue();
        result = Py_BuildValue("[[ffff][ffff][ffff][ffff]]",
                    value[0][0], value[0][1], value[0][2], value[0][3],
                    value[1][0], value[1][1], value[1][2], value[1][3],
                    value[2][0], value[2][1], value[2][2], value[2][3],
                    value[3][0], value[3][1], value[3][2], value[3][3]);
    }

    // if result is still -1, Py_BuildValue has not been executed
    if (result == (PyObject*)-1) {
        std::ostringstream errStr;
        errStr << "getPropertyMaxValue() Property '" << property->getFullyQualifiedID() << "'";
        errStr << " has unsupported type: '" << property->getTypeString() << "'";
        PyErr_SetString(PyExc_ValueError, errStr.str().c_str());
        return 0;
    }

    return result;
}

static PyObject* voreen_setCameraPosition(PyObject* /*self*/, PyObject* args) {

    // parse arguments
    char* processorName = 0;
    char* propertyID = 0;
    tgt::vec3 position;
    PyArg_ParseTuple(args, "ss(fff):setCameraPosition", &processorName, &propertyID,
        &position.x, &position.y, &position.z);
    if (PyErr_Occurred())
        return 0;

    // find property and set position
    if (CameraProperty* camProp = getTypedProperty<CameraProperty>(
            std::string(processorName), std::string(propertyID), "Camera", "setCameraPosition")) {
        camProp->setPosition(position);
        camProp->invalidate();
        Py_RETURN_NONE;
    }

    // indicate failure
    return 0;
}

static PyObject* voreen_setCameraFocus(PyObject* /*self*/, PyObject* args) {

    // parse arguments
    char* processorName = 0;
    char* propertyID = 0;
    tgt::vec3 focus;
    PyArg_ParseTuple(args, "ss(fff):setCameraFocus", &processorName, &propertyID,
        &focus.x, &focus.y, &focus.z);
    if (PyErr_Occurred())
        return 0;

    // find property and set position
    if (CameraProperty* camProp = getTypedProperty<CameraProperty>(
            std::string(processorName), std::string(propertyID), "Camera", "setCameraFocus")) {
        camProp->setFocus(focus);
        camProp->invalidate();
        Py_RETURN_NONE;
    }

    // indicate failure
    return 0;
}

static PyObject* voreen_setCameraUpVector(PyObject* /*self*/, PyObject* args) {

    // parse arguments
    char* processorName = 0;
    char* propertyID = 0;
    tgt::vec3 up;
    PyArg_ParseTuple(args, "ss(fff):setCameraUpVector", &processorName, &propertyID,
        &up.x, &up.y, &up.z);
    if (PyErr_Occurred())
        return 0;

    // find property and set position
    if (CameraProperty* camProp = getTypedProperty<CameraProperty>(
            std::string(processorName), std::string(propertyID), "Camera", "setCameraUpVector")) {
        camProp->setUpVector(up);
        camProp->invalidate();
        Py_RETURN_NONE;
    }

    // indicate failure
    return 0;
}

static PyObject* voreen_loadVolume(PyObject* /*self*/, PyObject* args) {

    const char* filename = 0;
    const char* procStr = 0;
    if (!PyArg_ParseTuple(args, "s|s:loadVolume", &filename, &procStr))
        return 0;

    ProcessorNetwork* network = getProcessorNetwork("loadVolume");
    if (!network)
        return 0;

#ifdef VRN_MODULE_BASE
    VolumeSource* volumeSource = 0;
    if (!procStr) {
        // select first volumesource in network
        std::vector<VolumeSource*> sources = network->getProcessorsByType<VolumeSource>();
        if (sources.empty()) {
            PyErr_SetString(PyExc_RuntimeError, "loadVolume() Network does not contain a VolumeSource.");
            return 0;
        }
        volumeSource = sources.front();
    }
    else {
        // retrieve volumesource with given name from network
        volumeSource = getTypedProcessor<VolumeSource>(std::string(procStr), "VolumeSource", "loadVolume");
        if (!volumeSource)
            return 0;
    }
    tgtAssert(volumeSource, "no source proc");

    try {
        volumeSource->loadVolume(std::string(filename));
        Py_RETURN_NONE;
    }
    catch (std::exception &e) {
        PyErr_SetString(PyExc_RuntimeError, std::string("loadVolume() Failed to load data set '" +
            std::string(filename) + "': " + e.what()).c_str());
        return 0;
    }
#else
    PyErr_SetString(PyExc_RuntimeError, std::string("loadVolume() Voreen has been compiled without 'Base' module: " +
        "VolumeSource processor not available.");
    return 0;
#endif
}

static PyObject* voreen_loadTransferFunction(PyObject* /*self*/, PyObject* args) {

    // parse arguments
    char* processorName = 0;
    char* propertyID = 0;
    char* filename = 0;
    PyArg_ParseTuple(args, "sss:loadTransferFunction", &processorName, &propertyID, &filename);
    if (PyErr_Occurred())
        return 0;

    // find property and set position
    if (TransFuncProperty* property = getTypedProperty<TransFuncProperty>(
            std::string(processorName), std::string(propertyID), "TransFunc", "loadTransferFunction")) {
        TransFunc* transFunc = property->get();
        if (!transFunc) {
            PyErr_SetString(PyExc_SystemError, std::string("loadTransferFunction() Property '" +
                property->getFullyQualifiedID() + "' does not contain a transfer function").c_str());
            return 0;
        }
        else if (!transFunc->load(std::string(filename))) {
            PyErr_SetString(PyExc_ValueError, std::string("loadTransferFunction() Failed to load '" +
                std::string(filename) + "'").c_str());
            return 0;
        }

        property->invalidate();
        Py_RETURN_NONE;
    }

    // indicate failure
    return 0;
}

static PyObject* voreen_render(PyObject* /*self*/, PyObject* args) {
    using namespace voreen;

    int sync = 0;
    if (!PyArg_ParseTuple(args, "|i:render", &sync))
        return 0;

    if (VoreenApplication::app() && VoreenApplication::app()->getNetworkEvaluator()) {
        VoreenApplication::app()->getNetworkEvaluator()->process();
        if (sync)
            glFinish();
        Py_RETURN_NONE;
    }

    return 0;
}

static PyObject* voreen_repaint(PyObject* /*self*/, PyObject* args) {
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    using namespace voreen;

    ProcessorNetwork* network = getProcessorNetwork("repaint");
    if (!network)
        return 0;

    std::vector<CanvasRenderer*> canvases = network->getProcessorsByType<CanvasRenderer>();
    for (size_t i=0; i<canvases.size(); i++) {
        if (canvases[i]->getCanvas())
            canvases[i]->getCanvas()->repaint();
    }

    Py_RETURN_NONE;
}

static PyObject* voreen_setViewport(PyObject* /*self*/, PyObject* args) {
    int i1, i2;

    using namespace voreen;

    if (!PyArg_ParseTuple(args, "ii:setViewport", &i1, &i2))
        return 0;

    ProcessorNetwork* network = getProcessorNetwork("setViewport");
    if (!network)
        return 0;
    std::vector<CanvasRenderer*> canvases = network->getProcessorsByType<CanvasRenderer>();

    for (std::vector<CanvasRenderer*>::const_iterator iter = canvases.begin(); iter != canvases.end(); iter++) {
        //CanvasRenderer* canvasProc = dynamic_cast<CanvasRenderer*>(*iter);
        //if (canvasProc && canvasProc->getProcessorWidget())
            //canvasProc->getProcessorWidget()->setSize(i1, i2);
        IntVec2Property* sizeProp = dynamic_cast<IntVec2Property*>((*iter)->getProperty("canvasSize"));
        if (sizeProp)
            sizeProp->set(tgt::ivec2(i1, i2));
        else
            return 0;
    }

    Py_RETURN_NONE;
}

static PyObject* voreen_snapshot(PyObject* /*self*/, PyObject* args) {

    const char* filename = 0;
    const char* canvasStr = 0;
    if (!PyArg_ParseTuple(args, "s|s:snapshot", &filename, &canvasStr))
        return 0;

    ProcessorNetwork* network = getProcessorNetwork("snapshot");
    if (!network)
        return 0;

    CanvasRenderer* canvasProc = 0;
    if (!canvasStr) {
        // select first canvas in network
        std::vector<CanvasRenderer*> canvases = network->getProcessorsByType<CanvasRenderer>();
        if (canvases.empty()) {
            PyErr_SetString(PyExc_RuntimeError, "snapshot() Network does not contain a CanvasRenderer.");
            return 0;
        }
        canvasProc = canvases.front();
    }
    else {
        // retrieve canvas with given name from network
        canvasProc = getTypedProcessor<CanvasRenderer>(std::string(canvasStr), "CanvasRenderer", "snapshot");
        if (!canvasProc)
            return 0;
    }
    tgtAssert(canvasProc, "no canvas proc");

    // take snapshot
    bool success = canvasProc->renderToImage(filename);
    if (!success) {
        PyErr_SetString(PyExc_ValueError, (std::string("snapshot() ") + canvasProc->getRenderToImageError()).c_str());
        return 0;
    }

    Py_RETURN_NONE;
}

static PyObject* voreen_canvas_count(PyObject* /*self*/, PyObject* args) {
    if (!PyArg_ParseTuple(args, ":canvasCount"))
        return NULL;

    using namespace voreen;

    ProcessorNetwork* network = getProcessorNetwork("canvasCount");
    if (!network)
        return 0;
    std::vector<CanvasRenderer*> canvases = network->getProcessorsByType<CanvasRenderer>();

    PyObject* py_count = Py_BuildValue("i", canvases.size());
    return py_count;
}

static PyObject* voreen_canvas_snapshot(PyObject* /*self*/, PyObject* args) {
    int index;
    const char* filename = 0;
    if (!PyArg_ParseTuple(args, "is:canvasSnapshot", &index, &filename))
        return NULL;

    using namespace voreen;

    ProcessorNetwork* network = getProcessorNetwork("canvasSnapshot");
    if (!network)
        return 0;
    std::vector<CanvasRenderer*> canvases = network->getProcessorsByType<CanvasRenderer>();

    int count = 0;
    for (std::vector<CanvasRenderer*>::const_iterator iter = canvases.begin(); iter != canvases.end(); iter++) {
        if (count == index) {
            bool success = (*iter)->renderToImage(filename, (*iter)->getCanvas()->getSize());
            if (!success) {
                PyErr_SetString(PyExc_RuntimeError, (std::string("canvasSnapshot() renderToImage() failed: ")
                    + (*iter)->getRenderToImageError()).c_str());
                return 0;
            }
            break;
        }
        count++;
    }

    Py_RETURN_NONE;
}

static PyObject* voreen_rotateCamera(PyObject* /*self*/, PyObject* args) {
    using namespace voreen;

    // parse arguments
    char* processorName = 0;
    char* propertyID = 0;
    float f1, f2, f3, f4;
    PyArg_ParseTuple(args, "ssf(fff):rotateCamera", &processorName, &propertyID,
        &f1, &f2, &f3, &f4);
    if (PyErr_Occurred())
        return 0;

    // find property
    CameraProperty* camProp = getTypedProperty<CameraProperty>(
        std::string(processorName), std::string(propertyID), "Camera", "rotateCamera");
    if (!camProp)
        return 0;

    // rotate by trackball
    VoreenTrackball track(camProp);
    track.rotate(tgt::quat::createQuat(f1, tgt::vec3(f2, f3, f4)));
    camProp->invalidate();

    Py_RETURN_NONE;
}

static PyObject* voreen_invalidateProcessors(PyObject* /*self*/, PyObject* args) {
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    using namespace voreen;
    if (VoreenApplication::app() && VoreenApplication::app()->getNetworkEvaluator()) {
        VoreenApplication::app()->getNetworkEvaluator()->invalidateProcessors();
        Py_RETURN_NONE;
    }

    return 0;
}

static PyObject* voreen_tickClockProcessor(PyObject* /*self*/, PyObject* args) {

    // parse arguments
    char* processorName = 0;
    PyArg_ParseTuple(args, "s:tickClockProcessor", &processorName);
    if (PyErr_Occurred())
        return 0;

#ifdef VRN_MODULE_BASE
    // find clock processor
    ClockProcessor* clockProcessor = getTypedProcessor<ClockProcessor>(
        std::string(processorName), "ClockProcessor", "tickClockProcessor");
    if (!clockProcessor)
        return 0;

    // trigger timer event
    clockProcessor->timerEvent(0);
    Py_RETURN_NONE;
#endif

    return 0;
}

static PyObject* voreen_resetClockProcessor(PyObject* /*self*/, PyObject* args) {
    // parse arguments
    char* processorName = 0;
    PyArg_ParseTuple(args, "s:resetClockProcessor", &processorName);
    if (PyErr_Occurred())
        return 0;

#ifdef VRN_MODULE_BASE
    // find clock processor
    ClockProcessor* clockProcessor = getTypedProcessor<ClockProcessor>(
        std::string(processorName), "ClockProcessor", "resetClockProcessor");
    if (!clockProcessor)
        return 0;

    // reset
    clockProcessor->resetCounter();
    Py_RETURN_NONE;
#endif

    return 0;
}

static PyObject* voreen_getBasePath(PyObject* /*self*/, PyObject* /*args*/) {

    if (!VoreenApplication::app()) {
        PyErr_SetString(PyExc_SystemError, "getBasePath() VoreenApplication not instantiated.");
        return 0;
    }

    std::string basePath = VoreenApplication::app()->getBasePath();
    PyObject* arg = Py_BuildValue("s", basePath.c_str());
    if (!arg) {
        PyErr_SetString(PyExc_SystemError, "getBasePath() failed to create argument");
        return 0;
    }

    return arg;
}

static PyObject* voreen_info(PyObject* /*self*/, PyObject* /*args*/) {
    return printModuleInfo("voreen",  true, 0, false, true);
}

//------------------------------------------------------------------------------
// Python binding method tables

// module 'voreen'
static PyMethodDef voreen_methods[] = {
    {
        "setPropertyValue",
        voreen_setPropertyValue,
        METH_VARARGS,
        "setPropertyValue(processor name, property id, scalar or tuple)\n\n"
        "Assigns a value to a processor property. The value has to be passed\n"
        "as scalar or tuple, depending on the property's cardinality.\n"
        "Camera properties take a 3-tuple of 3-tuples, containing the position,\n"
        "focus and up vectors. Option properties expect an option key."
    },
    {
        "getPropertyValue",
        voreen_getPropertyValue,
        METH_VARARGS,
        "getPropertyValue(processor name, property id) -> scalar or tuple\n\n"
        "Returns the value of a processor property as scalar or tuple,\n"
        "depending on the property's cardinality. See: setPropertyValue"
    },
    {
        "setPropertyMinValue",
        voreen_setPropertyMinValue,
        METH_VARARGS,
        "setPropertyMinValue(processor name, property id, scalar or tuple)\n\n"
        "Defines the minimum value of a numeric property."
    },
    {
        "setPropertyMaxValue",
        voreen_setPropertyMaxValue,
        METH_VARARGS,
        "setPropertyMaxValue(processor name, property id, scalar or tuple)\n\n"
        "Defines the maximum value of a numeric property."
    },
    {
        "getPropertyMinValue",
        voreen_getPropertyMinValue,
        METH_VARARGS,
        "getPropertyMinValue(processor name, property id) -> scalar or tuple\n\n"
        "Returns the minimum value of a numeric property as scalar or tuple,\n"
        "depending on the property's cardinality."
    },
    {
        "getPropertyMaxValue",
        voreen_getPropertyMaxValue,
        METH_VARARGS,
        "getPropertyMaxValue(processor name, property id) -> scalar or tuple\n\n"
        "Returns the maximum value of a numeric property as scalar or tuple,\n"
        "depending on the property's cardinality."
    },
    {
        "setCameraPosition",
        voreen_setCameraPosition,
        METH_VARARGS,
        "setCameraPosition(processor name, property id, (x, y, z))\n\n"
        "Convenience function for setting a camera's position.\n"
        "See also: setPropertyValue"
    },
    {
        "setCameraFocus",
        voreen_setCameraFocus,
        METH_VARARGS,
        "setCameraFocus(processor name, property id, (x, y, z))\n\n"
        "Convenience function for setting a camera's focus.\n"
        "See also: setPropertyValue"
    },
    {
        "setCameraUpVector",
        voreen_setCameraUpVector,
        METH_VARARGS,
        "setCameraUp(processor name, property id, (x, y, z))\n\n"
        "Convenience function for setting a camera's up vector.\n"
        "See also: setPropertyValue"
    },
    {
        "loadVolume",
        voreen_loadVolume,
        METH_VARARGS,
        "loadVolume(filename, [volume source])\n\n"
        "Loads a volume data set and assigns it to a VolumeSource processor.\n"
        "If no processor name is passed, the first volume source in the\n"
        "network is chosen."
    },
    {
        "loadTransferFunction",
        voreen_loadTransferFunction,
        METH_VARARGS,
        "loadTransferFunction(processor name, property id, filename)\n\n"
        "Loads a transfer function and assigns it to a transfer function property."
    },
    {
        "render",
        voreen_render,
        METH_VARARGS,
        "render([sync=0])\n\n"
        "Renders the current network by calling NetworkEvaluator::process().\n"
        "For sync=1, glFinish() is called afterwards."
    },
    {
        "repaint",
        voreen_repaint,
        METH_VARARGS,
        "repaint() Renders the network by forcing repaints of all canvases."
    },
    {
        "setViewport",
        voreen_setViewport,
        METH_VARARGS,
        "setViewport(width, height)\n\n"
        "Convenience function setting the canvas dimensions of\n"
        "of all CanvasRenderers in the network."
    },
    {
        "snapshot",
        voreen_snapshot,
        METH_VARARGS,
        "snapshot(filename, [canvas])\n\n"
        "Saves a snapshot of the specified canvas to the given file.\n"
        "If no canvas name is passed, the first canvas in the network is chosen."
    },
    {
        "snapshotCanvas",
        voreen_canvas_snapshot,
        METH_VARARGS,
        "snapshotCanvas(i, filename)\n\n"
        "Saves a snapshot of the ith canvas to the given file."
    },
    {
        "canvasCount",
        voreen_canvas_count,
        METH_VARARGS,
        "canvasCount() -> int\n\n"
        "Returns the number of canvases in the current network."
    },
    {
        "rotateCamera",
        voreen_rotateCamera,
        METH_VARARGS,
        "rotateCamera(processor name, property id, angle, (x,y,z))\n\n"
        "Rotates a camera by the specified angle around the specified axis."
    },
    {
        "invalidateProcessors",
        voreen_invalidateProcessors,
        METH_VARARGS,
        "invalidateProcessors() Invalidates all processors in the current network."
    },
    {
        "tickClockProcessor",
        voreen_tickClockProcessor,
        METH_VARARGS,
        "tickClockProcessor(processor name)\n\n"
        "Sends a timer event to a ClockProcessor."
    },
    {
        "resetClockProcessor",
        voreen_resetClockProcessor,
        METH_VARARGS,
        "resetClockProcessor(processor name)\n\n"
        "Resets the clock of a ClockProcessor."
    },
    {
        "getBasePath",
        voreen_getBasePath,
        METH_VARARGS,
        "getBasePath() -> path\n\n"
        "Returns the absolute Voreen base path."
   },
   {
        "info",
        voreen_info,
        METH_VARARGS,
        "info() Prints documentation of the module's functions."
    },
    { NULL, NULL, 0, NULL} // sentinal
};

namespace voreen {

const std::string PyVoreen::loggerCat_ = "voreen.Python.PyVoreen";

PyVoreen::PyVoreen() {
    if (Py_IsInitialized()) {
        // initialize voreen module
        Py_InitModule("voreen", voreen_methods);
    }
    else {
        LERROR("Python environment not initialized");
    }
}

} // namespace voreen


//-------------------------------------------------------------------------------------------------
// implementation of helper functions

namespace {

ProcessorNetwork* getProcessorNetwork(const std::string& functionName) {

    // retrieve evaluator from application
    if (!VoreenApplication::app()) {
        PyErr_SetString(PyExc_SystemError, std::string(functionName + "() VoreenApplication not instantiated").c_str());
        return 0;
    }
    else if (!VoreenApplication::app()->getNetworkEvaluator()) {
        PyErr_SetString(PyExc_SystemError, std::string(functionName + "() No network evaluator").c_str());
        return 0;
    }

    // get network from evaluator
    ProcessorNetwork* network = const_cast<ProcessorNetwork*>(VoreenApplication::app()->getNetworkEvaluator()->getProcessorNetwork());
    if (!network) {
        PyErr_SetString(PyExc_SystemError, std::string(functionName + "() No processor network").c_str());
        return 0;
    }

    return network;
}

Processor* getProcessor(const std::string& processorName, const std::string& functionName) {

    ProcessorNetwork* network = getProcessorNetwork(functionName);
    if (!network)
        return 0;

    // find processor
    Processor* processor = network->getProcessorByName(processorName);
    if (!processor) {
        PyErr_SetString(PyExc_NameError, std::string(functionName + "() Processor '" + processorName + "' not found").c_str());
        return 0;
    }

    return processor;
}

template<typename T>
T* getTypedProcessor(const std::string& processorName, const std::string& processorTypeString,
                     const std::string& functionName) {

    // fetch processor
    Processor* processor = getProcessor(processorName, functionName);
    if (!processor)
        return 0;

    // check type
    if (T* cProc = dynamic_cast<T*>(processor))
        return cProc;
    else {
        PyErr_SetString(PyExc_TypeError, std::string(functionName + "() Processor '" +
            processorName + "' is not of type " + processorTypeString).c_str());
        return 0;
    }
}

Property* getProperty(const std::string& processorName, const std::string& propertyID,
                      const std::string& functionName) {

    // fetch processor
    Processor* processor = getProcessor(processorName, functionName);
    if (!processor)
        return 0;

    // find property
    Property* property = processor->getProperty(propertyID);
    if (!property) {
        PyErr_SetString(PyExc_NameError, std::string(functionName + "() Processor '" +
            processorName + "' has no property '" + propertyID + "'").c_str());
        return 0;
    }

    return property;
}

template<typename T>
T* getTypedProperty(const std::string& processorName, const std::string& propertyID,
               const std::string& propertyTypeString, const std::string& functionName) {

    // fetch property
    Property* property = getProperty(processorName, propertyID, functionName);
    if (!property)
        return 0;

    // check type
    if (T* cProp = dynamic_cast<T*>(property))
        return cProp;
    else {
        PyErr_SetString(PyExc_TypeError, std::string(functionName + "() Property '" +
            property->getFullyQualifiedID() + "' is of type " + property->getTypeString() +
            ". Expected: " + propertyTypeString).c_str());
        return 0;
    }
}

template<typename PropertyType, typename ValueType>
bool setPropertyValue(const std::string& processorName, const std::string& propertyID, const ValueType& value,
                      const std::string& propertyTypeString, const std::string& functionName) {

    if (PropertyType* property = getTypedProperty<PropertyType>(
            processorName, propertyID, propertyTypeString, functionName)) {
        std::string errorMsg;
        if (property->isValidValue(value, errorMsg)) {
            property->set(value);
            return true;
        }
        else {
            // define Python exception
            PyErr_SetString(PyExc_ValueError, (functionName + std::string("() ") + errorMsg).c_str());
            return false;
        }
    }
    return false;
}

template<typename PropertyType, typename ValueType>
bool setPropertyValue(PropertyType* property, const ValueType& value,
                      const std::string& functionName) {
    tgtAssert(property, "Null pointer passed");

    std::string errorMsg;
    if (property->isValidValue(value, errorMsg)) {
        property->set(value);
        return true;
    }
    else {
        // define Python exception
        PyErr_SetString(PyExc_ValueError, (functionName + std::string("() ") + errorMsg).c_str());
        return false;
    }
}

static PyObject* printModuleInfo(const std::string& moduleName, bool omitFunctionName,
                                 int spacing, bool collapse, bool blanklines) {

    // import apihelper.py
    PyObject* apihelper = PyImport_ImportModule("apihelper");
    if (!apihelper) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() apihelper module not found")).c_str());
        return 0;
    }

    // get reference to info function
    PyObject* func = PyDict_GetItemString(PyModule_GetDict(apihelper), "info");
    if (!func) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() apihelper.info() not found")).c_str());
        Py_XDECREF(apihelper);
        return 0;
    }

    // get reference to module
    PyObject* mod = PyImport_AddModule(const_cast<char*>(moduleName.c_str())); // const cast required for
                                                                               // Python 2.4
    if (!mod) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() failed to access module ") + moduleName).c_str());
        Py_XDECREF(apihelper);
    }

    // build parameter tuple
    std::string docStr = "Module " + moduleName;
    PyObject* arg = Py_BuildValue("(O,s,i,i,i,i)", mod, docStr.c_str(),
        int(omitFunctionName), spacing, (int)collapse, (int)blanklines);
    if (!arg) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() failed to create arguments")).c_str());
        Py_XDECREF(apihelper);
        return 0;
    }

    PyObject* callObj = PyObject_CallObject(func, arg);
    bool success = (callObj != 0);

    Py_XDECREF(callObj);
    Py_XDECREF(arg);
    Py_XDECREF(apihelper);

    if (success)
        Py_RETURN_NONE;
    else
        return 0;
}

} // namespace anonymous
