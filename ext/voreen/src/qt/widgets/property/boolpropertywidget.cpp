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

#include "voreen/qt/widgets/property/boolpropertywidget.h"

#include "voreen/core/properties/boolproperty.h"

#include <QCheckBox>

namespace voreen {

BoolPropertyWidget::BoolPropertyWidget(BoolProperty* prop, QWidget* parent)
    : QPropertyWidget(prop, parent)
    , property_(prop)
    , chkBox_(new QCheckBox)
{
    chkBox_->setChecked(prop->get());
    addWidget(chkBox_);

    connect(chkBox_, SIGNAL(toggled(bool)), this, SLOT(setProperty(bool)));
    connect(chkBox_, SIGNAL(toggled(bool)), this, SIGNAL(widgetChanged()));

    addVisibilityControls();
}

void BoolPropertyWidget::updateFromProperty() {
    chkBox_->blockSignals(true);
    chkBox_->setChecked(property_->get());
    chkBox_->blockSignals(false);
}

void BoolPropertyWidget::setProperty(bool value) {
    if (!disconnected_) {
        property_->set(value);
        emit modified();
    }
}

} // namespace
