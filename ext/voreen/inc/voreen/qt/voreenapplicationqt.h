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

#ifndef VRN_APPLICATIONQT_H
#define VRN_APPLICATIONQT_H

#include "voreen/core/voreenapplication.h"

#include "voreen/qt/progressdialog.h"

#include <QString>

class QMainWindow;

namespace voreen {

class PyVoreenQt;

class VoreenApplicationQt : public VoreenApplication {
public:
    VoreenApplicationQt(const std::string& name, const std::string& displayName,
                        int argc, char** argv, ApplicationType appType = APP_DEFAULT);
    ~VoreenApplicationQt();

    virtual void init();

    virtual void initGL() throw (VoreenException);

    /**
     * Allows access to the global instance of this class.
     */
    static VoreenApplicationQt* qtApp();

    void setMainWindow(QMainWindow* mainWindow);

    QMainWindow* getMainWindow() const;

    /**
     * Creates a tgt::QtTimer.
     *
     * @param handler The event handler that will be used
     *  for broadcasting the timer events. Must not be null.
     */
    virtual tgt::Timer* createTimer(tgt::EventHandler* handler) const;

    /**
     * Creates a ProgressDialog.
     */
    virtual ProgressDialog* createProgressDialog() const;

    /**
     * Constructs an absolute path consisting of the VoreenQt shader source directory
     * and the given filename.
     */
    virtual std::string getShaderPathQt(const std::string& filename = "") const;


private:
    static VoreenApplicationQt* qtApp_;
    QMainWindow* mainWindow_;
    std::string shaderPathQt_;

#ifdef VRN_MODULE_PYTHON
    PyVoreenQt* pythonQt_;
#endif
};

} // namespace

#endif //VRN_APPLICATIONQT_H
