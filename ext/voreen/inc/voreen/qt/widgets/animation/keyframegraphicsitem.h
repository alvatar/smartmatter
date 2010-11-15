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

#ifndef VRN_KEYFRAMEGRAPHICSITEM_H
#define VRN_KEYFRAMEGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QLabel>
#include <QObject>

namespace voreen {

/**
* QGraphicsItem representing a KeyFrame
*/
#if (QT_VERSION >= 0x040600)
class KeyframeGraphicsItem : public QGraphicsObject {
#else
class KeyframeGraphicsItem : public QObject, public QGraphicsItem {
#endif
Q_OBJECT

public:
    KeyframeGraphicsItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent*){emit itemHoverEnter(this);}
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*){emit itemHoverLeave(this);}
    void hoverMoveEvent(QGraphicsSceneHoverEvent*){emit itemHoverMove(this);}

private:
    /// actual value of the Keyframe
    int frame_;

signals:
    void itemClicked(KeyframeGraphicsItem*);
    void itemReleased(KeyframeGraphicsItem*);
    void itemMoving(KeyframeGraphicsItem* kfgi);
    void itemHoverEnter(KeyframeGraphicsItem* kfgi);
    void itemHoverMove(KeyframeGraphicsItem* kfgi);
    void itemHoverLeave(KeyframeGraphicsItem* kfgi);
};

} // namespace voreen

#endif
