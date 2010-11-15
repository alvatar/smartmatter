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

#ifndef VRN_IMAGEANALYZERWIDGET_H
#define VRN_IMAGEANALYZERWIDGET_H

#include "voreen/modules/base/processors/utility/imageanalyzer.h"
#include "voreen/qt/widgets/processor/qprocessorwidget.h"

#include "tgt/qt/qtcanvas.h"
#include <QLabel>
#include <QCheckBox>

namespace voreen {

class ImageAnalyzerWidget : public QProcessorWidget {
    Q_OBJECT
public:
    ImageAnalyzerWidget(QWidget* parent, ImageAnalyzer* imageAnalyzer);
    virtual ~ImageAnalyzerWidget();

    void initialize();

    virtual void updateFromProcessor();

public slots:
    void checkBoxChange(int state);

private:
    std::vector<QLabel*> labelVector_;
    std::vector< std::pair<QCheckBox*,int> > checkboxVector_;

    static const std::string loggerCat_;

};

} // namespace voreen

#endif

