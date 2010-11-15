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

#include "voreen/modules/base/processors/geometry/pointlistrenderer.h"

#include "voreen/core/datastructures/geometry/pointlistgeometry.h"
#include "voreen/core/datastructures/geometry/pointsegmentlistgeometry.h"
#include "voreen/core/utils/voreenpainter.h"

#include "voreen/core/datastructures/transfunc/transfunc.h"

#include "tgt/material.h"

namespace voreen {

using tgt::vec3;
using tgt::vec4;

const std::string PointListRenderer::loggerCat_("voreen.PointListRenderer");

/*
    constructor
*/
PointListRenderer::PointListRenderer()
    : GeometryRendererBase()
    , displayList_(0)
    , coordinateSystem_("coordinateSystem", "Coordinate System")
    , renderingPrimitiveProp_("renderingPrimitive", "Rendering Primitive")
    , color_("color", "Primitive Color", tgt::Color(0.75f, 0.25f, 0.f, 1.f))
    , depthTest_("depthTest", "Depth Test", true)
    , pointSize_("pointSize", "Point Size", 3.f, 1.f, 20.f)
    , pointSmooth_("pointSmooth", "Point Smooth", false)
    , sphereDiameter_("sphereDiameter", "Sphere Diameter", 0.01f, 0.001f, 0.1f)
    , sphereSlicesStacks_("sphereSlicesStacks", "Sphere Slices/Stacks", 20, 10, 100)
    , geometryInport_(Port::INPORT, "geometry.input")
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

    coordinateSystem_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    renderingPrimitiveProp_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    color_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    depthTest_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    pointSize_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    pointSmooth_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    sphereDiameter_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));
    sphereSlicesStacks_.onChange(CallMemberAction<PointListRenderer>(this, &PointListRenderer::invalidateDisplayList));

    pointSize_.setStepping(0.5f);
    pointSize_.setNumDecimals(1);
    sphereDiameter_.setStepping(0.001f);
    sphereDiameter_.setNumDecimals(3);

    addProperty(coordinateSystem_);
    addProperty(renderingPrimitiveProp_);
    addProperty(color_);
    addProperty(depthTest_);
    addProperty(pointSize_);
    addProperty(pointSmooth_);
    addProperty(sphereDiameter_);
    addProperty(sphereSlicesStacks_);

    addPort(geometryInport_);
}

PointListRenderer::~PointListRenderer() {
    if (glIsList(displayList_))
        glDeleteLists(displayList_, 1);
}

std::string PointListRenderer::getProcessorInfo() const {
    return "Renders a list of points (PointListGeometryVec3 or PointSegmentListGeometryVec3) \
            as point primitives, spheres or illuminated spheres.<br>(see GeometrySource)";
}

void PointListRenderer::process() {
    tgtAssert(geometryInport_.isReady(), "inport not ready");
    // force display list update, if input has changed
    if (geometryInport_.hasChanged())
        invalidateDisplayList();
}

void PointListRenderer::render() {

    tgtAssert(geometryInport_.isReady(), "inport not ready");

    // regenerate display list, if input data has changed
    if (geometryInport_.hasChanged() || displayList_ == 0) {

        if (glIsList(displayList_))
            glDeleteLists(displayList_, 1);
        displayList_ = 0;

        // cast geometry to PointListGeometry or PointSegmentListGeometry and generate display list
        PointListGeometry<vec3>* pointList = dynamic_cast< PointListGeometry<vec3>* >(geometryInport_.getData());
        PointSegmentListGeometry<vec3>* segmentList = dynamic_cast< PointSegmentListGeometry<vec3>* >(geometryInport_.getData());
        if (pointList)
            generateDisplayList(pointList->getData());
        else if (segmentList)
            generateDisplayList(segmentList->getPoints());

        // message on invalid geometry
        if (!pointList && !segmentList) {
            LWARNING("Invalid geometry. PointListGeometry<vec3> or PointSegmentListGeometry<vec3> expected.");
        }
    }

    // render display list
    if (glIsList(displayList_)) {
        glCallList(displayList_);
    }
}

void PointListRenderer::generateDisplayList(const std::vector<vec3>& pointList) {

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
        glDisable(GL_DEPTH_TEST);

    // enable lighting for illuminated spheres
    if (renderingPrimitiveProp_.get() == "illuminated-spheres") {

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glLightfv(GL_LIGHT0, GL_AMBIENT, vec4(0.3f, 0.3f, 0.3f, 1.f).elem);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, vec4(0.6f, 0.6f, 0.6f, 1.f).elem);
        glLightfv(GL_LIGHT0, GL_SPECULAR, vec4(0.6f, 0.6f, 0.6f, 1.f).elem);
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 128.f);

        tgt::Material material(color_.get(), color_.get(), color_.get(), 75.f);
        material.activate();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glLightfv(GL_LIGHT0, GL_POSITION, vec4(1.f, 1.f, 10.f, 1.f).elem);
        glPopMatrix();
    }

    // render: point primitives
    if (renderingPrimitiveProp_.get() == "points") {
        glColor4fv(color_.get().elem);
        glPointSize(pointSize_.get());
        if (pointSmooth_.get())
            glEnable(GL_POINT_SMOOTH);
        glBegin(GL_POINTS);
        for (size_t i=0; i<pointList.size(); ++i) {
            tgt::vertex(pointList[i]);
        }
        glEnd();
    }

    // render: line strip
    if (renderingPrimitiveProp_.get() == "line-strip") {
        glColor4fv(color_.get().elem);
        glLineWidth(pointSize_.get());
        if (pointSmooth_.get())
            glEnable(GL_LINE_SMOOTH);
        glBegin(GL_LINE_STRIP);
        for (size_t i=0; i<pointList.size(); ++i) {
            tgt::vertex(pointList[i]);
        }
        glEnd();
    }

    // render: spheres
    else if (renderingPrimitiveProp_.get() == "spheres" || renderingPrimitiveProp_.get() == "illuminated-spheres") {
        GLUquadricObj* quadric = gluNewQuadric();
        glColor4fv(color_.get().elem);
        for (size_t i=0; i<pointList.size(); ++i) {
            glPushMatrix();
            tgt::translate(pointList[i]);
            gluSphere(quadric, sphereDiameter_.get(), sphereSlicesStacks_.get(), sphereSlicesStacks_.get());
            glPopMatrix();
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

void PointListRenderer::invalidateDisplayList() {

    if (!isInitialized())
        return;

    if (glIsList(displayList_))
        glDeleteLists(displayList_, 1);
    displayList_ = 0;

}

} // namespace voreen
