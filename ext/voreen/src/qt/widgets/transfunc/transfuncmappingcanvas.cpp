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

#include "voreen/qt/widgets/transfunc/transfuncmappingcanvas.h"

#include "voreen/qt/widgets/transfunc/histogrampainter.h"

#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumehandle.h"
#include "voreen/core/datastructures/volume/histogram.h"
#include "voreen/core/datastructures/transfunc/transfuncintensity.h"
#include "voreen/core/datastructures/transfunc/transfuncmappingkey.h"

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QToolTip>
#include <QThread>

#include <iostream>

namespace voreen {

using tgt::vec2;

HistogramThread::HistogramThread(Volume* volume, int count, QObject* parent)
    : QThread(parent)
    , volume_(volume)
    , count_(count)
{
    tgtAssert(volume, "No volume");
}

void HistogramThread::run() {
    HistogramIntensity* hist = new HistogramIntensity(volume_, count_);
    emit setHistogram(hist);
}

//-----------------------------------------------------------------------------

TransFuncMappingCanvas::TransFuncMappingCanvas(QWidget* parent, TransFuncIntensity* tf, bool noColor,
                                               bool clipThresholds, QString xAxisText,
                                               QString yAxisText)
    : QWidget(parent)
    , tf_(tf)
    , maximumIntensity_(255)
    , clipThresholds_(clipThresholds)
    , noColor_(noColor)
    , xAxisText_(xAxisText)
    , yAxisText_(yAxisText)
    , histogramThread_(0)
    , histogramNeedsUpdate_(false)
{
    xRange_ = vec2(0.f, 1.f);
    yRange_ = vec2(0.f, 1.f);
    padding_ = 12;
    arrowLength_ = 10;
    arrowWidth_ = 3;
    pointSize_ = 10;
    selectedKey_ = 0;
    selectedLeftPart_ = true;
    splitFactor_ = 1.5f;
    minCellSize_ = 8;
    dragging_ = false;
    dragLine_ = -1;
    dragLineAlphaLeft_ = -1.f;
    dragLineAlphaRight_ = -1.f;

    histogramPainter_ = new HistogramPainter(this, xRange_, yRange_, padding_, arrowLength_);

    setObjectName("TransFuncMappingCanvas");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    setFocus();

    setThreshold(0.f, 1.f);

    if (!noColor_) {
        QAction* cc = new QAction(tr("Change color of key"), this);
        keyContextMenu_.addAction(cc);
        connect(cc, SIGNAL(triggered()), this, SLOT(changeCurrentColor()));
    }

    splitMergeAction_ = new QAction(tr(""), this); // Text will be set later
    keyContextMenu_.addAction(splitMergeAction_);
    connect(splitMergeAction_, SIGNAL(triggered()), this, SLOT(splitMergeKeys()));

    zeroAction_ = new QAction("", this); // Text will be set later
    keyContextMenu_.addAction(zeroAction_);
    connect(zeroAction_, SIGNAL(triggered()), this, SLOT(zeroKey()));

    deleteAction_ = new QAction(tr("Delete this key"), this);
    keyContextMenu_.addAction(deleteAction_);
    connect(deleteAction_, SIGNAL(triggered()), this, SLOT(deleteKey()));

    loadAction_ = new QAction(tr("Load transfer function..."), this);
    noKeyContextMenu_.addAction(loadAction_);
    connect(loadAction_, SIGNAL(triggered()), this, SIGNAL(loadTransferFunction()));

    saveAction_ = new QAction(tr("Save transfer function..."), this);
    noKeyContextMenu_.addAction(saveAction_);
    connect(saveAction_, SIGNAL(triggered()), this, SIGNAL(saveTransferFunction()));

    resetAction_ = new QAction(tr("Reset transfer function"), this);
    noKeyContextMenu_.addAction(resetAction_);
    connect(resetAction_, SIGNAL(triggered()), this, SLOT(resetTransferFunc()));
}

TransFuncMappingCanvas::~TransFuncMappingCanvas() {
    if (histogramThread_) {
        histogramThread_->wait(); // wait for thread to finish before deleting
        delete histogramThread_;
    }
}

//--------- methods for reacting on Qt events ---------//

void TransFuncMappingCanvas::showNoKeyContextMenu(QMouseEvent *event) {
    noKeyContextMenu_.popup(event->globalPos());
}

void TransFuncMappingCanvas::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    //resize hisotgrampainter as well
    histogramPainter_->resize(width(), height());
    gridSpacing_ = vec2(1.0, 1.0);
    // refine gridSpacing_ as good as possible
    vec2 factor = vec2(0.1f, 0.2f);
    for (int k=0; k<2; ++k) {
        for (int component=0; component<2; ++component) {
            vec2 cellSize = wtos(gridSpacing_) - wtos(vec2(0.0, 0.0));
            cellSize[component] *= factor[k];
            while (cellSize[component] > minCellSize_) {
                gridSpacing_[component] *= factor[k];
                cellSize[component] *= factor[k];
            }
            cellSize[component] /= factor[k];
        }
    }
}

void TransFuncMappingCanvas::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateHistogram(); // only if necessary
}

void TransFuncMappingCanvas::showKeyContextMenu(QMouseEvent* event) {

    if (!tf_)
        return;

    // Set context-dependent text for menu items

    // Split/merge
    QString splitMergeText;
    if (selectedKey_->isSplit())
        splitMergeText = tr("Merge this key");
    else
        splitMergeText = tr("Split this key");
    splitMergeAction_->setText(splitMergeText);

    // Zero/unzero
    QString zeroText;
    if (selectedLeftPart_)
        zeroText = tr("Zero to the left");
    else
        zeroText = tr("Zero to the right");
    zeroAction_->setText(zeroText);

    // allow deletion of keys only if there are more than two keys
    deleteAction_->setEnabled(tf_->getNumKeys() > 2);

    keyContextMenu_.popup(event->globalPos());
}

void TransFuncMappingCanvas::paintEvent(QPaintEvent* event) {

    if (!tf_)
        return;

    //the histogram is automatically painted onto this widget
    //we do not need to call the paintevent for the Histogrampainter directly
    event->accept();

    QPainter paint(this);

    // put origin in lower lefthand corner
    QMatrix m;
    m.translate(0.0, static_cast<float>(height())-1);
    m.scale(1.f, -1.f);
    paint.setMatrix(m);

    paint.setMatrixEnabled(true);
    paint.setRenderHint(QPainter::Antialiasing, false);
    paint.setPen(Qt::NoPen);
    paint.setBrush(Qt::white);
    paint.drawRect(0, 0, width() - 1, height() - 1);

    // ----------------------------------------------

    // draw grid
    paint.setPen(QColor(220, 220, 220));
    paint.setRenderHint(QPainter::Antialiasing, false);

    vec2 pmin = vec2(0.f, 0.f);
    vec2 pmax = vec2(1.f, 1.f);

    for (float f=pmin.x; f<pmax.x+gridSpacing_.x*0.5; f+=gridSpacing_.x) {
        vec2 p = wtos(vec2(f, 0.f));
        vec2 a = wtos(vec2(0.f, 0.f));
        vec2 b = wtos(vec2(0.f, 1.f));
        paint.drawLine(QPointF(p.x, a.y),
                       QPointF(p.x, b.y));
    }

    for (float f=pmin.y; f<pmax.y+gridSpacing_.y*0.5; f+=gridSpacing_.y) {
        vec2 p = wtos(vec2(0.f, f));
        vec2 a = wtos(vec2(0.f, 0.f));
        vec2 b = wtos(vec2(1.f, 0.f));
        paint.drawLine(QPointF(a.x, p.y),
                       QPointF(b.x, p.y));
    }

    // draw x and y axes
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen(Qt::gray);
    paint.setBrush(Qt::gray);

    // draw axes independently from visible range
    float oldx0_ = xRange_[0];
    float oldx1_ = xRange_[1];
    xRange_[0] = 0.f;
    xRange_[1] = 1.f;

    vec2 origin = wtos(vec2(0.f, 0.f));
    origin.x = floor(origin.x) + 0.5f;
    origin.y = floor(origin.y) + 0.5f;

    paint.setRenderHint(QPainter::Antialiasing, true);

    paint.drawLine(QPointF(padding_, origin.y),
                   QPointF(width() - padding_, origin.y));

    paint.drawLine(QPointF(origin.x, padding_),
                   QPointF(origin.x, height() - padding_));

    QPointF arrow[3];
    arrow[0] = QPointF(origin.x, height() - padding_);
    arrow[1] = QPointF(origin.x + arrowWidth_, height() - padding_ - arrowLength_);
    arrow[2] = QPointF(origin.x - arrowWidth_, height() - padding_ - arrowLength_);

    paint.drawConvexPolygon(arrow, 3);

    arrow[0] = QPointF(width() - padding_, origin.y);
    arrow[1] = QPointF(width() - padding_ - arrowLength_, origin.y - arrowWidth_);
    arrow[2] = QPointF(width() - padding_ - arrowLength_, origin.y + arrowWidth_);

    paint.drawConvexPolygon(arrow, 3);

    paint.scale(-1.f, 1.f);
    paint.rotate(180.f);
    paint.drawText(static_cast<int>(width() - 6.2f * padding_), static_cast<int>(-1 * (origin.y - 0.8f * padding_)), xAxisText_);
    paint.drawText(static_cast<int>(1.6f * padding_), static_cast<int>(-1 * (height() - 1.85f * padding_)), yAxisText_);

    paint.rotate(180.f);
    paint.scale(-1.f, 1.f);

    xRange_[0] = oldx0_;
    xRange_[1] = oldx1_;

    // ----------------------------------------------

    // draw mapping function
    QPen pen = QPen(Qt::darkRed);
    pen.setWidthF(1.5f);
    paint.setPen(pen);

    origin = wtos(vec2(0.f));

    vec2 old;
    for (int i=0; i<tf_->getNumKeys(); ++i) {
        TransFuncMappingKey *key = tf_->getKey(i);
        vec2 p = wtos(vec2(key->getIntensity(), key->getColorL().a / 255.f));
        if (i == 0)  {
            if (tf_->getKey(0)->getIntensity() > 0.f)
                paint.drawLine(QPointF(wtos(vec2(0.f, 0.f)).x, p.y),
                               QPointF(p.x - 1.f, p.y));
        }
        else {
            paint.drawLine(QPointF(old.x + 1.f, old.y),
                           QPointF(p.x - 1.f, p.y));
        }
        old = p;
        if (key->isSplit())
            old = wtos(vec2(key->getIntensity(), key->getColorR().a / 255.f));
    }
    if (tf_->getKey(tf_->getNumKeys()-1)->getIntensity() < 1.f) {
        paint.drawLine(QPointF(old.x + 1.f, old.y),
                       QPointF(wtos(vec2(1.f, 0.f)).x, old.y));
    }

    if (xRange_[1] != xRange_[0])
        paintKeys(paint);

    // ----------------------------------------------

    // grey out threshold area
    paint.setBrush(QBrush(QColor(192, 192, 192, 230), Qt::SolidPattern));
    paint.setPen(Qt::NoPen);
    vec2 upperRight = wtos(vec2(1.f));
    vec2 lowerLeft = wtos(vec2(0.f));
    int w = static_cast<int>(upperRight.x - lowerLeft.x);
    int h = static_cast<int>(upperRight.y - lowerLeft.y);

    if (thresholdL_ > 0.f) {
        paint.drawRect(static_cast<int>(origin.x), static_cast<int>(origin.y),
                       static_cast<int>(thresholdL_ * w + 1), h);
    }
    if (thresholdU_ < 1.f) {
        paint.drawRect(static_cast<int>(origin.x + floor(thresholdU_ * w)),
                       static_cast<int>(origin.y), static_cast<int>((1 - thresholdU_) * w + 1), h);
    }

    paint.setRenderHint(QPainter::Antialiasing, false);

    paint.setPen(Qt::lightGray);
    paint.setBrush(Qt::NoBrush);
    paint.drawRect(0, 0, width() - 1, height() - 1);

    paint.setMatrixEnabled(false);
    if (histogramThread_ && !histogramThread_->isFinished()) {
        paint.setPen(Qt::red);
        paint.drawText(QRectF(0, 7, width() - 1, height() - 8), tr("Calculating histogram..."), QTextOption(Qt::AlignHCenter));
    }
}

void TransFuncMappingCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        emit toggleInteractionMode(true);

    event->accept();

    dragLine_ = hitLine(vec2(event->x(), event->y()));
    if (dragLine_ >= 0 && event->modifiers() == Qt::ShiftModifier) {
        dragLineStartY_ = event->y();
        return;
    }

    tgt::vec2 sHit = tgt::vec2(event->x(), static_cast<float>(height()) - event->y());
    tgt::vec2 hit = stow(sHit);

    // see if a key was selected
    selectedKey_ = 0;
    for (int i=0; i<tf_->getNumKeys(); ++i) {
        TransFuncMappingKey* key = tf_->getKey(i);
        tgt::vec2 sp = wtos(tgt::vec2(key->getIntensity(), key->getColorL().a / 255.0));
        tgt::vec2 spr = wtos(tgt::vec2(key->getIntensity(), key->getColorR().a / 255.0));
        if (key->isSplit()) {
            if (sHit.x > sp.x - splitFactor_ * pointSize_ && sHit.x <= sp.x &&
                sHit.y > sp.y - pointSize_ && sHit.y < sp.y + pointSize_)
            {
                selectedKey_ = key;
                selectedLeftPart_ = true;
            }
            if (sHit.x >= spr.x && sHit.x < spr.x + splitFactor_ * pointSize_ &&
                sHit.y > spr.y - pointSize_ && sHit.y < spr.y + pointSize_)
            {
                selectedKey_ = key;
                selectedLeftPart_ = false;
            }
        }
        else {
            if (sHit.x > sp.x - pointSize_ && sHit.x < sp.x + pointSize_ &&
                sHit.y > sp.y - pointSize_ && sHit.y < sp.y + pointSize_)
            {
                selectedKey_ = key;
                selectedLeftPart_ = false;
            }
        }
    }


    if (event->button() == Qt::RightButton) {
        if (selectedKey_ == 0)
            showNoKeyContextMenu(event);
        else
            showKeyContextMenu(event);
        return;
    }

    if (selectedKey_ != 0 && event->button() == Qt::LeftButton) {
        dragging_ = true;
        //keep values within valid range
        hit = tgt::clamp(hit, 0.f, 1.f);
        updateCoordinates(event->pos(), hit);
        if (selectedKey_->isSplit() && !selectedLeftPart_)
            emit colorChanged(Col2QColor(selectedKey_->getColorR()));
        else
            emit colorChanged(Col2QColor(selectedKey_->getColorL()));
        return;
    }

    // no key was selected -> insert new key
    if (hit.x >= 0.f && hit.x <= 1.f &&
        hit.y >= 0.f && hit.y <= 1.f &&
        event->button() == Qt::LeftButton)
    {
        insertNewKey(hit);
        dragging_ = true;
        dragLine_ = -1;
        updateCoordinates(event->pos(), hit);
        update();
        emit colorChanged(Col2QColor(selectedKey_->getColorL()));
        emit changed();
    }
}

void TransFuncMappingCanvas::mouseMoveEvent(QMouseEvent* event) {
    event->accept();
    mousePos_ = event->pos();

    vec2 sHit = vec2(event->x(), static_cast<float>(height()) - event->y());
    vec2 hit = stow(sHit);


    if (!dragging_ && hitLine(vec2(event->x(), event->y())) >= 0 && event->modifiers() == Qt::ShiftModifier)
        setCursor(Qt::SizeVerCursor);
    else
        unsetCursor();

    if (dragLine_ >= 0) {
        // a line between 2 keys is moved (shift modifier was used)
        float delta = dragLineStartY_ - event->y();
        dragLineStartY_ = event->y();
        //left key
        TransFuncMappingKey* key = tf_->getKey(dragLine_);
        if (dragLineAlphaLeft_ == -1.f)
            dragLineAlphaLeft_ = key->isSplit() ? key->getAlphaR() : key->getAlphaL();
        dragLineAlphaLeft_ = wtos(vec2(dragLineAlphaLeft_)).y;
        dragLineAlphaLeft_ += delta;
        dragLineAlphaLeft_ = stow(vec2(dragLineAlphaLeft_)).y;
        if (dragLineAlphaLeft_ < 0.f)
            dragLineAlphaLeft_ = 0.f;
        if (dragLineAlphaLeft_ > 1.f)
            dragLineAlphaLeft_ = 1.f;
        key->setAlphaR(dragLineAlphaLeft_);
        tf_->updateKey(key);
        if (tf_->getNumKeys() >= dragLine_+1) {
            //right key - when existing
            key = tf_->getKey(dragLine_+1);
            if (dragLineAlphaRight_ == -1.f)
                dragLineAlphaRight_ = key->getAlphaL();
            dragLineAlphaRight_ = wtos(vec2(dragLineAlphaRight_)).y;
            dragLineAlphaRight_ += delta;
            dragLineAlphaRight_ = stow(vec2(dragLineAlphaRight_)).y;
            if (dragLineAlphaRight_ < 0.f)
                dragLineAlphaRight_ = 0.f;
            if (dragLineAlphaRight_ > 1.f)
                dragLineAlphaRight_ = 1.f;
            key->setAlphaL(dragLineAlphaRight_);
            tf_->updateKey(key);
        }
        update();
        emit changed();
        return;
    }

    // return when no key was inserted or selected
    if (!dragging_)
        return;

    // keep location within valid texture coord range
    hit = tgt::clamp(hit, 0.f, 1.f);

    if (selectedKey_ != 0) {
        updateCoordinates(event->pos(), hit);
        if (event->modifiers() != Qt::ShiftModifier) {
            selectedKey_->setIntensity(hit.x);
        }
        if (event->modifiers() != Qt::ControlModifier) {
            if (selectedKey_->isSplit()) {
                if (selectedLeftPart_)
                    selectedKey_->setAlphaL(hit.y);
                else
                    selectedKey_->setAlphaR(hit.y);
            }
            else
                selectedKey_->setAlphaL(hit.y);
        }
        bool selectedFound = false;
        for (size_t i = 0; i < tf_->getKeys().size(); ++i) {
            TransFuncMappingKey* key = tf_->getKey(static_cast<int>(i));
            //is the tf key the selected one?
            if (key == selectedKey_) {
                selectedFound = true;
                continue;
            }
            if (selectedFound) {
                //change intensity of key if its lower than the intensity of selectedKey_
                if (key->getIntensity() < selectedKey_->getIntensity())
                    key->setIntensity(selectedKey_->getIntensity());
            }
            else {
                //change intensity of key if its higher than the intensity of selectedKey_
                if (key->getIntensity() > selectedKey_->getIntensity())
                    key->setIntensity(selectedKey_->getIntensity());
            }
        }
        tf_->updateKey(selectedKey_);

        update();
        emit changed();
    }
}

void TransFuncMappingCanvas::mouseReleaseEvent(QMouseEvent* event) {
    event->accept();
    if (event->button() == Qt::LeftButton) {
        dragging_ = false;
        dragLine_ = -1;
        dragLineAlphaLeft_ = -1.f;
        dragLineAlphaRight_ = -1.f;
        hideCoordinates();
        update();
        emit toggleInteractionMode(false);
    }
}

void TransFuncMappingCanvas::mouseDoubleClickEvent(QMouseEvent *event) {
    event->accept();
    if (event->button() == Qt::LeftButton)
        changeCurrentColor();
}

void TransFuncMappingCanvas::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Shift                    && underMouse() &&
        hitLine(vec2(mousePos_.x(), mousePos_.y())) >= 0 && !dragging_)
    {
        setCursor(Qt::SizeVerCursor);
    }
}

void TransFuncMappingCanvas::keyReleaseEvent(QKeyEvent* event) {
    unsetCursor();
    if (event->key() == Qt::Key_Delete && selectedKey_ != 0) {
        event->accept();
        deleteKey();
    }
}

//--------- slots ---------//

void TransFuncMappingCanvas::changeCurrentColor(const QColor& c) {
    if (!selectedKey_ || !c.isValid())
        return;

    tgt::col4 tgtcolor = QColor2Col(c);
    bool changedColor = false;
    if (selectedKey_->isSplit() && !selectedLeftPart_) {
        tgtcolor.a = selectedKey_->getColorR().a;
        if (selectedKey_->getColorR() != tgtcolor) {
            selectedKey_->setColorR(tgtcolor);
            changedColor = true;
        }
    }
    else {
        tgtcolor.a = selectedKey_->getColorL().a;
        if (selectedKey_->getColorL() != tgtcolor) {
            selectedKey_->setColorL(tgtcolor);
            changedColor = true;
        }
    }

    if (changedColor) {
        update();
        emit changed();
        emit colorChanged(c);
    }
}

void TransFuncMappingCanvas::splitMergeKeys() {
    if (!selectedKey_)
        return;

    selectedKey_->setSplit(!selectedKey_->isSplit());
    update();
    emit changed();
}

void TransFuncMappingCanvas::zeroKey() {
    if (!selectedKey_)
        return;

    TransFuncMappingKey* otherKey = getOtherKey(selectedKey_, selectedLeftPart_);
    if (otherKey) {
        if (!otherKey->isSplit())
            otherKey->setSplit(true);
        if (selectedLeftPart_)
            otherKey->setAlphaR(0.0);
        else
            otherKey->setAlphaL(0.0);
    }

    if (!selectedKey_->isSplit())
        selectedKey_->setSplit(true);

    if (selectedLeftPart_)
        selectedKey_->setAlphaL(0.0);
    else
        selectedKey_->setAlphaR(0.0);

    update();
    emit changed();
}

void TransFuncMappingCanvas::deleteKey() {

    if (!tf_)
        return;

    if (!selectedKey_ || tf_->getNumKeys() < 3)
        return;

    tf_->removeKey(selectedKey_);
    selectedKey_ = 0;

    update();
    emit changed();
}

void TransFuncMappingCanvas::resetTransferFunc() {
    selectedKey_ = 0;
    emit resetTransferFunction();
    update();
}

void TransFuncMappingCanvas::toggleClipThresholds(bool enabled) {
    clipThresholds_ = enabled;
    if (clipThresholds_)
        xRange_ = vec2(thresholdL_, thresholdU_);
    else
        xRange_ = vec2(0.f, 1.f);

    histogramPainter_->setxRange(xRange_);

    update();
}

//--------- protected helper functions ---------//

void TransFuncMappingCanvas::changeCurrentColor() {
    if (!selectedKey_ || noColor_)
        return;

    QColor oldColor;
    if (selectedKey_->isSplit() && !selectedLeftPart_)
        oldColor = Col2QColor( selectedKey_->getColorR() );
    else
        oldColor = Col2QColor( selectedKey_->getColorL() );

    QColor newColor = QColorDialog::getColor(oldColor, 0);
    if (newColor.isValid())
        changeCurrentColor(newColor);
}

void TransFuncMappingCanvas::insertNewKey(vec2& hit) {

    if (!tf_)
        return;

    hit = tgt::clamp(hit, 0.f, 1.f);

    TransFuncMappingKey* key = new TransFuncMappingKey(hit.x, QColor2Col(Qt::lightGray));

    tf_->addKey(key);
    TransFuncMappingKey* leftKey = getOtherKey(key, true);
    TransFuncMappingKey* rightKey = getOtherKey(key, false);

    // interpolate color of inserted key from neighbouring keys
    // (weighted by distance)
    // the alpha value is determined by hit.y
    tgt::col4 keyColor;
    if (!leftKey)
        keyColor = rightKey->getColorL();
    else if (!rightKey)
        keyColor = leftKey->getColorR();
    else {
        float leftSource = leftKey->getIntensity();
        float rightSource = rightKey->getIntensity();
        float distSource = rightSource - leftSource;
        tgt::vec4 leftColor = static_cast<tgt::vec4>(leftKey->getColorR());
        tgt::vec4 rightColor = static_cast<tgt::vec4>(rightKey->getColorL());

        keyColor = static_cast<tgt::col4>(
            leftColor* ( (distSource-(hit.x-leftSource))/distSource  ) +
            rightColor*( (distSource-(rightSource-hit.x))/distSource ) );
    }
    key->setColorL(keyColor);
    //overwrite alpha value with clicked position
    key->setAlphaL(hit.y);

    selectedKey_ = key;
}

TransFuncMappingKey* TransFuncMappingCanvas::getOtherKey(TransFuncMappingKey* selectedKey, bool selectedLeftPart) {

    if (!tf_)
        return 0;

    TransFuncMappingKey* otherKey = 0;
    for (int i=0; i < tf_->getNumKeys(); ++i) {
        if ((selectedLeftPart && i < tf_->getNumKeys() - 1 && tf_->getKey(i + 1) == selectedKey) ||
            (!selectedLeftPart && i > 0 && tf_->getKey(i - 1) == selectedKey))
        {
            otherKey = tf_->getKey(i);
        }
    }
    return otherKey;
}

int TransFuncMappingCanvas::hitLine(const tgt::vec2& p) {

    if (!tf_)
        return -1;

    int hit = -1;
    vec2 sHit = vec2(p.x, static_cast<float>(height()) - p.y);
    vec2 old;
    for (int i=0; i < tf_->getNumKeys(); ++i) {
        TransFuncMappingKey* key = tf_->getKey(i);
        vec2 p = wtos(vec2(key->getIntensity(), key->getColorL().a / 255.f));
        if (i > 0) {
            vec2 p1 = vec2(old.x + 1.f, old.y);
            vec2 p2 = vec2(p.x - 1.f, p.y);
            float s = (p2.y - p1.y) / (p2.x - p1.x);
            int a = static_cast<int>(p1.y + (sHit.x - p1.x) * s);
            if ((sHit.x >= p1.x+10) && (sHit.x <= p2.x-10) && (abs(static_cast<int>(sHit.y) - a) < 5)) {
                hit = i - 1;
            }
        }

        old = p;
        if (key->isSplit())
            old = wtos(vec2(key->getIntensity(), key->getColorR().a / 255.f));
    }
    return hit;
}

void TransFuncMappingCanvas::paintKeys(QPainter& paint) {

    if (!tf_)
        return;

    for (int i=0; i<tf_->getNumKeys(); ++i) {
        TransFuncMappingKey *key = tf_->getKey(i);
        vec2 p = wtos(vec2(key->getIntensity(), key->getColorL().a / 255.0));
        int props;
        if (key->isSplit()) {
            props = MARKER_LEFT;
            if (key == selectedKey_ && selectedLeftPart_)
                props |= MARKER_SELECTED;

            drawMarker(paint, key->getColorL(), p, props);

            p = wtos(vec2(key->getIntensity(), key->getColorR().a / 255.0));
            props = MARKER_RIGHT;
            if (key == selectedKey_ && !selectedLeftPart_)
                props |= MARKER_SELECTED;

            drawMarker(paint, key->getColorR(), p, props);
        }
        else {
            props = MARKER_NORMAL;
            if (key == selectedKey_)
                props |= MARKER_SELECTED;
            drawMarker(paint, key->getColorL(), p, props);
        }
    }
}

void TransFuncMappingCanvas::drawMarker(QPainter& paint, const tgt::col4& tgtcolor, const tgt::vec2& p, int props) {
    if (noColor_)
        paint.setBrush(Qt::transparent);
    else
        paint.setBrush(Col2QColor(tgtcolor));

    QPen pen(QBrush(Qt::darkGray), Qt::SolidLine);
    if (props & MARKER_SELECTED)
        pen.setWidth(3);
    paint.setPen(pen);

    if (props & MARKER_LEFT) {
        paint.drawPie(QRectF(p.x - splitFactor_ * pointSize_/2, p.y - pointSize_/2,
                             splitFactor_ * pointSize_, pointSize_),
                      90 * 16, 180 * 16);
    }
    else if (props & MARKER_RIGHT) {
        paint.drawPie(QRectF(p.x - splitFactor_ * pointSize_/2, p.y - pointSize_/2,
                             splitFactor_ * pointSize_, pointSize_),
                      270 * 16, 180 * 16);
    }
    else {
        paint.drawEllipse(QRectF(p.x - pointSize_/2, p.y - pointSize_/2,
                                 pointSize_, pointSize_));
    }
}

QColor TransFuncMappingCanvas::Col2QColor(const tgt::col4& color) {
    return QColor(color.r, color.g, color.b); // ignore alpha
}

tgt::col4 TransFuncMappingCanvas::QColor2Col(const QColor& color) {
    return tgt::col4(color.red(), color.green(), color.blue(), 255); // ignore alpha
}

tgt::vec2 TransFuncMappingCanvas::wtos(vec2 p) {
    float sx = (p.x - xRange_[0]) / (xRange_[1] - xRange_[0]) * (static_cast<float>(width())  - 2 * padding_ - 1.5 * arrowLength_) + padding_;
    float sy = (p.y - yRange_[0]) / (yRange_[1] - yRange_[0]) * (static_cast<float>(height()) - 2 * padding_ - 1.5 * arrowLength_) + padding_;
    return vec2(sx, sy);
}

tgt::vec2 TransFuncMappingCanvas::stow(vec2 p) {
    float wx = (p.x - padding_) / (static_cast<float>(width())  - 2 * padding_ - 1.5 * arrowLength_) * (xRange_[1] - xRange_[0]) + xRange_[0];
    float wy = (p.y - padding_) / (static_cast<float>(height()) - 2 * padding_ - 1.5 * arrowLength_) * (yRange_[1] - yRange_[0]) + yRange_[0];
    return vec2(wx, wy);
}

//--------- additional functions ---------//

QSize TransFuncMappingCanvas::minimumSizeHint () const {
    return QSize(300,100);
}

QSize TransFuncMappingCanvas::sizeHint () const {
    return QSize(300, 100);
}

QSizePolicy TransFuncMappingCanvas::sizePolicy () const {
    return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


void TransFuncMappingCanvas::setThreshold(float l, float u) {
    thresholdL_ = l;
    thresholdU_ = u;
    if (clipThresholds_)
        xRange_ = vec2(thresholdL_, thresholdU_);

    histogramPainter_->setxRange(xRange_);

    update();
}

void TransFuncMappingCanvas::hideCoordinates() {
    QToolTip::hideText();
}

void TransFuncMappingCanvas::updateCoordinates(QPoint pos, vec2 values) {
    std::ostringstream os;
    os.precision(2);
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << values.x*maximumIntensity_ << " / " << values.y*255.f;
    QToolTip::showText(mapToGlobal(pos), QString(os.str().c_str()));
}

void TransFuncMappingCanvas::updateHistogram() {

    if (!histogramNeedsUpdate_)
        return;
    histogramNeedsUpdate_ = false;

    tgtAssert(histogramPainter_, "No histogram painter");

    // calculate new histogram in background thread and propagate to HistogramPainter
    if (volumeHandle_ && volumeHandle_->getVolume()) {
        int bits = volumeHandle_->getVolume()->getBitsStored() / volumeHandle_->getVolume()->getNumChannels();
        if (bits > 16)
            bits = 16; // handle float data as if it was 16 bit to prevent overflow
        int maximumIntensity = (1 << bits) - 1;

        histogramThread_ = new HistogramThread(volumeHandle_->getVolume(), maximumIntensity + 1, this);
        connect(histogramThread_, SIGNAL(setHistogram(HistogramIntensity*)),
                histogramPainter_, SLOT(setHistogram(HistogramIntensity*)));
        connect(histogramThread_, SIGNAL(finished()),
                this, SLOT(update()));
        histogramThread_->start();
    }
    else {
        histogramPainter_->setHistogram(0);
    }
}

void TransFuncMappingCanvas::volumeChanged(VolumeHandle* volumeHandle) {
    // stop histogram thread and deregister from volume handle as observer
    if (histogramThread_) {
        stopObservation(volumeHandle_);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        histogramThread_->wait(); // wait for old thread to finish before deleting
        delete histogramThread_;
        histogramThread_ = 0;
        QApplication::restoreOverrideCursor();
    }
    histogramPainter_->setHistogram(0);

    volumeHandle_ = volumeHandle;

    if (volumeHandle_) {
        volumeHandle_->addObserver(this);
        histogramNeedsUpdate_ = true;
    }

    if (isVisible())
        updateHistogram();

    update();
}

void TransFuncMappingCanvas::setTransFunc(TransFuncIntensity* tf) {
    tf_ = tf;
    selectedKey_ = 0;
    update();
}

void TransFuncMappingCanvas::setXAxisText(const std::string& text) {
    xAxisText_ = QString(text.c_str());
}

void TransFuncMappingCanvas::setYAxisText(const std::string& text) {
    yAxisText_ = QString(text.c_str());
}

void TransFuncMappingCanvas::volumeChange(const VolumeHandle* source) {
    tgtAssert(source, "No volume handle");
    // perform calculations on new volume
    if (volumeHandle_ == source)
        volumeChanged(const_cast<VolumeHandle*>(source));
}

void TransFuncMappingCanvas::volumeHandleDelete(const VolumeHandle* source) {
    tgtAssert(source, "No volume handle");
    // make sure to stop calculations on the volume
    if (volumeHandle_ == source)
        volumeChanged(0);
}

} // namespace voreen
