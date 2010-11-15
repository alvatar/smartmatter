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

#include "voreen/qt/widgets/inputmappingdialog.h"
#include "voreen/qt/widgets/keydetectorwidget.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/eventproperty.h"
#include "voreen/core/interaction/interactionhandler.h"
#include "voreen/core/network/processornetwork.h"
#include "voreen/qt/widgets/eventpropertywidget.h"
#include "voreen/qt/widgets/property/qpropertywidgetfactory.h"
#include "voreen/qt/widgets/property/qpropertywidget.h"
#include "voreen/qt/widgets/customlabel.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QApplication>
#include <QShowEvent>

namespace voreen {

InputMappingDialog::InputMappingDialog(QWidget* parent, ProcessorNetwork* network)
    : QWidget(parent),
      processorNetwork_(network),
      scrollStretchItem_(0),
      widgetsValid_(false)
{

    if (processorNetwork_)
        processorNetwork_->addObserver(this);

    scrollArea_ = new QScrollArea();
    QWidget* widget = new QWidget();
    scrollArea_->setWidget(widget);
    scrollArea_->setWidgetResizable(true);
    scrollLayout_ = new QVBoxLayout(widget);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(scrollArea_);

    setLayout(mainLayout);

    setMaximumHeight(1024);
    setMinimumSize(400, 300);
    createWidgets();
}

void InputMappingDialog::setProcessorNetwork(ProcessorNetwork* network) {

    // stop observation of previously assigned network
    stopObservation(processorNetwork_);

    // update network and register as observer
    processorNetwork_ = network;
    if (processorNetwork_)
        processorNetwork_->addObserver(this);

    setEnabled(true);

    // only rebuild widgets immediately, if network is empty or the widget is visible
    if (!processorNetwork_ || processorNetwork_->empty() || isVisible())
        rebuildWidgets();
    else
        widgetsValid_ = false;
}

void InputMappingDialog::rebuildWidgets() {

    // delete current scroll widget for getting rid of previous event prop widgets
    delete scrollArea_->takeWidget();
    QWidget* widget = new QWidget();
    scrollArea_->setWidget(widget);
    scrollArea_->setWidgetResizable(true);
    scrollLayout_ = new QVBoxLayout(widget);

    createWidgets();
}

void InputMappingDialog::createWidgets() {

    if (processorNetwork_) {
        const std::vector<Processor*> processors = processorNetwork_->getProcessors();
        for (size_t i=0; i<processors.size(); i++) {
            addProcessorToLayout(processors[i]);
        }
    }

    scrollStretchItem_ = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    scrollLayout_->addSpacerItem(scrollStretchItem_);

    widgetsValid_ = true;
}

void InputMappingDialog::addProcessorToLayout(const Processor* processor) {

    tgtAssert(scrollLayout_, "No scroll layout");

    // processor's event properties
    std::vector<EventPropertyBase*> eventProps = processor->getEventProperties();

    // processor's interaction handlers (each with its own event property(ies) + maybe normal properties)
    const std::vector<InteractionHandler*> procHandlers = processor->getInteractionHandlers();
    std::vector<EventPropertyBase*> handlerEventProps;
    std::vector<Property*> handlerProps;
    for (size_t i=0; i<procHandlers.size(); i++) {
        for (size_t j=0; j<procHandlers[i]->getEventProperties().size(); j++) {
            handlerEventProps.push_back(procHandlers[i]->getEventProperties().at(j));
        }
        for (size_t j=0; j<procHandlers[i]->getProperties().size(); j++) {
            handlerProps.push_back(procHandlers[i]->getProperties().at(j));
        }
    }
    eventProps.insert(eventProps.begin(), handlerEventProps.begin(), handlerEventProps.end());

    if (eventProps.empty() && handlerProps.empty())
        return;

    // create event property widgets for processor
    QString title = QString::fromStdString(processor->getName());

    QGroupBox* processorBox = new QGroupBox(title);
    QVBoxLayout* boxLayout = new QVBoxLayout();
    boxLayout->setSpacing(1);
    processorBox->setLayout(boxLayout);

    // add widgets for collected interaction handler's normal properties
    QPropertyWidgetFactory propFactory;
    for (size_t j=0; j<handlerProps.size(); j++) {
        PropertyWidget* propWidget = handlerProps[j]->createAndAddWidget(&propFactory);
        if (QPropertyWidget* qPropWidget = dynamic_cast<QPropertyWidget*>(propWidget)) {
            QHBoxLayout* layoutTemp = new QHBoxLayout();
            layoutTemp->setContentsMargins(2,2,2,2);
            CustomLabel* nameLabel = qPropWidget->getNameLabel();
            nameLabel->setMinimumWidth(0);
            layoutTemp->addWidget(nameLabel);
            layoutTemp->addSpacing(5);
            layoutTemp->addWidget(qPropWidget);
            layoutTemp->addStretch();
            boxLayout->addLayout(layoutTemp);
        }
        else if (propWidget) {
            LERRORC("voreenqt.InputMappingDialog", "Created PropertyWidget is not of type QPropertyWidget");
        }
    }

    // add widgets for collected event properties
    for (size_t j=0; j<eventProps.size(); j++) {
        EventPropertyWidget* wdt = new EventPropertyWidget(eventProps[j]);
        eventProps[j]->addWidget(wdt);
        boxLayout->addWidget(wdt);
    }

    // add processor box
    processorBoxMap_.insert(const_cast<Processor*>(processor), processorBox);
    scrollLayout_->addWidget(processorBox);

}

void InputMappingDialog::processorAdded(const Processor* processor) {

    if (!scrollLayout_)
        return;

    if (scrollStretchItem_)
        scrollLayout_->removeItem(scrollStretchItem_);
    addProcessorToLayout(processor);
    if (scrollStretchItem_)
        scrollLayout_->addSpacerItem(scrollStretchItem_);
}

void InputMappingDialog::processorRemoved(const Processor* processor) {
    Processor* proc = const_cast<Processor*>(processor);
    if (processorBoxMap_.contains(proc)) {
        delete processorBoxMap_[proc];
        processorBoxMap_.remove(proc);
    }
}

void InputMappingDialog::networkChanged() {
    // nothing
}

void InputMappingDialog::showEvent(QShowEvent* /*event*/) {
    if (!widgetsValid_) {
        qApp->setOverrideCursor(Qt::WaitCursor);
        qApp->processEvents();
        rebuildWidgets();
        qApp->restoreOverrideCursor();
    }
}

} // namespace
