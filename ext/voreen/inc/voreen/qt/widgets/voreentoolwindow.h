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

#ifndef VRN_VOREENTOOLWINDOW_H
#define VRN_VOREENTOOLWINDOW_H

#include <QWidget>
#include <QDockWidget>

namespace voreen {

class VoreenToolWindowTitle : public QWidget {
    Q_OBJECT
public:
    VoreenToolWindowTitle(QDockWidget* parent, bool dockable=true);
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
private:
    bool dockable_;
    QPixmap closeButton_;
    QPixmap undockButton_;
    QPixmap maximizeButton_;
};

//----------------------------------------------------------------------------------------------------------------

class VoreenToolWindow : public QDockWidget {
Q_OBJECT
public:
    VoreenToolWindow(QAction* action, QWidget* parent, QWidget* child, const QString& name = "", bool dockable=true);
    QAction* action() const { return action_; }

private:
    QAction* action_;
};

} // namespace

#endif // VRN_VOREENTOOLWINDOW_H
