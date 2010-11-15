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

#include "voreen/qt/widgets/property/floatvec4propertywidget.h"

#include <QMenu>
#include <QMouseEvent>

namespace voreen {

FloatVec4PropertyWidget::FloatVec4PropertyWidget(FloatVec4Property* prop, QWidget* parent)
    : VecPropertyWidget<DoubleSliderSpinBoxWidget, FloatVec4Property, float>(prop, 4, parent)
{
    connect((const QObject*)widgets_[0], SIGNAL(valueChanged(double)), this, SLOT(setProperty(double)));
    connect((const QObject*)widgets_[1], SIGNAL(valueChanged(double)), this, SLOT(setProperty(double)));
    connect((const QObject*)widgets_[2], SIGNAL(valueChanged(double)), this, SLOT(setProperty(double)));
    connect((const QObject*)widgets_[3], SIGNAL(valueChanged(double)), this, SLOT(setProperty(double)));
    updateFromProperty();
}

void FloatVec4PropertyWidget::setProperty(double value) {
    if (disconnected_)
        return;

    FloatVec4Property::ElemType newValue = setPropertyComponent(sender(), static_cast<float>(value));
    emit valueChanged(newValue);
    emit modified();
}

} // namespace voreen
