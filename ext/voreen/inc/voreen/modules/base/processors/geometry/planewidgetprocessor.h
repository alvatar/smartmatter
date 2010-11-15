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

#ifndef VRN_CLIPPINGPLANEWIDGET_H
#define VRN_CLIPPINGPLANEWIDGET_H

#include "voreen/core/processors/geometryrendererbase.h"
#include "voreen/core/properties/eventproperty.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/floatproperty.h"
#include "voreen/core/properties/vectorproperty.h"

#include "voreen/core/ports/volumeport.h"

#include <vector>

namespace voreen {

/**
 * Renders 3D control elements to manipulate axis-aligned clipping planes.
 *
 * Use linking to connect the properties to a CubeProxyGeometry.
 *
 * @see MeshClipping
 */
class PlaneWidgetProcessor : public GeometryRendererBase {
public:
    PlaneWidgetProcessor();
    ~PlaneWidgetProcessor();
    virtual Processor* create() const;

    virtual std::string getClassName() const    { return "PlaneWidgetProcessor"; }
    virtual std::string getCategory() const     { return "Geometry"; }
    virtual CodeState getCodeState() const      { return CODE_STATE_STABLE; }
    virtual std::string getProcessorInfo() const;

    ///Max values of the clipping properties are set here to prevent out of range warnings.
    virtual void invalidate(int inv = INVALID_RESULT);
    virtual void render();
    virtual void renderPicking();
    void setIDManager(IDManager* idm);

protected:
    virtual void initialize() throw (VoreenException);
    virtual void deinitialize() throw (VoreenException);

private:
    struct Manipulator {
        Manipulator(int axis, int corner, FloatProperty* prop, FloatProperty* oppositeProp, bool inverted) :
    axis_(axis),
        corner_(corner),
        prop_(prop),
        oppositeProp_(oppositeProp),
        inverted_(inverted)
    {
    }

    int axis_;                      //0=x, 1=y, 2=z
    int corner_;                    //0-3
    FloatProperty* prop_;           //The property this arrow is associated with
    FloatProperty* oppositeProp_;   //The opposite property (used for synced movement)
    bool inverted_;                 //invert the arrows?
    };

    void planeMovement(tgt::MouseEvent* e);
    void planeMovementSync(tgt::MouseEvent* e);

    tgt::vec3 getLlf();
    tgt::vec3 getUrb();

    /**
     * @brief Helper method to get position on the edges of cube.
     *
     * @param axis Along which axis? (x=0, y=1, z=2)
     * @param num Which of the 4 edges (0-3)
     * @param t At which position (0.0 - 1.0)
     *
     * @return The Point on the edge.
     */
    tgt::vec3 getCorner(int axis, int num, float t);

    void paintManipulator(const Manipulator& a, bool useSphereManipulator);
    void paintManipulator(tgt::vec3 translation, tgt::vec3 rotationAxis, float rotationAngle, float scaleFactor, bool useSphereManipulator);

    std::vector<Manipulator> manipulators_;
    int grabbed_;                       //-1 if no arrow is grabbed, <indexOfManipulator> otherwise
    bool syncMovement_;                 //true if modifier key was pressed when grabbing an arrow

    GLuint arrowDisplayList_;           ///< displaylist for rendering of an arrow
    GLuint sphereDisplayList_;          ///< displaylist for rendering of a sphere

    FloatProperty manipulatorScaling_;
    FloatVec4Property xColor_;
    FloatVec4Property yColor_;
    FloatVec4Property zColor_;
    BoolProperty clipEnabledLeftX_;
    BoolProperty clipUseSphereManipulatorLeftX_;
    FloatProperty clipLeftX_;
    BoolProperty clipEnabledRightX_;
    BoolProperty clipUseSphereManipulatorRightX_;
    FloatProperty clipRightX_;
    BoolProperty clipEnabledFrontY_;
    BoolProperty clipUseSphereManipulatorFrontY_;
    FloatProperty clipFrontY_;
    BoolProperty clipEnabledBackY_;
    BoolProperty clipUseSphereManipulatorBackY_;
    FloatProperty clipBackY_;
    BoolProperty clipEnabledBottomZ_;
    BoolProperty clipUseSphereManipulatorBottomZ_;
    FloatProperty clipBottomZ_;
    BoolProperty clipEnabledTopZ_;
    BoolProperty clipUseSphereManipulatorTopZ_;
    FloatProperty clipTopZ_;

    FloatProperty width_;

    BoolProperty showInnerBB_;
    FloatVec4Property innerColor_;

    FloatVec4Property lightPosition_;

    EventProperty<PlaneWidgetProcessor>* moveEventProp_;
    EventProperty<PlaneWidgetProcessor>* syncMoveEventProp_;

    VolumePort inport_;
};

} // namespace voreen

#endif // VRN_CLIPPINGPLANEWIDGET_H
