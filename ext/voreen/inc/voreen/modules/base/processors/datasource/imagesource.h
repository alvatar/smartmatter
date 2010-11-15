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

#ifndef VRN_IMAGESOURCE_H
#define VRN_IMAGESOURCE_H

#include "voreen/core/processors/renderprocessor.h"
#include "voreen/core/ports/allports.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/vectorproperty.h"

using tgt::Texture;

namespace voreen {

/**
 * Loads an image file from disk and supplies it as rendering.
 */
class ImageSource : public RenderProcessor {

public:
    ImageSource();
    ~ImageSource();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "ImageSource"; }
    virtual std::string getCategory() const  { return "Data Source"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_STABLE; }
    virtual std::string getProcessorInfo() const;

    /**
     * Reloads the image, if file path has changed.
     */
    virtual void invalidate(int inv = INVALID_RESULT);

    /**
     * Loads the image specified by filename.
     *
     * @note The processor must have been initialized
     *       before calling this function.
     *
     * @param filename the image to load
     */
    void loadImage(const std::string& filename);

    /**
     * Assigns an image to this processor.
     *
     * @note The processor must have been initialized
     *       before calling this function.
     *
     * @param texture the 2D texture encapsulating the image.
     */
    void setImage(tgt::Texture* texture);

    /**
     * Returns the currently assigned image texture.
     */
    const tgt::Texture* getImage() const;

protected:
    virtual void beforeProcess();
    virtual void process();
    virtual void initialize() throw (VoreenException);
    virtual void deinitialize() throw (VoreenException);

    /**
     * Clears the loaded image. Processor needs to be initialized when calling this function.
     */
    void clearImage();

    RenderPort outport_;            ///< The render port the image is written to.
    FileDialogProperty imageFile_;  ///< Path of the loaded image file.
    ButtonProperty clearImage_;      ///< Executes clearImage().
    IntVec2Property imageSize_;     ///< Read-only property providing the dimensions of the loaded image.

    tgt::Texture* texture_;         ///< The 2D texture encapsulating the image file.
    tgt::Shader* shader_;           ///< Shader program used for rendering the loaded image to the outport.

    /// Determines whether the processor owns the texture, which
    /// is only the case, if it has loaded it.
    bool textureOwner_;

    bool forceReload_;
    static const std::string loggerCat_;
};

} // namespace

#endif
