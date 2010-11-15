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

#include "voreen/qt/widgets/property/camerapropertywidget.h"

#include "voreen/core/properties/cameraproperty.h"

#include "voreen/qt/widgets/voreentoolwindow.h"
#include "voreen/qt/widgets/property/camerawidget.h"

#include "tgt/camera.h"

#include <QPushButton>

namespace voreen {

using tgt::Camera;

CameraPropertyWidget::CameraPropertyWidget(CameraProperty* prop, QWidget* parent)
    : QPropertyWidgetWithEditorWindow(prop, parent, true, false)
    , editBt_(new QPushButton(tr("edit")))
    , property_(prop)
    , cameraWidget_(0)
{
    createEditorWindow(Qt::LeftDockWidgetArea);

    addWidget(editBt_);
    connect(editBt_, SIGNAL(clicked()), this, SLOT(toggleWidgetVisibility()));

    addVisibilityControls();
    QFontInfo fontInfo(font());
    editBt_->setFont(QFont(fontInfo.family(), QPropertyWidget::fontSize_));
}

void CameraPropertyWidget::updateFromProperty() {
    if (cameraWidget_)
        cameraWidget_->updateFromCamera();
}

void CameraPropertyWidget::setProperty(Camera* value) {
    if (!disconnected_)
        property_->set(*value);
}

void CameraPropertyWidget::toggleWidgetVisibility() {

    tgtAssert(editorWindow_, "No editor window");

    editorWindow_->setVisible(!editorWindow_->isVisible());
}

void CameraPropertyWidget::customizeEditorWindow() {
    editorWindow_->adjustSize();
    editorWindow_->setFixedSize(editorWindow_->size());
}

QWidget* CameraPropertyWidget::createEditorWindowWidget() {
    cameraWidget_ = new CameraWidget(property_, 0.05f, 15.f, parentWidget());
    cameraWidget_->createWidgets();
    cameraWidget_->createConnections();

    return cameraWidget_;
}

Property* CameraPropertyWidget::getProperty() {
    return property_;
}

} // namespace
