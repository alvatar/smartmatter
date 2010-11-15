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

#ifndef VRN_SLIDERSPINBOXWIDGET_H
#define VRN_SLIDERSPINBOXWIDGET_H

#include <QApplication>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpacerItem>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "voreen/core/properties/property.h"

namespace voreen {

class SliderSpinBoxWidget : public QWidget {
    Q_OBJECT;
    Q_PROPERTY( int value READ getValue WRITE setValue );
    Q_PROPERTY( int minValue READ getMinValue WRITE setMinValue );
    Q_PROPERTY( int maxValue READ getMaxValue WRITE setMaxValue );
public:
    SliderSpinBoxWidget(QWidget* parent = 0);
    virtual int getValue()const;
    virtual int getMinValue()const;
    virtual int getMaxValue()const;
    virtual bool isSliderDown()const;
    virtual QSize sizeHint () const;
    virtual void setFocusPolicy(Qt::FocusPolicy policy);
    virtual void setView(Property::View);

    /**
     * If slider tracking is disabled, valueChanged signals
     * are not fired during user interaction, i.e.,
     * while the user is dragging the slider.
     * Instead, valueChanged is emitted after
     * the slider has been released.
     * Slider tracking is enabled by default.
     *
     * @note The spin box is always kept in sync with
     *  the slider, regardless of the slider tracking mode.
     *
     * @see QAbstractSlider
     */
    virtual void setSliderTracking(bool tracking);

    /// Returns whether slider tracking is enabled (default: true).
    bool hasSliderTracking() const;

    /**
     * If spin box tracking is disabled, valueChanged signals
     * are not fired during user interaction, i.e.,
     * while the user is typing into the spinbox.
     * Instead, valueChanged is emitted on return
     * or when the spin box looses focus.
     * Spinbox tracking is disabled by default.
     *
     * @see QAbstractSpinbox
     */
    virtual void setSpinboxTracking(bool tracking);

    /// Returns whether spin box tracking is enabled (default: false).
    bool hasSpinboxTracking() const;

signals:
    void valueChanged(int);
    void sliderPressedChanged(bool);
    void editingFinished();
public slots:
    virtual void setValue( int value );
    virtual void setMaxValue( int value );
    virtual void setMinValue( int value );
    virtual void setSingleStep(int value);

protected slots:
    virtual void spinEditingFinished();
    virtual void sliderPressed();
    virtual void sliderReleased();

protected:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QSlider *sliderSLD;
    QSpinBox *spinBoxSPB;
    QSpacerItem *spacerItem;

    bool sliderTracking_;
    bool spinboxTracking_;
    int value_;
};


// ---------------------------------------------------------------------------


class DoubleSliderSpinBoxWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY( double value READ getValue WRITE setValue )
    Q_PROPERTY( double minValue READ getMinValue WRITE setMinValue )
    Q_PROPERTY( double maxValue READ getMaxValue WRITE setMaxValue )
public:
    DoubleSliderSpinBoxWidget(QWidget* parent = 0);
    virtual double getValue() const;
    virtual double getMinValue() const;
    virtual double getMaxValue() const;
    virtual double getSingleStep() const;
    virtual int getDecimals() const;
    virtual bool isSliderDown() const;
    virtual QSize sizeHint () const;
    virtual void setFocusPolicy(Qt::FocusPolicy policy);
    virtual void setView(Property::View);

    /**
     * If slider tracking is disabled, valueChanged signals
     * are not fired during user interaction, i.e.,
     * while the user is dragging the slider.
     * Instead, valueChanged is emitted after
     * the slider has been released.
     * Slider tracking is enabled by default.
     *
     * @note The spin box is always kept in sync with
     *  the slider, regardless of the slider tracking mode.
     *
     * @see QAbstractSlider
     */
    virtual void setSliderTracking(bool tracking);

    /// Returns whether slider tracking is enabled (default: true).
    bool hasSliderTracking() const;

    /**
     * If spin box tracking is disabled, valueChanged signals
     * are not fired during user interaction, i.e.,
     * while the user is typing into the spinbox.
     * Instead, valueChanged is emitted on return
     * or when the spin box looses focus.
     * Spinbox tracking is disabled by default.
     *
     * @see QAbstractSpinbox
     */
    virtual void setSpinboxTracking(bool tracking);

    /// Returns whether spin box tracking is enabled (default: false).
    bool hasSpinboxTracking() const;

signals:
    void valueChanged(double);
    void sliderPressedChanged(bool);
    void editingFinished();
public slots:
    virtual void setValue( double value );
    virtual void setMaxValue( double value );
    virtual void setMinValue( double value );
    virtual void setSingleStep( double step );
    virtual void setDecimals( int decimals );
    virtual void sliderPressed();
    virtual void sliderReleased();

protected slots:
    virtual void sliderValueChanged( int value );
    virtual void spinEditingFinished();
    virtual void adjustSliderScale();

protected:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QSlider *sliderSLD;
    QDoubleSpinBox *spinBoxSPB;
    QSpacerItem *spacerItem;

    bool sliderTracking_;
    bool spinboxTracking_;
    double value_;
};


} // namespace voreen;

#endif //VRN_SLIDERSPINBOXWIDGET_H
