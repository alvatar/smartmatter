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

#ifndef VRN_INPUTMAPPINGDIALOG_H
#define VRN_INPUTMAPPINGDIALOG_H

#include "voreen/core/network/processornetworkobserver.h"

#include <QWidget>
#include <QScrollArea>
#include <QMap>

class QVBoxLayout;
class QLabel;
class QGroupBox;
class QSpacerItem;
class QShowEvent;

namespace voreen {

class EventPropertyBase;
class EventPropertyWidget;
class ProcessorNetwork;
class Processor;

class InputMappingDialog : public QWidget, public ProcessorNetworkObserver {
Q_OBJECT
public:
    InputMappingDialog(QWidget* parent = 0, ProcessorNetwork* network = 0);

public slots:
    /**
     * This method passes the ProcessorNetwork whose processors' event properties
     * are to be displayed by the InputMappingDialog.
     */
    void setProcessorNetwork(ProcessorNetwork* network);
    void rebuildWidgets();

    // Implementation of the ProcessorNetworkObserver interface
    virtual void networkChanged();
    virtual void processorAdded(const Processor* processor);
    virtual void processorRemoved(const Processor* processor);

protected:
    /// We need to create the widgets on show (lazy instantiation).
    void showEvent(QShowEvent* event);

    void createWidgets();

    void addProcessorToLayout(const Processor* processor);

    ProcessorNetwork* processorNetwork_;

    QMap<Processor*, QGroupBox*> processorBoxMap_;
    QScrollArea* scrollArea_;
    QVBoxLayout* scrollLayout_;
    QSpacerItem* scrollStretchItem_;

    bool widgetsValid_;     ///< Determines, whether widgets have to be rebuild on next show
};

} // namespace

#endif // VRN_INPUTMAPPINGDIALOG_H
