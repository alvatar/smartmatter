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

#include "voreen/qt/widgets/transfunc/transfuncplugin.h"

#include "voreen/qt/widgets/transfunc/transfunceditorintensity.h"
#include "voreen/qt/widgets/transfunc/transfunceditorintensitygradient.h"
#include "voreen/qt/widgets/transfunc/transfunceditorintensitypet.h"
#include "voreen/qt/widgets/transfunc/transfunceditorintensityramp.h"

#include "voreen/core/datastructures/transfunc/transfuncintensity.h"
#include "voreen/core/datastructures/transfunc/transfuncintensitygradient.h"

#include <QComboBox>
#include <QMessageBox>
#include <QString>
#include <QTabWidget>
#include <QVBoxLayout>

namespace voreen {

TransFuncPlugin::TransFuncPlugin(TransFuncProperty* prop, QWidget* parent, Qt::Orientation orientation)
    : QWidget(parent)
    , property_(prop)
    , tabWidget_(0)
    , oldIndex_(-1)
{
    createEditors(parent, orientation);
}

void TransFuncPlugin::createEditors(QWidget* parent, Qt::Orientation orientation) {
    TransFuncEditor* tfEditor;
    if (property_->isEditorEnabled(TransFuncProperty::INTENSITY)) {
        tfEditor = new TransFuncEditorIntensity(property_, parent, orientation);
        editors_.push_back(tfEditor);
    }
    if (property_->isEditorEnabled(TransFuncProperty::INTENSITY_RAMP)) {
        tfEditor = new TransFuncEditorIntensityRamp(property_, parent, orientation);
        editors_.push_back(tfEditor);
    }
/*    if (property_->isEditorEnabled(TransFuncProperty::INTENSITY_PET)) {
        tfEditor = new TransFuncEditorIntensityPet(property_, parent);
        editors_.push_back(tfEditor);
    } */
    if (property_->isEditorEnabled(TransFuncProperty::INTENSITY_GRADIENT)) {
        tfEditor = new TransFuncEditorIntensityGradient(property_, parent, orientation);
        editors_.push_back(tfEditor);
    }
}

TransFuncPlugin::~TransFuncPlugin() {
    editors_.clear();
}

void TransFuncPlugin::createWidgets() {
    tabWidget_ = new QTabWidget();
    for (size_t i = 0; i < editors_.size(); ++i) {
        editors_[i]->createWidgets();
        tabWidget_->addTab(editors_[i], editors_[i]->getTitle());
    }

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(tabWidget_);
    setLayout(vbox);

    oldIndex_ = 0;

    // select editor according to currently assigned transfer function
    // (needs to be revised in the near future)
    if (dynamic_cast<TransFuncIntensity*>(property_->get())) {
        for (size_t i=0; i < editors_.size(); ++i) {
            if (dynamic_cast<TransFuncEditorIntensity*>(editors_[i])) {
                oldIndex_ = i;
                break;
            }
        }
    }
    else if (dynamic_cast<TransFuncIntensityGradient*>(property_->get())) {
        for (size_t i=0; i < editors_.size(); ++i) {
            if (dynamic_cast<TransFuncEditorIntensityGradient*>(editors_[i])) {
                oldIndex_ = i;
                break;
            }
        }
    }

    tabWidget_->setCurrentIndex(oldIndex_);
}

void TransFuncPlugin::createConnections() {
    for (size_t i = 0; i < editors_.size(); ++i) {
        editors_[i]->createConnections();
        connect(editors_[i], SIGNAL(transferFunctionChanged()), this, SIGNAL(transferFunctionChanged()));
    }
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(editorChanged(int)));
}

void TransFuncPlugin::updateFromProperty() {
    if (editors_.size() > 0 && tabWidget_) {
        int index = tabWidget_->currentIndex();
        editors_[index]->updateFromProperty();
    }
}

void TransFuncPlugin::editorChanged(int index) {
    if (index != oldIndex_ && index > -1) {
        //TODO: check for compatibility and not reset tf by default (cdoer)

        // set tabwidget back to previous editor
        tabWidget_->blockSignals(true);
        tabWidget_->setCurrentIndex(oldIndex_);
        tabWidget_->blockSignals(false);
        QString text(tr("The Editor you selected is not compatible with the previous one.\n"));
        text.append(tr("The transfer function and the thresholds will be reset to default.\n\n"));
        text.append(tr("Do you want to proceed?"));
        int answer = QMessageBox::question(this, tr("Proceed?"), text, QMessageBox::Yes, QMessageBox::No);
        // set editor to new one when the user decides to proceed
        if (answer == QMessageBox::Yes) {
            oldIndex_ = index;

            tabWidget_->blockSignals(true);
            tabWidget_->setCurrentIndex(index);
            tabWidget_->blockSignals(false);

            // resets transfer function and thresholds to default
            editors_[index]->resetEditor();
            // updates editor to current rendered volume
            editors_[index]->update();
        }
    }
}

void TransFuncPlugin::showEvent(QShowEvent* /*event*/) {
    int index = tabWidget_->currentIndex();
    editors_[index]->update();
}

void TransFuncPlugin::disconnect() {
    editors_.clear();
}

} // namespace voreen
