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

#ifndef VRN_CLOCKPROCESSOR_H
#define VRN_CLOCKPROCESSOR_H

#include "voreen/core/processors/processor.h"

#include "tgt/timer.h"
#include "tgt/event/eventhandler.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/optionproperty.h"

namespace voreen {

/**
 * A Processor class which performs clocking by generating TimeEvents which are
 * handled by the processor itself. A counter represented by a property is
 * increased on every TimeEvent. This counter is intended to be linked with other
 * properties in order to propagate the information about clocking.
 *
 * This processor has neither any ports nor does it require any connections an
 * can used as a 'floating' processor in the networks.
 *
 * @author: Dirk Feldmann
 * @date:   2009/11/11
 */
class ClockProcessor : public Processor {
public:
    ClockProcessor();
    virtual ~ClockProcessor();

    virtual std::string getClassName() const    { return "ClockProcessor"; }
    virtual std::string getCategory() const     { return "Utility"; }
    virtual CodeState getCodeState() const      { return CODE_STATE_STABLE; }
    virtual bool isUtility() const              { return true; }

    virtual std::string getProcessorInfo() const;
    virtual Processor* create() const { return new ClockProcessor(); }

    virtual void timerEvent(tgt::TimeEvent* te);

    /**
     * Indicates whether the timer is currently running.
     */
    bool isTimerActive() const;

    /**
     * Starts the timer with the intervals contained in intervalProp_ property.
     */
    void startTimer();

    /**
     * Stops the timer.
     */
    void stopTimer();

    void resetCounter();

protected:
    enum CounterStyle { COUNTER_LINEAR, COUNTER_CYCLIC, COUNTER_REVERSING };

protected:
    virtual void process();
    virtual void initialize() throw (VoreenException);

    void triggerHighResCounter();
    void toggleTimer();
    void onCounterStyleChange();
    void onIntervalChange();
    void onPeriodChange();
    void onResolutionChange();
    void onUseHighResCounterChange();

protected:
    int counter_;
    float highResCounter_;
    int period_;
    bool timerIsActive_;                /** Indicates whether the timer is active. */
    tgt::Timer* timer_;                 /** Timer object. */
    tgt::EventHandler eventHandler_;    /** A local eventhanlde which is added to the timer. */
    BoolProperty enableTimerProp_;          /** Determines whether the clocking is active. */
    IntProperty intervalProp_;              /** Determines the inveral for the clock. */
    OptionProperty<ClockProcessor::CounterStyle> counterStyleProp_;
    IntProperty periodProp_;                /** Period for cyclic and reversing counters. */
    IntProperty tickCounterProp_;           /** The counter which is modified on every timeEvent. */
    BoolProperty useHighResCounterProp_;
    FloatProperty resolutionProp_;
    FloatProperty highResCounterProp_;
    ButtonProperty resetProp_;
};

}   // namespace

#endif
