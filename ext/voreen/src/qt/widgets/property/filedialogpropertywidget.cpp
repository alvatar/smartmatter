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

#include "voreen/qt/widgets/property/filedialogpropertywidget.h"

#include "voreen/core/properties/filedialogproperty.h"

#include <QDir>
#include <QFileDialog>
#include <QPushButton>

namespace voreen {

FileDialogPropertyWidget::FileDialogPropertyWidget(FileDialogProperty* prop, QWidget* parent)
    : QPropertyWidget(prop, parent)
    , property_(prop)
    , openFileDialogBtn_(new QPushButton)
{
    updateButtonText(prop->get());

    connect(openFileDialogBtn_, SIGNAL(clicked(void)), this, SLOT(setProperty(void)));

    addWidget(openFileDialogBtn_);

    addVisibilityControls();

    QFontInfo fontInfo(font());
    openFileDialogBtn_->setFont(QFont(fontInfo.family(), QPropertyWidget::fontSize_));
}

void FileDialogPropertyWidget::setProperty() {
    if (!disconnected_) {
        QString dialogCaption = QString::fromStdString(property_->getDialogCaption());
        QString directory;
        // use directory of current property value if any, default directory otherwise
        if (!property_->get().empty())
            directory = QString::fromStdString(property_->get());
        else
            directory = QString::fromStdString(property_->getDirectory());

        const QString fileFilter = QString::fromStdString(property_->getFileFilter()) + ";;" + tr("All files (*)");

        QString filename;
        if (property_->getFileMode() == FileDialogProperty::OPEN_FILE) {
            filename = QFileDialog::getOpenFileName(QWidget::parentWidget(), dialogCaption, directory, fileFilter);
        }
        else if (property_->getFileMode() == FileDialogProperty::SAVE_FILE) {
            filename = QFileDialog::getSaveFileName(QWidget::parentWidget(), dialogCaption, directory, fileFilter);
        }
        else if (property_->getFileMode() == FileDialogProperty::DIRECTORY) {
            filename = QFileDialog::getExistingDirectory(QWidget::parentWidget(), dialogCaption, QString::fromStdString(property_->get()));
        }

        if (!filename.isEmpty()) {
            property_->set(filename.toStdString());
            emit modified();
        }
    }
}

void FileDialogPropertyWidget::updateButtonText(const std::string& filename) {

    if (!filename.empty()) {
        if ((property_->getFileMode() == FileDialogProperty::OPEN_FILE) || (property_->getFileMode() == FileDialogProperty::SAVE_FILE)) {
            size_t index = filename.find_last_of('/');
            if (index == filename.npos)
                index = filename.find_last_of('\\');
            std::string endFilename = filename;
            if (index != filename.npos)
                endFilename = filename.substr(index + 1, filename.length());
            openFileDialogBtn_->setText(QString::fromStdString(endFilename));
        }
        else if (property_->getFileMode() == FileDialogProperty::DIRECTORY) {
            std::string directory = filename;
            if (directory.length() >= 20)
                directory = "..." + directory.substr(directory.length()-20);
            openFileDialogBtn_->setText(QString::fromStdString(directory));
        }
    }
    else {
        if (property_->getFileMode() == FileDialogProperty::OPEN_FILE)
             openFileDialogBtn_->setText(tr("Select File"));
        else if (property_->getFileMode() == FileDialogProperty::SAVE_FILE)
             openFileDialogBtn_->setText(tr("Select File"));
        else if (property_->getFileMode() == FileDialogProperty::DIRECTORY)
            openFileDialogBtn_->setText(tr("Select Directory"));
    }

    openFileDialogBtn_->update();
}

void FileDialogPropertyWidget::updateFromProperty() {
    updateButtonText(property_->get());
}

} // namespace
