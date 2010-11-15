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

#ifndef VRN_DOUBLESLIDER_H
#define VRN_DOUBLESLIDER_H

#include <QWidget>

class QMouseEvent;

namespace voreen {

/**
 * DoubleSlider is a QWidget providing two sliders on one ridge to define a range.
 * The sliders can be moved by mouse interaction or by \a setMaxValue() and \a setMinValue()
 * method. The range of the values is from 0.0 to 1.0 and stored in float variables.
 * To avoid an overlay of the two sliders the active one shunts the inactive slider.
 */
class DoubleSlider : public QWidget {
    Q_OBJECT
public:
    /**
     * Constructor - inits member variables to default
     *
     * @param parent the parent widget
     */
    DoubleSlider(QWidget* parent = 0);

    /**
     * The border to left and right are set to given values.
     *
     * @param left left border
     * @param right right border
     */
    void setOffsets(int left, int right);

    /**
     * Sets the minimal allowed distance between both sliders to given value.
     *
     * @param dist minimal allowed distance between both sliders
     */
    void setMinimalAllowedSliderDistance(float dist);

    /**
     * Returns the minimum of the range marked out by the two sliders, e.g.
     * the x-coordinate of the left slider in a normalized coordinate system.
     */
    float getMinValue();

    /**
     * Returns the maximum of the range marked out by the two sliders, e.g.
     * the x-coordinate of the right slider in a normalized coordinate system.
     */
    float getMaxValue();

public slots:
     /**
      * Sets the maximal value to max and emits valuesChanged(float, float).
      *
      * @param max new maximal value
      */
     void setMaxValue(float max);

     /**
      * Sets the minimal value to min and emits valuesChanged(float, float).
      *
      * @param min new minimal value
      */
     void setMinValue(float min);

     /**
      * Sets the minimal and maximal value. Emits valuesChanged(float, float).
      *
      * @param min new minimal value
      * @param max new maximal value
      */
     void setValues(float min, float max);

     /**
      * Sets the width of a slider. Default value is 5 pixels.
      *
      * @param sliderW width of a slider
      */
     void setSliderWidth(int sliderW);

signals:
     /**
      * This signal is emitted when the user drags a slider.
      *
      * @param min the current max value
      * @param max the current min value
      */
     void valuesChanged(float min, float max);

     /**
      * This signal is emitted when the user drags a slider. It will switch
      * the coarseness mode of volume rendering on and off.
      *
      * @param on should coarseness mode switched on or off?
      */
     void toggleInteractionMode(bool on);

protected:
    /**
     * This method is called by Qt whenever a repaint of the widget is necessary.
     * It will draw a horizontal line and two sliders.
     *
     * @param event the paint event emitted by Qt
     */
    void paintEvent(QPaintEvent* event);

    /**
     * Calculates the slider that is next to mouse position, sets it active
     * and calls \a moveSlider(float). Emits valuesChanged(float, float).
     *
     * @param event the mouse event
     */
    void mousePressEvent(QMouseEvent* event);

    /**
     * Moves the active slider by calling moveSlider() and emits
     * valuesChanged(float, float).
     *
     * @param event the mouse event
     */
    void mouseMoveEvent(QMouseEvent* event);

    /**
     * Only emits the toggleInteractionMode signal, so that coarseness mode is siwtched off.
     *
     * @param event the mouse event
     */
    void mouseReleaseEvent(QMouseEvent* event);

private:
    /**
     * Internal Function to move the active slider(s). Called by \a mouseMoveEvent() and \a mousePressEvent().
     *
     * @param pos new slider position
     */
    void moveSlider(float pos);

    float minValue_;           ///< relative x coordinate of position of left slider
    float maxValue_;           ///< relative x coordinate of position of right slider
    float normalizedMousePos_; ///< position where the drag of a slider started
    float mV1_;                ///< x coordinate of left slider when dragging started
    float mV2_;                ///< x coordinate of right slider when dragging started
    float minimalAllowedSliderDistance_; ///< mininmal allowed distance between both sliders
    int sliderWidth_;          ///< pixel width of sliders
    int leftOffset_;           ///< offset to left before slider and line will start
    int rightOffset_;          ///< offset to right before slider and line will end
    bool leftSliderActive_;    ///< true when left slider was clicked, false otherwise
    bool rightSliderActive_;   ///< true when right slider was clicked, false otherwise
};

} // namespace voreen

#endif // VRN_DOUBLESLIDER_H
