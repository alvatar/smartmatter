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

#ifndef VRN_PLOTDATAMERGE_H
#define VRN_PLOTDATAMERGE_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/ports/plotport.h"

#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/vectorproperty.h"
#include "voreen/core/properties/intproperty.h"

namespace voreen {

class PlotBase;
class PlotData;

class PlotDataMerge : public Processor {
public:
    PlotDataMerge();
    virtual Processor* create() const;
    virtual std::string getCategory() const  { return "Plotting"; }
    virtual std::string getClassName() const { return "PlotDataMerge"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; }
    //virtual bool usesExpensiveComputation() const { return true; }
    virtual std::string getProcessorInfo() const;
    virtual bool isEndProcessor() const;
    virtual bool isReady() const;

    /// @see Serializer::serialize
    //virtual void serialize(XmlSerializer& s) const;

    /// @see Deserializer::deserialize
    //virtual void deserialize(XmlDeserializer& s);

    /// returns the link to the PlotData object after the merge
    const PlotData* getPlotDataOut() const;
    /// returns the link to the first PlotData object before the merge
    const PlotData* getPlotDataFirst() const;
    /// returns the link to the second PlotData object before the merge
    const PlotData* getPlotDataSecond() const;

    /// this function is to swap the PlotDatas on the Inports
    void swapPlotData();
    /// change the property of ignoreColumnLabels from false to true or otherwise
    void ignoreColumnLabels();

    /// default destructor
    virtual ~PlotDataMerge();
protected:
    virtual void process();
    virtual void initialize() throw (VoreenException);
    virtual void deinitialize() throw (VoreenException);


private:
    void setOutPortData();
    void readData();
    void calculate();
    void deleteList();
    void activateComputeButton();
    void updateView();

    PlotPort outPort_;
    PlotPort inPortFirst_;
    PlotPort inPortSecond_;

    PlotData* pFirstData_;
    PlotData* pSecondData_;
    PlotData* pDataOut_;

    BoolProperty swapData_;
    BoolProperty ignoreColumnLabel_;

    IntProperty maxTableColumnWidth_;

    FloatVec4Property keyColumnColor_;
    FloatVec4Property dataColumnColor_;

    static const std::string loggerCat_;

};

} // namespace voreen

#endif // VRN_PLOTDATAMERGE_H
