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

#include "voreen/core/datastructures/transfunc/transfuncprimitive.h"

#include "tgt/glmath.h"

namespace voreen {

TransFuncPrimitive::TransFuncPrimitive()
    : color_(255, 255, 0, 255)
    , selected_(false)
    , fuzziness_(1.f)
    , cpSize_(0.02f)
    , grabbed_(-1)
    , scaleFactor_(1.f)
{}

TransFuncPrimitive::TransFuncPrimitive(const tgt::col4& color, float scaleFactor)
    : color_(color)
    , selected_(false)
    , fuzziness_(1.f)
    , cpSize_(0.02f)
    , grabbed_(-1)
    , scaleFactor_(scaleFactor)
{}

TransFuncPrimitive::~TransFuncPrimitive() {}

void TransFuncPrimitive::setColor(const tgt::col4& c) {
    color_ = c;
}

tgt::col4 TransFuncPrimitive::getColor() const {
    return color_;
}

void TransFuncPrimitive::setFuzziness(float f) {
    fuzziness_ = f;
}

float TransFuncPrimitive::getFuzziness() const {
    return fuzziness_;
}

float TransFuncPrimitive::getControlPointSize() const {
    return cpSize_;
}

void TransFuncPrimitive::select(const tgt::vec2& pos) {
    selected_ = true;
    selectControlPoint(pos);
}

void TransFuncPrimitive::deselect() {
    selected_ = false;
    grabbed_ = -1;
}

void TransFuncPrimitive::paintControlPoint(const tgt::vec2& v) {
    glTranslatef(v.x, v.y, 0.1f);
    tgt::vec2 t;

    glBegin(GL_POLYGON);
    glColor4ub(150, 150, 150, 255);
    for (int i = 0; i < 20; ++i) {
        t.x = cosf((i / 20.f) * 2.f * tgt::PIf) * cpSize_;
        t.y = sinf((i / 20.f) * 2.f * tgt::PIf) * cpSize_;
        tgt::vertex(t);
    }
    glEnd();

    glTranslatef(0.f, 0.f, 0.1f);

    glBegin(GL_LINE_LOOP);
    if (selected_)
        glColor4ub(255, 255, 255, 255);
    else
        glColor4ub(128, 128, 128, 255);
    for (int i = 0; i < 20; ++i) {
        t.x = cosf((i / 20.f) * 2.f * tgt::PIf) * cpSize_;
        t.y = sinf((i / 20.f) * 2.f * tgt::PIf) * cpSize_;
        tgt::vertex(t);
    }
    glEnd();

    glTranslatef(-v.x, -v.y, -0.2f);
}

void TransFuncPrimitive::serialize(XmlSerializer& s) const {
    s.serialize("fuzzy", fuzziness_);
    s.serialize("color", color_);
}

void TransFuncPrimitive::deserialize(XmlDeserializer& s) {
    s.deserialize("fuzzy", fuzziness_);
    s.deserialize("color", color_);
}

//-----------------------------------------------------------------------------

TransFuncQuad::TransFuncQuad()
    : TransFuncPrimitive()
    , scaleCoords_(true)
{
}

TransFuncQuad::TransFuncQuad(const tgt::vec2& center, float size, const tgt::col4& col, float scaleFactor)
    : TransFuncPrimitive(col, scaleFactor)
    , scaleCoords_(true)
{
    size *= 0.5f;
    coords_[0] = center + tgt::vec2(-size, -size);
    coords_[1] = center + tgt::vec2( size, -size);
    coords_[2] = center + tgt::vec2( size,  size);
    coords_[3] = center + tgt::vec2(-size,  size);
}

TransFuncQuad::~TransFuncQuad() {}

void TransFuncQuad::setScaleFactor(float scaleFactor) {
    scaleFactor_ = scaleFactor;
    // adjust coords
    for (int i = 0; i < 4; ++i)
        coords_[i].y = tgt::clamp(coords_[i].y / scaleFactor_, 0.f, 1.f);
}

void TransFuncQuad::paint() {
    tgt::vec2 coords[4];
    if (scaleCoords_) {
        coords[0] = tgt::vec2(coords_[0].x, scaleFactor_ * coords_[0].y);
        coords[1] = tgt::vec2(coords_[1].x, scaleFactor_ * coords_[1].y);
        coords[2] = tgt::vec2(coords_[2].x, scaleFactor_ * coords_[2].y);
        coords[3] = tgt::vec2(coords_[3].x, scaleFactor_ * coords_[3].y);
    }
    else {
        coords[0] = coords_[0];
        coords[1] = coords_[1];
        coords[2] = coords_[2];
        coords[3] = coords_[3];
    }

    tgt::vec2 center = coords[0] + coords[1] + coords[2] + coords[3];
    center /= 4.f;

    glTranslatef(0.f, 0.f, -0.5f);
    glBegin(GL_QUADS);

        for (int i = 1; i <= 4; ++i) {
            glColor4ub(color_.r, color_.g, color_.b, 0);
            tgt::vertex(coords[i-1]);
            tgt::vertex(coords[i%4]);
            glColor4ubv(color_.elem);
            tgt::vertex(fuzziness_ * coords[i%4] + (1.f - fuzziness_) * center);
            tgt::vertex(fuzziness_ * coords[i-1] + (1.f - fuzziness_) * center);
        }

        tgt::vertex(fuzziness_ * coords[0] + (1.f - fuzziness_) * center);
        tgt::vertex(fuzziness_ * coords[1] + (1.f - fuzziness_) * center);
        tgt::vertex(fuzziness_ * coords[2] + (1.f - fuzziness_) * center);
        tgt::vertex(fuzziness_ * coords[3] + (1.f - fuzziness_) * center);

    glEnd();
    glTranslatef(0.f, 0.f, 0.5f);
}

void TransFuncQuad::paintForSelection(GLubyte id) {
    glBegin(GL_QUADS);
        glColor3ub(id, 123, 123);
        tgt::vertex(coords_[0]);
        tgt::vertex(coords_[1]);
        tgt::vertex(coords_[2]);
        tgt::vertex(coords_[3]);
    glEnd();
}

void TransFuncQuad::paintInEditor() {
    scaleCoords_ = false;
    paint();
    scaleCoords_ = true;

    glBegin(GL_LINE_LOOP);
        if (selected_)
            glColor4ub(255, 255, 255, 255);
        else
            glColor4ub(128, 128, 128, 255);
        tgt::vertex(coords_[0]);
        tgt::vertex(coords_[1]);
        tgt::vertex(coords_[2]);
        tgt::vertex(coords_[3]);
    glEnd();

    paintControlPoint(coords_[0]);
    paintControlPoint(coords_[1]);
    paintControlPoint(coords_[2]);
    paintControlPoint(coords_[3]);
}

float TransFuncQuad::getClosestControlPointDist(const tgt::vec2& pos) {
    float min = distance(pos, coords_[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = distance(pos, coords_[i]);
        if (d < min)
            min = d;
    }
    return min;
}

bool TransFuncQuad::selectControlPoint(const tgt::vec2& pos) {
    grabbed_ = -1;
    int n = 0;
    float min = distance(pos, coords_[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = distance(pos, coords_[i]);
        if (d < min) {
            min = d;
            n = i;
        }
    }
    if (min < cpSize_) {
        grabbed_ = n;
        return true;
    }
    return false;
}

void TransFuncQuad::serialize(XmlSerializer& s) const {
    TransFuncPrimitive::serialize(s);
    for (int i = 0; i < 4; ++i)
        s.serialize("coords", coords_[i]);
}

void TransFuncQuad::deserialize(XmlDeserializer& s) {
    TransFuncPrimitive::deserialize(s);
    for (int i = 0; i < 4; ++i)
        s.deserialize("coords", coords_[i]);
}

bool TransFuncQuad::move(const tgt::vec2& offset) {
    // only move the control point when one is grabbed
    if (grabbed_ > -1) {
        tgt::vec2 temp = coords_[grabbed_] + offset;
        // do not move control point when it is outside of allowed region
        if ((temp.x < 0.f) || (temp.x > 1.f) ||
            (temp.y < 0.f) || (temp.y > 1.f))
        {
            return false;
        }
        else
            coords_[grabbed_] += offset;
    }
    else {
        for (int i = 0; i < 4; ++i) {
            tgt::vec2 temp = coords_[i] + offset;
            // do not move primitive when one point is outside of allowed region
            if ((temp.x < 0.f) || (temp.x > 1.f) ||
                (temp.y < 0.f) || (temp.y > 1.f))
            {
                return false;
            }
        }
        for (int i = 0; i < 4; ++i)
            coords_[i] += offset;
    }

    return true;
}

//-----------------------------------------------------------------------------

TransFuncBanana::TransFuncBanana()
    : TransFuncPrimitive()
    , steps_(20)
    , scaleCoords_(true)
{}

TransFuncBanana::TransFuncBanana(const tgt::vec2& a, const tgt::vec2& b1, const tgt::vec2& b2, const tgt::vec2& c, const tgt::col4& col, float scaleFactor)
    : TransFuncPrimitive(col, scaleFactor)
    , steps_(20)
    , scaleCoords_(true)
{
    coords_[0] = a;
    coords_[1] = b1;
    coords_[2] = b2;
    coords_[3] = c;
}

TransFuncBanana::~TransFuncBanana() {}

void TransFuncBanana::setScaleFactor(float scaleFactor) {
    scaleFactor_ = scaleFactor;
    // adjust coords
    for (int i = 0; i < 4; ++i)
        coords_[i].y = tgt::clamp(coords_[i].y / scaleFactor_, 0.f, 1.f);
}

void TransFuncBanana::paint() {
    glTranslatef(0.f, 0.f, -0.5f);
    glColor4ubv(color_.elem);
    paintInner();
    glTranslatef(0.f, 0.f, 0.5f);
}

void TransFuncBanana::paintForSelection(GLubyte id) {
    glColor3ub(id, 123, 123);
    float t;
    tgt::vec2 v1, v2, t1, t2, t3, t4, tc;
    t1 = (2.f * coords_[1]) - (0.5f * coords_[0]) - (0.5f * coords_[3]);
    t2 = (2.f * coords_[2]) - (0.5f * coords_[0]) - (0.5f * coords_[3]);

    // fill the space between the two bezier curves:
    glBegin(GL_TRIANGLE_STRIP);
    tgt::vertex(coords_[0]);
    for (int i = 0; i < steps_; ++i) {
        t = i / static_cast<float>(steps_ - 1);
        v1 = (((1 - t) * (1 - t)) * coords_[0]) + ((2 * (1 - t) * t) * t1) + ((t * t) * coords_[3]);
        v2 = (((1 - t) * (1 - t)) * coords_[0]) + ((2 * (1 - t) * t) * t2) + ((t * t) * coords_[3]);
        tgt::vertex(v1);
        tgt::vertex(v2);
    }
    tgt::vertex(coords_[3]);
    glEnd();
}

void TransFuncBanana::paintInner() {
    float t;
    tgt::vec2 v1, v2, t1, t2, t3, t4, tc;

    tgt::vec2 coords[4];
    if (scaleCoords_) {
        coords[0] = tgt::vec2(coords_[0].x, scaleFactor_ * coords_[0].y);
        coords[1] = tgt::vec2(coords_[1].x, scaleFactor_ * coords_[1].y);
        coords[2] = tgt::vec2(coords_[2].x, scaleFactor_ * coords_[2].y);
        coords[3] = tgt::vec2(coords_[3].x, scaleFactor_ * coords_[3].y);
    }
    else {
        coords[0] = coords_[0];
        coords[1] = coords_[1];
        coords[2] = coords_[2];
        coords[3] = coords_[3];
    }

    t1 = (2.f * coords[1]) - (0.5f * coords[0]) - (0.5f * coords[3]);
    t2 = (2.f * coords[2]) - (0.5f * coords[0]) - (0.5f * coords[3]);

    tc = (t1 + t2) / 2.f;
    t3 = fuzziness_ * t1 + (1.f - fuzziness_) * tc;
    t4 = fuzziness_ * t2 + (1.f - fuzziness_) * tc;

    // fill the space between the two bezier curves:
    glBegin(GL_TRIANGLE_STRIP);
    glColor4ubv(color_.elem);
    tgt::vertex(coords[0]);
    for (int i = 0; i < steps_; ++i) {
        t = i / static_cast<float>(steps_ - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t1) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t3) + ((t * t) * coords[3]);
        glColor4ub(color_.r, color_.g, color_.b, 0);
        tgt::vertex(v1);
        glColor4ubv(color_.elem);
        tgt::vertex(v2);
    }
    tgt::vertex(coords[3]);

    glColor4ubv(color_.elem);
    tgt::vertex(coords[0]);
    for (int i = 0; i < steps_; ++i) {
        t = i / static_cast<float>(steps_ - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t3) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t4) + ((t * t) * coords[3]);
        tgt::vertex(v1);
        tgt::vertex(v2);
    }
    tgt::vertex(coords[3]);

    tgt::vertex(coords[0]);
    for (int i = 0; i < steps_; ++i) {
        t = i / static_cast<float>(steps_ - 1);
        v1 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t4) + ((t * t) * coords[3]);
        v2 = (((1 - t) * (1 - t)) * coords[0]) + ((2 * (1 - t) * t) * t2) + ((t * t) * coords[3]);
        glColor4ubv(color_.elem);
        tgt::vertex(v1);
        glColor4ub(color_.r, color_.g, color_.b, 0);
        tgt::vertex(v2);
    }
    tgt::vertex(coords[3]);

    glEnd();
}

void TransFuncBanana::paintInEditor() {
    scaleCoords_ = false;
    paint();
    scaleCoords_ = true;

    float t;
    tgt::vec2 v, t1, t2;
    t1 = (2.f * coords_[1]) - (0.5f * coords_[0]) - (0.5f * coords_[3]);
    t2 = (2.f * coords_[2]) - (0.5f * coords_[0]) - (0.5f * coords_[3]);

    // draw outer line of double bezier curve:
    glBegin(GL_LINE_LOOP);
        if (selected_)
            glColor4ub(255, 255, 255, 255);
        else
            glColor4ub(128, 128, 128, 255);

        for (int i = 0; i < steps_; ++i) {
            t = i / static_cast<float>(steps_ - 1);
            v = (((1 - t) * (1 - t)) * coords_[0]) + ((2 * (1 - t) * t) * t1) + ((t * t) * coords_[3]);
            tgt::vertex(v);
        }

        for (int i = 0; i < steps_; ++i) {
            t = 1.f - (i / static_cast<float>(steps_ - 1));
            v = (((1 - t) * (1 - t)) * coords_[0]) + ((2 * (1 - t) * t) * t2) + ((t * t) * coords_[3]);
            tgt::vertex(v);
        }
    glEnd();

    paintControlPoint(coords_[0]);
    paintControlPoint(coords_[1]);
    paintControlPoint(coords_[2]);
    paintControlPoint(coords_[3]);
}

float TransFuncBanana::getClosestControlPointDist(const tgt::vec2& pos) {
    float min = distance(pos, coords_[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = distance(pos, coords_[i]);
        if (d < min)
            min = d;
    }
    return min;
}

bool TransFuncBanana::selectControlPoint(const tgt::vec2& pos) {
    grabbed_ = -1;
    int n = 0;
    float min = distance(pos, coords_[0]);
    float d;
    for (int i = 1; i < 4; ++i) {
        d = distance(pos, coords_[i]);
        if (d < min) {
            min = d;
            n = i;
        }
    }
    if (min < cpSize_) {
        grabbed_ = n;
        return true;
    }
    return false;
}

void TransFuncBanana::serialize(XmlSerializer& s) const {
    TransFuncPrimitive::serialize(s);
    for (int i = 0; i < 4; ++i)
        s.serialize("coords", coords_[i]);
}

void TransFuncBanana::deserialize(XmlDeserializer& s) {
    TransFuncPrimitive::deserialize(s);
    for (int i = 0; i < 4; ++i)
        s.deserialize("coords", coords_[i]);
}

bool TransFuncBanana::move(const tgt::vec2& offset) {
    if (grabbed_ > -1) {
        tgt::vec2 temp = coords_[grabbed_] + offset;
        // do not move control point when it is outside of allowed region
        if ((temp.x < 0.f) || (temp.x > 1.f) ||
            (temp.y < 0.f) || (temp.y > 1.f))
        {
            return false;
        }
        else
            coords_[grabbed_] += offset;
    }
    else {
        for (int i = 0; i < 4; ++i) {
            tgt::vec2 temp = coords_[i] + offset;
            //do not move primitive when one point is outside of allowed region
            if ((temp.x < 0.f) || (temp.x > 1.f) ||
                (temp.y < 0.f) || (temp.y > 1.f))
            {
                return false;
            }
        }
        for (int i = 0; i < 4; ++i)
            coords_[i] += offset;
    }

    return true;
}

TransFuncPrimitive* TransFuncQuad::clone() const {
    TransFuncQuad* prim = new TransFuncQuad();

    prim->color_.r = color_.r;
    prim->color_.g = color_.g;
    prim->color_.b = color_.b;
    prim->color_.a = color_.a;
    prim->selected_ = selected_;
    prim->fuzziness_ = fuzziness_;
    prim->cpSize_ = cpSize_;
    prim->grabbed_ = grabbed_;
    prim->scaleFactor_ = scaleFactor_;

    prim->coords_[0].x = coords_[0].x;
    prim->coords_[0].y = coords_[0].y;
    prim->coords_[1].x = coords_[1].x;
    prim->coords_[1].y = coords_[1].y;
    prim->coords_[2].x = coords_[2].x;
    prim->coords_[2].y = coords_[2].y;
    prim->coords_[3].x = coords_[3].x;
    prim->coords_[3].y = coords_[3].y;
    prim->scaleCoords_ = scaleCoords_;
    return prim;
}


TransFuncPrimitive* TransFuncBanana::clone() const {
    TransFuncBanana* prim = new TransFuncBanana();

    prim->color_.r = color_.r;
    prim->color_.g = color_.g;
    prim->color_.b = color_.b;
    prim->color_.a = color_.a;
    prim->selected_ = selected_;
    prim->fuzziness_ = fuzziness_;
    prim->cpSize_ = cpSize_;
    prim->grabbed_ = grabbed_;
    prim->scaleFactor_ = scaleFactor_;

    prim->coords_[0].x = coords_[0].x;
    prim->coords_[0].y = coords_[0].y;
    prim->coords_[1].x = coords_[1].x;
    prim->coords_[1].y = coords_[1].y;
    prim->coords_[2].x = coords_[2].x;
    prim->coords_[2].y = coords_[2].y;
    prim->coords_[3].x = coords_[3].x;
    prim->coords_[3].y = coords_[3].y;
    prim->scaleCoords_ = scaleCoords_;
    prim->steps_ = steps_;

    return prim;
}



} // namespace voreen
