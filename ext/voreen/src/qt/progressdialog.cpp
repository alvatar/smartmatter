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

#include "voreen/qt/progressdialog.h"

namespace {
    const int stepGranularity = 200; // number of steps on the QProgressDialog
    const int minimalUpdateWait = 50; // time to wait between progress bar updates
}

namespace voreen {

ProgressDialog::ProgressDialog(QWidget* parent, const std::string& message)
    : progressDialog_(new QProgressDialog(parent))
{
    progressDialog_->setCancelButton(0);
    progressDialog_->setWindowModality(Qt::WindowModal);
    progressDialog_->setLabelText(QString::fromStdString(message));
    progressDialog_->setRange(0, stepGranularity);
    progressDialog_->setMinimumDuration(0);
    progressDialog_->setMinimumWidth(300);

    time_ = new QTime();
    time_->start();
}

ProgressDialog::~ProgressDialog() {
    delete time_;
}

void ProgressDialog::update() {
    int intProgress = static_cast<int>(progress_ * stepGranularity);
    if ((time_->elapsed() > minimalUpdateWait || intProgress == progressDialog_->maximum())
        && progressDialog_->value() != intProgress)
    {
        progressDialog_->setValue(intProgress);
        time_->restart();
    }
}

void ProgressDialog::show() {
    setProgress(0.f);
    progressDialog_->show();
    progressDialog_->raise();
    progressDialog_->activateWindow();
}

void ProgressDialog::hide() {
    setProgress(1.f);
    progressDialog_->hide();
}

void ProgressDialog::setMessage(const std::string& message) {
    ProgressBar::setMessage(message);
    progressDialog_->setLabelText(QString::fromStdString(message));
}

void ProgressDialog::setTitle(const std::string& title) {
    ProgressBar::setTitle(title);
    progressDialog_->setWindowTitle(QString::fromStdString(title));
}

void ProgressDialog::forceUpdate() {
    progressDialog_->repaint();
}

} // namespace
