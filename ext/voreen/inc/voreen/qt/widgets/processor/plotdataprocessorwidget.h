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

#ifndef VRN_PLOTDATAPROCESSORWIDGET_H
#define VRN_PLOTDATAPROCESSORWIDGET_H

#include "voreen/qt/widgets/property/qpropertywidgetwitheditorwindow.h"
#include "voreen/qt/widgets/processor/qprocessorwidget.h"

class QPushButton;


namespace voreen {

// some forward declarations
class PlotData;
class PlotDataProperty;
class VoreenToolWindow;
class PlotDataWidget;

class PlotDataProcessorWidget : public QProcessorWidget {
    Q_OBJECT
public:
    PlotDataProcessorWidget(QWidget* parent, Processor* proc, PlotDataProperty* data);
    virtual ~PlotDataProcessorWidget();

    void initialize();
    virtual void updateFromProcessor();

private:
    PlotDataProperty* data_;
    PlotDataWidget* plotDataWidget_;
};

} // namespace

#endif // VRN_PLOTDATAPROCESSORWIDGET_H
