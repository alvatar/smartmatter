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

#ifndef CAMERAPLUGIN_H
#define CAMERAPLUGIN_H

#include "voreen/core/processors/processor.h"
#include "tgt/quaternion.h"

#include <QLabel>
#include <QComboBox>
#include <QBasicTimer>
#include <QPushButton>
#include <QMenu>
#include <QCheckBox>
#include <QToolButton>
#include <QPushButton>
#include <QGroupBox>

#include <fstream>

#include "voreen/core/properties/vectorproperty.h"

namespace voreen {

class VoreenTrackball;
class CameraProperty;
class FloatVec3PropertyWidget;

class CameraWidget : public QWidget {
    Q_OBJECT
public:
    CameraWidget(CameraProperty* camera, float minDist = 0.01f, float maxDist = 50.f, QWidget* parent = 0);
    ~CameraWidget();

    void updateFromCamera();

public slots:
    void toAbove();
    void toBelow();
    void toBehind();
    void toFront();
    void toLeft();
    void toRight();
    void updateDistance();
    void orientationChanged(int);
    void positionChange(FloatVec3Property::ElemType);
    void focusChange(FloatVec3Property::ElemType);
    void upChange(FloatVec3Property::ElemType);
    void distanceSliderChanged(int);
    void distanceSliderPressed();
    void distanceSliderReleased();

    void saveCameraToDisk();
    void saveCameraToDisk(std::string s);
    void restoreCamera();
    void restoreCamera(std::string s);

    void enableContSpin(bool b);

    void enableX(bool b);
    void enableY(bool b);
    void enableZ(bool b);

    virtual void createWidgets();
    virtual void createConnections();

private:

    /**
     * Helper class ensuring freeing used resources in case an excpetion is thrown.
     */
    class SerializationResource {
    public:
        /**
         * Creates a @c SerializationResource.
         *
         * @param serializer the serializer or deserializer
         * @param filename the filename
         * @param openMode the open mode
         */
        SerializationResource(XmlSerializerBase* serializer, const std::string& filename, const std::ios_base::openmode& openMode);

        /**
         * Frees used resources.
         */
        ~SerializationResource();

        /**
         * Returns the file stream of this resource.
         *
         * @return the file stream
         */
        std::fstream& getStream();

    private:
        /**
         * File stream for serialization process.
         */
        std::fstream stream_;
    };

    void checkCameraState();

    void applyOrientation(const tgt::quat& q);
    void applyOrientationAndDistanceAnimated(std::vector<float> keyframe);

    void resetCameraPosition();

    /// enabled or disables timer based on whether rotation is active
    void setTimerState();
    virtual void timerEvent(QTimerEvent* event);
    void showEvent(QShowEvent* event);

    CameraProperty* cameraProp_;
    VoreenTrackball* track_;

    int minDist_;
    int maxDist_;
    const float CAM_DIST_SCALE_FACTOR;

    const tgt::vec3 AXIAL_VIEW;
    const tgt::vec3 CORONAL_VIEW;
    const tgt::vec3 SAGITTAL_VIEW;
    const tgt::vec3 AXIAL_INV_VIEW;
    const tgt::vec3 CORONAL_INV_VIEW;
    const tgt::vec3 SAGITTAL_INV_VIEW;
    FloatVec3Property* cameraPosition_;
    FloatVec3Property* focusVector_;
    FloatVec3Property* upVector_;

    QGroupBox* orientationBox_;
    QGroupBox* motionBox_;
    QGroupBox* trackballBox_;
    QComboBox* comboOrientation_;
    QSlider* slDistance_;

    QCheckBox* rotateAroundX_;
    QCheckBox* rotateAroundY_;
    QCheckBox* rotateAroundZ_;
    QCheckBox* continueSpin_;
    QBasicTimer* timer_;

    float dist_;
    bool rotateX_;
    bool rotateY_;
    bool rotateZ_;

    QToolButton* buSaveTrackball_;
    QToolButton* buRestoreTrackball_;
};

} // namespace voreen

#endif
