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

#ifndef VRN_WINDOWSTATEMETADATA_H
#define VRN_WINDOWSTATEMETADATA_H

#include "voreen/core/io/serialization/serialization.h"

namespace voreen {

/**
 * The @c WindowStateMetaData class stores window state information.
 *
 * @see MetaDataBase
 */
class WindowStateMetaData : public MetaDataBase {
public:
    /**
     * Creates a @c WindowStateMetaData object storing the given window state.
     *
     * @param visible the window visiblity flag
     * @param x the window x position
     * @param y the window y position
     * @param width the window width
     * @param height the window height
     */
    WindowStateMetaData(const bool& visible = false, const int& x = -1, const int& y = -1, const int& width = -1, const int& height = -1);
    virtual ~WindowStateMetaData() {}

    /**
     * @see Serializable::serialize
     */
    virtual void serialize(XmlSerializer& s) const;

    /**
     * @see Serializable::deserialize
     */
    virtual void deserialize(XmlDeserializer& s);

    /**
     * @see SerializableFactory::getTypeString
     */
    virtual const std::string getTypeString(const std::type_info& type) const;

    /**
     * @see SerializableFactory::createType
     */
    virtual Serializable* createType(const std::string& typeString);

    /**
     * Sets the window visibility flag.
     *
     * @param value the visibility flag
     */
    void setVisible(const bool& value);

    /**
     * Returns the window visibility flag.
     *
     * @return the visibility flag
     */
    bool getVisible() const;

    /**
     * Sets the window x position.
     *
     * @param value the x position
     */
    void setX(const int& value);

    /**
     * Returns the window x position.
     *
     * @return the x position
     */
    int getX() const;

    /**
     * Sets the window y position.
     *
     * @param value the y position
     */
    void setY(const int& value);

    /**
     * Returns the window y position.
     *
     * @return the y position
     */
    int getY() const;

    /**
     * Sets the window width.
     *
     * @param value the width
     */
    void setWidth(const int& value);

    /**
     * Returns the window width.
     *
     * @return the width
     */
    int getWidth() const;

    /**
     * Sets the window height.
     *
     * @param value the height
     */
    void setHeight(const int& value);

    /**
     * Returns the window height.
     *
     * @return the height
     */
    int getHeight() const;

private:
    /**
     * Window visibility flag.
     */
    bool visible_;

    /**
     * Window x position.
     */
    int x_;

    /**
     * Window y position.
     */
    int y_;

    /**
     * Window width.
     */
    int width_;

    /**
     * Window height.
     */
    int height_;

};

} // namespace

#endif // VRN_WINDOWSTATEMETADATA_H
