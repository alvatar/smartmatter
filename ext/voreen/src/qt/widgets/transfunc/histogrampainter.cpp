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

#include "voreen/qt/widgets/transfunc/histogrampainter.h"

#include "voreen/core/datastructures/volume/histogram.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPoint>

namespace voreen {

HistogramPainter::HistogramPainter(QWidget* parent, tgt::vec2 xRange, tgt::vec2 yRange,
                                   int padding, int arrowLength)
    : QWidget(parent)
    , xRange_(xRange)
    , yRange_(yRange)
    , padding_(padding)
    , arrowLength_(arrowLength)
    , histogram_(0)
    , cache_(0)
{
}

HistogramPainter::~HistogramPainter() {
    delete histogram_;
    delete cache_;
}

void HistogramPainter::setHistogram(HistogramIntensity* histogram) {
    delete histogram_;
    histogram_ = histogram;
    delete cache_;
    cache_ = 0;
}

void HistogramPainter::setxRange(const tgt::vec2& xRange) {
    if (xRange != xRange_) {
        xRange_ = xRange;
        delete cache_;
        cache_ = 0;
    }
}

void HistogramPainter::paintEvent(QPaintEvent* event) {
    event->accept();

    if (cache_ == 0 || cache_->rect() != rect()) {
        delete cache_;
        cache_ = new QPixmap(rect().size());
        cache_->fill(Qt::transparent);

        QPainter paint(cache_);

        // put origin in lower lefthand corner
        QMatrix m;
        m.translate(0.0, static_cast<float>(height())-1);
        m.scale(1.f, -1.f);
        paint.setMatrix(m);
        paint.setMatrixEnabled(true);

        if (histogram_) {
            // draw histogram
            paint.setPen(Qt::NoPen);
            paint.setBrush(QColor(200, 0, 0, 120));
            paint.setRenderHint(QPainter::Antialiasing, true);

            int histogramWidth = static_cast<int>(histogram_->getBucketCount());
            tgt::vec2 p;

            QPointF* points = new QPointF[histogramWidth + 2];
            int count = 0;

            for (int x=0; x < histogramWidth; ++x) {
                float xpos = static_cast<float>(x) / histogramWidth;
                // Do some simple clipping here, as the automatic clipping of drawPolygon()
                // gets very slow if lots of polygons have to be clipped away, e.g. when
                // zooming to small part of the histogram.
                if (xpos >= xRange_[0] && xpos <= xRange_[1]) {
                    float value = histogram_->getLogNormalized(x);
                    p = wtos(tgt::vec2(xpos, value * (yRange_[1] - yRange_[0]) + yRange_[0]));

                    // optimization: if the y-coord has not changed from the two last points
                    // then just update the last point's x-coord to the current one
                    if( (count >= 2 ) && (points[count - 2].ry() == p.y) && (points[count - 1].ry() == p.y) && (count >= 2) ){
                        points[count - 1].rx() = p.x;
                    } else {
                        points[count].rx() = p.x;
                        points[count].ry() = p.y;
                        count++;
                    }
                }
            }

            // Qt can't handle polygons that have more than 65536 points
            // so we have to split the polygon
            bool needSplit = false;
            if (count > 65536 - 2) { // 16 bit dataset
                needSplit = true;
                count = 65536 - 2; // 2 points needed for closing the polygon
            }

            if (count > 0) {
                // move x coordinate of first and last points to prevent vertical holes caused
                // by clipping
                points[0].rx() = wtos(tgt::vec2(xRange_[0], 0.f)).x;
                if (count < histogramWidth - 2) // only when last point was actually clipped
                    points[count - 1].rx() = wtos(tgt::vec2(xRange_[1], 0.f)).x;

                // needed for a closed polygon
                p = wtos(tgt::vec2(0.f, yRange_[0]));
                points[count].rx() = points[count - 1].rx();
                points[count].ry() = p.y;
                count++;
                p = wtos(tgt::vec2(0.f, yRange_[0]));
                points[count].rx() = points[0].rx();
                points[count].ry() = p.y;
                count++;

                paint.drawPolygon(points, count);
            }

            // draw last points when splitting is needed
            if (needSplit && false) {
                delete[] points;
                points = new QPointF[5];
                count = 0;
                for (int x=histogramWidth - 2; x < histogramWidth; ++x) {
                    float xpos = static_cast<float>(x) / histogramWidth;
                    if (xpos >= xRange_[0] && xpos <= xRange_[1]) {
                        float value = histogram_->getLogNormalized(x);
                        p = wtos(tgt::vec2(xpos, value * (yRange_[1] - yRange_[0]) + yRange_[0]));
                        points[x-histogramWidth+3].rx() = p.x;
                        points[x-histogramWidth+3].ry() = p.y;
                        count++;
                    }
                }
                if (count > 0) {
                    // move x coordinate of last point to prevent vertical holes caused by clipping
                    points[count - 1].rx() = wtos(tgt::vec2(xRange_[1], 0.f)).x;

                    // needed for a closed polygon
                    p = wtos(tgt::vec2(0.f, yRange_[0]));
                    points[count].rx() = points[count - 1].rx();
                    points[count].ry() = p.y;
                    count++;
                    p = wtos(tgt::vec2(0, yRange_[0]));
                    points[count].rx() = points[0].rx();
                    points[count].ry() = p.y;
                    count++;

                    paint.drawPolygon(points, 5);
                }
            }
            delete[] points;
        }
    }

    QPainter paint(this);
    paint.drawPixmap(0, 0, *cache_);
}

tgt::vec2 HistogramPainter::wtos(const tgt::vec2& p) const {
    float sx = (p.x - xRange_[0]) / (xRange_[1] - xRange_[0]) * (static_cast<float>(width())  - 2 * padding_ - 1.5 * arrowLength_) + padding_;
    float sy = (p.y - yRange_[0]) / (yRange_[1] - yRange_[0]) * (static_cast<float>(height()) - 2 * padding_ - 1.5 * arrowLength_) + padding_;
    return tgt::vec2(sx, sy);
}

} // namespace
