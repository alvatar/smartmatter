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

#ifndef VRN_EXPENDABLEHEADERBUTTON_H
#define VRN_EXPENDABLEHEADERBUTTON_H

#include <QFrame>

class QLabel;
class QToolButton;

namespace voreen {

class ExpandableHeaderButton : public QFrame {
Q_OBJECT
public:
    ExpandableHeaderButton(const QString& name, QWidget* parent = 0, bool expanded = false, bool userExpandanble = true);

    void setExpanded(bool expanded);
    void setUserExpandable(bool userExpandable);
    bool isExpanded() const;
    bool userExpandable() const;
    void updateNameLabel(const std::string& name);

    void showLODControls();
    void hideLODControls();

signals:
    void toggled(bool expanded);
    void setLODHidden();
    void setLODVisible();

protected:
    void mousePressEvent(QMouseEvent* event);
    void updateState();

    QLabel* expandLabel_;
    QLabel* rendName_;
    QToolButton* lodControlSetVisible_;
    QToolButton* lodControlSetHidden_;
    bool expanded_;
    bool userExpandable_;
};

} // namespace

#endif // VRN_EXPENDABLEHEADERBUTTON_H
