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

#ifndef VRN_EXPLOSIONPROXYGEOMETRY_H
#define VRN_EXPLOSIONPROXYGEOMETRY_H

#include "voreen/core/processors/renderprocessor.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/interaction/idmanager.h"
#include "voreen/core/properties/eventproperty.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/vectorproperty.h"
#include "voreen/core/properties/cameraproperty.h"

#include "voreen/core/datastructures/geometry/meshlistgeometry.h"
#include <stack>

namespace voreen {

/**
 * Provides a ray casting proxy geometry that can be sectioned into bricks for exploded views.
 * The processor's loop port has to be connected to a ExplosionCompositor.
 *
 * Three bricking modes are offered:
 *
 * Regular Grid:
 * In this mode the proxy geometry of the volume can be split into a regular grid by choosing the number
 * of bricks on each axis. The bricks can "explode" by using the gap property of this mode.
 *
 * Manual Grid:
 * In this mode the proxy geometry of the volume can be split into a manual grid by manually adding
 * axis-aligned planes as boundaries for the bricks. As in regularGrid mode bricks can "explode" by using
 * the gap property of this mode.
 *
 * Custom:
 * In addition to manualGrid mode one or several bricks can be selected in this mode, so that added
 * axis-aligned planes only split the selected bricks and not the whole volume.
 * The "explosion" of the bricks no longer depends on a gap-value. Instead each brick can be translated everywhere
 * by using the translation property or the drag-and-drop translation mouse event of this mode.
 * Furthermore bricks can be hidden in this mode.
 *
 * The created proxy geometries of regular grid or manual grid mode can be used for custom mode by
 * using the "Use this for custom mode" property.
 *
 * @see ExplosionCompositor
 * @see MeshEntryExitPoints
 */
class ExplosionProxyGeometry : public RenderProcessor {

public:
    ExplosionProxyGeometry();
    virtual ~ExplosionProxyGeometry();
    virtual Processor* create() const;

    virtual std::string getClassName() const    { return "ExplosionProxyGeometry"; }
    virtual std::string getCategory() const     { return "Volume Proxy Geometry";  }
    virtual CodeState getCodeState() const      { return CODE_STATE_STABLE;        }

    virtual std::string getProcessorInfo() const;
    virtual bool isReady() const;

protected:
    virtual void beforeProcess();
    virtual void process();
    virtual void initialize() throw (VoreenException);
    virtual void deinitialize() throw (VoreenException);

    /**
     * Struct Brick.
     */
    struct Brick
    {
        tgt::vec3 coordLlf;
        tgt::vec3 coordUrb;
        tgt::vec3 texLlf;
        tgt::vec3 texUrb;
        tgt::ivec3 sliceIndexLlf;
        tgt::ivec3 sliceIndexUrb;
        tgt::vec3 translation;
        int brickId;
        bool selected;
        bool hidden;
    };


    /**
     * Sets visible required properties for the selected bricking mode
     * and updates plane-widgets.
     */
    virtual void onBrickingModeChange();

    /**
     * If selected is true:
     * Sets the hidden value of all selected bricks to true, so that they
     * will be hidden. (After this the hidden bricks are no longer selected.
     * Instead all other bricks are selected.)
     * If selected is false:
     * Sets the hidden value of all non-selected bricks to true, so that they
     * will be hidden.
     */
    virtual void onHideBricksChange(bool selected);

    /**
     * Sets the hidden value of all bricks to false, so that all bricks
     * will be shown again.
     */
    virtual void onShowAllBricksChange();

    /**
     * For regularGrid and manualGrid bricking mode:
     * Initiates createBricklist-process and sets (new) number of loop iterations
     * For custom mode:
     * Initiates createBricklist-process when customBricklist_ is empty.
     * Sets number of loop iterations to the size of customBricklist_.
     */
    virtual void doBricking();

    /**
     * Initiates addBrickingPlane-process for the over handed axis
     * @param axis
     */
    virtual void onAddBrickingPlaneChange(char axis);

    /**
     * For manualGrid bricking mode:
     * Calculates values (offset and tex) from the over handed axis's BrickingPlane-property value
     * and adds these to axis's-bricking lists (e.g. xPlanescoord_ and xPlanestex_).
     * Then bricks the volume in consideration of the new added brickingplane.
     * (with doBricking-process)
     * For custom mode:
     * Calculates values (offset and tex) from the over handed axis's BrickingPlane-property value
     * and initiates createCustomBricklist-process with these values
     * @param axis
     * @param *numBricks
     * @param *planescoord
     * @param *planestex
     * @param *coords
     */
    virtual void addBrickingPlane(char axis, size_t *numBricks, std::vector<float> *planescoord, std::vector<float> *planestex, std::stack<float> *coords);

    /**
     * Initiates deletLastBrickingPlane-process for the over handed axis
     * @param axis
     */
    virtual void onDeleteLastBrickingPlaneChange(char axis);

    /**
     * Deletes the last added brickingplane from the bricking lists (e.g. xPlanescoord_ and xPlanestex_)
     * of the over handed axis and bricks the volume in consideration of the deleted brickingplane.
     * (with doBricking-process)
     * @param *numBricks
     * @param *planescoord
     * @param *planestex
     * @param *coords
     */
    virtual void deleteLastBrickingPlane(size_t *numBricks, std::vector<float> *planescoord, std::vector<float> *planestex, std::stack<float> *coords);

    /**
     * Initiates clearBrickingList-process for the over handed axis
     * and then initiates doBricking-process
     * @param axis
     */
    virtual void onClearBrickingListChange(char axis);

    /**
     * Clears the bricking lists (e.g. xPlanescoord_ and xPlanestex_) and the coords-Stack
     * of the over handed axis
     * @param axis
     */
    virtual void clearBrickingList(char axis);

    /**
     * Initiates the createRegularOrManualBricklist-process where the bricklists for
     * regularGrid or manualGrid bicking mode are created in consideration of the new gap
     */
    virtual void onExplosionGapChange();

    /**
     * Saves the value of the translation_ property as new translation value
     * for the selected bricks in custom bricking mode
     */
    virtual void onTranslationChange();

    /**
     * Resets custom bricking mode, takes the current bricklist (e.g. regularBricklist_)
     * from the current bricking mode (e.g. regularGrid bricking mode) as new customBricklist_
     * and then changes bricking mode to custom
     */
    virtual void takeForCustom();

    /**
     * Takes the topmost bricklist from the customBricklistStack as new customBricklist_
     * (and deletes it from the Stack)
     */
    virtual void undo();

    /**
     * Resets the current bricking mode
     */
    virtual void resetMode();

    /**
     * Creates the bricklist for regularGrid (regularBricklist_) or
     * manualGrid (manualBricklist_) bricking mode
     * @param numIterations
     */
    virtual void createRegularOrManualBricklist(int numIterations);

    /**
     * Creates the bricklist for custom bricking mode
     * (customBricklist_) and registers it for selecting (registerForSelecting-process)
     * @param addPlaneAxis
     * @param addPlaneCoordOffset
     * @param addPlaneTex
     */
    virtual void createCustomBricklist(char addPlaneAxis, float addPlaneCoordOffset, float addPlaneTex);

    /**
     * Sorts the over handed bricklist (e.g. regularBricklist_) by
     * the distances of the bricks towards the camera
     * @param unsortedBricklist
     * @return sortedBricklist
     */
    std::vector<Brick> sortBricklist(std::vector<Brick> unsortedBricklist);

    /**
     * Searches the middle of the bricks from the over handed bricklist
     * (if the bricklist only contains one brick, this method returns the middle of this brick)
     * @param bricklist
     * @return middle
     */
    tgt::vec3 findMiddle(std::vector<Brick> bricklist);

    /**
     * Calculates distance between cameraposition and brickmiddle
     * @param cameraposition
     * @param brickmiddle
     * @return distance
     */
    float getCameraDistance(tgt::vec3 cameraposition, tgt::vec3 brickmiddle);

    /**
     * Returns the sliceWidth of the passed volume
     * @param Volume*
     * @return sliceWidth
     */
    tgt::vec3 getSliceWidth(Volume*);

    /**
     * Updates the min-, max- and current values of the plane-widgets (e.g. XbrickingPlane_)
     * in consideration of the bricking mode (and selected-values)
     */
    virtual void updatePlaneWidgets();

    /**
     * Searches the minimum sliceIndex for the over handed axis
     * from the over handed bricklist
     * @param bricklist
     * @param axis
     * @return minSliceIndex
     */
    int findMinSlice(std::vector<Brick> bricklist, char axis);

    /**
     * Searches the maximum sliceIndex for the over handed axis
     * from the over handed bricklist
     * @param bricklist
     * @param axis
     * @return maxSliceIndex
     */
    int findMaxSlice(std::vector<Brick> bricklist, char axis);

    /**
     * Registers the bricks of the over handed bricklist in the idManager.
     * This is necessary for the selecting events.
     * @param bricklist
     */
    virtual void registerForSelecting(std::vector<Brick> bricklist);

    /**
     * Processes the mouse event for selecting just one brick.
     * If a brick is hit by the mouse pointer, this brick will become the
     * only selected brick and all other currently selected bricks will
     * become non-selected.
     * If no brick is hit by the mouse pointer, all currently selected bricks will
     * become non-selected, so that there is no selected brick anymore.
     */
    virtual void onSelectingOneBrickEvent(tgt::MouseEvent *);

    /**
     * Processes the mouse event for selecting several brick.
     * If a non-selected brick is hit by the mouse pointer, this brick will become selected.
     * If a selected brick is hit by the mouse pointer, this brick will become non-selected.
     * If only one brick is selected and this brick is hit by the mouse pointer,
     * all bricks will become selected.
     * If no brick is hit by the mouse pointer, all currently selected bricks will
     * become non-selected, so that there is no selected brick anymore.
     */
    virtual void onSelectingSeveralBricksEvent(tgt::MouseEvent *);

    /**
     * Processes the mouse event for translating selected bricks.
     * If a non-selected brick is hit by the mouse pointer, nothing happens.
     * If a selected brick is hit by the mouse pointer, all selected bricks
     * will "follow" the mouse pointer.
     * The particular translation for each selected brick will be saved
     * as translation-value of these bricks
     */
    virtual void onTranslateSelectedBricksEvent(tgt::MouseEvent *);

    /**
     * Bricking proxy geometry.
     */
    MeshListGeometry* proxyGeometry_;
    MeshListGeometry* renderGeometry_;

    /**
     * Bricklists for the different bricking modes.
     */
    std::vector<Brick> regularBricklist_;
    std::vector<Brick> manualBricklist_;
    std::vector<Brick> customBricklist_;

    /**
     * Sorted bricklist to be rendered (e.g. sorted regularBricklist_).
     */
    std::vector<Brick> outputBricklist_;

    /**
     * Number of manual added bricks on X-, Y- and Z-axis.
     */
    size_t numBricksXm_;
    size_t numBricksYm_;
    size_t numBricksZm_;

    /**
     * X-, Y- and Z-bricking lists:
     *  {axis}Planescoord_ safes offset values between coordLlf.{axis} value of the
     *  original volume and the added {axis}-bricking plane values
     *
     *  {axis}Planestex_ safes the appropriate tex values
     */
    std::vector<float> xPlanescoord_;
    std::vector<float> xPlanestex_;
    std::vector<float> yPlanescoord_;
    std::vector<float> yPlanestex_;
    std::vector<float> zPlanescoord_;
    std::vector<float> zPlanestex_;

    /**
     * Stacks needed for deleteLast{Axis}brickingPlane operation.
     */
    std::stack<float> xCoords_;
    std::stack<float> yCoords_;
    std::stack<float> zCoords_;

    /**
     * Bricklist for selected bricks.
     */
    std::vector<Brick> selectedBricks_;

    /**
     * Bricklist-stack needed for the undo operation in custom bricking mode.
     */
    std::stack<std::vector<Brick> > customBricklistStack_;

    /**
     * IDManager needed for the selecting process of the bricks
    */
    IDManager idManager_;

    /**
     * Boolean needed for TranslateSelectedBricks-event
    */
    bool isClicked_;

    /**
     * Start-coordinates of the mouse pointer needed for TranslateSelectedBricks-event
    */
    tgt::ivec2 startCoord_;

    /**
     * Start-position of selected bricks (middle of theses bricks)
     * needed for TranslateSelectedBricks-event
    */
    tgt::vec3 startPos_;


    // Properties:

    /**
     * To be linked with the canvas' property,
     * necessary for adjusting the picking buffer's size.
     */
    IntVec2Property canvasSize_;

    /**
     * Bricking mode property.
     */
    StringOptionProperty brickingMode_;

    /**
     * Show bounding-boxes of the bricks property.
     */
    StringOptionProperty showBoundingBoxes_;

    /**
     * Color of the bounding-boxes of the non-selected bricks property.
     */
    FloatVec4Property brickColor_;

    /**
     * Color of the bounding-boxes of the selected bricks property.
     */
    FloatVec4Property selectedBrickColor_;

    /**
     * Hide non-selected bricks property.
     */
    ButtonProperty hideNonSelectedBricks_;

    /**
     * Hide selected bricks property.
     */
    ButtonProperty hideSelectedBricks_;

    /**
     * Show all bricks property.
     */
    ButtonProperty showAllBricks_;

    /**
     * Number of regularGrid bricks on X-axis property.
     */
    IntProperty numBricksXp_;

    /**
     * Number of regularGrid bricks on Y-axis property.
     */
    IntProperty numBricksYp_;

    /**
     * Number of regularGrid bricks on Z-axis property.
     */
    IntProperty numBricksZp_;

    /**
     * ExplosionGap between bricks property.
     */
    FloatProperty regularExplosionGap_;
    FloatProperty manualExplosionGap_;

    /**
     * X brickingplane properties.
     */
    IntProperty XbrickingPlane_;
    ButtonProperty addXbrickingPlane_;
    ButtonProperty deleteLastXbrickingPlane_;
    ButtonProperty clearXbrickingList_;

    /**
     * Y brickingplane properties.
     */
    IntProperty YbrickingPlane_;
    ButtonProperty addYbrickingPlane_;
    ButtonProperty deleteLastYbrickingPlane_;
    ButtonProperty clearYbrickingList_;

    /**
     * Z brickingplane properties.
     */
    IntProperty ZbrickingPlane_;
    ButtonProperty addZbrickingPlane_;
    ButtonProperty deleteLastZbrickingPlane_;
    ButtonProperty clearZbrickingList_;

    /**
     * Reset current bricking mode property.
     */
    ButtonProperty resetMode_;

    /**
     * Undo last performed action property for custom mode.
     */
    ButtonProperty undo_;

    /**
     * Take current bricklist for custom mode property.
     */
    ButtonProperty takeForCustom_;

    /**
     * Translation property.
     */
    FloatVec3Property translation_;

    /**
     * Camera property.
     */
    CameraProperty camera_;

    /**
     * Mouse Event property for selecting one brick.
     */
    EventProperty<ExplosionProxyGeometry> selectingOneBrickEvent_;

    /**
     * Mouse Event property for selecting several bricks.
     */
    EventProperty<ExplosionProxyGeometry> selectingSeveralBricksEvent_;

    /**
     * Mouse Event property for translating selected bricks.
     */
    EventProperty<ExplosionProxyGeometry> translateSelectedBricksEvent_;


    /// Inport for the dataset to be bricked.
    VolumePort inportVolume_;

    /// To be connected to the corresponding ExplosionCompositor's loop port.
    Port loopInport_;

    /// Outport for the generated mesh proxy geometry.
    GeometryPort outportProxyGeometry_;

    /// Outport providing bricks to be directly rendered.
    GeometryPort outportRenderGeometry_;

    /// Picking buffer, is added as private render port.
    RenderPort pickingBuffer_;

private:
    /// Change listener of the canvasSize_ property. Calls RenderProcessor::portResized
    void canvasSizeChanged();
};

} // namespace

#endif // VRN_EXPLOSIONPROXYGEOMETRY_H
