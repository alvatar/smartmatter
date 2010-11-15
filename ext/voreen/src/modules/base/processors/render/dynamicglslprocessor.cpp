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

#include "voreen/modules/base/processors/render/dynamicglslprocessor.h"

using tgt::TextureUnit;

//TODOs:
// - enable transfer function usage
// - prepare for shader version 120

namespace voreen {

const std::string DynamicGLSLProcessor::loggerCat_("voreen.DynamicGLSLProcessor");

DynamicGLSLProcessor::DynamicGLSLProcessor()
    : VolumeRaycaster()
{
    shader_ = new ShaderProperty("shader", "Fragment shader", "dynamicglsl.frag");
    shader_->onChange(CallMemberAction<DynamicGLSLProcessor>(this, &DynamicGLSLProcessor::initializePortsAndProperties));
    // store initial properties, that they are not deleted accidentally
    std::vector<Property*> properties = getProperties();
    for (unsigned int i=0; i<properties.size(); i++)
        baseClassProperties_.push_back(properties[i]->getID());

    initializePortsAndProperties();
}

DynamicGLSLProcessor::~DynamicGLSLProcessor() {
    delete shader_;
}


std::string DynamicGLSLProcessor::getProcessorInfo() const {
    return "This is a highly customizable processor. It can be customized \
            by using the associated GLSL shader. Within the shader it is \
            possible to define a varying number of inports for receiving \
            image or volume data. Furthermore, up to eight render outports \
            can be defined. To allow further customization, properties can \
            be initialized and used as regular properties, e.g., linking \
            is possible. To learn how to setup ports and properties have \
            a look at the standard shader, which is associated with the \
            processor."
            "<br><b>Note:</b> To update ports and properties the refresh button \
            has to be pressed.";
}

Processor* DynamicGLSLProcessor::create() const {
    return new DynamicGLSLProcessor();
}

void DynamicGLSLProcessor::initialize() throw (VoreenException) {
    // assign header and create shader
    // Add definition of sampler type to allow shader compilation:
    initialized_ = true;
    shader_->setHeader(generateHeader() + "\n #define TF_SAMPLER_TYPE sampler1D\n");
    shader_->rebuild();
    initialized_ = false;
    VolumeRaycaster::initialize();  // initializes the shader and transfunc properties

    tgt::Shader* sh = shader_->getShader();
    if (!sh || !sh->isLinked()) {
        LERROR("Failed to load shaders!");
        initialized_ = false;
        throw VoreenException(getClassName() + ": Failed to load shaders!");
    }
    getProcessorWidget()->updateFromProcessor();

    portGroup_.initialize();
    setupPortGroup();
}

void DynamicGLSLProcessor::deinitialize() throw (VoreenException) {
    portGroup_.deinitialize();
    shader_->deinitialize();
    LGL_ERROR;
    VolumeRaycaster::deinitialize();
}

void DynamicGLSLProcessor::loadShader() {
    shader_->rebuild();
}

void DynamicGLSLProcessor::compile() {
    shader_->setHeader(generateHeader());
    shader_->rebuild();
}

void DynamicGLSLProcessor::process() {
    // compile program if needed
    if (getInvalidationLevel() >= Processor::INVALID_PROGRAM)
        compile();

    tgt::Shader* sh = shader_->getShader();
    if (!sh || !sh->isLinked()) {
        LERROR("Failed to link shader.");
        return;
    }

    portGroup_.reattachTargets();
    portGroup_.activateTargets();
    portGroup_.clearTargets();
    LGL_ERROR;

    // vector containing the volumes to bind; is passed to bindVolumes()
    std::vector<VolumeStruct> volumeTextures;

    // bind inport textures and set according uniforms
    for (unsigned int i=0; i<inportIDs_.size(); i++) {
        Port* curPort = getPort(inportIDs_[i].name_);
        if (dynamic_cast<RenderPort*>(curPort)) {
            // we are dealing with a RenderPort
            RenderPort* renderPort = dynamic_cast<RenderPort*>(curPort);
            TextureUnit colorUnit, depthUnit;
            // bind entry params
            renderPort->bindTextures(colorUnit.getEnum(), depthUnit.getEnum());
            inportIDs_[i].mainTexUnit_ = colorUnit.getUnitNumber();
            inportIDs_[i].depthTexUnit_ = depthUnit.getUnitNumber();
        } else if (dynamic_cast<VolumePort*>(curPort)) {
            // we are dealing with a volume port
            // add volume
            VolumePort* volumePort = dynamic_cast<VolumePort*>(curPort);
            TextureUnit volUnit;
            volumeTextures.push_back(VolumeStruct(
                volumePort->getData()->getVolumeGL(),
                &volUnit,
                inportIDs_[i].name_,
                inportIDs_[i].paramsIdentifier_,
                true)
            );
        }
    }
    LGL_ERROR;

    //updateBrickingParameters(volumePort_.getData());
    //addBrickedVolumeModalities(volumePort_.getData(), volumeTextures);

    // activate shader
    sh->activate();

    /*
    transferFunc_.setVolumeHandle(volumePort_.getData());
    // bind transfer function
    TextureUnit transferUnit;
    transferUnit.activate();
    if (transferFunc_.get())
        transferFunc_.get()->bind();
    sh->setUniform("transferFunc_", transferUnit.getUnitNumber());
    */

    //setBrickedVolumeUniforms(sh, volumePort_.getData());

    // set common uniforms used by all shaders
    setGlobalShaderParameters(sh);

    // bind inport textures and set according uniforms
    for (unsigned int i=0; i<inportIDs_.size(); i++) {
        Port* curPort = getPort(inportIDs_[i].name_);
        if (dynamic_cast<RenderPort*>(curPort)) {
            // we are dealing with a RenderPort
            sh->setUniform(inportIDs_[i].mainSamplerIdentifier_, inportIDs_[i].mainTexUnit_);
            if (inportIDs_[i].depthSamplerIdentifier_ != "")
                sh->setUniform(inportIDs_[i].depthSamplerIdentifier_, inportIDs_[i].depthTexUnit_);
            if (inportIDs_[i].paramsIdentifier_ != "")
                dynamic_cast<RenderPort*>(curPort)->setTextureParameters(sh, inportIDs_[i].paramsIdentifier_);
        }
    }
    LGL_ERROR;

    // set uniforms based on properties
    std::vector<Property*> properties = getProperties();
    for (unsigned int i=0; i<properties.size(); i++) {
        Property* curProp = properties[i];
        if(dynamic_cast<BoolProperty*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<BoolProperty*>(curProp)->get());
        else if(dynamic_cast<ColorProperty*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<ColorProperty*>(curProp)->get());
        else if(dynamic_cast<FloatProperty*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatProperty*>(curProp)->get());
        else if(dynamic_cast<FloatMat2Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatMat2Property*>(curProp)->get());
        else if(dynamic_cast<FloatMat3Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatMat3Property*>(curProp)->get());
        else if(dynamic_cast<FloatMat4Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatMat4Property*>(curProp)->get());
        else if(dynamic_cast<FloatVec2Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatVec2Property*>(curProp)->get());
        else if(dynamic_cast<FloatVec3Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatVec3Property*>(curProp)->get());
        else if(dynamic_cast<FloatVec4Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<FloatVec4Property*>(curProp)->get());
        else if(dynamic_cast<IntProperty*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<IntProperty*>(curProp)->get());
        else if(dynamic_cast<IntVec2Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<IntVec2Property*>(curProp)->get());
        else if(dynamic_cast<IntVec3Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<IntVec3Property*>(curProp)->get());
        else if(dynamic_cast<IntVec4Property*>(curProp))
            sh->setUniform(curProp->getID(), dynamic_cast<IntVec4Property*>(curProp)->get());
    }

    // bind the volumes and pass the necessary information to the shader
    bindVolumes(sh, volumeTextures);
    LGL_ERROR;

    renderQuad();

    sh->deactivate();

    portGroup_.deactivateTargets();
    glActiveTexture(GL_TEXTURE0);
    LGL_ERROR;
}

std::string DynamicGLSLProcessor::generateHeader(VolumeHandle* /*volumeHandle*/) {
    std::string header = VolumeRaycaster::generateHeader();
    //if(transferFunc_.get())
    //    header += transferFunc_.get()->getShaderDefines();
    return header;
}

void DynamicGLSLProcessor::setupPortGroup() {
    // add new ports to the port group
    for (unsigned int i=0; i<outportIDs_.size(); i++) {
        Port* curPort = getPort(outportIDs_[i]);
        if (!portGroup_.containsPort(dynamic_cast<RenderPort*>(curPort)))
            portGroup_.addPort(dynamic_cast<RenderPort*>(curPort));
        shader_->getShader()->bindFragDataLocation(outportIDs_.size()-1, curPort->getName());
    }
    // remove obsolete ports from the port group
    std::vector<Port*> outports = getOutports();
    for (unsigned int i=0; i<outports.size(); i++) {
        Port* curPort = outports[i];
        if (std::find(outportIDs_.begin(), outportIDs_.end(), curPort->getName()) == outportIDs_.end())
            portGroup_.removePort(dynamic_cast<RenderPort*>(curPort));
    }
}

GLint DynamicGLSLProcessor::getColorFormat(glslparser::GLSLVariableSymbol* symbol) {
    std::string colorFormat = symbol->getAnnotationValueString("colorformat");
    if (colorFormat == "GL_RGB")
        return GL_RGB;
    else if (colorFormat == "GL_RGBA")
        return GL_RGBA;
    else if (colorFormat == "GL_RGBA16")
        return GL_RGBA16;
    else if (colorFormat == "GL_RGB16F_ARB")
        return GL_RGB16F_ARB;
    else if (colorFormat == "GL_RGBA16F_ARB")
        return GL_RGBA16F_ARB;
    else if (colorFormat == "GL_RGBA32F_ARB")
        return GL_RGBA32F_ARB;
    else {
        if (colorFormat != "")
            LWARNING("Unknown color format specified for port '" + symbol->getAnnotationValueString("name") + "' (assuming GL_RGBA16)");
        return GL_RGBA16;
    }
}

void DynamicGLSLProcessor::addNewInport(glslparser::GLSLVariableSymbol* symbol) {
    std::string portName = symbol->getAnnotationValueString("name");
    // add new inport, define it based on sampler uniforms
    switch (symbol->getNumInternalElements()) {
            case 1 : {
                LWARNING("No sampler1D as inport allowed. Uniform declaration ignored.");
                break;
            }
            case 2 : {
                LINFO("Adding image inport '" + portName + "'");
                RenderPort* imgInport = new RenderPort(Port::INPORT, portName, false, Processor::INVALID_RESULT, getColorFormat(symbol));
                addPort(imgInport);
                initializePort(imgInport);
                break;
            }
            case 3 : {
                LINFO("Adding volume inport '" + portName + "'");
                VolumePort* volInport = new VolumePort(Port::INPORT, portName);
                addPort(volInport);
                initializePort(volInport);
                break;
            }
    }
}

void DynamicGLSLProcessor::updateInports(glslparser::GLSLVariableSymbol* symbol) {
    std::string portName = symbol->getAnnotationValueString("name");
    if (!portName.empty()) {
        if (getPort(portName)) {
            Port* oldPort = getPort(portName);
            if (oldPort->isOutport()) {
                // old port was an outport, but now its an inport
                removeOldPort(oldPort);
                addNewInport(symbol);
            }
            else {
                if (dynamic_cast<RenderPort*>(oldPort)) {
                    // old port was a render port, but now its a volume port
                    if (symbol->getNumInternalElements() != 2) {
                        removeOldPort(oldPort);
                        addNewInport(symbol);
                    }
                }
                else if (dynamic_cast<VolumePort*>(oldPort)) {
                    // old port was a volume port, but now its a render port
                    if (symbol->getNumInternalElements() != 3) {
                        removeOldPort(oldPort);
                        addNewInport(symbol);
                    }
                }
            }
        }
        else
            addNewInport(symbol);

        if (symbol->getNumInternalElements() == 2 || symbol->getNumInternalElements() == 3)
            inportIDs_.push_back(InportStruct(portName, symbol->getID(),
                                              symbol->getAnnotationValueString("depthtex"),
                                              symbol->getAnnotationValueString("params")));
    }
}

void DynamicGLSLProcessor::updateOutports(glslparser::GLSLVariableSymbol* symbol) {
    if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_FLOAT &&
        symbol->getNumInternalElements() == 4) {
        std::string portName = symbol->getAnnotationValueString("name");
        if (!portName.empty()) {
            if (getPort(portName)) {
                Port* oldPort = getPort(portName);
                if (oldPort->isInport()) {
                    removeOldPort(oldPort);
                    RenderPort* outport = new RenderPort(RenderPort::OUTPORT, portName, false, Processor::INVALID_RESULT, getColorFormat(symbol));
                    addPort(outport);
                    initializePort(outport);
                }
            }
            else {
                LINFO("Adding outport '" + portName + "'");
                RenderPort* outport = new RenderPort(RenderPort::OUTPORT, portName, false, Processor::INVALID_RESULT, getColorFormat(symbol));
                addPort(outport);
                initializePort(outport);
            }
            outportIDs_.push_back(portName);
        }
        else LWARNING("Annotation 'name' not defined for '" + symbol->getID() + "'.");
    }
    else LWARNING("Found an 'out' declaration, which is not of type 'vec4'. Ignoring for now.");
}

void DynamicGLSLProcessor::addNewProperty(glslparser::GLSLVariableSymbol* symbol) {
    std::string propertyName = symbol->getAnnotationValueString("name");
    std::string labelText = symbol->getAnnotationValueString("label");
    if (propertyName != "" && labelText != "") {
        switch (symbol->getInternalType()) {
            case glslparser::GLSLSymbol::INTERNAL_BOOL : {
                if (symbol->getNumInternalElements() == 1) {
                    LINFO("Adding bool property '" + symbol->getID() + "'");
                    BoolProperty* boolProp = new BoolProperty(propertyName, labelText);
                    updatePropertyValues(boolProp, symbol);
                    addProperty(boolProp);
                }
                else
                    LWARNING("Type of uniform '" + symbol->getID() + "' not supported.");
                break;
            }
            case glslparser::GLSLSymbol::INTERNAL_FLOAT : {
                if (symbol->getNumInternalElements() == 1) {
                    LINFO("Adding float property '" + symbol->getID() + "'");
                    FloatProperty* floatProp = new FloatProperty(propertyName, labelText);
                    updatePropertyValues(floatProp, symbol);
                    addProperty(floatProp);
                }
                else if (symbol->getNumInternalElements() == 2) {
                    LINFO("Adding vec2 property '" + symbol->getID() + "'");
                    FloatVec2Property* vec2Prop = new FloatVec2Property(propertyName, labelText, tgt::vec2(0.0));
                    updatePropertyValues(vec2Prop, symbol);
                    addProperty(vec2Prop);
                }
                else if (symbol->getNumInternalElements() == 3) {
                    LINFO("Adding vec3 property '" + symbol->getID() + "'");
                    FloatVec3Property* vec3Prop = new FloatVec3Property(propertyName, labelText, tgt::vec3(0.0));
                    updatePropertyValues(vec3Prop, symbol);
                    addProperty(vec3Prop);
                }
                else if (symbol->getNumInternalElements() == 4) {
                    LINFO("Adding vec4 property '" + symbol->getID() + "'");
                    FloatVec4Property* vec4Prop = new FloatVec4Property(propertyName, labelText, tgt::vec4(0.0));
                    updatePropertyValues(vec4Prop, symbol);
                    addProperty(vec4Prop);
                }
                else
                    LWARNING("Type of uniform '" + symbol->getID() + "' not supported.");
                break;
            }
            case glslparser::GLSLSymbol::INTERNAL_INT :
            case glslparser::GLSLSymbol::INTERNAL_UINT : {
                if (symbol->getNumInternalElements() == 1) {
                    LINFO("Adding int property '" + symbol->getID() + "'");
                    IntProperty* intProp = new IntProperty(propertyName, labelText);
                    updatePropertyValues(intProp, symbol);
                    addProperty(intProp);
                }
                else if (symbol->getNumInternalElements() == 2) {
                    LINFO("Adding ivec2 property '" + symbol->getID() + "'");
                    IntVec2Property* ivec2Prop = new IntVec2Property(propertyName, labelText, tgt::ivec2(0));
                    updatePropertyValues(ivec2Prop, symbol);
                    addProperty(ivec2Prop);
                }
                else if (symbol->getNumInternalElements() == 3) {
                    LINFO("Adding ivec3 property '" + symbol->getID() + "'");
                    IntVec3Property* ivec3Prop = new IntVec3Property(propertyName, labelText, tgt::ivec3(0));
                    updatePropertyValues(ivec3Prop, symbol);
                    addProperty(ivec3Prop);
                }
                else if (symbol->getNumInternalElements() == 4) {
                    LINFO("Adding ivec4 property '" + symbol->getID() + "'");
                    IntVec4Property* ivec4Prop = new IntVec4Property(propertyName, labelText, tgt::ivec4(0));
                    updatePropertyValues(ivec4Prop, symbol);
                    addProperty(ivec4Prop);
                }
                else
                    LWARNING("Type of uniform '" + symbol->getID() + "' not supported.");
                break;
            }
            default :
                LWARNING("Type of uniform '" + symbol->getID() + "' not supported.");
        }
    }
}

void DynamicGLSLProcessor::removeOldProperty(Property* property) {
    removeProperty(property);
    if (std::find(baseClassProperties_.begin(), baseClassProperties_.end(), property->getID()) == baseClassProperties_.end()) {
        LINFO("Removing property '" + property->getID() + "'");
        property->disconnectWidgets();
        delete property;
    }
}


void DynamicGLSLProcessor::removeOldPort(Port* port) {
    LINFO("Removing port '" + port->getName() + "'");
    deinitializePort(port);
    removePort(port);
}

void DynamicGLSLProcessor::updatePropertyValues(Property* property, glslparser::GLSLVariableSymbol* symbol) {
    property->setGuiName(symbol->getAnnotationValueString("label"));
    // obtain the set values from the parser
    const glslparser::GLSLAnnotation* value = symbol->getAnnotation("value");
    const glslparser::GLSLAnnotation* minValue = symbol->getAnnotation("min");
    const glslparser::GLSLAnnotation* maxValue = symbol->getAnnotation("max");
    // check which property type we have and update its values accordingly
    if (dynamic_cast<BoolProperty*>(property)) {
        BoolProperty* castedProp = dynamic_cast<BoolProperty*>(property);
        if (value) castedProp->set(value->getValueAs<bool>()[0]);
    } else if (dynamic_cast<FloatProperty*>(property)) {
        FloatProperty* castedProp = dynamic_cast<FloatProperty*>(property);
        const glslparser::GLSLAnnotation* value = symbol->getAnnotation("value");
        if (minValue) castedProp->setMinValue(minValue->getValueAs<float>()[0]);
        if (maxValue) castedProp->setMaxValue(maxValue->getValueAs<float>()[0]);
        if (value) castedProp->set(value->getValueAs<float>()[0]);
    } else if (dynamic_cast<FloatVec2Property*>(property)) {
        FloatVec2Property* castedProp = dynamic_cast<FloatVec2Property*>(property);
        if (minValue) castedProp->setMinValue(tgt::vec2(minValue->getValueAs<float>()[0], value->getValueAs<float>()[1]));
        if (maxValue) castedProp->setMaxValue(tgt::vec2(maxValue->getValueAs<float>()[0], value->getValueAs<float>()[1]));
        if (value) castedProp->set(tgt::vec2(value->getValueAs<float>()[0], value->getValueAs<float>()[1]));
    } else if (dynamic_cast<FloatVec3Property*>(property)) {
        FloatVec3Property* castedProp = dynamic_cast<FloatVec3Property*>(property);
        if (minValue) castedProp->setMinValue(tgt::vec3(minValue->getValueAs<float>()[0], value->getValueAs<float>()[1], value->getValueAs<float>()[2]));
        if (maxValue) castedProp->setMaxValue(tgt::vec3(maxValue->getValueAs<float>()[0], value->getValueAs<float>()[1], value->getValueAs<float>()[2]));
        if (value) castedProp->set(tgt::vec3(value->getValueAs<float>()[0], value->getValueAs<float>()[1], value->getValueAs<float>()[2]));
    } else if (dynamic_cast<FloatVec4Property*>(property)) {
        FloatVec4Property* castedProp = dynamic_cast<FloatVec4Property*>(property);
        if (minValue) castedProp->setMinValue(tgt::vec4(minValue->getValueAs<float>()[0], value->getValueAs<float>()[1], value->getValueAs<float>()[2], value->getValueAs<float>()[3]));
        if (maxValue) castedProp->setMaxValue(tgt::vec4(maxValue->getValueAs<float>()[0], value->getValueAs<float>()[1], value->getValueAs<float>()[2], value->getValueAs<float>()[3]));
        if (value) castedProp->set(tgt::vec4(value->getValueAs<float>()[0], value->getValueAs<float>()[1], value->getValueAs<float>()[2], value->getValueAs<float>()[3]));
    } else if (dynamic_cast<IntProperty*>(property)) {
        IntProperty* castedProp = dynamic_cast<IntProperty*>(property);
        if (minValue) castedProp->setMinValue(minValue->getValueAs<int>()[0]);
        if (maxValue) castedProp->setMaxValue(maxValue->getValueAs<int>()[0]);
        if (value) castedProp->set(value->getValueAs<int>()[0]);
    } else if (dynamic_cast<IntVec2Property*>(property)) {
        IntVec2Property* castedProp = dynamic_cast<IntVec2Property*>(property);
        if (minValue) castedProp->setMinValue(tgt::ivec2(minValue->getValueAs<int>()[0], value->getValueAs<int>()[1]));
        if (maxValue) castedProp->setMaxValue(tgt::ivec2(maxValue->getValueAs<int>()[0], value->getValueAs<int>()[1]));
        if (value) castedProp->set(tgt::ivec2(value->getValueAs<int>()[0], value->getValueAs<int>()[1]));
    } else if (dynamic_cast<IntVec3Property*>(property)) {
        IntVec3Property* castedProp = dynamic_cast<IntVec3Property*>(property);
        if (minValue) castedProp->setMinValue(tgt::ivec3(minValue->getValueAs<int>()[0], value->getValueAs<int>()[1], value->getValueAs<int>()[2]));
        if (maxValue) castedProp->setMaxValue(tgt::ivec3(maxValue->getValueAs<int>()[0], value->getValueAs<int>()[1], value->getValueAs<int>()[2]));
        if (value) castedProp->set(tgt::ivec3(value->getValueAs<int>()[0], value->getValueAs<int>()[1], value->getValueAs<int>()[2]));
    } else if (dynamic_cast<IntVec4Property*>(property)) {
        IntVec4Property* castedProp = dynamic_cast<IntVec4Property*>(property);
        if (minValue) castedProp->setMinValue(tgt::ivec4(minValue->getValueAs<int>()[0], value->getValueAs<int>()[1], value->getValueAs<int>()[2], value->getValueAs<int>()[3]));
        if (maxValue) castedProp->setMaxValue(tgt::ivec4(maxValue->getValueAs<int>()[0], value->getValueAs<int>()[1], value->getValueAs<int>()[2], value->getValueAs<int>()[3]));
        if (value) castedProp->set(tgt::ivec4(value->getValueAs<int>()[0], value->getValueAs<int>()[1], value->getValueAs<int>()[2], value->getValueAs<int>()[3]));
    }
}

void DynamicGLSLProcessor::updateProperties(glslparser::GLSLVariableSymbol* symbol) {
    std::string propertyName = symbol->getAnnotationValueString("name");
    if (propertyName != "") {
        if (getProperty(propertyName)) {
            // a property with the same name exists
            bool onlyUpdateValues = false;
            Property* oldProperty = getProperty(propertyName);
            if (dynamic_cast<BoolProperty*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_BOOL)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<FloatProperty*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_FLOAT &&
                    symbol->getNumInternalElements() == 1)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<FloatVec2Property*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_FLOAT &&
                    symbol->getNumInternalElements() == 2)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<FloatVec3Property*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_FLOAT &&
                    symbol->getNumInternalElements() == 3)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<FloatVec4Property*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_FLOAT &&
                    symbol->getNumInternalElements() == 4)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<IntProperty*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_INT &&
                    symbol->getNumInternalElements() == 1)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<IntVec2Property*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_INT &&
                    symbol->getNumInternalElements() == 2)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<IntVec3Property*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_INT &&
                    symbol->getNumInternalElements() == 3)
                    onlyUpdateValues = true;
            }
            else if (dynamic_cast<IntVec4Property*>(oldProperty)) {
                if (symbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_INT &&
                    symbol->getNumInternalElements() == 4)
                    onlyUpdateValues = true;
            }

            if (onlyUpdateValues) {
                // we have the same property type and only the values are updated
                updatePropertyValues(oldProperty, symbol);
            }
            else {
                // the property type has changed and a new property is generated
                removeOldProperty(oldProperty);
                addNewProperty(symbol);
            }
        }
        else
            addNewProperty(symbol);

        propertyIDs_.push_back(propertyName);
    }
}

void DynamicGLSLProcessor::initializePortsAndProperties() {
    if (isInitialized()) {
        shader_->rebuild();
        getProcessorWidget()->updateFromProcessor();
    }

    // clear vectors of currently used IDs
    inportIDs_.clear();
    outportIDs_.clear();
    propertyIDs_.clear();
    // avoid removing shader property later on in this method
    propertyIDs_.push_back("shader");

    // analyze fragment shader and extract ports
    if (!shader_->get().fragmentSource_.empty()) {
        std::istringstream shaderStream(shader_->get().fragmentSource_);
        glslparser::GLSLProgram glsl(&shaderStream);

        glsl.setShaderHeader(generateHeader() + "\n #define TF_SAMPLER_TYPE sampler1D\n");

        if (glsl.parse()) {
            const std::vector<glslparser::GLSLVariableSymbol*>& uniforms = glsl.getUniformDeclarations();
            // uniforms are handled as inports or properties
            for (unsigned int i=0; i<uniforms.size(); i++) {
                glslparser::GLSLVariableSymbol* curSymbol = uniforms[i];
                if (curSymbol->getInternalType() == glslparser::GLSLSymbol::INTERNAL_SAMPLER)
                    // found a "sampler" symbol, which is handled as inport
                    updateInports(curSymbol);
                else
                    // everything else is tried to be handled as property
                    updateProperties(curSymbol);
            }

            const std::vector<glslparser::GLSLVariableSymbol*>& outs = glsl.getOutDeclarations();
            // out symbols are handled as outports
            for (unsigned int i=0; i<outs.size(); i++) {
                glslparser::GLSLVariableSymbol* curSymbol = outs[i];
                updateOutports(curSymbol);
            }

        }
    }

    // remove obsolete inports
    std::vector<Port*> inports = getInports();
    for (unsigned int i=0; i<inports.size(); i++) {
        Port* curPort = inports[i];
        bool curPortInUse = false;
        for (unsigned int i=0; i<inportIDs_.size(); i++)
            if (inportIDs_[i].name_ == curPort->getName())
                curPortInUse = true;
        if (!curPortInUse)
            removeOldPort(curPort);
    }
    // remove obsolete outports
    std::vector<Port*> outports = getOutports();
    for (unsigned int i=0; i<outports.size(); i++) {
        Port* curPort = outports[i];
        if (std::find(outportIDs_.begin(), outportIDs_.end(), curPort->getName()) == outportIDs_.end())
            removeOldPort(curPort);
    }
    // remove obsolete properties
    std::vector<Property*> properties = getProperties();
    for (unsigned int i=0; i<properties.size(); i++) {
        Property* curProp = properties[i];
        if (std::find(propertyIDs_.begin(), propertyIDs_.end(), curProp->getID()) == propertyIDs_.end())
            removeOldProperty(curProp);
    }

    if (isInitialized())
        setupPortGroup();

    // inform the observers that ports and properties might have changed
    std::vector<ProcessorObserver*> observers = Observable<ProcessorObserver>::getObservers();
    for (size_t i = 0; i < observers.size(); ++i)
        observers[i]->portsAndPropertiesChanged(this);
}

void DynamicGLSLProcessor::serialize(XmlSerializer& s) const {
    VolumeRaycaster::serialize(s);
    try {
        shader_->serialize(s);
    }
    catch (SerializationException &e) {
        LWARNING(std::string("Shader serialization failed: ") + e.what());
    }
}

void DynamicGLSLProcessor::deserialize(XmlDeserializer& s) {
    try {
        shader_->deserialize(s);
    }
    catch (XmlSerializationNoSuchDataException) {
        // just no shader data present => ignore
        s.removeLastError();
    }
    catch (SerializationException &e) {
        LWARNING(std::string("Shader deserialization failed: ") + e.what());
    }
    initializePortsAndProperties();
    VolumeRaycaster::deserialize(s);
}


DynamicGLSLProcessor::InportStruct::InportStruct()
{}

DynamicGLSLProcessor::InportStruct::InportStruct(const std::string& name, const std::string& mainSamplerIdentifier,
                                                 const std::string& depthSamplerIdentifier, const std::string& paramsIdentifier)
                                                 : name_(name),
                                                 mainSamplerIdentifier_(mainSamplerIdentifier),
                                                 depthSamplerIdentifier_(depthSamplerIdentifier),
                                                 paramsIdentifier_(paramsIdentifier)
{}

DynamicGLSLProcessor::InportStruct::InportStruct(const std::string& name, const std::string& mainSamplerIdentifier,
                                                 const std::string& depthSamplerIdentifier, const std::string& paramsIdentifier,
                                                 GLint mainTexUnit, GLint depthTexUnit)
                                                 : name_(name),
                                                   mainSamplerIdentifier_(mainSamplerIdentifier),
                                                   depthSamplerIdentifier_(depthSamplerIdentifier),
                                                   paramsIdentifier_(paramsIdentifier),
                                                   mainTexUnit_(mainTexUnit),
                                                   depthTexUnit_(depthTexUnit)
{}

} // namespace voreen
