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

#include "voreen/modules/base/processors/plotting/plotprocessor.h"

#include "voreen/core/interaction/plotcamerainteractionhandler.h"
#include "voreen/core/interaction/camerainteractionhandler.h"

#include "voreen/core/plotting/plotrow.h"

#include <iomanip>

namespace voreen {

PlotProcessor::PlotProcessor(PlotEntitySettings::Entity entity, bool threeDimensional)
    : ImageProcessor()
    , inport_(Port::INPORT, "inport")
    , outport_(Port::OUTPORT, "outport", false, Processor::INVALID_RESULT, GL_RGBA)
    , pickingBuffer_(Port::OUTPORT, "pickingBuffer", false, Processor::INVALID_RESULT, GL_RGBA)
    , data_(0,0)
    , plotLib_()
    , dataProp_("plotDataTable", "Plot Data Table", NULL)
    , plotEntitiesProp_("plotEntitiesProperty", "Entity Selection", entity)
    , plotLabel_("plotLabel", "Label", "", INVALID_RESULT)
    , xLabel_("xLabel", "X Axis Label", "x axis")
    , yLabel_("yLabel", "Y Axis Label", "y axis")
    , zLabel_("zLabel", "Z Axis Label", "z axis")
    , plotLabelSize_("plotLabelSize", "Label Size", 18, 8, 48)
    , plotLabelColor_("plotLabelColor", "Label Color", tgt::vec4(0, 0, 1, 1))
    , selectionPlaneColor_("selectionPlaneColor", "Selection Plane Color", tgt::vec4(.4f, .4f, .4f, 0.4f))
    , highlightColor_("highlightColor", "Highlight Color", tgt::vec4(1, .9, .8, 1))
    , enablePicking_("enablePicking", "Enable Picking", true)
    , renderMousePosition_("renderMousePosition", "Render Mouse Position", true)
    , renderLegend_("renderLegend", "Render Colormap Legend", true)
    , marginLeft_("marginLeft", "Left Margin", 60, 0, 200)
    , marginRight_("marginRight", "Right Margin", 100, 0, 200)
    , marginBottom_("marginBottom", "Bottom Margin", 50, 0, 200)
    , marginTop_("marginTop", "Top Margin", 50, 0, 200)
    , discreteStep_("descreteStep", "Number of Discretization Steps", 32, 1, (threeDimensional ? 128 : 4096))
    , selectionProp_("selectionProperty", "Selection", &data_, plotEntitiesProp_, threeDimensional)
    , camera_("camera", "Camera", tgt::Camera(tgt::vec3(0.f, 1.f, 0.f), tgt::vec3(0.f, 0.f, 0.f), tgt::vec3(0.f, 0.f, 1.f)))
    , orthographicCamera_("orthographicCamera", "Orthographic Camera", true)
    , renderAxes_("renderAxes", "Render Axes", true)
    , renderScales_("renderScales", "Render Scales", false)
    , axesWidth_("axesWidth", "Axes Width", 1.5, 1.f, 5.f)
    , xScaleStep_("xScaleStep", "Minimum x Scale Step (Pixels)", 40, 32, 256)
    , yScaleStep_("yScaleStep", "Minimum y Scale Step (Pixels)", 40, 32, 256)
    , zScaleStep_("zScaleStep", "Minimum z Scale Step (Pixels)", 40, 32, 256)
    , renderXHelperLines_("renderXHelperLines", "Show x Helper Lines", true)
    , renderYHelperLines_("renderYHelperLines", "Show y Helper Lines", true)
    , renderZHelperLines_("renderZHelperLines", "Show z Helper Lines", true)
    , mousePressed_(false)
    , mouseInsideCanvas_(false)
    , selectionEdgeIndex_(-1)
    , inportHasPlotFunction_(false)
    , threeDimensional_(threeDimensional)
    , regenDataList_(true)
    , regenPickingList_(true)
{
    //create event properties
    eventZoomAdditive_ = new EventProperty<PlotProcessor>("zoomAdditive", "Additive Zoom", this,
        &PlotProcessor::zoomAdditive, tgt::MouseEvent::MOUSE_BUTTON_RIGHT,
        tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED,
        tgt::Event::SHIFT);
    eventHighlightAdditive_ = new EventProperty<PlotProcessor>("highlightAdditive", "Additive Highlight", this,
        &PlotProcessor::highlightAdditive, tgt::MouseEvent::MOUSE_BUTTON_RIGHT,
        tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED,
        tgt::Event::MODIFIER_NONE);
    eventLabelAdditive_ = new EventProperty<PlotProcessor>("labelAdditive", "Additive Label", this,
        &PlotProcessor::labelAdditive, tgt::MouseEvent::MOUSE_BUTTON_RIGHT,
        tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED,
        tgt::Event::CTRL);
    eventHighlight_ = new EventProperty<PlotProcessor>("highlight", "Highlight selection",
        this, &PlotProcessor::highlight, tgt::MouseEvent::MOUSE_BUTTON_LEFT,
        tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED,
        tgt::Event::MODIFIER_NONE);
    eventLabel_ = new EventProperty<PlotProcessor>("label", "Label selection",
        this, &PlotProcessor::label, tgt::MouseEvent::MOUSE_BUTTON_LEFT,
        tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED,
        tgt::Event::CTRL);
    eventZoom_ = new EventProperty<PlotProcessor>("zoom", "Zoom to selection",
        this, &PlotProcessor::zoom, tgt::MouseEvent::MOUSE_BUTTON_LEFT,
        tgt::MouseEvent::MOTION | tgt::MouseEvent::PRESSED | tgt::MouseEvent::RELEASED,
        tgt::Event::SHIFT);
    mousePositionUpdateEvent_ = new EventProperty<PlotProcessor>("mousePositionUpdate",
        "Update Mouse Position", this, &PlotProcessor::updateMousePosition,
        tgt::MouseEvent::MOUSE_BUTTON_NONE, tgt::MouseEvent::MOTION, tgt::MouseEvent::MODIFIER_NONE);
    mouseEventEnterExit_ = new EventProperty<PlotProcessor>("enterExitEvent", "Enter/Exit Event",
        this, &PlotProcessor::enterExitEvent, tgt::MouseEvent::MOUSE_BUTTON_NONE,
        tgt::MouseEvent::ENTER_EXIT, tgt::MouseEvent::MODIFIER_NONE);

    // create interactionhandlers
    cameraHandler_ = new CameraInteractionHandler("cameraHandler", "Camera", &camera_);
    plotCameraHandler_ = new PlotCameraInteractionHandler("plotCameraHandler", "Orthographic Camera", &camera_);

    // set up onChange calls
    plotEntitiesProp_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::toggleProperties));
    plotEntitiesProp_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::calcDomains));
    discreteStep_.setTracking(false);
    discreteStep_.setVisible(false);
    discreteStep_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::selectDataFromFunction));
    selectionProp_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::selectDataFromFunction));
    selectionProp_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::regenDisplayLists));
    plotEntitiesProp_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::regenDisplayLists));
    plotEntitiesProp_.onChange(CallMemberAction<PlotSelectionProperty>(&selectionProp_, &PlotSelectionProperty::applyHighlights));
    highlightColor_.onChange(CallMemberAction<PlotProcessor>(this, &PlotProcessor::regenDisplayLists));

    camera_.setVisible(false);
    selectionPlaneColor_.setViews(Property::COLOR);
    highlightColor_.setViews(Property::COLOR);
    plotLabelColor_.setViews(Property::COLOR);

    // set up ports
    addPort(inport_);
    addPort(outport_);
    addPrivateRenderPort(pickingBuffer_);

    // add properties
    addProperty(dataProp_);
    addProperty(plotEntitiesProp_);
    addProperty(plotLabel_);
    addProperty(plotLabelSize_);
    addProperty(plotLabelColor_);
    addProperty(xLabel_);
    addProperty(yLabel_);
    addProperty(selectionProp_);
    addProperty(highlightColor_);
    addProperty(enablePicking_);
    addProperty(axesWidth_);
    addProperty(renderAxes_);
    addProperty(renderScales_);

    // group properties
    dataProp_.setGroupID("data");
    plotEntitiesProp_.setGroupID("data");
    setPropertyGroupGuiName("data", "");

    plotLabel_.setGroupID("labels");
    plotLabelSize_.setGroupID("labels");
    plotLabelColor_.setGroupID("labels");
    xLabel_.setGroupID("labels");
    yLabel_.setGroupID("labels");
    zLabel_.setGroupID("labels");
    setPropertyGroupGuiName("labels", "Labels");

    selectionProp_.setGroupID("select");
    highlightColor_.setGroupID("select");
    selectionPlaneColor_.setGroupID("select");
    setPropertyGroupGuiName("select", "Selection");

    enablePicking_.setGroupID("general");
    renderMousePosition_.setGroupID("general");
    discreteStep_.setGroupID("general");
    orthographicCamera_.setGroupID("general");
    renderLegend_.setGroupID("general");
    setPropertyGroupGuiName("general", "General");

    renderAxes_.setGroupID("axis");
    renderScales_.setGroupID("axis");
    renderXHelperLines_.setGroupID("axis");
    renderYHelperLines_.setGroupID("axis");
    renderZHelperLines_.setGroupID("axis");
    axesWidth_.setGroupID("axis");
    xScaleStep_.setGroupID("axis");
    yScaleStep_.setGroupID("axis");
    zScaleStep_.setGroupID("axis");
    setPropertyGroupGuiName("axis", "Axis Settings");

    marginLeft_.setGroupID("margins");
    marginRight_.setGroupID("margins");
    marginBottom_.setGroupID("margins");
    marginTop_.setGroupID("margins");
    setPropertyGroupGuiName("margins", "Margins");
}

PlotProcessor::~PlotProcessor() {
    delete eventHighlight_;
    delete eventLabel_;
    delete eventZoom_;
    delete eventHighlightAdditive_;
    delete eventLabelAdditive_;
    delete eventZoomAdditive_;
    delete mousePositionUpdateEvent_;
    delete mouseEventEnterExit_;
    delete cameraHandler_;
    delete plotCameraHandler_;
}

void PlotProcessor::initialize() throw (VoreenException) {
    ImageProcessor::initialize();
    plotPickingManager_.setRenderTarget(pickingBuffer_.getRenderTarget());
    plotPickingManager_.initializeTarget();
    plotLib_.setPlotPickingManager(&plotPickingManager_);
}

void PlotProcessor::zoomAdditive(tgt::MouseEvent* e) {
    select(e, false, false, true, true);
}

void PlotProcessor::highlightAdditive(tgt::MouseEvent* e) {
    select(e, true, false, false, true);
}

void PlotProcessor::labelAdditive(tgt::MouseEvent* e) {
    select(e, false, true, false, true);
}

void PlotProcessor::select(tgt::MouseEvent* e, bool highlight, bool label, bool zoom, bool additive) {
    mousePosition_ = tgt::ivec2(e->x(),e->viewport().y-e->y());
    if (e->action() == tgt::MouseEvent::PRESSED) {
        mousePressed_ = true;
        mousePressedPosition_ = mousePosition_;
        if (threeDimensional_ && startSelection3D(tgt::dvec2(e->x(), e->viewport().y-e->y()))) {
            plotCameraHandler_->setEnabled(false);
            cameraHandler_->setEnabled(false);
        }
        e->accept();
    }
    else if (e->action() == tgt::MouseEvent::RELEASED) {
        mousePressed_ = false;
        if (tgt::length(mousePosition_ - mousePressedPosition_) < 4) {
            if (pickingBuffer_.hasRenderTarget()) {
                tgt::ivec2 cell = plotPickingManager_.getCellAtPos(tgt::ivec2(e->x(), e->viewport().y-e->y()));
                if (cell != tgt::ivec2(-1,-1)) {
                    PlotSelectionEntry entry = {PlotSelection(cell), highlight, label, zoom};
                    selectionProp_.add(entry, additive);
                }
                else {
                    selectionProp_.clearFlags(highlight, label, zoom);
                }
            }
            else {
                selectionProp_.clearFlags(highlight, label, zoom);
            }
        }
        else if (threeDimensional_ && calculateSelectEnd(tgt::dvec2(e->x(), e->viewport().y-e->y()))) {
            PlotSelectionEntry entry = { select3D(), highlight, false, zoom };
            regenDisplayLists();
            selectionProp_.add(entry, additive);
        }
        else if (!threeDimensional_) {
            PlotSelectionEntry entry = {select2D(mousePressedPosition_, mousePosition_), highlight, false, zoom};
            selectionProp_.add(entry, additive);
        }
        if (threeDimensional_) {
            // reenable camera handler
            plotCameraHandler_->setEnabled(orthographicCamera_.get());
            cameraHandler_->setEnabled(!orthographicCamera_.get());
            // disable selection plane rendering
            selectionEdgeIndex_ = -1;
        }
        e->accept();
    }
    else if (e->action() == tgt::MouseEvent::MOTION && mousePressed_) {
        if (calculateSelectEnd(tgt::dvec2(e->x(), e->viewport().y-e->y()))) {
            e->accept();
        }
    }
    invalidate();
}

void PlotProcessor::highlight(tgt::MouseEvent* e) {
    select(e, true, false, false, false);
}

void PlotProcessor::zoom(tgt::MouseEvent* e) {
    select(e, false, false, true, false);
}

void PlotProcessor::label(tgt::MouseEvent* e) {
    select(e, false, true, false, false);
}

void PlotProcessor::updateMousePosition(tgt::MouseEvent* e) {
    mousePosition_ = tgt::ivec2(e->x(),e->viewport().y-e->y());
    invalidate();
    e->accept();
}

void PlotProcessor::enterExitEvent(tgt::MouseEvent* e) {
    mouseInsideCanvas_ = e->action() == tgt::MouseEvent::ENTER;
    invalidate();
    e->accept();
}

void PlotProcessor::process() {
    if (inport_.isReady() && inport_.hasChanged())
        readFromInport();
    if (plotEntitiesProp_.dataValid())
        render();
}

void PlotProcessor::selectDataFromFunction() {
    if (inportHasPlotFunction_) {
        // there is no way we can determine the 'right' base zoom state, so let's just use [-1,1]
        PlotZoomState baseZoomState(Interval<plot_t>(-1,1), Interval<plot_t>(-1,1), Interval<plot_t>(-1,1));
        selectionProp_.setBaseZoomState(baseZoomState);
        if (!selectionProp_.getZoom().xZoom_.empty() && (!threeDimensional_ || !selectionProp_.getZoom().yZoom_.empty())) {
            std::vector<Interval<plot_t> > zoom;
            zoom.push_back(selectionProp_.getZoom().xZoom_);
            if (threeDimensional_)
                zoom.push_back(selectionProp_.getZoom().yZoom_);
            std::vector<plot_t> step;
            step.push_back(selectionProp_.getZoom().xZoom_.size()/discreteStep_.get());
            if (threeDimensional_)
                step.push_back(selectionProp_.getZoom().yZoom_.size()/discreteStep_.get());

            // select the data
            function_.select(zoom, step, data_);
        }
        // adjust y- (2D) or z-domain (3D)
        if (!threeDimensional_ && data_.getDataColumnCount() >= 1)
            baseZoomState.yZoom_ = data_.getInterval(data_.getKeyColumnCount());
        else if (threeDimensional_ && data_.getDataColumnCount() >= 1)
            baseZoomState.zZoom_ = data_.getInterval(data_.getKeyColumnCount());
        selectionProp_.setBaseZoomState(baseZoomState);

        plotEntitiesProp_.setPlotData(&data_); // the adress of data hasn't changed - everything necessary is done by updating entitiesProp
        dataProp_.set(&data_);
        plotPickingManager_.setColumnCount(data_.getColumnCount());
    }
}

void PlotProcessor::toggleProperties() {/* Do nothing here. */ }

void PlotProcessor::renderMousePosition() {
    // this assures that we do not render the mouse position, while using camerahandler
    if (renderMousePosition_.get() && mouseInsideCanvas_ && (!threeDimensional_ || !mousePressed_ || selectionEdgeIndex_ != -1)) {
        plotLib_.setFontColor(tgt::Color::black);
        plotLib_.setFontSize(10);
        plotLib_.renderMousePosition(mousePosition_);
    }
}

std::pair<tgt::ivec2, tgt::ivec2> PlotProcessor::clipToPlotBounds(tgt::ivec2 first, tgt::ivec2 second) {
    // adapt positions to margins
    if (first.x < marginLeft_.get()) {
        first.x = 0;
        second.x = outport_.getSize().x;
    }
    else if (first.x > outport_.getSize().x-marginRight_.get()) {
        first.x = outport_.getSize().x;
        second.x = 0;
    }
    if (first.y < marginBottom_.get()) {
        first.y = 0;
        second.y = outport_.getSize().y;
    }
    else if (first.y > outport_.getSize().y-marginTop_.get()) {
        first.y = outport_.getSize().y;
        second.y = 0;
    }
    return std::pair<tgt::ivec2, tgt::ivec2>(first, second);
}

void PlotProcessor::renderSelectedRegion() {
    //only if the mouse is pressed, you can select a region
    if (mousePressed_) {
        std::pair<tgt::ivec2, tgt::ivec2> rectPosition = clipToPlotBounds(mousePressedPosition_, mousePosition_);
        glDisable(GL_DEPTH_TEST);
        glPushAttrib(GL_COLOR_BUFFER_BIT);
        glEnable (GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,outport_.getSize().x,0,outport_.getSize().y);
        tgt::Color c = selectionPlaneColor_.get();
        glColor4f(c.r,c.g,c.b,c.a);
        glBegin(GL_QUADS);
            glVertex2i(rectPosition.first.x,rectPosition.first.y);   glVertex2i(rectPosition.first.x,rectPosition.second.y);
            glVertex2i(rectPosition.second.x,rectPosition.second.y); glVertex2i(rectPosition.second.x,rectPosition.first.y);
        glEnd();
        glColor3f(0.f,0.f,0.f);
        glBegin(GL_LINE_LOOP);
            glVertex2i(rectPosition.first.x,rectPosition.first.y);   glVertex2i(rectPosition.first.x,rectPosition.second.y);
            glVertex2i(rectPosition.second.x,rectPosition.second.y); glVertex2i(rectPosition.second.x,rectPosition.first.y);
        glEnd();
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glPopAttrib();
    }
}

void PlotProcessor::renderPlotLabel() {
    if (plotLabel_.get() != "") {
        plotLib_.setFontColor(plotLabelColor_.get());
        plotLib_.setFontSize(plotLabelSize_.get());
        plotLib_.renderLabel(plotLib_.getCenterAbovePlot(), SmartLabel::CENTERED, plotLabel_.get(), true);
    }
}

void PlotProcessor::renderSelectionPlanes() {
    tgt::Color c = selectionPlaneColor_.get();
    glColor4f(c.r,c.g,c.b,c.a);
    Interval<plot_t> xInterval = selectionProp_.getZoom().xZoom_;
    Interval<plot_t> yInterval = selectionProp_.getZoom().yZoom_;
    Interval<plot_t> zInterval = selectionProp_.getZoom().zZoom_;
    if (selectionEdgeIndex_ == 0) {
        glBegin(GL_POLYGON);
            glVertex3d(selectStart_, yInterval.getLeft(), zInterval.getLeft());
            glVertex3d(selectStart_, yInterval.getLeft(), zInterval.getRight());
            glVertex3d(selectStart_, yInterval.getRight(), zInterval.getRight());
            glVertex3d(selectStart_, yInterval.getRight(), zInterval.getLeft());
        glEnd();
        glBegin(GL_POLYGON);
            glVertex3d(selectEnd_, yInterval.getLeft(), zInterval.getLeft());
            glVertex3d(selectEnd_, yInterval.getLeft(), zInterval.getRight());
            glVertex3d(selectEnd_, yInterval.getRight(), zInterval.getRight());
            glVertex3d(selectEnd_, yInterval.getRight(), zInterval.getLeft());
        glEnd();
    }
    if (selectionEdgeIndex_ == 1) {
        glBegin(GL_POLYGON);
            glVertex3d(xInterval.getLeft(), selectStart_, zInterval.getLeft());
            glVertex3d(xInterval.getLeft(), selectStart_, zInterval.getRight());
            glVertex3d(xInterval.getRight(), selectStart_, zInterval.getRight());
            glVertex3d(xInterval.getRight(), selectStart_, zInterval.getLeft());
        glEnd();
        glBegin(GL_POLYGON);
            glVertex3d(xInterval.getLeft(), selectEnd_, zInterval.getLeft());
            glVertex3d(xInterval.getLeft(), selectEnd_, zInterval.getRight());
            glVertex3d(xInterval.getRight(), selectEnd_, zInterval.getRight());
            glVertex3d(xInterval.getRight(), selectEnd_, zInterval.getLeft());
        glEnd();
    }
    if (selectionEdgeIndex_ == 2) {
        glBegin(GL_POLYGON);
            glVertex3d(xInterval.getLeft(), yInterval.getLeft(), selectStart_);
            glVertex3d(xInterval.getLeft(), yInterval.getRight(), selectStart_);
            glVertex3d(xInterval.getRight(), yInterval.getRight(), selectStart_);
            glVertex3d(xInterval.getRight(), yInterval.getLeft(), selectStart_);
        glEnd();
        glBegin(GL_POLYGON);
            glVertex3d(xInterval.getLeft(), yInterval.getLeft(), selectEnd_);
            glVertex3d(xInterval.getLeft(), yInterval.getRight(), selectEnd_);
            glVertex3d(xInterval.getRight(), yInterval.getRight(), selectEnd_);
            glVertex3d(xInterval.getRight(), yInterval.getLeft(), selectEnd_);
        glEnd();
    }
}

PlotSelection PlotProcessor::select2D(tgt::ivec2 first, tgt::ivec2 second) {
    std::pair<tgt::ivec2, tgt::ivec2> clippedPos = clipToPlotBounds(first, second);
    //if a non empty area is selected and the domains exist, we zoom in
    if ( clippedPos.first.x != clippedPos.second.x && clippedPos.first.y != clippedPos.second.y) {
        plotLib_.setOpenGLStatus();
        tgt::dvec2 firstPosPlot = plotLib_.convertViewportToPlotCoordinates(clippedPos.first);
        tgt::dvec2 secondPosPlot = plotLib_.convertViewportToPlotCoordinates(clippedPos.second);
        plotLib_.resetOpenGLStatus();
        double xLeft = (secondPosPlot.x < firstPosPlot.x ? secondPosPlot.x : firstPosPlot.x);
        double xRight = (secondPosPlot.x < firstPosPlot.x ? firstPosPlot.x : secondPosPlot.x);
        double yBottom = (secondPosPlot.y < firstPosPlot.y ? secondPosPlot.y : firstPosPlot.y);
        double yTop = (secondPosPlot.y < firstPosPlot.y ? firstPosPlot.y : secondPosPlot.y);
        Interval<plot_t> xSelection = Interval<plot_t>(xLeft,xRight);
        Interval<plot_t> ySelection = Interval<plot_t>(yBottom,yTop);
        //make sure that we select only data we can see
        xSelection.intersectWith(selectionProp_.getZoom().xZoom_);
        ySelection.intersectWith(selectionProp_.getZoom().yZoom_);
        // we limit the size of the selected region for accuracy reasons
        plot_t minSelect = 0.0001;
        if (fabs(xSelection.size()/xSelection.getLeft()) > minSelect && fabs(ySelection.size()/ySelection.getLeft()) > minSelect) {
            PlotPredicateBetweenOrEqual xPred(xSelection.getLeft(), xSelection.getRight());
            PlotPredicateBetweenOrEqual yPred(ySelection.getLeft(), ySelection.getRight());
            PlotSelection selection;
            // there are cases where, x-axis has label scales but there is an index column for the ordering. In this case
            // we need to apply the predicate (which contains the numerical values of the index column) to the index column
            if (data_.getColumnType(plotEntitiesProp_.getXColumnIndex()) == PlotData::STRING && PlotData::isIndexColumn(data_, 0))
                selection.addPredicate(0, &xPred);
            else
                selection.addPredicate(PlotSelectionProperty::X_AXIS_COLUMN, &xPred);
            selection.addPredicate(PlotSelectionProperty::Y_AXIS_COLUMN, &yPred);
            return selection;
        }
    }
    // if nothing was selected
    return PlotSelection();
}

PlotSelection PlotProcessor::select3D() {
    tgtAssert(selectionEdgeIndex_ != -1, "call PlotProcessor::select3D only if in selection mode");

    const PlotZoomState& current = selectionProp_.getZoom();
    if (selectStart_ > selectEnd_)
        std::swap(selectStart_, selectEnd_);

    PlotSelection selection;
    PlotPredicateBetweenOrEqual xPred(current.xZoom_.getLeft(), current.xZoom_.getRight());
    PlotPredicateBetweenOrEqual yPred(current.yZoom_.getLeft(), current.yZoom_.getRight());
    PlotPredicateBetweenOrEqual zPred(current.zZoom_.getLeft(), current.zZoom_.getRight());

    if (selectionEdgeIndex_ == 0) { // do x-axis zoom
        xPred = PlotPredicateBetweenOrEqual(selectStart_, selectEnd_);
    }
    else if (selectionEdgeIndex_ == 1) { // do y-axis zoom
        yPred = PlotPredicateBetweenOrEqual(selectStart_, selectEnd_);
    }
    else if (selectionEdgeIndex_ == 2) { // do z-axis zoom
        zPred = PlotPredicateBetweenOrEqual(selectStart_, selectEnd_);
    }

    selection.addPredicate(PlotSelectionProperty::X_AXIS_COLUMN, &xPred);
    selection.addPredicate(PlotSelectionProperty::Y_AXIS_COLUMN, &yPred);
    selection.addPredicate(PlotSelectionProperty::Z_AXIS_COLUMN, &zPred);
    // finish selecting
    selectionEdgeIndex_ = -1;
    return selection;
}

bool PlotProcessor::startSelection3D(tgt::dvec2 startPosition) {
    // check if position was outside of the cube and which axis to select
    tgt::dvec2 center(outport_.getSize().x/2, outport_.getSize().y/2);

    selectionEdgeIndex_ = -1; // -1 means no axis found
    Interval<plot_t> interval;

    std::vector<PlotLibrary::SelectionEdge>::const_iterator it;
    // check against x-axes
    for (it = plotLib_.getSelectionEdgesX().begin(); it < plotLib_.getSelectionEdgesX().end(); ++it) {
        if (PlotLibrary::rightTurn(center, startPosition, it->startVertex_)
            && PlotLibrary::leftTurn(center, startPosition, it->endVertex_)
            && PlotLibrary::rightTurn(it->startVertex_, it->endVertex_, startPosition)) {
                selectionEdge_ = *it;
                selectionEdgeIndex_ = 0;
                interval = selectionProp_.getZoom().xZoom_;
        }
    }
    if (selectionEdgeIndex_ == -1) {
        // check against y-axes
        for (it = plotLib_.getSelectionEdgesY().begin(); it < plotLib_.getSelectionEdgesY().end(); ++it) {
            if (PlotLibrary::rightTurn(center, startPosition, it->startVertex_)
                && PlotLibrary::leftTurn(center, startPosition, it->endVertex_)
                && PlotLibrary::rightTurn(it->startVertex_, it->endVertex_, startPosition)) {
                    selectionEdge_ = *it;
                    selectionEdgeIndex_ = 1;
                    interval = selectionProp_.getZoom().yZoom_;
            }
        }
    }
    if (selectionEdgeIndex_ == -1) {
        // check against z-axes
        for (it = plotLib_.getSelectionEdgesZ().begin(); it < plotLib_.getSelectionEdgesZ().end(); ++it) {
            if (PlotLibrary::rightTurn(center, startPosition, it->startVertex_)
                && PlotLibrary::leftTurn(center, startPosition, it->endVertex_)
                && PlotLibrary::rightTurn(it->startVertex_, it->endVertex_, startPosition)) {
                    selectionEdge_ = *it;
                    selectionEdgeIndex_ = 2;
                    interval = selectionProp_.getZoom().zZoom_;
            }
        }
    }
    // if selectionEdgeIndex_ != -1, we are outside of the cube
    if (selectionEdgeIndex_ != -1) {
        // determine position of the selection planes by orthogonal projection of
        // start-click onto the selection edge (scaled dot product)
        tgt::dvec2 edge = selectionEdge_.endVertex_ - selectionEdge_.startVertex_;
        tgt::dvec2 click = startPosition - selectionEdge_.startVertex_;
        double dot = (edge.x*click.x + edge.y*click.y) / (tgt::length(edge) * tgt::length(edge));

        // calculate plot coordinates of selection plane
        if (selectionEdge_.ascOrientation_)
            selectStart_ = interval.getLeft() + (dot*interval.size());
        else
            selectStart_ = interval.getRight() - (dot*interval.size());

        // clamp value to interval
        selectStart_ = interval.clampValue(selectStart_);
        selectEnd_ = selectStart_;
        return true;
    }
    else
        return false; // no selectionEdge found
}

bool PlotProcessor::calculateSelectEnd(tgt::dvec2 mousePressPosition) {
    // check if we are in selection mode
    if (selectionEdgeIndex_ != -1) {
        // get current interval of selected axis
        Interval<plot_t> interval;
        if (selectionEdgeIndex_ == 0)
            interval = selectionProp_.getZoom().xZoom_;
        else if (selectionEdgeIndex_ == 1)
            interval = selectionProp_.getZoom().yZoom_;
        else if (selectionEdgeIndex_ == 2)
            interval = selectionProp_.getZoom().zZoom_;
        // determine position of the selection planes by orthogonal projection of
        // start-click onto the selection edge (scaled dot product)
        tgt::dvec2 edge = selectionEdge_.endVertex_ - selectionEdge_.startVertex_;
        tgt::dvec2 click = mousePressPosition - selectionEdge_.startVertex_;
        double dot = (edge.x*click.x + edge.y*click.y) / (tgt::length(edge) * tgt::length(edge));

        // calculate plot coordinates of selection plane
        if (selectionEdge_.ascOrientation_)
            selectEnd_ = interval.getLeft() + (dot*interval.size());
        else
            selectEnd_ = interval.getRight() - (dot*interval.size());

        // clamp value to interval
        selectEnd_ = interval.clampValue(selectEnd_);
        return true;
    }
    else
        return false;
}

void PlotProcessor::regenDisplayLists() {
    regenDataList_ = true;
    regenPickingList_ = true;
}

void PlotProcessor::renderLegends() {
    if (renderLegend_.get()){
        // render legends
        int i = 0;
        plotLib_.setDrawingColor(tgt::Color::black);
        plotLib_.setLineWidth(1);
        plotLib_.setFontSize(9);
        plotLib_.setFontColor(tgt::Color::black);
        for (std::vector<PlotEntitySettings>::const_iterator it = plotEntitiesProp_.get().begin(); it < plotEntitiesProp_.get().end(); ++it) {
            // check if used colormap
            if (it->getOptionalColumnIndex() != -1) {
                plotLib_.setColorMap(it->getColorMap());
                plotLib_.renderColorMapLegend(data_, it->getOptionalColumnIndex(), i);
                ++i;
            }
        }
    }
}

void PlotProcessor::createPlotLabels() {
    plotLib_.resetPlotLabels();
    PlotSelectionProperty::LabelSelectionIterator lit = selectionProp_.getLabelsBegin();
    if (lit == selectionProp_.getLabelsEnd()) // no labels
        return;
    std::stringstream ss;
    for (; lit != selectionProp_.getLabelsEnd(); ++lit) {
        if (lit->isTablePosition()) {
            tgt::ivec2 cell = lit->getTablePosition();
            if (cell.x >= -1 && cell.x < data_.getRowsCount() && cell.y >= 0 && cell.y < data_.getColumnCount()) {
                int start = 0;
                int end = data_.getRowsCount();
                if (cell.x != -1) {
                    start = cell.x;
                    end = cell.x+1;
                }
                for (int i = start; i < end; ++i) {
                    const PlotRowValue& row = data_.getRow(i);
                    plot_t x = row.getValueAt(plotEntitiesProp_.getXColumnIndex());
                    if (threeDimensional_) {
                        plot_t y = row.getValueAt(plotEntitiesProp_.getYColumnIndex());
                        plot_t z = row.getValueAt(lit->getTablePosition().y);
                        if (selectionProp_.getZoom().xZoom_.contains(x) && selectionProp_.getZoom().yZoom_.contains(y)
                            && selectionProp_.getZoom().zZoom_.contains(z)) {
                            tgt::dvec3 viewportCoords = plotLib_.convertPlotCoordinatesToViewport3(tgt::dvec3(x, y, z));
                            ss.str("");
                            ss.clear();
                            ss << std::fixed << std::setprecision(4) << "x: " << x << std::endl << "y: " << y << std::endl <<  "z: " << z;
                            plotLib_.addPlotLabel(ss.str(), viewportCoords, tgt::Color::black, 10, SmartLabel::CENTERED);
                        }
                    }
                    else {
                        plot_t y = row.getValueAt(lit->getTablePosition().y);
                        if (selectionProp_.getZoom().xZoom_.contains(x) && selectionProp_.getZoom().yZoom_.contains(y)) {
                            tgt::dvec3 viewportCoords = plotLib_.convertPlotCoordinatesToViewport3(tgt::dvec3(x, y, 0));
                            ss.str("");
                            ss.clear();
                            ss << std::fixed << std::setprecision(4) << "x: " << x << std::endl << "y: " << y;
                            plotLib_.addPlotLabel(ss.str(), viewportCoords, tgt::Color::black, 10, SmartLabel::CENTERED);
                        }
                    }
                }
            }
        }
    }
}

void PlotProcessor::createSimpleZoomState() {
    if (selectionProp_.get().empty()) {
        PlotPredicateBetweenOrEqual xPred(-1, 1);
        PlotPredicateBetweenOrEqual yPred(-1, 1);
        PlotSelection selection;
        selection.addPredicate(PlotSelectionProperty::X_AXIS_COLUMN, &xPred);
        selection.addPredicate(PlotSelectionProperty::Y_AXIS_COLUMN, &yPred);
        if (threeDimensional_) {
            PlotPredicateBetweenOrEqual zPred(-1, 1);
            selection.addPredicate(PlotSelectionProperty::Z_AXIS_COLUMN, &zPred);
        }
        PlotSelectionEntry entry = {selection, false, false, true};
        selectionProp_.add(entry, false);
    }
}

} // namespace voreen
