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

#include "voreen/core/datastructures/imagesequence.h"

#include "tgt/assert.h"

#include <algorithm>

using std::vector;

namespace voreen {

const std::string ImageSequence::loggerCat_ = "voreen.ImageSequence";

ImageSequence::ImageSequence() {
}

ImageSequence::~ImageSequence() {
}

void ImageSequence::add(tgt::Texture* texture){
    tgtAssert(texture, "Null pointer passed");
    textures_.push_back(texture);
}

void ImageSequence::remove(const tgt::Texture* texture) {
    std::vector<tgt::Texture*>::iterator iter = std::find(textures_.begin(), textures_.end(), texture);
    if (iter != textures_.end())
        textures_.erase(iter);
}

void ImageSequence::remove(size_t i) {
    tgtAssert(i<size(), "Invalid index");
    textures_.erase(textures_.begin() + i);
}

bool ImageSequence::contains(const tgt::Texture* texture) const {
    return (std::find(textures_.begin(), textures_.end(), texture) != textures_.end());
}

tgt::Texture* ImageSequence::at(size_t i) const {
    tgtAssert(i<size(), "Invalid index");
    return textures_.at(i);
}

tgt::Texture* ImageSequence::front() const {
    if (empty())
        return 0;
    else
        return textures_.front();
}

tgt::Texture* ImageSequence::back() const {
    if (empty())
        return 0;
    else
        return textures_.back();
}

void ImageSequence::clear() {
    textures_.clear();
}

size_t ImageSequence::size() const {
    return textures_.size();
}

bool ImageSequence::empty() const {
    return (size() == 0);
}

} // namespace
