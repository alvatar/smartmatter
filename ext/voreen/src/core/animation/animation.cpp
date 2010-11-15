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

#include "voreen/core/animation/animation.h"
#include "voreen/core/animation/animatedprocessor.h"
#include "voreen/core/animation/propertytimeline.h"
#include "voreen/core/animation/undoableanimation.h"
#include "voreen/core/processors/processorfactory.h"
#include "voreen/core/properties/shaderproperty.h"
#include "voreen/core/animation/serializationfactories.h"
#include "voreen/core/animation/interpolationfunctionfactory.h"

#include "voreen/core/datastructures/transfunc/transfunc.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "tgt/camera.h"

namespace voreen {

const std::string Animation::loggerCat_("voreen.Animation");

Animation::Animation() {}

Animation::Animation(ProcessorNetwork* network)
    : fps_(25)
    , undoSteps_(100)
    , currentTime_(0)
    , duration_(5*60)
    , isRendering_(false) {

    const std::vector<Processor*> procs = network->getProcessors();

    std::vector<Processor*>::const_iterator it2;
    for (it2=procs.begin();it2!= procs.end();it2++) {
        processors_.push_back(new AnimatedProcessor(*it2));
    }

    // register this as observer on all propertytimelines for undo / redo
    const std::vector<AnimatedProcessor*> animproc = getAnimatedProcessors();
    std::vector<AnimatedProcessor*>::const_iterator it;
    for (it = animproc.begin(); it != animproc.end(); ++it) {
        const std::vector<PropertyTimeline*> timelines = (*it)->getPropertyTimelines();
        std::vector<PropertyTimeline*>::const_iterator it2;
        for (it2 = timelines.begin(); it2 != timelines.end(); ++it2) {
            (*it2)->registerUndoObserver(this);
        }
    }

    // register this as observer in the processornetwork to register added and removed processors
    network->addObserver(this);
}

Animation::~Animation() {
    std::vector<AnimatedProcessor*>::const_iterator it;
    for (it = processors_.begin(); it!=processors_.end(); ++it) {
        delete (*it);
    }
    processors_.clear();
}

bool Animation::isEmpty() const {
    std::vector<AnimatedProcessor*>::const_iterator it;
    bool result = true;
    for (it = processors_.begin(); it!=processors_.end(); ++it) {
        const std::vector<PropertyTimeline*> propertyTimelines = (*it)->getPropertyTimelines();
        std::vector<PropertyTimeline*>::const_iterator ptit;
        for (ptit = propertyTimelines.begin(); ptit != propertyTimelines.end(); ++ptit) {
            result &= (*ptit)->isEmpty();
        }
    }
    return result;
}

const std::vector<AnimatedProcessor*>& Animation::getAnimatedProcessors() const {
    return processors_;
}

void Animation::renderAt(float time) {
    if (isRendering_)
        return;

    currentTime_ = time;
    isRendering_ = true;

    std::vector<AnimatedProcessor*>::const_iterator it;
    for (it = processors_.begin(); it!=processors_.end(); ++it) {
        (*it)->renderAt(time);
    }

    isRendering_ = false;
}

void Animation::animationChanged(UndoableAnimation* changedObject) {
    std::deque<UndoableAnimation*>::iterator it;
    // change made -> delete all redostates
    for (it = lastUndos_.begin(); it != lastUndos_.end(); ++it) {
        (*it)->clearRedoStates();
    }
    lastUndos_.clear();

    // save the pointer to the last changed propertytimeline
    lastChanges_.push_back(changedObject);

    // if there are already too many undosteps -> delete oldest states
    while (lastChanges_.size() > static_cast<size_t>(undoSteps_)) {
        UndoableAnimation* tmp = lastChanges_.front();
        tmp->removeOldestUndoState();
        lastChanges_.pop_front();
    }
}

void Animation::undoLastChange() {
    if (lastChanges_.size() > 0) {
        //call the undo-function of the last registered changed propertytimeline
        UndoableAnimation* temp = lastChanges_.back();
        lastChanges_.pop_back();
        lastUndos_.push_back(temp);
        temp->undo();
    }
    else
        LWARNING("Undo impossible: No last changes");
}

void Animation::redoLastUndo() {
    if (lastUndos_.size() > 0) {
        // call the redo-function of the last undid-propertytimeline
        UndoableAnimation* temp = lastUndos_.back();
        lastUndos_.pop_back();
        lastChanges_.push_back(temp);
        temp->redo();
    }
    else
        LWARNING("Redo impossible: No last undo state");
}

float Animation::getFPS() const {
    return fps_;
}

void Animation::setFPS(float fps) {
    fps_ = fps;
}

void Animation::setUndoSteps(int steps) {
    if (steps < 1)
        undoSteps_ = 1;
    else
        undoSteps_= steps;

    // if the new value is smaller than the number of undostates -> delete oldest states
    while (lastChanges_.size() > static_cast<size_t>(undoSteps_)) {
        UndoableAnimation* tmp = lastChanges_.front();
        tmp->removeOldestUndoState();
        lastChanges_.pop_front();
    }
}

void Animation::setNetwork(ProcessorNetwork* network) {
    network_ = network;
}

void Animation::networkChanged() {
    // do nothing
}

void Animation::processorAdded(const Processor *processor) {
    // called if a new processor is added to the rendernetwork
    AnimatedProcessor* proc = new AnimatedProcessor(const_cast<Processor*>(processor));
    processors_.push_back(proc);

    // registration of this class for undo / redo at the new propertytimelines
    const std::vector<PropertyTimeline*>& timelines = proc->getPropertyTimelines();
    std::vector<PropertyTimeline*>::const_iterator it2;
    for (it2 = timelines.begin(); it2 != timelines.end(); ++it2) {
        (*it2)->registerUndoObserver(this);
    }
    // inform animationobservers of the new processor
    const std::vector<AnimationObserver*> observer = getObservers();
    std::vector<AnimationObserver*>::const_iterator it;
    for (it = observer.begin(); it != observer.end(); ++it) {
        (*it)->animatedProcessorAdded(proc);
    }
}

void Animation::processorRemoved(const voreen::Processor* processor) {
    // calles if a processor is removed from the rendernetwork

    // delete all undo and redo pointers to propertytimelines belonging to the removed processor
    std::vector<AnimatedProcessor*>::iterator it;
    for (it = processors_.begin(); it != processors_.end(); ++it) {
        if (processor == (*it)->getCorrespondingProcessor()) {

            const std::vector<AnimationObserver*> observer = getObservers();
            std::vector<AnimationObserver*>::const_iterator itObserver;
            for (itObserver = observer.begin(); itObserver != observer.end(); ++itObserver) {
                (*itObserver)->animatedProcessorRemoved(*it);
            }

            // remove corresponding undos
            std::deque<UndoableAnimation*> newLastChanges;
            std::deque<UndoableAnimation*>::iterator undoIt;
            std::vector<PropertyTimeline*>::const_iterator tlIt;
            for (undoIt = lastChanges_.begin(); undoIt != lastChanges_.end(); ++undoIt) {
                PropertyTimeline* tl = dynamic_cast<PropertyTimeline*>(*undoIt);
                if (tl) {
                    const std::vector<PropertyTimeline*>& timelines = (*it)->getPropertyTimelines();
                    bool toBoDeleted = false;
                    for (tlIt = timelines.begin(); tlIt != timelines.end(); ++tlIt) {
                        if ((tl) == (*tlIt)) {
                            toBoDeleted = true;
                            break;
                        }
                    }
                    if (!toBoDeleted) {
                        newLastChanges.push_back(*undoIt);
                    }
                }
            }
            lastChanges_ = newLastChanges;


            //remove corresponding redos
            std::deque<UndoableAnimation*> newLastUndos;
            for (undoIt = lastUndos_.begin(); undoIt != lastUndos_.end(); ++undoIt) {
                PropertyTimeline* tl = dynamic_cast<PropertyTimeline*>(*undoIt);
                if (tl) {
                    const std::vector<PropertyTimeline*>& timelines = (*it)->getPropertyTimelines();
                    bool toBoDeleted = false;
                    for (tlIt = timelines.begin(); tlIt != timelines.end(); ++tlIt) {
                        if ((tl) == (*tlIt)) {
                            toBoDeleted = true;
                            break;
                        }
                    }
                    if (!toBoDeleted) {
                        newLastUndos.push_back(*undoIt);
                    }
                }
            }
            lastChanges_ = newLastUndos;

            processors_.erase(it);
            break;
        }
    }
}

float Animation::getCurrentTime() const {
    return currentTime_;
}

void Animation::setActualNetworkAsKeyvalues(float time) {
    // snapshotfunction
    // calls the corresponding function of all propertytimelines
    const std::vector<AnimatedProcessor*>& animproc = getAnimatedProcessors();
    std::vector<AnimatedProcessor*>::const_iterator it;
    for (it = animproc.begin(); it != animproc.end(); ++it)
    {
        const std::vector<PropertyTimeline*>& timelines = (*it)->getPropertyTimelines();
        std::vector<PropertyTimeline*>::const_iterator it2;
        for (it2 = timelines.begin(); it2 != timelines.end(); ++it2) {
            (*it2)->setCurrentSettingAsKeyvalue(time, false);
        }
    }
}

void Animation::setInteractionMode(bool interactionmode) {
    // calls the corresponding function of all propertytimelines
    const std::vector<AnimatedProcessor*>& animproc = getAnimatedProcessors();
    std::vector<AnimatedProcessor*>::const_iterator it;
    for (it = animproc.begin(); it != animproc.end(); ++it) {
        const std::vector<PropertyTimeline*>& timelines = (*it)->getPropertyTimelines();
        std::vector<PropertyTimeline*>::const_iterator it2;
        for (it2 = timelines.begin(); it2 != timelines.end(); ++it2) {
            (*it2)->setInteractionMode(interactionmode, this);
        }
    }
}

float Animation::getDuration() const {
    return duration_;
}

void Animation::setDuration(float duration) {
    duration_ = floor(duration*10000.f)/10000.f;

    // delete all undo- / redosteps
    const std::vector<AnimatedProcessor*>& animproc = getAnimatedProcessors();
    std::vector<AnimatedProcessor*>::const_iterator it;
    for (it = animproc.begin(); it != animproc.end(); ++it) {
        const std::vector<PropertyTimeline*>& timelines = (*it)->getPropertyTimelines();
        std::vector<PropertyTimeline*>::const_iterator it2;
        for (it2 = timelines.begin(); it2 != timelines.end(); ++it2) {
            (*it2)->clearAllStates();
            (*it2)->setDuration(duration);
        }
    }
    // delete all keyvalues after duration
}

void Animation::serialize(XmlSerializer& s) const {
    if (!isEmpty()) {
        s.registerFactory(PropertyTimelineFactory::getInstance());
        s.registerFactory(TemplatePropertyTimelineStateFactory::getInstance());
        s.registerFactory(KeyValueFactory::getInstance());
        s.registerFactory(ProcessorFactory::getInstance());

        s.registerFactory(InterpolationFunctionFactory<int>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<float>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<bool>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::ivec2>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::ivec3>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::ivec4>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::vec2>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::vec3>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::vec4>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::mat2>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::mat3>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::mat4>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<tgt::Camera>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<ShaderSource>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<std::string>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<TransFunc*>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<VolumeHandle*>::getInstance());
        s.registerFactory(InterpolationFunctionFactory<VolumeCollection*>::getInstance());

        s.serialize("processors", processors_, "Processor");
        s.serialize("undoSteps", undoSteps_);
        s.serialize("fps", fps_);
        s.serialize("duration", duration_);
        s.serialize("currentTime", currentTime_);
        s.serialize("isRendering", isRendering_);
    }
}

void Animation::deserialize(XmlDeserializer& s) {
    s.registerFactory(PropertyTimelineFactory::getInstance());
    s.registerFactory(TemplatePropertyTimelineStateFactory::getInstance());
    s.registerFactory(KeyValueFactory::getInstance());
    s.registerFactory(ProcessorFactory::getInstance());

    s.registerFactory(InterpolationFunctionFactory<int>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<float>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<bool>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::ivec2>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::ivec3>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::ivec4>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::vec2>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::vec3>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::vec4>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::mat2>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::mat3>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::mat4>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<tgt::Camera>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<ShaderSource>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<std::string>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<TransFunc*>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<VolumeHandle*>::getInstance());
    s.registerFactory(InterpolationFunctionFactory<VolumeCollection*>::getInstance());

    s.deserialize("processors", processors_, "Processor");
    s.deserialize("undoSteps", undoSteps_);
    s.deserialize("fps", fps_);
    s.deserialize("duration", duration_);
    s.deserialize("currentTime", currentTime_);
    s.deserialize("isRendering", isRendering_);
}

} // namespace voreen
