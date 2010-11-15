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

#ifndef VRN_TRANSFUNCEDITORINTENSITYGRADIENT_H
#define VRN_TRANSFUNCEDITORINTENSITYGRADIENT_H

#include "voreen/qt/widgets/transfunc/transfunceditor.h"

namespace tgt {
    class QtCanvas;
}

class QCheckBox;
class QLayout;
class QSlider;
class QToolButton;

namespace voreen {

class TransFuncIntensityGradientPainter;
class TransFuncIntensityGradient;
class Volume;

/**
 * TransFuncEditorIntensityGradient is an editor widget for 2D transfer functions.
 * It provides a canvas where the transfer function can be edited and the histogram
 * of the rendered dataset is shown. Furthermore it provides several control elements
 * to adjust the appearance of the histogram and the primitives.
 */
class TransFuncEditorIntensityGradient : public TransFuncEditor {
    Q_OBJECT
public:
    /**
     * Constructor
     *
     * @param prop the transfer function property
     * @param parent the parent widget
     * @param orientation orientation of the widget, e.g. vertical or horizontal layout for the buttons
     */
    TransFuncEditorIntensityGradient(TransFuncProperty* prop, QWidget* parent = 0, Qt::Orientation orientation = Qt::Horizontal);

    /**
     * Destructor
     */
    ~TransFuncEditorIntensityGradient();

    /**
     * Creates the whole layout for this widget.
     */
    void createWidgets();

    /**
     * Creates the connections for all control elements.
     */
    void createConnections();

    /**
     * Tests whether the volume associated with the transFuncProperty has changed and updates
     * the editor to the new volume when necessary.
     */
    void updateFromProperty();

    /**
     * Propagates the new volume to the painter for the mapping canvas and updates the control
     * elements for the histogram.
     */
    void volumeChanged();

    /**
     * Tests whether the value inside the transFuncProp is the same as transFuncGradient_.
     * A new transfer function object is created if they do not match. The transfer function
     * is reset if they match. Furthermore a repaint for the volume rendering is triggered.
     */
    void resetEditor();

public slots:
    /**
     * Opens a filedialog and loads the selected transfer function.
     */
    void loadTransferFunction();

    /**
     * Opens a filedialog and saves the current transfer function to the desired file.
     */
    void saveTransferFunction();

    /**
     * Enables or disables the grid in the background.
     */
    void toggleShowGrid();

    /**
     * Enables or disables the display of the histogram texture.
     */
    void toggleShowHistogram();

    /**
     * Slot that is called when a primitive was selected. It enables the transparency
     * and fuzzines slider.
     */
    void primitiveSelected();

    /**
     * Slot that is called when a primitive was deselected. The transparency and fuzzines
     * slider are disabled.
     */
    void primitiveDeselected();

    /**
     * Sets the transparency slider to given value.
     *
     * @param trans new value for transparency slider
     */
    void setTransparency(int trans);

    /**
     * Activates the coarseness mode.
     */
    void startTracking();

    /**
     * Deactivates the coarseness mode.
     */
    void stopTracking();

    /**
     * Informs the transfer function property about a change in transfer function
     * and emits transferFunctionChanged() signal.
     */
    void repaintSignal();

private:

    /**
     * Creates the layout with mapping canvas.
     *
     * @return the layout with mapping canvas
     */
    QLayout* createMappingLayout();

    /**
     * Creates the layout with all buttons.
     *
     * @return the layout with all buttons
     */
    QLayout* createButtonLayout();

    /**
     * Creates the layout with transparency and fuzzines slider and control elements for the histogram.
     *
     * @return layout with 3 sliders and a checkbox
     */
    QLayout* createSliderLayout();

    TransFuncIntensityGradient* transFuncGradient_; ///< the 2D transfer function
    tgt::QtCanvas* transCanvas_;                    ///< canvas in which the tf can be edited
    TransFuncIntensityGradientPainter* painter_;    ///< painter for the canvas

    QToolButton* loadButton_;             ///< button for loading of a 2D tf
    QToolButton* saveButton_;             ///< button for saving of a 2D tf
    QToolButton* clearButton_;            ///< button for resetting of the tf
    QToolButton* gridEnabledButton_;      ///< button for enabling and disabling the grid
    QToolButton* histogramEnabledButton_; ///< button for enabling and disabling the histogram
    QToolButton* quadButton_;             ///< button for adding a quad primitive
    QToolButton* bananaButton_;           ///< button for adding a banana primitive
    QToolButton* deleteButton_;           ///< button for deletion of current selected primitive
    QToolButton* colorButton_;            ///< button for opening of a colordialog for the current selected primitive
    QCheckBox* histogramLog_;             ///< checkbox for enabling and disabling the logarithmic display of the histogram
    QSlider* histogramBrightness_;        ///< slider for adjusting the brightness of the histogram
    QSlider* fuzziness_;                  ///< slider for adjusting the fuzzines of a primitive
    QSlider* transparency_;               ///< slider for adjusting the transparency of a primitive
    Qt::Orientation orientation_;         ///< orientation of the widget

    int maximumIntensity_;                ///< maximum intensity that can occur in the current rendered volume
    bool supported_;                      ///< indicates whether 2D tf's are supported by current graphics hardware

    static const std::string loggerCat_;  ///< the logger category
};

} // namespace voreen

#endif // VRN_TRANSFUNCEDITORINTENSITYGRADIENT_H
