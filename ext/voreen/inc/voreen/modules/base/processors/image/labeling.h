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

#ifndef VRN_LABELING_H
#define VRN_LABELING_H

#include "voreen/modules/base/processors/image/labelingmath.h"

#include "voreen/modules/base/processors/proxygeometry/proxygeometry.h"
#include "voreen/core/datastructures/transfunc/transfunc.h"
#include "voreen/core/processors/processor.h"
#include "voreen/core/processors/imageprocessor.h"
#include "voreen/core/interaction/idmanager.h"
#include "voreen/core/properties/intproperty.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/cameraproperty.h"
#include "voreen/core/properties/vectorproperty.h"
#include "tgt/stopwatch.h"
#include "tinyxml/tinyxml.h"
#include "tgt/event/eventlistener.h"

#ifdef VRN_WITH_FONTRENDERING
#include <FTGL/ftgl.h>
#include "tgt/font.h"
#endif

namespace voreen {

class CameraProperty;

/**
 * Abstraction layer for GUI access. This class has to be implemented by the
 * used GUI library (eg. voreenqt).
 */
class LabelingWidget {
public:
    virtual ~LabelingWidget() {}

    /**
     * Opens an input dialog and returns the text entered by the user.
     *
     * @param title the title of the input box
     * @param label the message of the input box
     * @param text the text to be initially displayed in the text field
     */
    virtual std::string showInputDialog(const std::string &title,
        const std::string &label,
        const std::string &text)=0;
};


//---------------------------------------------------------------------------

/**
 * Performs an interactive illustration of the volume rendering.
 * An object of this class takes the id-raycasting result
 * and detects anchor points for all segments contained by the id map.
 * Based on these anchor points a hybrid labeling layout is calculated.
 * Labels are either placed:
 * - externally around the convex hull of the volume rendering
 * - internally on the corresponding segment
 *
 * also \see IDRaycaster
 */
class Labeling : public ImageProcessor {
public:

    /**
     * Default constructor.
     */
    Labeling();

    /**
     * Destructor. Frees all allocated resources.
     */
    ~Labeling();

    virtual std::string getCategory() const { return "Image Processing"; }
    virtual std::string getClassName() const { return "Labeling"; }
    virtual Processor::CodeState getCodeState() const { return CODE_STATE_TESTING; }
    virtual std::string getProcessorInfo() const;
    virtual Processor* create() const {return new Labeling();}

    /// The external label layout.
    enum LabelLayouts {
        SILHOUETTE,        ///< labels are placed around the object's convex hull
        LEFTRIGHT,        ///< labels are stacked on the left/right
    };

    /// Determines which labels are rendered.
    enum showModes {
        SHOW_ALL,                ///< show all labels
        SHOW_NONE,                ///< show no label
        SHOW_EXTERNAL_ONLY,        ///< show external labels only
        SHOW_INTERNAL_ONLY        ///< show internal labels only
    };

    static const std::string setLabelingWidget_;
    static const std::string setLayout_;
    static const std::string setLabelColor_;
    static const std::string setHaloColor_;
    static const std::string setFont_;
    static const std::string setFontSize_;
    static const std::string setSegmentDescriptionFile_;
    static const std::string switchDrawHalo_;

protected:
    virtual void process();
    void initialize() throw (VoreenException);
    void deinitialize() throw (VoreenException);

private:
    // Object for GUI access
    LabelingWidget* labelingWidget_;

    // Proxy geometry used by the volume renderers.
    ProxyGeometry* pg_;
    // Shader used for labeling.
    tgt::Shader* labelShader_;

    //
    // Properties
    //
    StringOptionProperty showLabels_;           // determines whether/which labels are shown
    FileDialogProperty labelFile_;              // Path to xml file containing the segment labels.
    FloatVec4Property labelColorExtern_;            // color of external labels
    FloatVec4Property haloColorExtern_;             // halo-color of external labels
    IntProperty fontSizeExtern_;                // font size of internal labels
    BoolProperty lockInternalFontSettings_;     // if true, internal/external font settings are synchronized
    FloatVec4Property labelColorIntern_;            // color of internal labels
    FloatVec4Property haloColorIntern_;             // halo-color of internal labels
    IntProperty fontSizeIntern_;                // font size of internal labels
    BoolProperty shape3D_;                      // 3d shape fitting of internal labels
    BoolProperty drawHalo_;                     // if true, a halo is drawn around labels
    StringOptionProperty layout_;               // silhouette or left-right-layout
    IntProperty minSegmentSize_;                // minimum segment of a segment on screen for being labeled
    IntProperty maxIterations_;                 // max count of iterations for improving label layout
    IntOptionProperty filterKernel_;            // filter kernel to be used for distance map filtering
    IntProperty filterDelta_;                   // offset of this kernel
    IntProperty distanceMapStep_;               // only each distanceMapStep_ pixel of distance map is considered
    IntProperty glyphAdvance_;                  // Additional gap between two glyphs.
    IntProperty polynomialDegree_;              // degree of 2D and 3D polynomials
    IntProperty bezierHorzDegree_;              // horizontal degree of bezier patch
    IntProperty bezierVertDegree_;              // vertical degree of bezier patch

    static const std::string labelLayoutAsString_;
    std::string fontPath_;


    // posesses a segment's text properties
    struct LabelText {
        std::string text;
        int width;                 // width, height of text bounding box
        int height;                // of external label (pixel coords)
        int widthIntern;           // width, height of text bounding box
        int heightIntern;          // of internal label (pixel coords)
        float widthWorld;          // width, height of text bounding box
        float heightWorld;         // of exernal label (world coords)
        float widthInternWorld;    // width, height of text bounding box
        float heightInternWorld;   // of internal label (world coords)
        GLuint textureExtern;      // texture containing the rendered text for extern labels
        GLuint textureIntern;      /// texture containing the rendered text for intern labels
        int textureExternWidth;    // width of external texture in pixels
        int textureExternHeight;   // height of external texture in pixels
        int textureInternWidth;    // width of internal texture in pixels
        int textureInternHeight;   // height of internal texture in pixels
        GLenum textureTargetType;
    };

    // result / properties of id-raycasting
    struct Image {
        labeling::Bitmap<int> idImage;             // blue channel of id-raycasting result (complete object)
        labeling::Bitmap<int> idBuffer;            // color-coded segments, one color channel per pixel
       // GLfloat *depthField;                     // depth values created by ID-raycaster
        labeling::FirstHitBitmap firstHitPositions; // first hit positions generated by ID-raycaster
        labeling::Bitmap<int> distanceField;       // pixel distances to segment boundary
        labeling::Bitmap<int> horzDistance;        // horizontal distance map
        labeling::Bitmap<int> vertDistance;        // vertical distance map
        labeling::Bitmap<int> ascDiagDistance;     // ascending diagonal distance map
        labeling::Bitmap<int> descDiagDistance;    // descending diagonal distance map
        labeling::Bitmap<int> convexHull;   // 3 color channels: ch 1 hull, ch 2+3 hull pixel's normal
        std::vector<tgt::ivec2> silPoints;  // points of the silhouette
        std::vector<tgt::ivec2> hullPoints; // points of the convex hull
        int width;                          // width of all bitmaps in this struct
        int height;                         // height of all bitmaps in this struct
        tgt::ivec2 bl;                      // bottom-left of silhouette
        tgt::ivec2 tr;                      // top-right of silhouette
    } image_;

    // contains segment's information read from xml-file
    // Is mainly constant over frames.
    struct LabelData {
        int id;                          // segment's id
        std::string idstr;               // segment's id as string
        LabelText text;                  // name of the segment
        bool belongsToSegment;
        TiXmlNode* xmlNode;              // node in the XML-DOM representing this segment
        tgt::vec3 anchorPoint3D;
        labeling::Curve3DPolynomial* curve3D;
        bool internPreferred;            // determines if internal label position is preferred
    };
    std::vector<LabelData*> labelPersistentData_;

    // Properties of one label.
    // Are regenerated each frame!
    struct Label {
        tgt::ivec2 anchorPoint;          // anchor point in image pixel coords
        tgt::vec2  anchorPointWorld;     //  ""       "" in world coords
        tgt::ivec2 connectionPoint;      // connection point (end point of line) in image pixel coords
        tgt::vec2  connectionPointWorld; //  ""       ""          ""             in world coords
        tgt::ivec2 normal;               // normal at intersection of connection line with convex hull
        tgt::ivec2 labelPos;             // bottom-left of label-text bounding box in image pixel coords
        tgt::vec2  labelPosWorld;        //  ""       ""          ""               in worlds coords
        float rotAngle;                  // an external label quad is rotated by this angle
        LabelData* labelData;            // persistent label properties
        int segmentSize;                 // count of visible segment-pixels
        bool intern;                     // is label an internal one?
        bool offLabel;                   // an Off-Label is an unsegmented label which has not been attached to
                                         // to the volume. Off-Labels are placed in the upper-right window corner
        std::vector<tgt::ivec2> controlPoints;
        labeling::Curve2DPolynomial* curve2D;        // fitting function in image space (is fit to control points)
        labeling::BezierPatch bezierPatch;           // bezier patch the intern texture is mapped onto
    };
    typedef std::vector<Label> LabelVec;
    LabelVec labels_;                   // contains all labels of current frame

    // offsets (in pixels)
    struct Gaps {
        // General gaps
        static const int LabelConnpoint = 2;    // gap between label and its connection point
        static const int LineLine = 10;         // gap between two connection lines

        // Gaps for LeftRight-Layout
        static const int LR_LabelLabel = 6;       // gap between two labels in left-right layout
        static const int LR_BboxObject = 20;    // gap between bounding box of object and labels

        // Gaps for silhouette-Layout
        static const int SIL_LabelLabel = 14;   // gap between two labels in silhouette layout
        static const int SIL_LabelHull = 16;    // gap between text and hull in silhouette layout
    } gaps_;

    // adjustments of a label regarding to connectionpoint
    enum Adjustments {
        NONE = 20,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum MidPointLineActions{
        HullIntersect = 30,
        DrawHull
    };

    // circle for finding label path of internal labels
    struct CirclePixel {
        tgt::ivec2 coords;
        float angle;
    };
    struct Circle {
        float radius;
        std::vector<CirclePixel> pixels;
        int numPixels;
    };
    Circle circle_;

    // filter kernel used to filter distance map
    struct FilterKernel {
        FilterKernel(int s, std::string cap) : size(s), caption(cap) {
            kernel = new int[s*s];
            coefficientsSum = s*s;
            for (int i=0; i<(s*s); i++)
                kernel[i] = 1;
        }
        ~FilterKernel() {
            delete[] kernel;
        }
        int size;
        int coefficientsSum;
        int* kernel;
        std::string caption;
    };
    std::vector<FilterKernel*> kernels_;

#ifdef VRN_WITH_FONTRENDERING
    FT_Library library_;
    FT_Face face_;
#endif

    bool valid_;        // is current label rendering valid? (used for user interactions)
    bool editMode_;     // indicates that label-layout should not be recalculated
                        // e.g. because a label is edited or moved by the user
    bool texturesGenerated_; // have the label textures been generated?
    TiXmlDocument xmlDoc_;  // tiny xml handle for xml-file containing segment captions
    static const std::string loggerCat_;

    RenderPort idMapPort_;

#ifdef labelDEBUG
    GLuint segmentationTarget_;
    GLuint distanceMapTarget_;
    BoolProperty showSegmentIDs_;
    BoolProperty drawConvexHull_;
    BoolProperty drawDebugMaps_;
    std::vector<tgt::vec2> renderPoints_;
#endif

    // Labeling-Methods
    void labelLayout();
    void genTextures();
    void renderTextToBitmap(std::string text, int fontSize, const FloatVec4Property& labelColor, const FloatVec4Property& haloColor,
                            labeling::Bitmap<GLfloat> &bitmap,
                            bool antialias, int border, int glyphAdvance,
                            bool drawHalo, int haloOffset );
    void findAnchors();
    void calcLabelPositions();
    void toWorld(Label* pLabel = NULL);
    void renderLabels();
    void deleteLabels();

    // common support methods
    void connPointFromLabelPos(Label const &pLabel, tgt::ivec2 &connPoint, tgt::ivec2 &normal, bool calcNormal=true);
    void labelPosFromConnPoint(Label const &pLabel, tgt::ivec2 &labelPos);
    bool resolveLineIntersections(LabelVec &pLabels);
    void lineIntersection(tgt::ivec2 const P1, tgt::ivec2 const Q1,
                          tgt::ivec2 const P2, tgt::ivec2 const Q2,
                          tgt::ivec2 &intersection, bool &onSegments);
    void drawHalo(labeling::Bitmap<GLfloat> &bitmap, int width, int height, tgt::Color haloColor, GLfloat alphaTreshold);

    // support-methods left-right
    void stackLabels(LabelVec &pLabels, int min, int max, int &bottom, int &top);

    // support-methods silhouette
    bool midPointLine(tgt::ivec2 const &start, const tgt::ivec2 &end, MidPointLineActions action,
        tgt::ivec2 &intersection, tgt::ivec2 &normal);
    bool resolveLabelOverlaps(LabelVec &pLabels);
    void correctHullGap(Label pLabel, tgt::ivec2 &labelPos);
    bool intersectBoxHull(tgt::ivec2 const &bl, tgt::ivec2 const &tr);
    bool intersectBoxBox(tgt::ivec2 const &bl1, tgt::ivec2 const &tr1,
        tgt::ivec2 const &bl2, tgt::ivec2 const &tr2, int &mask);
    bool pointInBox(tgt::ivec2 const &bl, tgt::ivec2 const &tr, tgt::ivec2 const &point);

    // internal label path detection
    Circle generateCircle(float radius);
    CirclePixel getNextCirclePixel(int id, Circle circle, tgt::ivec2 curPoint,
                                   float minAngle, float maxAngle);
    bool findLabelPathOne(const Label &pLabel, float initAngle,  float step, float initMaxStepAngle,
                                        float maxStepAngle, float maxAngle, float length,
                                        std::vector<tgt::ivec2> &result);
    bool findLabelPathBest(Label &pLabel);
    bool findBezierPoints(Label &pLabel);
    bool checkPatchQuality(Label pLabel);

    // unsegmented labels
    void appendUnsegmentedLabels();
    void placeOffLabels(LabelVec &offLabels);
    void addUnsegmentedLabelData(std::string text);
    void removeUnsegmentedLabelData(std::string text);
    void rotateLabel(Label &pLabel, float angle);

    // image-methods
    void readImage();
    void calcConvexHull();

    // misc
    void loadFont();
    void labelFileChanged();
    bool readSegmentData(std::string filename);
    void createFilterKernels();

#ifdef labelDEBUG
    void renderMaps();
    void writeDistanceMapToFile();
#endif

    // property methods invoked by onChange()
    void updateExternColorsEvt();   // invoked by setLabelColor_, setHaloColor_
    void updateFontSizeEvt();       // invoked by fontSizeExtern_
    void updateFontSettingsEvt();   // invoked by lockInternalFontSettings_
    void setShape3DEvt();           // invoked by shape3D_
    void unlockInternalFontSettingsEvt(); // invoked by labelColorIntern_, haloColorIntern_, fontSizeIntern_

    // user interaction
    Label* getPickedLabel(int x, int y);
    bool catchedBySegment(const Label &pLabel, tgt::ivec2 mousePos);
    bool updateSegmentCaption(Label &pLabel, std::string const &newCaption);
    VolumePort volumePort_;
    Label* pickedLabel_;
    bool drag_;                     // true if user is currently dragging a label
    tgt::ivec2 pickedPointOffset_;  // vector from picked point to picked label's labelPos
    tgt::ivec2 lastDragPoint_;        // pixel coords of last dragging event
    void mousePressEvent(tgt::MouseEvent* e);
    void mouseDoubleClickEvent(tgt::MouseEvent* e);
    void mouseReleaseEvent(tgt::MouseEvent* e);
    void mouseMoveEvent(tgt::MouseEvent* e);
    void wheelEvent(tgt::MouseEvent* e);
    tgt::Stopwatch time;

    RenderPort labelingPort_;
    CameraProperty camera_;
    VolumeHandle* currentVolumeHandle_;
};

} // namespace voreen

#endif //VRN_LABELING_H
