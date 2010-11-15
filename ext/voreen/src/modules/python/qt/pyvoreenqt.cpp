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

// Ignore "dereferencing type-punned pointer will break strict-aliasing rules" warnings on gcc
// caused by Py_RETURN_TRUE and such. The problem lies in Python so we don't want the warning
// here.
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

// Do this at very first
#include "voreen/modules/python/pythonmodule.h"

#include "voreen/modules/python/qt/pyvoreenqt.h"
#include "voreen/modules/python/pyvoreen.h"

#include "voreen/core/voreenapplication.h"
#include "voreen/core/network/processornetwork.h"
#include "voreen/core/network/networkevaluator.h"
#include "voreen/core/processors/canvasrenderer.h"
#include "voreen/qt/widgets/processor/canvasrendererwidget.h"

#include <cstdlib>
#include <QApplication>
#include <QMessageBox>

static PyObject* voreenqt_messageBox(PyObject* /*self*/, PyObject* args) {
    const char* str;

    if (!PyArg_ParseTuple(args, "s", &str))
        return NULL;

    qApp->setOverrideCursor(Qt::ArrowCursor);
    QMessageBox::information(0, "Script message", str);
    qApp->restoreOverrideCursor();

    Py_RETURN_NONE;
}

static PyObject* voreenqt_questionBox(PyObject* /*self*/, PyObject* args) {
    const char* str;

    if ( !PyArg_ParseTuple(args, "s", &str) )
        return NULL;

    qApp->setOverrideCursor(Qt::ArrowCursor);
    bool result = (QMessageBox::question(0, "Script question", str,
        QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes);
    qApp->restoreOverrideCursor();

    if (result)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static PyObject* voreenqt_quit(PyObject* /*self*/, PyObject* args) {
    if (!PyArg_ParseTuple(args, ""))
        return NULL;

    qApp->closeAllWindows();
    exit(0);
    Py_RETURN_NONE;
}

static PyObject* voreenqt_processEvents(PyObject* /*self*/, PyObject* args) {
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    qApp->processEvents();
    Py_RETURN_NONE;
}

static PyObject* printModuleInfo(const std::string& moduleName, bool omitFunctionName = false,
                                 int spacing=0, bool collapse=false, bool blanklines=false) {

    // import apihelper.py
    PyObject* apihelper = PyImport_ImportModule("apihelper");
    if (!apihelper) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() apihelper module not found")).c_str());
        return 0;
    }

    // get reference to info function
    PyObject* func = PyDict_GetItemString(PyModule_GetDict(apihelper), "info");
    if (!func) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() apihelper.info() not found")).c_str());
        Py_XDECREF(apihelper);
        return 0;
    }

    // get reference to module
    PyObject* mod = PyImport_AddModule(const_cast<char*>(moduleName.c_str())); // const cast required for
                                                                               // Python 2.4

    if (!mod) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() failed to access module ") + moduleName).c_str());
        Py_XDECREF(apihelper);
    }

    // build parameter tuple
    std::string docStr = "Module " + moduleName;
    PyObject* arg = Py_BuildValue("(O,s,i,i,i,i)", mod, docStr.c_str(),
        int(omitFunctionName), spacing, (int)collapse, (int)blanklines);
    if (!arg) {
        PyErr_SetString(PyExc_SystemError,
            (moduleName + std::string(".info() failed to create arguments")).c_str());
        Py_XDECREF(apihelper);
        return 0;
    }

    PyObject* callObj = PyObject_CallObject(func, arg);
    bool success = (callObj != 0);

    Py_XDECREF(callObj);
    Py_XDECREF(arg);
    Py_XDECREF(apihelper);

    if (success)
        Py_RETURN_NONE;
    else
        return 0;
}

static PyObject* voreenqt_info(PyObject* /*self*/, PyObject* /*args*/) {
    return printModuleInfo("voreenqt"/*, true, 0, 0, true*/);
}

//------------------------------------------------------------------------------
// method table
static PyMethodDef voreenqt_methods[] = {
    {
        "messageBox",
        voreenqt_messageBox,
        METH_VARARGS,
        "Show a message box."
    },
    {
        "questionBox",
        voreenqt_questionBox,
        METH_VARARGS,
        "Show a question box."
    },
    {
        "quit",
        voreenqt_quit,
        METH_VARARGS,
        "Quit the application."
    },
    {
        "processEvents",
        voreenqt_processEvents,
        METH_VARARGS,
        "Processes all events waiting in the Qt event queue."
    },
    {
        "info",
        voreenqt_info,
        METH_VARARGS,
        "Prints documentation of the module's functions."
    },
    { NULL, NULL, 0, NULL} // sentinal
};

//------------------------------------------------------------------------------

namespace voreen {

PyVoreenQt::PyVoreenQt() {

    if (Py_IsInitialized())
        Py_InitModule("voreenqt", voreenqt_methods);
    else
        LERRORC("voreen.Python.PyVoreenQt", "Python environment not initialized");
}

} // namespace

