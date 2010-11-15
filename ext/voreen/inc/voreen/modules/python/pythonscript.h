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

#ifndef VRN_PYTHONSCRIPT_H
#define VRN_PYTHONSCRIPT_H

// include this at very first
#include <Python.h>

#include <string>

namespace voreen {

/**
 * Stores a Python script, runs it and checks for errors.
 * Additionally, the script source can be compiled to byte code,
 * resulting in a faster script execution and clearer error messages.
 *
 * @note The caller has to make sure that the Python interpreter is initialized
 *  during all interactions with this class.
 *
 * @note Convenient loading of Python scripts with search path handling
 *  is provided by the PythonModule class.
 */
class PythonScript {

public:
    PythonScript();

    /**
     * Frees the stored byte code. Make sure that the
     * Python interpreter is still initialized
     * when deleting the script.
     */
    ~PythonScript();

    /**
     * Loads a Python script from file and compiles it to byte code by default.
     *
     * @return true, if the script has been loaded successfully.
     *  Note: Does not check the script for validity.
     */
    bool load(const std::string& filename, bool compile = true);

    /**
     * Assigns the Python script source, replacing the current source.
     */
    void setSource(const std::string& source);

    /**
     * Returns the script's source.
     */
    std::string getSource() const;

    /**
     * Assigns a filename to the script. This is optionally,
     * since it is only used in debug/error messages.
     */
    void setFilename(const std::string& filename);

    /**
     * Returns the script's filename. Usually, this is
     * the name of file the script has been loaded from,
     * but it may have been overwritten or even empty.
     */
    std::string getFilename() const;

    /**
     * Compiles the script source to byte code, which speeds up script execution
     * and is generally recommended, since it also produces more clear error messages.
     *
     * If an error occurs, the error message is stored and can be retrieved through getLog().
     *
     * @param logErrors if true, error messages are also passed to the logger.
     *  The internal log buffer is not affected by this parameter.
     *
     * @return true, if script compilation has been successful
     */
    bool compile(bool logErrors = true);

    /**
     * Runs the script source as string or runs the byte code,
     * if the script has been compiled.
     *
     * If an error occurs, the error message is stored and can be retrieved through getLog().
     *
     * @param logErrors if true, error messages are also passed to the logger.
     *  The internal log buffer is not affected by this parameter.
     *
     * @return true, if script execution has been successful
     */
    bool run(bool logErrors = true);

    /**
     * Returns the error that has occured during the last operation (compilation of exection).
     * If the last operation has been successful, an empty string is returned.
     */
    std::string getLog() const;

    /**
     * Returns the source line number where the last error has occurred.
     * Returns -1, if the last operation has been successful or the line number could not be retrieved.
     */
    int getErrorLine() const;

    /**
     * Returns the source column number where the last error has occurred.
     * Returns -1, if the last operation has been successful or the col number could not be retrieved.
     */
    int getErrorCol() const;

private:
    bool checkCompileError(bool logErrors = true);
    bool checkRuntimeError(bool logErrors = true);

    std::string     source_;
    std::string     filename_;
    PyObject*       byteCode_;
    bool            compiled_;
    std::string     log_;
    int             errorLine_;
    int             errorCol_;

    bool printErrorsToStdOut_;  //< only for debugging

    static const std::string loggerCat_;
};

}

#endif // VRN_PYTHONSCRIPT_H
