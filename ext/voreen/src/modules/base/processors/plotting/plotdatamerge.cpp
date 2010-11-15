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

#include "voreen/modules/base/processors/plotting/plotdatamerge.h"
#include "voreen/core/processors/processorwidgetfactory.h"

#include "voreen/core/plotting/plotdata.h"

namespace voreen {

const std::string PlotDataMerge::loggerCat_("voreen.Plotting.PlotDataMerge");


PlotDataMerge::PlotDataMerge()
    :Processor()
    , outPort_(Port::OUTPORT,"PlotData.OutPort")
    , inPortFirst_(Port::INPORT,"PlotData.InPortFirst")
    , inPortSecond_(Port::INPORT,"PlotData.InPortSecond")
    , pFirstData_(0)
    , pSecondData_(0)
    , pDataOut_(0)
    , swapData_("swap", "Swap Data",Processor::VALID)
    , ignoreColumnLabel_("Ignore Columnlabels","Ignore Columnlabels",Processor::VALID)
    , maxTableColumnWidth_("Max Column Width","Max Column Width",0,0,500,Processor::VALID)
    , keyColumnColor_("Key-Column Color","Key-Column Color",tgt::vec4(200.0/255.0,200.0/255.0,200.0/255.0,1.0),
        tgt::vec4(0.0),tgt::vec4(1.0),Processor::VALID)
    , dataColumnColor_("Data-Column Color","Data-Column Color",tgt::vec4(240.0/255.0,240.0/255.0,240.0/255.0,1.0),
        tgt::vec4(0.0),tgt::vec4(1.0),Processor::VALID)
{
    swapData_.onChange(CallMemberAction<PlotDataMerge>(this, &PlotDataMerge::activateComputeButton));
    ignoreColumnLabel_.onChange(CallMemberAction<PlotDataMerge>(this, &PlotDataMerge::activateComputeButton));
    keyColumnColor_.onChange(CallMemberAction<PlotDataMerge>(this, &PlotDataMerge::updateView));
    dataColumnColor_.onChange(CallMemberAction<PlotDataMerge>(this, &PlotDataMerge::updateView));
    maxTableColumnWidth_.onChange(CallMemberAction<PlotDataMerge>(this, &PlotDataMerge::updateView));

    keyColumnColor_.setViews(Property::COLOR);
    dataColumnColor_.setViews(Property::COLOR);

    addPort(inPortFirst_);
    addPort(inPortSecond_);
    addPort(outPort_);

    addProperty(&swapData_);
    addProperty(&ignoreColumnLabel_);
    addProperty(maxTableColumnWidth_);

    addProperty(keyColumnColor_);
    addProperty(dataColumnColor_);

    setPropertyGroupGuiName("Input Configuration","Input Configuration");
    setPropertyGroupGuiName("Merge Parameter","Merge Parameter");
    swapData_.setGroupID("Input Configuration");
    ignoreColumnLabel_.setGroupID("Merge Parameter");

    setPropertyGroupGuiName("Widget-Configuration","Widget-Configuration");
    keyColumnColor_.setGroupID("Widget-Configuration");
    dataColumnColor_.setGroupID("Widget-Configuration");
    maxTableColumnWidth_.setGroupID("Widget-Configuration");
}

PlotDataMerge::~PlotDataMerge() {
}

Processor* PlotDataMerge::create() const {
    return new PlotDataMerge();
}

std::string PlotDataMerge::getProcessorInfo() const {
    return std::string("Merge the Data on both Inport to the Outport.");
}

void PlotDataMerge::initialize() throw (VoreenException) {
    Processor::initialize();

    if (inPortFirst_.hasData()) {
        readData();
        calculate();
    }
    setOutPortData();
}
void PlotDataMerge::deinitialize() throw (VoreenException) {
    outPort_.setData(0, false);
    delete pDataOut_;
    pFirstData_ = 0;
    pSecondData_ = 0;
    pDataOut_ = 0;

    Processor::deinitialize();
}


bool PlotDataMerge::isReady() const {
    return true;
}

bool PlotDataMerge::isEndProcessor() const {
    return (!outPort_.isConnected());
}

void PlotDataMerge::setOutPortData(){
    if (outPort_.isConnected()){
        if ((pDataOut_)){
            outPort_.setData(pDataOut_);
        }
        else
            outPort_.setData(0);
    }
}

void PlotDataMerge::readData() {
    if (!swapData_.get()) {
        if (dynamic_cast<PlotData*>(inPortFirst_.getData())) {
            pFirstData_ = dynamic_cast<PlotData*>(inPortFirst_.getData());
        }
        else if (!inPortFirst_.hasData())
            pFirstData_ = 0;
        else {
            pFirstData_ = 0;
            LWARNING("Data on the first PlotBase-Port ist not a PlotData-Type");
        }
        if (dynamic_cast<PlotData*>(inPortSecond_.getData())) {
            pSecondData_ = dynamic_cast<PlotData*>(inPortSecond_.getData());
        }
        else if (!inPortSecond_.hasData())
            pSecondData_ = 0;
        else {
            pSecondData_ = 0;
            LWARNING("Data on the second PlotBase-Port ist not a PlotData-Type");
        }
    }
    else {
        if (dynamic_cast<PlotData*>(inPortFirst_.getData())) {
            pSecondData_ = dynamic_cast<PlotData*>(inPortFirst_.getData());
        }
        else if (!inPortFirst_.hasData())
            pSecondData_ = 0;
        else {
            pSecondData_ = 0;
            LWARNING("Data on the first PlotBase-Port ist not a PlotData-Type");
        }
        if (dynamic_cast<PlotData*>(inPortSecond_.getData())) {
            pFirstData_ = dynamic_cast<PlotData*>(inPortSecond_.getData());
        }
        else if (!inPortSecond_.hasData())
            pFirstData_ = 0;
        else {
            pFirstData_ = 0;
            LWARNING("Data on the second PlotBase-Port ist not a PlotData-Type");
        }
    }
}

const PlotData* PlotDataMerge::getPlotDataOut() const {
    return pDataOut_;
}

const PlotData* PlotDataMerge::getPlotDataFirst() const {
    return pFirstData_;
}

const PlotData* PlotDataMerge::getPlotDataSecond() const {
    return pSecondData_;
}

void PlotDataMerge::activateComputeButton() {
    if (initialized_) {
        readData();
        calculate();
    }
}

void PlotDataMerge::updateView() {
    if (getProcessorWidget()){
        getProcessorWidget()->updateFromProcessor();
    }
}

void PlotDataMerge::process() {
    if (inPortFirst_.hasData() && inPortSecond_.hasData() && (inPortFirst_.hasChanged() || inPortSecond_.hasChanged())) {
        readData();
        calculate();
    }
    else if (!inPortFirst_.hasData() || !inPortSecond_.hasData()){
        readData();
        calculate();
    }
    else if (!inPortFirst_.hasData() && !inPortSecond_.hasData()){
        pFirstData_ = 0;
        pSecondData_ = 0;
        outPort_.setData(pDataOut_);
        if (getProcessorWidget()){
            getProcessorWidget()->updateFromProcessor();
        }
    }
    else if (inPortFirst_.hasData() && inPortSecond_.hasData() && (!inPortFirst_.hasChanged() && !inPortSecond_.hasChanged())) {
        calculate();
    }
    else {
        if (getProcessorWidget()){
            getProcessorWidget()->updateFromProcessor();
        }
    }
}

void PlotDataMerge::calculate() {
    tgtAssert(isInitialized(), "not initialized");

    PlotData* oldData;
    oldData = pDataOut_;
    if (pFirstData_ && !pSecondData_)
        pDataOut_ = new PlotData(*pFirstData_);
    else if (!pFirstData_ && pSecondData_)
        pDataOut_ = new PlotData(*pSecondData_);
    else if (!pFirstData_ && !pSecondData_)
        pDataOut_ = 0;
    else {
        pDataOut_ = new PlotData(1,5);
        if (ignoreColumnLabel_.get())
            pFirstData_->mergeWith(*pSecondData_,*pDataOut_,PlotData::IGNORECOLUMNLABELS);
        else
            pFirstData_->mergeWith(*pSecondData_,*pDataOut_);
    }
    setOutPortData();
    if (getProcessorWidget()){
        getProcessorWidget()->updateFromProcessor();
    }
    delete oldData;
}

void PlotDataMerge::swapPlotData() {
    if (swapData_.get())
        swapData_.set(false);
    else
        swapData_.set(true);
}

void PlotDataMerge::ignoreColumnLabels() {
    if (ignoreColumnLabel_.get())
        ignoreColumnLabel_.set(false);
    else
        ignoreColumnLabel_.set(true);
}

}
