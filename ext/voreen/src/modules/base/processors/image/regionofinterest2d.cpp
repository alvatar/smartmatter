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

#include "voreen/modules/base/processors/image/regionofinterest2d.h"

#include "voreen/core/datastructures/geometry/pointsegmentlistgeometry.h"
#include "voreen/core/voreenapplication.h"

#include "tgt/textureunit.h"

using tgt::TextureUnit;

namespace voreen {

using tgt::vec3;
using tgt::vec2;
using tgt::ivec2;
using tgt::MouseEvent;
using std::vector;

//---------------------------------------------------------------------------

namespace {

// GLU tesselation callbacks

#ifndef CALLBACK
#define CALLBACK
#endif

    void CALLBACK tessBegin(GLenum primitive) {
        glBegin(primitive);
    }

    void CALLBACK tessEnd() {
        glEnd();
    }

    void CALLBACK tessVertex2dv(GLdouble* data) {
        glVertex2dv(data);
    }

    void CALLBACK tessCombine(GLdouble coords[3],
        GLdouble* /*vertex_data[4]*/, GLfloat /*weight[4]*/,
        GLdouble** dataOut)
    {
        GLdouble* vertex = new GLdouble[3];
        vertex[0] = coords[0];
        vertex[1] = coords[1];
        vertex[2] = coords[2];

        *dataOut = vertex;
    }
}

//---------------------------------------------------------------------------

RegionOfInterest2D::RegionOfInterestGeometry::RegionOfInterestGeometry() :
    Geometry(),
    layer_(0),
    transformMatrix_(tgt::mat4::createIdentity()),
    geometryMode_(LINE_STRIP),
    roiColor_(tgt::Color::white),
    maskColor_(tgt::Color::white),
    size_(1.f),
    antialiasing_(false) {
}

void RegionOfInterest2D::RegionOfInterestGeometry::renderBoundingBox() const {
    // apply roi transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    tgt::multMatrix(transformMatrix_);

    tgt::vec2 ll = tgt::vec2(boundingBox_.x, boundingBox_.y);
    tgt::vec2 ur = tgt::vec2(boundingBox_.z, boundingBox_.w);
    tgt::vec2 lr = tgt::vec2(ur.x, ll.y);
    tgt::vec2 ul = tgt::vec2(ll.x, ur.y);

    glBegin(GL_LINE_LOOP);
    tgt::vertex(ll);
    tgt::vertex(lr);
    tgt::vertex(ur);
    tgt::vertex(ul);
    glEnd();

    glPopMatrix();

    LGL_ERROR;
}

void RegionOfInterest2D::RegionOfInterestGeometry::computeBoundingBox() {
    // compute bounding box
    tgt::vec2 ll = tgt::vec2(static_cast<float>(1<<20));
    tgt::vec2 ur = tgt::vec2(-1.f);
    for (size_t i=0; i<points_.size(); i++) {
        ll = tgt::min(ll, points_.at(i));
        ur = tgt::max(ur, points_.at(i));
    }
    boundingBox_ = tgt::vec4(ll.x, ll.y, ur.x, ur.y);
}

void RegionOfInterest2D::RegionOfInterestGeometry::render() {

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // apply roi transformation matrix
    glPushMatrix();
    tgt::multMatrix(transformMatrix_);

    // Use OpenGL anti-aliasing?
    if (antialiasing_) {
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
    }
    else {
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }

    LGL_ERROR;

    RegionOfInterest2D::RegionOfInterestGeometry::GeometryMode mode = geometryMode_;
    if (mode == POINTS || mode == LINES || mode == LINE_STRIP || mode == LINE_LOOP || mode == RECTANGLE) {

        // determine GL geometry mode
        GLenum glMode;
        switch (mode) {
                case POINTS:
                    glMode = GL_POINTS;
                    break;
                case LINES:
                    glMode = GL_LINES;
                    break;
                case LINE_STRIP:
                    glMode = GL_LINE_STRIP;
                    break;
                case LINE_LOOP:
                    glMode = GL_LINE_LOOP;
                    break;
                case RECTANGLE:
                    glMode = GL_QUADS;
                    break;
                default:
                    glMode = GL_POINTS;
                    break;
        }

        // render curve with specified line / point size
        if (geometryMode_ == POINTS)
            glPointSize(size_);
        else
            glLineWidth(size_);

        glBegin(glMode);
        for (size_t i = 0; i < points_.size(); ++i) {
            tgt::vertex(points_[i]);
        }
        glEnd();
        LGL_ERROR;

    }
    else if (mode == SPLINE) {

        // transform ROI points to spline points
        std::vector<tgt::vec2> roiPoints = points_;
        std::vector<vec3> splinePoints;
        float layer = static_cast<float>(layer_);
        vec3 last = tgt::vec3(roiPoints.front(), layer);
        last.z = 0.f;
        splinePoints.push_back(last);
        splinePoints.push_back(vec3(last));
        for (size_t i = 1; i < points_.size(); i++) {
            vec3 cur = tgt::vec3(roiPoints.at(i), layer);
            cur.z = 0.f;
            if (length(cur - last) > 0.f) {
                splinePoints.push_back(cur);
                last = cur;
            }
        }
        last = tgt::vec3(roiPoints.back(), layer);
        last.z = 0.f;
        splinePoints.push_back(last);
        splinePoints.push_back(last);

        // Create glu nurbs renderer
        GLUnurbsObj* theNurb = gluNewNurbsRenderer();

        // Create NURBS knots and control points
        const int order = 4;
        int nknots = splinePoints.size() + order;
        GLfloat* knots = new GLfloat[nknots];
        for (int i = 0; i < nknots; i++) {
            knots[i] = GLfloat(i);
        }
        GLfloat* ctlarray = new GLfloat[(nknots-order)*3];
        for (int i = 0; i < nknots-order; i++) {
            ctlarray[i*3] = splinePoints.at(i).x;
            ctlarray[i*3+1] = splinePoints.at(i).y;
            ctlarray[i*3+2] = splinePoints.at(i).z;
        }

        // render NURBS curve with specified size
        glLineWidth(size_);
        gluBeginCurve(theNurb);
        gluNurbsCurve(theNurb, nknots, knots, 3, ctlarray, 4, GL_MAP1_VERTEX_3);
        gluEndCurve(theNurb);

        delete[] knots;
        delete[] ctlarray;
        gluDeleteNurbsRenderer(theNurb);

        LGL_ERROR;
    }

    else if (mode == PATCH || mode == CIRCLE_PATCH) {

        // initialize GLU tesselator
        GLUtesselator* tobj = gluNewTess();
        gluTessCallback( tobj, GLU_TESS_BEGIN, ( void(CALLBACK*)() ) &tessBegin );
        gluTessCallback( tobj, GLU_TESS_END, ( void(CALLBACK*)() ) &tessEnd );
        gluTessCallback( tobj, GLU_TESS_VERTEX, ( void(CALLBACK*)() ) &tessVertex2dv );
        gluTessCallback( tobj, GLU_TESS_COMBINE, ( void(CALLBACK*)() ) &tessCombine );
        gluTessProperty( tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO );

        // tesselate freeform patch and write result to OpenGL feedback buffer
        gluTessBeginPolygon(tobj, NULL);
        gluTessBeginContour(tobj);
        for (size_t i=0; i < points_.size(); i++) {
            GLdouble* vertex = new GLdouble[3];
            vertex[0] = static_cast<double>(points_[i].x);
            vertex[1] = static_cast<double>(points_[i].y);
            vertex[2] = 0.0;
            gluTessVertex(tobj, vertex, vertex);
        }
        gluTessEndContour(tobj);
        gluTessEndPolygon(tobj);
        gluDeleteTess(tobj);

        LGL_ERROR;
    }
    else {
        LERROR("Unknown ROI geometry mode");
    }

    // revert roi transformation
    glPopMatrix();

    glPopAttrib();
    LGL_ERROR;
}

void RegionOfInterest2D::RegionOfInterestGeometry::serialize(XmlSerializer& s) const {
    s.serialize("points", points_);
    s.serialize("layer", layer_);
    s.serialize("viewport", viewportSize_);
    s.serialize("trafoMatrix", transformMatrix_);
    s.serialize("geometryMode", geometryMode_);
    s.serialize("roiColor", roiColor_);
    s.serialize("maskColor", maskColor_);
    s.serialize("size", size_);
    s.serialize("antialiasing", antialiasing_);
}

void RegionOfInterest2D::RegionOfInterestGeometry::deserialize(XmlDeserializer& s) {

    try {
        s.deserialize("points", points_);
        s.deserialize("viewport", viewportSize_);
        int mode;
        s.deserialize("geometryMode", mode);
        geometryMode_ = static_cast<RegionOfInterestGeometry::GeometryMode>(mode);
        s.deserialize("roiColor", roiColor_);
        s.deserialize("maskColor", maskColor_);
        s.deserialize("size", size_);
        s.deserialize("antialiasing", antialiasing_);
        s.deserialize("layer", layer_);
        s.deserialize("trafoMatrix", transformMatrix_);
    }
    catch (XmlSerializationNoSuchDataException&) {
        // superfluous or missing XML data item, just ignore
        s.removeLastError();
    }

    computeBoundingBox();
}

//---------------------------------------------------------------------------

RegionOfInterest2D::RegionOfInterest2D()
    : ImageProcessor("copyimage"),
      inport_(Port::INPORT, "inport"),
      outportMask_(Port::OUTPORT, "image.mask"),
      outportROI_(Port::OUTPORT, "image.roi"),
      outportOverlay_(Port::OUTPORT, "image.overlay"),
      pickingPort_(Port::OUTPORT, "pickingTarget"),
      outportROIGeometryLayer_(Port::OUTPORT, "geometry.roiGeometryLayer"),
      outportROIGeometry_(Port::OUTPORT, "geometry.roiGeometry"),
      geometryMode_("geometryMode", "Geometry"),
      roiColor_("roiColor", "ROI Color", tgt::Color(1.f, 1.f, 0.0f, 0.5f)),
      maskColor_("maskColor", "Mask Color", tgt::Color(1.f, 1.f, 1.f, 1.f)),
      maskBackgroundColor_("maskBackgroundColor", "Mask Background", tgt::Color(0.f, 0.f, 0.f, 0.f)),
      size_("size", "Line/Point size", 3.f, 1.f, 15.f),
      antialiasing_("antialiasing", "Antialiasing", false),
      currentLayer_("roiLayer", "ROI Layer", 0, 0, 1000),
      renderCurrentLayerOnly_("renderCurrentLayerOnly", "Render current layer only", false),
      boundingBoxColor_("boundingBoxColor", "Bounding Box Color", tgt::Color(0.8f, 0.8f, 0.8f, 0.8f)),
      minRoiPointDistance_("minRoiPointDistance", "Min ROI Point Distance", 2.f, 0.f, 20.f),
      roiTransformMatrix_("transformationMatrix", "Transformation Matrix", tgt::mat4::createIdentity(), tgt::mat4(-1e6), tgt::mat4(1e6)),
      loadROIs_("loadROIs", "Load ROIs", "Select ROI file", VoreenApplication::app()->getDataPath(),
        "Voreen ROI files (*.vrf)", FileDialogProperty::OPEN_FILE, Processor::VALID),
      saveROIs_("saveROIs", "Save ROIs", "Select ROI file", VoreenApplication::app()->getDataPath(),
        "Voreen ROI files (*.vrf)", FileDialogProperty::SAVE_FILE, Processor::VALID),
      clearROIs_("clearROIs", "Clear ROIs"),
      removeLastROI_("deleteLastRoi", "Remove Last ROI"),
      numberOfROIs_("numberOfRois", "Number of ROIs", 0, 0, 10000, Processor::VALID),
      mouseEventAddROI_("mouseEvent.addROI", "Add ROI", this, &RegionOfInterest2D::addROIEvent,
        tgt::MouseEvent::MOUSE_BUTTON_LEFT,
        static_cast<tgt::MouseEvent::MouseAction>(tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED),
        tgt::Event::CTRL),
      mouseEventRemoveLastROI_("mouseEvent.removeLastROI", "Remove Last ROI", this, &RegionOfInterest2D::removeLastROI,
        tgt::MouseEvent::MOUSE_BUTTON_RIGHT, tgt::MouseEvent::PRESSED, tgt::Event::CTRL),
      mouseEventClearROIs_("mouseEvent.clearROIs", "Clear ROIs", this, &RegionOfInterest2D::clearROIs,
        tgt::MouseEvent::MOUSE_BUTTON_LEFT, tgt::MouseEvent::DOUBLECLICK, tgt::Event::CTRL),
      mouseEventShiftROI_("mouseEvent.shiftROI", "Shift ROI", this, &RegionOfInterest2D::shiftROI,
        tgt::MouseEvent::MOUSE_BUTTON_LEFT,
        static_cast<tgt::MouseEvent::MouseAction>(tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED),
        tgt::Event::ALT),
      mouseEventScaleROI_("mouseEvent.scaleROI", "Scale ROI", this, &RegionOfInterest2D::scaleROI,
        tgt::MouseEvent::MOUSE_BUTTON_RIGHT,
        static_cast<tgt::MouseEvent::MouseAction>(tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED),
        tgt::Event::ALT),
      blendShader_(0),
      selectedROI_(-1),
      lastMousePos_(-1)
{
    addPort(inport_);
    addPort(outportMask_);
    addPort(outportROI_);
    addPort(outportOverlay_);
    addPrivateRenderPort(pickingPort_);
    addPort(outportROIGeometryLayer_);
    addPort(outportROIGeometry_);

    geometryMode_.addOption("points",    "Points",          RegionOfInterestGeometry::POINTS);
    geometryMode_.addOption("lines",     "Lines",           RegionOfInterestGeometry::LINES);
    geometryMode_.addOption("lineStrip", "Line-Strip",      RegionOfInterestGeometry::LINE_STRIP);
    geometryMode_.addOption("lineLoop",  "Line-Loop",       RegionOfInterestGeometry::LINE_LOOP);
    geometryMode_.addOption("spline",    "Spline",          RegionOfInterestGeometry::SPLINE);
    geometryMode_.addOption("rectangle", "Rectangle",       RegionOfInterestGeometry::RECTANGLE);
    geometryMode_.addOption("circle",    "Circle",          RegionOfInterestGeometry::CIRCLE_PATCH);
    geometryMode_.addOption("patch",     "Free-form patch", RegionOfInterestGeometry::PATCH);
    geometryMode_.set("patch");

    removeLastROI_.onChange(Call1ParMemberAction<RegionOfInterest2D, tgt::MouseEvent*>(this, &RegionOfInterest2D::removeLastROI, 0));
    clearROIs_.onChange(Call1ParMemberAction<RegionOfInterest2D, tgt::MouseEvent*>(this, &RegionOfInterest2D::clearROIs, 0));
    saveROIs_.onChange(CallMemberAction<RegionOfInterest2D>(this, &RegionOfInterest2D::saveFileIssued));
    loadROIs_.onChange(CallMemberAction<RegionOfInterest2D>(this, &RegionOfInterest2D::loadFileIssued));

    mouseEventShiftROI_.setEnabled(false);
    mouseEventScaleROI_.setEnabled(false);

    minRoiPointDistance_.setStepping(1.f);
    numberOfROIs_.setWidgetsEnabled(false);

    addProperty(geometryMode_);
    addProperty(roiColor_);
    addProperty(maskColor_);
    addProperty(maskBackgroundColor_);
    addProperty(size_);
    addProperty(antialiasing_);
    addProperty(currentLayer_);
    addProperty(renderCurrentLayerOnly_);
    addProperty(boundingBoxColor_);
    addProperty(minRoiPointDistance_);
    addProperty(roiTransformMatrix_);
    addProperty(loadROIs_);
    addProperty(saveROIs_);
    addProperty(clearROIs_);
    addProperty(removeLastROI_);
    addProperty(numberOfROIs_);

    roiColor_.setViews(Property::COLOR);
    maskColor_.setViews(Property::COLOR);
    maskBackgroundColor_.setViews(Property::COLOR);
    boundingBoxColor_.setViews(Property::COLOR);

    addEventProperty(mouseEventAddROI_);
    addEventProperty(mouseEventRemoveLastROI_);
    addEventProperty(mouseEventClearROIs_);
    addEventProperty(mouseEventShiftROI_);
    addEventProperty(mouseEventScaleROI_);
}

std::string RegionOfInterest2D::getProcessorInfo() const {
    return "<p>Allows the user to sketch arbitrarily shaped 2D regions of interest: points, line strip, spline curve, "
           "free-form patch. The ROIs are put out as mask and additionally as overlay drawn over the input rendering. "
           "Multiple ROIs can be defined consecutively. Each of them is assigned the properties that are set "
           "when the user starts drawing the ROI. Double-click clears the current ROI set.<p/>"
           "<p>"
           "The RegionOfInterest2D processor is designed to be used in combination with a SliceViewer: "
           "The picking matrix generated by the SliceViewer can be linked with RegionOfInterest2D's matrix property "
           "in order to convert screen coordinates into volume coordinates."
           "</p>";
}

Processor* RegionOfInterest2D::create() const {
    return new RegionOfInterest2D();
}

bool RegionOfInterest2D::isReady() const {
    bool ready = false;
    ready |= outportMask_.isConnected();
    ready |= outportOverlay_.isConnected();
    ready |= outportROI_.isConnected();
    ready |= outportROIGeometryLayer_.isConnected();
    ready |= outportROIGeometry_.isConnected();
    ready &= inport_.isReady();
    return ready;
}

void RegionOfInterest2D::initialize() throw (VoreenException) {

    ImageProcessor::initialize();

    blendShader_ = ShdrMgr.loadSeparate("passthrough.vert", "blendwithimage.frag",
        generateHeader(), false);
    if (!blendShader_) {
        LERROR("Failed to load shader");
        initialized_ = false;
        throw VoreenException(getClassName() + ": Failed to load shader!");
    }

    idManager_.setRenderTarget(pickingPort_.getRenderTarget());
    idManager_.initializeTarget();

    numberOfROIs_.set(regionsOfInterest_.size());
}

void RegionOfInterest2D::deinitialize() throw (VoreenException) {
    ShdrMgr.dispose(blendShader_);
    blendShader_ = 0;

    delete outportROIGeometryLayer_.getData();
    outportROIGeometryLayer_.setData(0);

    delete outportROIGeometry_.getData();
    outportROIGeometry_.setData(0);

    ImageProcessor::deinitialize();
}

void RegionOfInterest2D::process() {

    // mask
    if (outportMask_.isReady()) {

        // clear output
        outportMask_.activateTarget();
        tgt::Color bgCol = maskBackgroundColor_.get();
        glClearColor(bgCol.r, bgCol.g, bgCol.b, bgCol.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.f, 0.f);

        // render ROIs with mask color
        renderROIs(MASK_MODE);

        outportMask_.deactivateTarget();
    }

    LGL_ERROR;

    // rendered ROI
    if (outportROI_.isReady()) {

        // clear output buffer
        outportROI_.activateTarget();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render ROIs with roi color, including alpha
        renderROIs(COLOR_MODE);

        // bounding box of selected roi
        if (selectedROI_ > -1) {
            tgtAssert(selectedROI_ >= 0 && selectedROI_ < (int)regionsOfInterest_.size(), "Invalid selected roi");
            if (!renderCurrentLayerOnly_.get() || (regionsOfInterest_.at(static_cast<size_t>(selectedROI_))->layer_ == currentLayer_.get()) )
                renderBoundingBox(regionsOfInterest_.at(static_cast<size_t>(selectedROI_)));
            LGL_ERROR;
        }

        outportROI_.deactivateTarget();
    }

    LGL_ERROR;

    // image with ROI overlay
    if (outportOverlay_.isReady()) {

        outportOverlay_.activateTarget();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // copy input image to output buffer
        TextureUnit colorUnit, depthUnit;
        inport_.bindTextures(colorUnit.getEnum(), depthUnit.getEnum());
        program_->activate();
        setGlobalShaderParameters(program_);
        program_->setUniform("colorTex_", colorUnit.getUnitNumber());
        program_->setUniform("depthTex_", depthUnit.getUnitNumber());
        inport_.setTextureParameters(program_, "texParams_");
        renderQuad();
        program_->deactivate();

        // render ROIs with ROI color and blend them over the input image
        inport_.bindColorTexture(colorUnit.getEnum());
        blendShader_->activate();
        setGlobalShaderParameters(blendShader_);
        program_->setUniform("colorTex_", colorUnit.getUnitNumber());
        inport_.setTextureParameters(blendShader_, "textureParameters_");
        renderROIs(COLOR_MODE);
        blendShader_->deactivate();

        // bounding box of selected roi
        if (selectedROI_ > -1) {
            tgtAssert(selectedROI_ >= 0 && selectedROI_ < (int)regionsOfInterest_.size(), "Invalid selected roi");
            if (!renderCurrentLayerOnly_.get() || (regionsOfInterest_.at(static_cast<size_t>(selectedROI_))->layer_ == currentLayer_.get()) )
                renderBoundingBox(regionsOfInterest_.at(static_cast<size_t>(selectedROI_)));
        }

        outportOverlay_.deactivateTarget();
    }

    // picking
    renderROIs(PICKING_MODE);

    // roi geometry (layer only)
    if (outportROIGeometryLayer_.isConnected()) {
        PointSegmentListGeometryVec3* segmentListGeom = new PointSegmentListGeometryVec3();
        std::vector<std::vector<tgt::vec3> > segmentList;
        for (size_t i=0; i<regionsOfInterest_.size(); i++) {

            RegionOfInterestGeometry* roi = regionsOfInterest_[i];
            if (roi->layer_ != currentLayer_.get())
                continue;

            std::vector<tgt::vec3> pointList;
            for (size_t p=0; p<roi->points_.size(); p++) {
                pointList.push_back((roi->transformMatrix_ * tgt::vec4(static_cast<vec2>(roi->points_[p]), static_cast<float>(roi->layer_), 1.f)).xyz());
            }
            segmentList.push_back(pointList);
        }
        segmentListGeom->setData(segmentList);
        delete outportROIGeometryLayer_.getData();
        outportROIGeometryLayer_.setData(segmentListGeom);
    }

    // roi geometry (all rois)
    if (outportROIGeometry_.isConnected()) {
        PointSegmentListGeometryVec3* segmentListGeom = new PointSegmentListGeometryVec3();
        std::vector<std::vector<tgt::vec3> > segmentList;
        for (size_t i=0; i<regionsOfInterest_.size(); i++) {
            RegionOfInterestGeometry* roi = regionsOfInterest_[i];
            std::vector<tgt::vec3> pointList;
            for (size_t p=0; p<roi->points_.size(); p++) {
                pointList.push_back((roi->transformMatrix_ * tgt::vec4(roi->points_[p], static_cast<float>(roi->layer_), 1.f)).xyz());
            }
            segmentList.push_back(pointList);
        }
        segmentListGeom->setData(segmentList);
        delete outportROIGeometry_.getData();
        outportROIGeometry_.setData(segmentListGeom);
    }

    TextureUnit::setZeroUnit();
    LGL_ERROR;
}

void RegionOfInterest2D::renderROIs(RoiMode roiMode) const {

    if (!regionsOfInterest_.empty()) {

        glDepthFunc(GL_ALWAYS);

        // set up transformation matrices for specifying
        // ROI points in screen coordinates
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // viewport transformation
        glLoadIdentity();
        glTranslatef(-1.f, -1.f, 0.f);
        tgt::ivec2 viewportDim = inport_.getSize();
        glScalef(2.f/viewportDim.x , 2.f / viewportDim.y, 1.f);

        // inverse transformation matrix taking us back to screen coords
        tgt::mat4 transformMatrixInverse;
        roiTransformMatrix_.get().invert(transformMatrixInverse);
        tgt::multMatrix(transformMatrixInverse);
        LGL_ERROR;

        if (roiMode == PICKING_MODE) {
            idManager_.clearRegisteredObjects();
            idManager_.activateTarget(getName());
            idManager_.clearTarget();
            LGL_ERROR;
        }

        //
        // Iterate over all ROIs
        //
        for (size_t k=0; k<regionsOfInterest_.size(); k++) {

            if (renderCurrentLayerOnly_.get() && (regionsOfInterest_[k]->layer_ != currentLayer_.get()))
                continue;

            if (regionsOfInterest_[k]->points_.empty())
                continue;

            if (roiMode == MASK_MODE)
                glColor4fv(regionsOfInterest_[k]->maskColor_.elem);
            else if (roiMode == COLOR_MODE)
                glColor4fv(regionsOfInterest_[k]->roiColor_.elem);
            else if (roiMode == PICKING_MODE) {
                idManager_.registerObject(regionsOfInterest_[k]);
                idManager_.setGLColor(regionsOfInterest_[k]);
                LGL_ERROR;
            }

            regionsOfInterest_[k]->render();
            LGL_ERROR;
        }

        if (roiMode == PICKING_MODE) {
            idManager_.deactivateTarget();
            LGL_ERROR;
        }

        // restore attributes and matrices
        glDepthFunc(GL_LESS);
        glColor4f(0.f, 0.f, 0.f, 0.f);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        LGL_ERROR;
    }
}

void RegionOfInterest2D::renderBoundingBox(RegionOfInterestGeometry* roi) const {

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDepthFunc(GL_ALWAYS);

    // set up transformation matrices for specifying
    // ROI points in screen coordinates
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // viewport transformation
    glLoadIdentity();
    glTranslatef(-1.f, -1.f, 0.f);
    tgt::ivec2 viewportDim = inport_.getSize();
    glScalef(2.f/viewportDim.x , 2.f / viewportDim.y, 1.f);

    // inverse transformation matrix taking us back to screen coords
    tgt::mat4 transformMatrixInverse;
    roiTransformMatrix_.get().invert(transformMatrixInverse);
    tgt::multMatrix(transformMatrixInverse);

    // render bounding box
    glColor4fv(boundingBoxColor_.get().elem);
    roi->renderBoundingBox();

    // restore attributes and matrices
    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    LGL_ERROR;
}

void RegionOfInterest2D::addROIEvent(tgt::MouseEvent* e) {

    tgtAssert(e, "No event");

    if (!inport_.isReady())
        return;

    tgt::ivec2 mousePos(e->x(), inport_.getSize().y - e->y());

    enum Actions {
        NO_ACTION = 0,
        ADD_ROI = 1,
        ADD_ROI_POINT = 2,
        FINISH_ROI = 4
    };

    int actions = NO_ACTION;

    //
    // determine actions
    //
    if (e->action() == MouseEvent::PRESSED) {
        actions |= ADD_ROI;
        actions |= ADD_ROI_POINT;
    }
    if (e->action() == MouseEvent::RELEASED) {
        actions |= FINISH_ROI;
    }
    if (e->action() == MouseEvent::MOTION) {
        actions |= ADD_ROI_POINT;
    }

    //
    // perform actions
    //
    if (actions & ADD_ROI) {
        RegionOfInterestGeometry* roi = new RegionOfInterestGeometry();
        roi->geometryMode_ = geometryMode_.getValue();
        roi->layer_ = currentLayer_.get();
        roi->viewportSize_ = inport_.getSize();
        roi->transformMatrix_ = roiTransformMatrix_.get();
        roi->roiColor_ = roiColor_.get();
        roi->maskColor_ = maskColor_.get();
        roi->size_ = size_.get();
        roi->antialiasing_ = antialiasing_.get();

        // render patch contour during interaction
        if (roi->geometryMode_ == RegionOfInterestGeometry::PATCH)
            roi->geometryMode_ = RegionOfInterestGeometry::LINE_STRIP;

        regionsOfInterest_.push_back(roi);

        lastMousePos_ = mousePos;
        toggleInteractionMode(true, this);
    }

    if (actions & ADD_ROI_POINT) {
        if (!regionsOfInterest_.empty()) {
            vec2 point((float)e->coord().x, (float)e->viewport().y - e->coord().y);
            if (regionsOfInterest_.back()->geometryMode_ == RegionOfInterestGeometry::CIRCLE_PATCH) {
                float radius = tgt::length(tgt::vec2(mousePos - lastMousePos_));
                if (radius > 0.f)
                    regionsOfInterest_.back()->points_ = generateCircle(lastMousePos_, radius);
            }
            else if (regionsOfInterest_.back()->geometryMode_ == RegionOfInterestGeometry::RECTANGLE) {
                regionsOfInterest_.back()->points_.clear();
                if (tgt::length(tgt::vec2(mousePos - lastMousePos_)) > 0.f) {
                    regionsOfInterest_.back()->points_ = generateRectangle(tgt::vec2(lastMousePos_), tgt::vec2(mousePos));
                }
            }
            else {
                if (regionsOfInterest_.back()->points_.empty() || tgt::distance(point, regionsOfInterest_.back()->points_.back()) >= minRoiPointDistance_.get()) {
                    regionsOfInterest_.back()->points_.push_back(point);
                }
            }
        }
    }

    if (actions & FINISH_ROI) {
        if (!regionsOfInterest_.empty()) {
            // now render patch as patch
            if (geometryMode_.getValue() == RegionOfInterestGeometry::PATCH ||
                geometryMode_.getValue() == RegionOfInterestGeometry::CIRCLE_PATCH) {
                regionsOfInterest_.back()->geometryMode_ = RegionOfInterestGeometry::PATCH;
            }

            // compute bounding box of roi points
            regionsOfInterest_.back()->computeBoundingBox();
        }
        toggleInteractionMode(false, this);
        lastMousePos_ = tgt::ivec2(-1);
    }
    numberOfROIs_.set(regionsOfInterest_.size());

    e->accept();
    invalidate();
}

void RegionOfInterest2D::removeLastROI(tgt::MouseEvent* e) {

    if (!regionsOfInterest_.empty()) {
        delete regionsOfInterest_.back();
        regionsOfInterest_.pop_back();
        if (selectedROI_ >= static_cast<int>(regionsOfInterest_.size()))
            selectedROI_ = -1;
        invalidate();
    }
    numberOfROIs_.set(regionsOfInterest_.size());

    if (e)
        e->accept();
}

void RegionOfInterest2D::clearROIs(tgt::MouseEvent* e) {

    if (!regionsOfInterest_.empty()) {
        for (size_t i=0; i<regionsOfInterest_.size(); i++)
            delete regionsOfInterest_[i];
        regionsOfInterest_.clear();
        selectedROI_ = -1;
        invalidate();
    }
    numberOfROIs_.set(regionsOfInterest_.size());

    if (e)
        e->accept();
}

void RegionOfInterest2D::shiftROI(tgt::MouseEvent* e){

    tgtAssert(e, "No mouse event");
    e->accept();

    if (!inport_.isReady())
        return;

    tgt::ivec2 mousePos(e->x(), inport_.getSize().y - e->y());

    if (e->action() == tgt::MouseEvent::PRESSED) {
        selectedROI_ = -1;
        lastMousePos_ = tgt::ivec2(-1);

        for (size_t i=0; i<regionsOfInterest_.size() && (selectedROI_ == -1); i++) {
            if (idManager_.isHit(mousePos, regionsOfInterest_[i])) {
                selectedROI_ = i;
                lastMousePos_ = mousePos;
                toggleInteractionMode(true, this);
            }
        }

        if (selectedROI_ == -1) {
            LINFO("miss");
        }

        invalidate();
    }
    else if (e->action() == tgt::MouseEvent::MOTION) {
        if (selectedROI_ > -1 && lastMousePos_ != tgt::ivec2(-1)) {
            tgtAssert(selectedROI_ >= 0 && selectedROI_ < (int)regionsOfInterest_.size(), "Invalid selected roi");
            RegionOfInterestGeometry* roi = regionsOfInterest_.at(static_cast<size_t>(selectedROI_));
            tgt::vec2 offset = tgt::vec2(mousePos - lastMousePos_);
            if (offset != tgt::vec2(0.f)) {
                for (size_t i=0; i<roi->points_.size(); i++) {
                    roi->points_[i] += offset;
                }
                roi->boundingBox_ += tgt::vec4(offset.x, offset.y, offset.x, offset.y);

                lastMousePos_ = mousePos;
                invalidate();
            }
        }
    }
    else if (e->action() == tgt::MouseEvent::RELEASED) {
        selectedROI_ = -1;
        lastMousePos_ = tgt::ivec2(-1);
        toggleInteractionMode(false, this);
        invalidate();
    }
}

void RegionOfInterest2D::scaleROI(tgt::MouseEvent* e) {

    tgtAssert(e, "No mouse event");
    e->accept();

    if (!inport_.isReady())
        return;

    tgt::ivec2 mousePos(e->x(), inport_.getSize().y - e->y());

    if (e->action() == tgt::MouseEvent::PRESSED) {
        selectedROI_ = -1;
        lastMousePos_ = tgt::ivec2(-1);

        for (size_t i=0; i<regionsOfInterest_.size() && (selectedROI_ == -1); i++) {
            if (idManager_.isHit(mousePos, regionsOfInterest_[i])) {
                LINFO("Hit: " << i);
                selectedROI_ = i;
                lastMousePos_ = mousePos;
                toggleInteractionMode(true, this);
            }
        }

        if (selectedROI_ == -1) {
            LINFO("miss");
        }

        invalidate();
    }
    else if (e->action() == tgt::MouseEvent::MOTION) {
        if (selectedROI_ > -1 && lastMousePos_ != tgt::ivec2(-1)) {
            tgtAssert(selectedROI_ >= 0 && selectedROI_ < (int)regionsOfInterest_.size(), "Invalid selected roi");
            RegionOfInterestGeometry* roi = regionsOfInterest_.at(static_cast<size_t>(selectedROI_));
            tgt::vec2 offset = tgt::vec2(mousePos - lastMousePos_);
            if (offset != tgt::vec2(0.f)) {
                tgt::vec2 bbCenter = tgt::vec2(roi->boundingBox_.z + roi->boundingBox_.x, roi->boundingBox_.w + roi->boundingBox_.y) / 2.f;
                tgt::vec2 scale = (1.f + (offset / tgt::vec2(inport_.getSize())) * 6.f);
                for (size_t i=0; i<roi->points_.size(); i++) {
                    roi->points_[i] = (roi->points_[i] - bbCenter) * scale + bbCenter;
                }
                roi->boundingBox_ = (roi->boundingBox_ - tgt::vec4(bbCenter.x, bbCenter.y, bbCenter.x, bbCenter.y)) * tgt::vec4(scale.x, scale.y, scale.x, scale.y)
                    + tgt::vec4(bbCenter.x, bbCenter.y, bbCenter.x, bbCenter.y);

                lastMousePos_ = mousePos;
                invalidate();
            }
        }
    }
    else if (e->action() == tgt::MouseEvent::RELEASED) {
        selectedROI_ = -1;
        lastMousePos_ = tgt::ivec2(-1);
        toggleInteractionMode(false, this);
        invalidate();
    }
}

void RegionOfInterest2D::serialize(XmlSerializer& s) const {
    ImageProcessor::serialize(s);
    try {
        s.serialize("RegionsOfInterest", regionsOfInterest_, "Region");
    }
    catch (SerializationException &e) {
        LWARNING(std::string("Region-of-interest serialization failed: ") + e.what());
    }
}

void RegionOfInterest2D::deserialize(XmlDeserializer& s) {
    ImageProcessor::deserialize(s);
    try {
        clearROIs();
        s.deserialize("RegionsOfInterest", regionsOfInterest_, "Region");
    }
    catch (XmlSerializationNoSuchDataException) {
        // just no ROI data present => ignore
        s.removeLastError();
    }
    catch (SerializationException &e) {
        LWARNING(std::string("Region-of-interest deserialization failed: ") + e.what());
    }
}

void RegionOfInterest2D::saveROIs(const std::string& filename) const {

    if (filename.empty()) {
        LERROR("ROI filename is empty");
        return;
    }

    LINFO("Saving regions of interest to file: " << filename);

    // open file stream
    std::ofstream stream(filename.c_str(), std::ios_base::out);
    if (stream.fail()) {
        LWARNING("Unable to open file " << filename << " for writing.");
        return;
    }

    // serialize to stream
    bool success = true;
    try {
        XmlSerializer s(filename);
        s.serialize("RegionsOfInterest", regionsOfInterest_, "Region");

        s.write(stream);
        if (stream.bad()) {
            LWARNING("Unable to write to file: " << filename);
            success = false;
        }
        stream.close();
    }
    catch (SerializationException &e) {
        LWARNING("SerializationException: " << e.what());
        stream.close();
        success = false;
    }

    // log result
    if (!success)
        LWARNING("Saving regions of interest failed.");
}

void RegionOfInterest2D::loadROIs(const std::string& filename) {

    if (filename.empty()) {
        LERROR("ROI filename is empty");
        return;
    }

    LINFO("Loading regions of interest from file: " << filename);

    // open file stream
    std::ifstream stream(filename.c_str(), std::ios_base::in);
    if (stream.fail()) {
        LWARNING("Unable to open file " << filename << " for reading.");
        return;
    }

    // deserialize from stream
    bool success = true;
    try {
        XmlDeserializer d(filename);
        d.read(stream);
        d.deserialize("RegionsOfInterest", regionsOfInterest_, "Region");
        stream.close();
    }
    catch (SerializationException &e) {
        LWARNING("SerializationException: " << e.what());
        stream.close();
        success = false;
    }

    if (!success) {
        LWARNING("Loading regions of interest failed.");
    }
    numberOfROIs_.set(regionsOfInterest_.size());

    invalidate();
}

void RegionOfInterest2D::saveFileIssued() {
    if (!saveROIs_.get().empty()) {
        saveROIs(saveROIs_.get());
        saveROIs_.set("");
    }
}

void RegionOfInterest2D::loadFileIssued() {
    if (!loadROIs_.get().empty()) {
        loadROIs(loadROIs_.get());
        loadROIs_.set("");
    }
}

std::vector<tgt::vec2> RegionOfInterest2D::generateCircle(tgt::vec2 center, float radius) const {

    std::vector<tgt::vec2> result;
    int numPoints = tgt::ifloor(2.f*radius*tgt::PIf);
    float step = 2.f*tgt::PIf / numPoints;
    for (float t=0.f; t<=2.f*tgt::PIf; t += step) {
        tgt::vec2 nextPoint = radius * tgt::vec2(cos(t), sin(t)) + center;
        if (result.empty() || tgt::length(result.back() - nextPoint) >= minRoiPointDistance_.get())
            result.push_back(nextPoint);
    }
    return result;
}

std::vector<tgt::vec2> RegionOfInterest2D::generateRectangle(tgt::vec2 first, tgt::vec2 second) const {
    std::vector<tgt::vec2> result;
    result.push_back(first);
    result.push_back(tgt::vec2(first.x, second.y));
    result.push_back(second);
    result.push_back(tgt::vec2(second.x, first.y));
    return result;
}

} // voreen namespace

