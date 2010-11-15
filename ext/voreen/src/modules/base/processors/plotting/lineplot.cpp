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

#include "voreen/modules/base/processors/plotting/lineplot.h"

#include "voreen/core/plotting/plotrow.h"

#include <iomanip>

namespace voreen {

const std::string LinePlot::loggerCat_("voreen.LinePlot");

LinePlot::LinePlot()
    : PlotProcessor(PlotEntitySettings::LINE, false)
    , lineWidth_("lineWidth", "Line Width", 2.f, 1.f, 5.f)
    , pointSize_("pointSize", "Point Size", 2.f, 1.f, 9.f)
    , logXAxis_("logXAxes", "Logarithmic x Axis", false)
    , logYAxis_("logYAxes", "Logarithmic y Axis", false)
    , renderLineLabel_("renderLineLabel", "Line Labels", false)
    , dataList_(0)
    , pickingList_(0)
{
    plotEntitiesProp_.setGuiName("Line Data");
    addProperty(selectionPlaneColor_);
    addProperty(renderMousePosition_);
    addProperty(discreteStep_);
    addProperty(renderXHelperLines_);
    addProperty(renderYHelperLines_);
    addProperty(logXAxis_);
    addProperty(logYAxis_);
    addProperty(renderLineLabel_);
    addProperty(lineWidth_);
    addProperty(pointSize_);
    addProperty(xScaleStep_);
    addProperty(yScaleStep_);
    addProperty(marginLeft_);
    addProperty(marginRight_);
    addProperty(marginBottom_);
    addProperty(marginTop_);

    // group properties
    renderLineLabel_.setGroupID("line");
    lineWidth_.setGroupID("line");
    pointSize_.setGroupID("line");
    setPropertyGroupGuiName("line", "Line Settings");

    logXAxis_.setGroupID("axis");
    logYAxis_.setGroupID("axis");

    addEventProperty(eventHighlight_);
    addEventProperty(eventLabel_);
    addEventProperty(eventZoom_);
    addEventProperty(eventHighlightAdditive_);
    addEventProperty(eventLabelAdditive_);
    addEventProperty(eventZoomAdditive_);
    addEventProperty(mousePositionUpdateEvent_);
    addEventProperty(mouseEventEnterExit_);

    //if one of the following properties is changed we handle it like plot entities property is changed
    logXAxis_.onChange       (CallMemberAction<LinePlot>(this, &LinePlot::regenDisplayLists));
    logYAxis_.onChange       (CallMemberAction<LinePlot>(this, &LinePlot::regenDisplayLists));
    renderLineLabel_.onChange(CallMemberAction<LinePlot>(this, &LinePlot::regenDisplayLists));
    lineWidth_.onChange      (CallMemberAction<LinePlot>(this, &LinePlot::regenDisplayLists));
    pointSize_.onChange      (CallMemberAction<LinePlot>(this, &LinePlot::regenDisplayLists));
}

Processor* LinePlot::create() const {
    return new LinePlot();
}

std::string LinePlot::getProcessorInfo() const {
    return std::string("This processor is able to plot lines and candlesticks.");
}

void LinePlot::initialize() throw (VoreenException) {
    PlotProcessor::initialize();
    dataList_ = glGenLists(1);
    pickingList_ = glGenLists(1);
}

void LinePlot::deinitialize() throw (VoreenException) {
    if (dataList_) {
        glDeleteLists(dataList_, 1);
        dataList_ = 0;
    }
    if (pickingList_) {
        glDeleteLists(pickingList_, 1);
        pickingList_ = 0;
    }

    PlotProcessor::deinitialize();
}

void LinePlot::render() {
    outport_.activateTarget();
    plotLib_.setUsePlotPickingManager(false);
    setPlotStatus();
    if (plotLib_.setOpenGLStatus()) {
        renderAxes();
        if (regenDataList_) {
            glNewList(dataList_, GL_COMPILE);
                renderData();
            glEndList();
            regenDataList_ = false;
        }
        glCallList(dataList_);
        createLineLabels();
        plotLib_.renderLineLabels();
        createPlotLabels();
        plotLib_.renderPlotLabels();
        renderPlotLabel();
        renderMousePosition();
    }
    plotLib_.resetOpenGLStatus();
    renderSelectedRegion();
    outport_.deactivateTarget();
    plotPickingManager_.activateTarget();
    plotPickingManager_.clearTarget();
    if (enablePicking_.get()) {
        plotLib_.setUsePlotPickingManager(true);
        if (plotLib_.setOpenGLStatus() && regenPickingList_) {
            glNewList(pickingList_, GL_COMPILE);
                renderData();
            glEndList();
            regenPickingList_ = false;
        }
        glCallList(pickingList_);
        plotLib_.resetOpenGLStatus();
    }
    plotPickingManager_.deactivateTarget();
}

void LinePlot::setPlotStatus() {
    plotLib_.setWindowSize(outport_.getSize());
    plotLib_.setLogarithmicAxis(logXAxis_.get(), PlotLibrary::X_AXIS);
    plotLib_.setLogarithmicAxis(logYAxis_.get(), PlotLibrary::Y_AXIS);
    plotLib_.setAxesWidth(axesWidth_.get());
    plotLib_.setDrawingColor(tgt::Color::black);
    plotLib_.setLineWidth(lineWidth_.get());
    plotLib_.setMaxGlyphSize(pointSize_.get());
    plotLib_.setMarginBottom(marginBottom_.get());
    plotLib_.setMarginTop(marginTop_.get());
    plotLib_.setMarginLeft(marginLeft_.get());
    plotLib_.setMarginRight(marginRight_.get());
    plotLib_.setMinimumScaleStep(xScaleStep_.get(), PlotLibrary::X_AXIS);
    plotLib_.setMinimumScaleStep(yScaleStep_.get(), PlotLibrary::Y_AXIS);
    plotLib_.setDomain(selectionProp_.getZoom().xZoom_, PlotLibrary::X_AXIS);
    plotLib_.setDomain(selectionProp_.getZoom().yZoom_, PlotLibrary::Y_AXIS);
}

void LinePlot::renderData() {
    plotLib_.setHighlightColor(highlightColor_.get());
    // iterate through plot entity settings
    std::vector<PlotEntitySettings>::const_iterator it = plotEntitiesProp_.get().begin();
    for (; it < plotEntitiesProp_.get().end(); ++it) {
        // handle candle stick
        if (it->getCandleStickFlag()) {
            plotLib_.setDrawingColor(it->getFirstColor());
            plotLib_.setFillColor(it->getSecondColor());
            plotLib_.setFontColor(it->getFirstColor());
            plotLib_.renderCandlesticks(data_, plotEntitiesProp_.getXColumnIndex(), it->getStickTopColumnIndex(),
                it->getStickBottomColumnIndex(), it->getCandleTopColumnIndex(), it->getCandleBottomColumnIndex());
        }
        // handle line
        else {
            // render errorline / errorbar
            if (it->getOptionalColumnIndex() != -1) {
                plotLib_.setDrawingColor(it->getSecondColor());
                if (it->getErrorbarFlag())
                    plotLib_.renderErrorbars(data_, plotEntitiesProp_.getXColumnIndex(),
                            it->getMainColumnIndex(), it->getOptionalColumnIndex());
                else {
                    tgt::Color c = it->getSecondColor();
                    c.a = 0.5;
                    plotLib_.setFillColor(c);
                    if (it->getSplineFlag())
                        plotLib_.renderErrorspline(data_, plotEntitiesProp_.getXColumnIndex(),
                                it->getMainColumnIndex(), it->getOptionalColumnIndex());
                    else
                        plotLib_.renderErrorline(data_, plotEntitiesProp_.getXColumnIndex(),
                                it->getMainColumnIndex(), it->getOptionalColumnIndex());
                }
            }
            plotLib_.setDrawingColor(it->getFirstColor());
            plotLib_.setFontColor(it->getFirstColor());
            plotLib_.setLineStyle(it->getLineStyle());
            if (it->getSplineFlag())
                plotLib_.renderSpline(data_, plotEntitiesProp_.getXColumnIndex(), it->getMainColumnIndex());
            else
                plotLib_.renderLine(data_, plotEntitiesProp_.getXColumnIndex(), it->getMainColumnIndex());
        }
    }
    LGL_ERROR;
}

void LinePlot::renderAxes() {
    if (renderAxes_.get()) {
        plotLib_.setDrawingColor(tgt::Color::black);
        plotLib_.renderAxes();
        plotLib_.setDrawingColor(tgt::Color(0, 0, 0, .5f));
        if (renderScales_.get()) {
            plotLib_.setFontSize(10);
            plotLib_.setFontColor(tgt::Color::black);
            if (data_.getColumnType(plotEntitiesProp_.getXColumnIndex()) == PlotBase::STRING)
                plotLib_.renderAxisLabelScales(data_, plotEntitiesProp_.getXColumnIndex(), renderXHelperLines_.get());
            else
                plotLib_.renderAxisScales(PlotLibrary::X_AXIS, renderXHelperLines_.get());
            plotLib_.renderAxisScales(PlotLibrary::Y_AXIS, renderYHelperLines_.get());
            plotLib_.setFontSize(12);
            plotLib_.renderAxisLabel(PlotLibrary::X_AXIS, xLabel_.get());
            plotLib_.renderAxisLabel(PlotLibrary::Y_AXIS, yLabel_.get());
        }
    }
}

void LinePlot::readFromInport() {
    // create local copy of data and assign it to property
    if (dynamic_cast<PlotData*>(inport_.getData())) {
        data_ = *dynamic_cast<PlotData*>(inport_.getData());
        if (!data_.sorted())
            data_.sortRows();
        dataProp_.set(&data_);
        plotEntitiesProp_.setPlotData(&data_);
        selectionProp_.setPlotData(&data_);
        plotPickingManager_.setColumnCount(data_.getColumnCount());
        inportHasPlotFunction_ = false;
        discreteStep_.setVisible(false);
    }
    else if (dynamic_cast<PlotFunction*>(inport_.getData()) && inport_.getData()->getKeyColumnCount() == 1) {
        function_ = *dynamic_cast<PlotFunction*>(inport_.getData());
        inportHasPlotFunction_ = true;
        selectDataFromFunction();
        createSimpleZoomState();
        discreteStep_.setVisible(true);
    }
    else {
        LWARNINGC("LinePlot", "LinePlot can only handle PlotData objects and PlotFunction objects with one key column");
        data_ = PlotData(0,0);
        dataProp_.set(&data_);
        plotEntitiesProp_.setPlotData(&data_);
        selectionProp_.setPlotData(&data_);
        plotPickingManager_.setColumnCount(data_.getColumnCount());
    }
}

void LinePlot::calcDomains() {
    if (plotEntitiesProp_.dataValid() && !inportHasPlotFunction_) {
        Interval<plot_t> xDomain = data_.getInterval(plotEntitiesProp_.getXColumnIndex());
        Interval<plot_t> yDomain = Interval<plot_t>();
        std::vector<PlotEntitySettings>::const_iterator it = plotEntitiesProp_.get().begin();
        for (; it < plotEntitiesProp_.get().end(); ++it) {
            if (!it->getCandleStickFlag()) {
                Interval<plot_t> lineDomain = data_.getInterval(it->getMainColumnIndex());
                if (it->getOptionalColumnIndex() != -1) {
                    Interval<plot_t> errorDomain = data_.getInterval(it->getOptionalColumnIndex());
                    plot_t error = std::max(abs(errorDomain.getLeft()), abs(errorDomain.getRight()));
                    lineDomain = Interval<plot_t>(lineDomain.getLeft()-error, lineDomain.getRight()+error);
                }
                yDomain.unionWith(lineDomain);
            }
            else {
                yDomain.unionWith(data_.getInterval(it->getStickTopColumnIndex()));
                yDomain.unionWith(data_.getInterval(it->getStickBottomColumnIndex()));
                yDomain.unionWith(data_.getInterval(it->getCandleTopColumnIndex()));
                yDomain.unionWith(data_.getInterval(it->getCandleBottomColumnIndex()));
            }
        }
        yDomain.enlarge(1.1);
        selectionProp_.setBaseZoomState(PlotZoomState(xDomain, yDomain));
    }
}

void LinePlot::toggleProperties() {
    if (plotEntitiesProp_.dataValid()) {
        if (data_.getColumnType(plotEntitiesProp_.getXColumnIndex()) == PlotBase::STRING) {
            xScaleStep_.setVisible(false);
            logXAxis_.set(false);
            logXAxis_.setVisible(false);
        }
        else {
            xScaleStep_.setVisible(true);
            logXAxis_.setVisible(true);
        }
    }
}

void LinePlot::createLineLabels() {
    plotLib_.resetLineLabels();
    if (!renderLineLabel_.get())
        return;
    plot_t xr = selectionProp_.getZoom().xZoom_.getRight();
    // we have to find last rendered x value
    int row = data_.getRowsCount() - 1;
    bool xAxisIsString = (data_.getColumnType(plotEntitiesProp_.getXColumnIndex()) == PlotBase::STRING);
    if (xAxisIsString)
        row = static_cast<int>(floor(xr));
    else {
        while(row > 0 && data_.getRow(row - 1).getValueAt(plotEntitiesProp_.getXColumnIndex()) > xr)
            --row;
    }
    if (row < 1)
        return;
    for (std::vector<PlotEntitySettings>::const_iterator it = plotEntitiesProp_.get().begin();
            it < plotEntitiesProp_.get().end(); ++it) {
        if (it->getCandleStickFlag()) {
            plot_t stickTop = data_.getRow(row).getValueAt(it->getStickTopColumnIndex());
            plot_t stickBottom = data_.getRow(row).getValueAt(it->getStickBottomColumnIndex());
            plot_t candleTop = data_.getRow(row).getValueAt(it->getCandleTopColumnIndex());
            plot_t candleBottom = data_.getRow(row).getValueAt(it->getCandleBottomColumnIndex());
            if (selectionProp_.getZoom().yZoom_.contains(stickTop))
                plotLib_.addLineLabel(data_.getColumnLabel(it->getStickTopColumnIndex()),
                                         tgt::dvec3(10, 0, 0) + plotLib_.convertPlotCoordinatesToViewport3(
                                         tgt::dvec3(xr, stickTop, 0)),
                                         it->getFirstColor(), 12, SmartLabel::MIDDLELEFT);
            if (selectionProp_.getZoom().yZoom_.contains(stickBottom))
                plotLib_.addLineLabel(data_.getColumnLabel(it->getStickBottomColumnIndex()),
                                         tgt::dvec3(10, 0, 0) + plotLib_.convertPlotCoordinatesToViewport3(
                                         tgt::dvec3(xr, stickBottom, 0)),
                                         it->getFirstColor(), 12, SmartLabel::MIDDLELEFT);
            if (selectionProp_.getZoom().yZoom_.contains(candleTop))
                plotLib_.addLineLabel(data_.getColumnLabel(it->getCandleTopColumnIndex()),
                                         tgt::dvec3(10, 0, 0) + plotLib_.convertPlotCoordinatesToViewport3(
                                         tgt::dvec3(xr, candleTop, 0)),
                                         it->getSecondColor(), 12, SmartLabel::MIDDLELEFT);
            if (selectionProp_.getZoom().yZoom_.contains(candleBottom))
                plotLib_.addLineLabel(data_.getColumnLabel(it->getCandleBottomColumnIndex()),
                                         tgt::dvec3(10, 0, 0) + plotLib_.convertPlotCoordinatesToViewport3(
                                         tgt::dvec3(xr, candleBottom, 0)),
                                         it->getSecondColor(), 12, SmartLabel::MIDDLELEFT);
        }
        else {
            tgt::dvec2 last(xAxisIsString ? static_cast<double>(row) :
                                            data_.getRow(row).getValueAt(plotEntitiesProp_.getXColumnIndex()),
                                            data_.getRow(row).getValueAt(it->getMainColumnIndex()));
            tgt::dvec2 lastButOne(xAxisIsString ? static_cast<double>(row) - 1 :
                                                  data_.getRow(row-1).getValueAt(plotEntitiesProp_.getXColumnIndex()),
                                 data_.getRow(row-1).getValueAt(it->getMainColumnIndex()));
            double dydx = (last.y-lastButOne.y)/(last.x-lastButOne.x);
            last.y = dydx * (xr-last.x) + last.y;
            if (selectionProp_.getZoom().yZoom_.contains(last.y)) {
                plotLib_.addLineLabel(data_.getColumnLabel(it->getMainColumnIndex()),
                                         tgt::dvec3(10, 0, 0) + plotLib_.convertPlotCoordinatesToViewport3(
                                         tgt::dvec3(xr, last.y, 0)),
                                         it->getFirstColor(), 12, SmartLabel::MIDDLELEFT);
            }
        }
    }
}

void LinePlot::createPlotLabels() {
    plotLib_.resetPlotLabels();
    PlotSelectionProperty::LabelSelectionIterator lit = selectionProp_.getLabelsBegin();
    if (lit == selectionProp_.getLabelsEnd()) // no labels
        return;
    std::stringstream ss;
    // iterate label selection
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
                    // check if line or error is labeled
                    std::vector<PlotEntitySettings>::const_iterator eit;
                    for (eit = plotEntitiesProp_.get().begin(); eit < plotEntitiesProp_.get().end(); ++eit) {
                        if (eit->getMainColumnIndex() == lit->getTablePosition().y
                            || eit->getStickTopColumnIndex() == lit->getTablePosition().y
                            || eit->getCandleBottomColumnIndex() == lit->getTablePosition().y
                            || eit->getCandleTopColumnIndex() == lit->getTablePosition().y
                            || eit->getStickBottomColumnIndex() == lit->getTablePosition().y) {
                            const PlotRowValue& row = data_.getRow(i);
                            plot_t x = (data_.getColumnType(plotEntitiesProp_.getXColumnIndex()) == PlotBase::STRING ?
                                            i : row.getValueAt(plotEntitiesProp_.getXColumnIndex()));
                            plot_t y = row.getValueAt(lit->getTablePosition().y);
                            if (selectionProp_.getZoom().xZoom_.contains(x) && selectionProp_.getZoom().yZoom_.contains(y)) {
                                tgt::dvec3 viewportCoords = plotLib_.convertPlotCoordinatesToViewport3(tgt::dvec3(x, y, 0));
                                ss.str("");
                                ss.clear();
                                ss << std::fixed << std::setprecision(4) << "x: " << x << std::endl << "y: " << y;
                                plotLib_.addPlotLabel(ss.str(), viewportCoords, tgt::Color::black, 10, SmartLabel::CENTERED);
                            }
                        }
                        else if (eit->getOptionalColumnIndex() == lit->getTablePosition().y && eit->getErrorbarFlag()) {
                            const PlotRowValue& row = data_.getRow(i);
                            plot_t x = row.getValueAt(plotEntitiesProp_.getXColumnIndex());
                            plot_t y = row.getValueAt(eit->getMainColumnIndex());
                            plot_t error = row.getValueAt(lit->getTablePosition().y);
                            if (selectionProp_.getZoom().xZoom_.contains(x) && selectionProp_.getZoom().yZoom_.contains(y)) {
                                tgt::dvec3 viewportCoords = plotLib_.convertPlotCoordinatesToViewport3(tgt::dvec3(x, y, 0));
                                ss.str("");
                                ss.clear();
                                ss << std::fixed << std::setprecision(4) << "Error: " << error;
                                plotLib_.addPlotLabel(ss.str(), viewportCoords, tgt::Color::black, 10, SmartLabel::CENTERED);
                            }
                        }
                    }
                }
            }
        }
    }
}

} // namespace voreen
