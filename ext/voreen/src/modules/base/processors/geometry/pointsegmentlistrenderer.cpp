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

#include "voreen/modules/base/processors/geometry/pointsegmentlistrenderer.h"

#include "voreen/core/datastructures/geometry/pointlistgeometry.h"
#include "voreen/core/datastructures/geometry/pointsegmentlistgeometry.h"
#include "voreen/core/utils/voreenpainter.h"
#include "voreen/core/datastructures/transfunc/transfunc.h"

#include "tgt/material.h"

namespace voreen {

using tgt::vec3;
using tgt::vec4;

const std::string PointSegmentListRenderer::loggerCat_("voreen.PointSegmentListRenderer");


PointSegmentListRenderer::PointSegmentListRenderer()
    : GeometryRendererBase(),
      geometry_(0),
      displayList_(0),
      coordinateSystem_("coordinateSystem", "Coordinate System"),
      renderingPrimitiveProp_("renderingPrimitive", "Rendering Primitive"),
      applyUniformColor_("applyUniformColor", "Apply Uniform Color", false),
      color_("color", "Primitive Color", tgt::Color(0.75f, 0.25f, 0.f, 1.f)),
      depthTest_("depthTest", "Depth Test", true),
      pointSize_("pointSize", "Point Size", 3.f, 1.f, 20.f),
      pointSmooth_("pointSmooth", "Point Smooth", false),
      sphereDiameter_("sphereDiameter", "Sphere Diameter", 0.01f, 0.001f, 0.1f),
      sphereSlicesStacks_("sphereSlicesStacks", "Sphere Slices/Stacks", 20, 10, 100),
      geometryInport_(Port::INPORT, "geometry.input")
{
    color_.setViews(Property::COLOR);

    // Coordinate systems
    coordinateSystem_.addOption("world", "World coordinates");
    coordinateSystem_.addOption("viewport", "Viewport coordinates");
    coordinateSystem_.addOption("untransformed", "No transformation");

    // Rendering primitives
    renderingPrimitiveProp_.addOption("points", "Points");
    renderingPrimitiveProp_.addOption("line-strip", "Line Strip");
    renderingPrimitiveProp_.addOption("spheres", "Spheres");
    renderingPrimitiveProp_.addOption("illuminated-spheres", "Illuminated Spheres");

    // Segment colors
    std::vector<Property*> colorProps;
    for (int i=0; i<100; ++i) {
        std::ostringstream segmentID;
        segmentID << i;
        FloatVec4Property* colorProp = new FloatVec4Property("segmentColor" + segmentID.str(), "Segment " + segmentID.str(),
            tgt::Color(0.75f, 0.25f, 0.f, 1.f));
        colorProp->setViews(Property::COLOR);
        colorProp->onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
        colorProps.push_back(colorProp);
    }
    segmentColors_ = new PropertyVector("segmentColors", "Segment Colors:", colorProps);
    segmentColors_->setViews(Property::COLOR);
    initializeColorMap();

    // display list invalidation callback
    coordinateSystem_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    renderingPrimitiveProp_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    applyUniformColor_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    color_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    depthTest_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    pointSize_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    pointSmooth_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    sphereDiameter_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));
    sphereSlicesStacks_.onChange(CallMemberAction<PointSegmentListRenderer>(this, &PointSegmentListRenderer::invalidateDisplayList));

    pointSize_.setStepping(0.5f);
    pointSize_.setNumDecimals(1);
    sphereDiameter_.setStepping(0.001f);
    sphereDiameter_.setNumDecimals(3);

    addProperty(coordinateSystem_);
    addProperty(renderingPrimitiveProp_);
    addProperty(applyUniformColor_);
    addProperty(color_);
    addProperty(segmentColors_);
    addProperty(depthTest_);
    addProperty(pointSize_);
    addProperty(pointSmooth_);
    addProperty(sphereDiameter_);
    addProperty(sphereSlicesStacks_);

    addPort(geometryInport_);
}

PointSegmentListRenderer::~PointSegmentListRenderer() {
    delete segmentColors_;
}

std::string PointSegmentListRenderer::getProcessorInfo() const {
    return "Renders a list of segments, each consisting of points (PointSegmentListGeometryVec3),  \
            as point primitives, spheres or illuminated spheres. Each segment can be assigned a different color.\
            <br>(see GeometrySource)";
}

void PointSegmentListRenderer::process() {
    tgtAssert(geometryInport_.isReady(), "inport not ready");
    // force display list update, if input has changed
    if (geometryInport_.hasChanged())
        invalidateDisplayList();
}

void PointSegmentListRenderer::render() {

    tgtAssert(geometryInport_.isReady(), "inport not ready");

    // regenerate display list, if input data has changed
    if (geometryInport_.hasChanged() || displayList_ == 0) {

        if (glIsList(displayList_))
            glDeleteLists(displayList_, 1);
        displayList_ = 0;

        // cast geometry to PointListGeometry and generate display list
        PointSegmentListGeometry<vec3>* segmentList = dynamic_cast< PointSegmentListGeometry<vec3>* >(geometryInport_.getData());
        if (segmentList)
            generateDisplayList(segmentList->getData());
        else
            LWARNING("Invalid geometry. PointSegmentListGeometry<vec3> expected.");
    }

    // render display list
    if (glIsList(displayList_)) {
        glCallList(displayList_);
    }
}

void PointSegmentListRenderer::generateDisplayList(const std::vector<std::vector<vec3> >& segmentList) {

    if (glIsList(displayList_))
        glDeleteLists(displayList_, 1);

    displayList_ = glGenLists(1);
    glNewList(displayList_, GL_COMPILE);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (coordinateSystem_.get() != "world") {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        if (coordinateSystem_.get() == "viewport") {
            glTranslatef(-1.f, -1.f, 0.f);
            glScalef(2.f/viewport_.x, 2.f/viewport_.y, 1.f);
        }
    }

    if (!depthTest_.get())
        glDepthFunc(GL_ALWAYS);

    // enable lighting for illuminated spheres
    if (renderingPrimitiveProp_.get() == "illuminated-spheres") {

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glLightfv(GL_LIGHT0, GL_AMBIENT, vec4(0.3f, 0.3f, 0.3f, 1.f).elem);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4(0.6f, 0.6f, 0.6f, 1.f).elem);
        glLightfv(GL_LIGHT0, GL_SPECULAR, vec4(0.6f, 0.6f, 0.6f, 1.f).elem);
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 128.f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glLightfv(GL_LIGHT0, GL_POSITION, vec4(1.f, 1.f, 10.f, 1.f).elem);
        glPopMatrix();
    }

    // render: point primitives
    if (renderingPrimitiveProp_.get() == "points") {

        if (applyUniformColor_.get())
            glColor4fv(color_.get().elem);
        glPointSize(pointSize_.get());
        if (pointSmooth_.get())
            glEnable(GL_POINT_SMOOTH);

        glBegin(GL_POINTS);
        for (size_t i=0; i<segmentList.size(); ++i) {
            if (!applyUniformColor_.get())
                glColor4fv(segmentColors_->getProperty<FloatVec4Property*>(i % segmentColors_->size())->get().elem);
            for (size_t j=0; j<segmentList[i].size(); ++j) {
                tgt::vertex(segmentList[i][j]);
            }
        }
        glEnd();
    }

    // render: line strip
    if (renderingPrimitiveProp_.get() == "line-strip") {

        if (applyUniformColor_.get())
            glColor4fv(color_.get().elem);
        glLineWidth(pointSize_.get());
        if (pointSmooth_.get())
            glEnable(GL_LINE_SMOOTH);

        for (size_t i=0; i<segmentList.size(); ++i) {
            glBegin(GL_LINE_STRIP);
            if (!applyUniformColor_.get())
                glColor4fv(segmentColors_->getProperty<FloatVec4Property*>(i % segmentColors_->size())->get().elem);
            for (size_t j=0; j<segmentList[i].size(); ++j) {
                tgt::vertex(segmentList[i][j]);
            }
            glEnd();
        }
    }

    // render: spheres
    else if (renderingPrimitiveProp_.get() == "spheres" || renderingPrimitiveProp_.get() == "illuminated-spheres") {

        // apply uniform color once
        if (applyUniformColor_.get()) {
            if (renderingPrimitiveProp_.get() == "spheres")
                glColor4fv(color_.get().elem);
            else if (renderingPrimitiveProp_.get() == "illuminated-spheres") {
                tgt::Material material(color_.get(), color_.get(), color_.get(), 75.f);
                material.activate();
            }

        }

        GLUquadricObj* quadric = gluNewQuadric();
        for (size_t i=0; i<segmentList.size(); ++i) {
            for (size_t j=0; j<segmentList[i].size(); ++j) {

                // apply segment color
                if (!applyUniformColor_.get()) {
                    tgt::Color segColor = segmentColors_->getProperty<FloatVec4Property*>(i % segmentColors_->size())->get();
                    if (renderingPrimitiveProp_.get() == "spheres")
                        glColor4fv(segColor.elem);
                    else if (renderingPrimitiveProp_.get() == "illuminated-spheres") {
                        tgt::Material material(segColor, segColor, segColor, 75.f);
                        material.activate();
                    }
                }

                glPushMatrix();
                tgt::translate(segmentList[i][j]);
                gluSphere(quadric, sphereDiameter_.get(), sphereSlicesStacks_.get(), sphereSlicesStacks_.get());
                glPopMatrix();

            }
        }
        gluDeleteQuadric(quadric);

    }

    if (coordinateSystem_.get() != "world") {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    glPopAttrib();
    glEndList();
}

void PointSegmentListRenderer::invalidateDisplayList() {

    if (!isInitialized())
        return;

    if (glIsList(displayList_))
        glDeleteLists(displayList_, 1);
    displayList_ = 0;

}

void PointSegmentListRenderer::initializeColorMap() {

    if (!segmentColors_) {
        LWARNING("SegmentColors property vector not initialized");
        return;
    }

    std::vector<tgt::Color> colorMap;
    colorMap.push_back(vec4(255,0,0,255) / 255.f);
    colorMap.push_back(vec4(0,255,0,255) / 255.f);
    colorMap.push_back(vec4(0,0,255,255) / 255.f);
    colorMap.push_back(vec4(255,0,255,255) / 255.f);
    colorMap.push_back(vec4(0,255,255,255) / 255.f);
    colorMap.push_back(vec4(255,255,0,255) / 255.f);
    colorMap.push_back(vec4(255,100,20,255) / 255.f);
    colorMap.push_back(vec4(250,200,150,255) / 255.f);
    colorMap.push_back(vec4(150,200,250,255) / 255.f);
    colorMap.push_back(vec4(30,30,30,255) / 255.f);

    for (int i=0; i<segmentColors_->size(); ++i)
        segmentColors_->getProperty<FloatVec4Property*>(i)->set(colorMap[i % colorMap.size()]);

}


} // namespace voreen
