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

#ifndef VRN_BOOLPROPERTYWIDGET_H
#define VRN_BOOLPROPERTYWIDGET_H

#include "voreen/qt/widgets/property/qpropertywidget.h"

class QCheckBox;

namespace voreen {

class BoolProperty;

class BoolPropertyWidget : public QPropertyWidget {
Q_OBJECT
public:
    BoolPropertyWidget(BoolProperty* prop, QWidget* parent = 0);

    void updateFromProperty();

public slots:
    void setProperty(bool value);

protected:
    BoolProperty* property_;
    QCheckBox* chkBox_;
};

} // namespace

#endif // VRN_BOOLPROPERTYWIDGET_H
