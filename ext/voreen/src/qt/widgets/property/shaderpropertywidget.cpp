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

#include "voreen/qt/widgets/property/shaderpropertywidget.h"

#include "voreen/qt/widgets/voreentoolwindow.h"
#include "voreen/qt/widgets/shaderplugin.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/shaderproperty.h"

#include <QPushButton>

namespace voreen {

ShaderPropertyWidget::ShaderPropertyWidget(ShaderProperty* prop, QWidget* parent)
    : QPropertyWidgetWithEditorWindow(prop, parent)
    , plugin_(0)
    , property_(prop)
    , editBt_(new QPushButton(tr("edit")))
{

    if (editorVisibleOnStartup())
        createEditorWindow(Qt::LeftDockWidgetArea, QString::fromStdString(" (original source: " + property_->get().fragmentFilename_ + ")"), 700, 700);

    addWidget(editBt_);

    connect(editBt_, SIGNAL(clicked()), this, SLOT(setProperty()));
    connect(editBt_, SIGNAL(clicked()), this, SIGNAL(widgetChanged()));

    addVisibilityControls();

    QFontInfo fontInfo(font());
    editBt_->setFont(QFont(fontInfo.family(), QPropertyWidget::fontSize_));
}

void ShaderPropertyWidget::updateFromProperty() {
    if (plugin_) {
        plugin_->updateFromProperty();
        plugin_->update();
    }
}

void ShaderPropertyWidget::setProperty() {
    if (!disconnected_) {
        // lazy instantiation of shader editor window
        if (!editorWindow_) {
            createEditorWindow(Qt::LeftDockWidgetArea, QString::fromStdString(" (original source: " + property_->get().fragmentFilename_ + ")"), 700, 700);

            tgtAssert(editorWindow_, "Shader editor not instantiated");
        }

        if (editorWindow_->isVisible()) {
            //close widget
            editorWindow_->close();
        }
        else {
            //open Widget
            editorWindow_->showNormal();
        }
    }
}

void ShaderPropertyWidget::disconnect() {
    disconnected_ = true;
    if (plugin_)
        plugin_->disconnect();
}

QWidget* ShaderPropertyWidget::createEditorWindowWidget() {
    plugin_ = new ShaderPlugin(property_, parentWidget());
    plugin_->createWidgets();
    plugin_->createConnections();
    connect(plugin_, SIGNAL(modified()), this, SIGNAL(modified()));

    return plugin_;
}

void ShaderPropertyWidget::customizeEditorWindow() {
}

Property* ShaderPropertyWidget::getProperty() {
    return property_;
}

} // namespace voreen
