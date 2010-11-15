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

#ifndef VRN_SHADERPLUGIN_H
#define VRN_SHADERPLUGIN_H

#include "voreen/core/voreenapplication.h"
#include "voreen/core/properties/shaderproperty.h"
#include "voreen/qt/widgets/glslhighlighter.h"
#include "voreen/qt/widgets/codeedit.h"

#include <QToolButton>
#include <QUrl>
#include <QWidget>

class QTextEdit;

namespace voreen {

class ShaderPlugin : public QWidget {
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * @param prop the shader property that belongs to this plugin
     * @param parent the parent widget
     */
    ShaderPlugin(ShaderProperty* prop, QWidget* parent = 0);

    /**
     * Destructor
     */
    ~ShaderPlugin();

    /**
     * Creates all necessary widgets.
     */
    void createWidgets();

    /**
     * Creates all necessary connections.
     */
    void createConnections();
    void updateFromProperty();

signals:
    void modified();

public slots:
    void undoShader();
    void fullUndoShader();
    void openShader();
    void saveShader();
    void setProperty();

private:
    const QString getOpenFileName(QString filter);
    const QString getSaveFileName(QStringList filters);

    ShaderProperty* property_;               ///< shader property that belongs to this plugin
    QToolButton* undoBt_;
    QToolButton* fullUndoBt_;
    QToolButton* openBt_;
    QToolButton* saveBt_;
    QToolButton* updateBt_;
    CodeEdit* codeEdit_;
    QTextEdit* compilerLogWidget_;
    GLSLHighlighter* highlighter_;
};

} // namespace voreen

#endif // VRN_SHADERPLUGIN_H
