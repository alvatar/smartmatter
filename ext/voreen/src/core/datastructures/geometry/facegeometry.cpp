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

#include <map>

#include "tgt/glmath.h"

#include "voreen/core/datastructures/geometry/facegeometry.h"

namespace voreen {

FaceGeometry::FaceGeometry()
    : Geometry()
{
}

size_t FaceGeometry::getVertexCount() const {
    return vertices_.size();
}

bool FaceGeometry::empty() const {
    return (getVertexCount() == 0);
}

void FaceGeometry::addVertex(const VertexGeometry& vertex) {
    vertices_.push_back(vertex);

    setHasChanged(true);
}

const VertexGeometry& FaceGeometry::getVertex(size_t index) const {
    return vertices_.at(index);
}

VertexGeometry& FaceGeometry::getVertex(size_t index) {
    return vertices_.at(index);
}

void FaceGeometry::clear() {
    vertices_.clear();

    setHasChanged(true);
}

FaceGeometry::iterator FaceGeometry::begin() {
    return vertices_.begin();
}

FaceGeometry::const_iterator FaceGeometry::begin() const {
    return vertices_.begin();
}

FaceGeometry::iterator FaceGeometry::end() {
    return vertices_.end();
}

FaceGeometry::const_iterator FaceGeometry::end() const {
    return vertices_.end();
}

VertexGeometry& FaceGeometry::operator[](size_t index) {
    return vertices_[index];
}

void FaceGeometry::render() {
    glBegin(GL_POLYGON);
    for (iterator it = begin(); it != end(); ++it) {
        glColor4fv(it->getColor().elem);
        tgt::texCoord(it->getTexCoords());
        tgt::vertex(it->getCoords());
    }
    glEnd();
}

void FaceGeometry::transform(const tgt::mat4& transformation) {
    for (iterator it = begin(); it != end(); ++it)
        it->transform(transformation);

    setHasChanged(true);
}

void FaceGeometry::clip(const tgt::vec4& clipplane, double epsilon) {
    if (vertices_.size() < 2)
        return;

    // Since the clipped face vertices are appended to the vertex list, remember the current vertex list size.
    size_t vertexCount = vertices_.size();
    double lastDistance = vertices_.at(0).getDistanceToPlane(clipplane, epsilon);

    // Process face edges...
    for (size_t i = 0; i < vertexCount; ++i) {
        double distance = vertices_.at((i + 1) % vertexCount).getDistanceToPlane(clipplane, epsilon);

        // Keep both vertices?
        if (lastDistance <= 0 && distance <= 0) {
            // If processing the first edge, insert first vertex...
            if (i == 0)
                vertices_.push_back(vertices_.at(i));

            // If NOT processing the last edge, insert second vertex...
            if (i < (vertexCount - 1))
                vertices_.push_back(vertices_.at(i + 1));
        }
        // Discard first vertex, but keep second vertex?
        else if (lastDistance > 0 && distance <= 0) {
            // If NOT clipplane intersection vertex and second vertex are equal, insert clipplane intersection vertex...
            VertexGeometry intersectionVertex = VertexGeometry::interpolate(vertices_.at(i), vertices_.at((i + 1) % vertexCount), lastDistance / (lastDistance - distance));
            if (!vertices_.at((i + 1) % vertexCount).equals(intersectionVertex, epsilon))
                vertices_.push_back(intersectionVertex);

            // If NOT processing the last edge, insert second vertex...
            if (i < (vertexCount - 1))
                vertices_.push_back(vertices_.at(i + 1));
        }
        // Keep first vertex, but discard second vertex?
        else if (lastDistance <= 0 && distance > 0) {
            // If processing the first edge, insert first vertex...
            if (i == 0)
                vertices_.push_back(vertices_.at(i));

            // If NOT clipplane intersection vertex and first vertex are equal, insert clipplane intersection vertex...
            VertexGeometry intersectionVertex = VertexGeometry::interpolate(vertices_.at(i), vertices_.at((i + 1) % vertexCount), lastDistance / (lastDistance - distance));
            if (!vertices_.at(i).equals(intersectionVertex, epsilon))
                vertices_.push_back(intersectionVertex);
        }

        lastDistance = distance;
    }

    // Remove unclipped face vertices from the vertex list...
    vertices_.erase(vertices_.begin(), vertices_.begin() + vertexCount);

    setHasChanged(true);
}


} // namespace
