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

// This class is based on the GLSL syntax highlighter written by Markus Kramer for
// Shader Maker - a cross-platform GLSL editor

#ifndef VRN_GLSLHIGHLIGHTER_H
#define VRN_GLSLHIGHLIGHTER_H

#include "voreen/qt/widgets/syntaxhighlighter.h"

namespace voreen {

class GLSLHighlighter : public SyntaxHighlighter {
public:
    GLSLHighlighter(QTextDocument* doc);

protected:
    void setupKeywords();
    void setupReservedKeywords();
    void setupComments();
    void setupNumberRules();
    void setupRegisters();
    void setupBuiltInFunctions();
    void setupPreprocessor();
};

} // namespace

#endif // VRN_GLSLHIGHLIGHTER_H
