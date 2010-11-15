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

#ifndef VRN_PLOTENTITIESPROPERTYWIDGET_H
#define VRN_PLOTENTITIESPROPERTYWIDGET_H

#include "voreen/qt/widgets/property/qpropertywidget.h"

class QPushButton;
class QListWidget;
class QComboBox;
class QListWidgetItem;

namespace voreen {

class PlotEntitiesProperty;

class PlotEntitiesPropertyWidget : public QPropertyWidget {
    Q_OBJECT
public:
    PlotEntitiesPropertyWidget(PlotEntitiesProperty* prop, QWidget* parent = 0);

public slots:

private slots:
    void buttonAddClicked();
    void buttonAddAllClicked();
    void buttonDeleteClicked();
    void buttonApplyColormapClicked();
    void updateXAxis(int index);
    void updateYAxis(int index);
    void updateColormap(int index);
    void listViewDoubleClicked(QListWidgetItem* item);
    void updateFromProperty();

protected:

    tgt::Color toTgtColor(QColor color);
    QColor toQColor(tgt::Color color);

    // if plotentityproperty::setdata is called, we have to recreate the widgets
    void reCreateWidgets();

    QPushButton* btAdd_;
    QPushButton* btAddAll_;
    QPushButton* btDelete_;
    QPushButton* btApplyColormap_;
    QListWidget* lwEntities_;

    QComboBox* cbXAxis_;
    QComboBox* cbYAxis_;
    QComboBox* cbColormap_;

    PlotEntitiesProperty* property_;

};

} // namespace

#endif // VRN_PLOTENTITIESPROPERTYWIDGET_H
