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

#ifndef VRN_VECPROPERTYWIDGET_H
#define VRN_VECPROPERTYWIDGET_H

#include "voreen/qt/widgets/property/qpropertywidget.h"

#include <QBoxLayout>

class QMenu;

namespace voreen {

template<class WIDGETTYPE, class VECTORPROP, typename ELEMTYPE>
class VecPropertyWidget : public QPropertyWidget {
public:
    VecPropertyWidget(VECTORPROP* const prop, const size_t numComponents, QWidget* parent = 0);
    virtual ~VecPropertyWidget() = 0;   // ensures that this class remains abstract.
    virtual void updateFromProperty();

protected:
    typename VECTORPROP::ElemType setPropertyComponent(QObject* sender, ELEMTYPE value);
    virtual void mousePressEvent(QMouseEvent*);
    QMenu* precisionMenu_;
    QAction* highAction_;
    QAction* instantValueChangeAction_;

protected:
    const size_t numComponents_;
    QBoxLayout* myLayout_;
    WIDGETTYPE** widgets_;
    VECTORPROP* vectorProp_;
};

}   // namespace

#endif
