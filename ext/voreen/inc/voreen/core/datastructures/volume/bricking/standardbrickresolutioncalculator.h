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

#ifndef VRN_STANDARDBRICKRESOLUTIONCALCULATOR_H
#define VRN_STANDARDRICKRESOLUTIONCALCULATOR_H

#include "voreen/core/datastructures/volume/bricking/brickresolutioncalculator.h"

namespace voreen {

    /**
    * This class calculates the brick resolutions so that at all bricks have
    * either one of two resolutions, and that at least half of the bricks have
    * the higher one of those two resolutions. The lower resolution will usually
    * be 64 voxels to guarantee at least some degree of accuracy for those bricks.
    */
    class StandardBrickResolutionCalculator : public BrickResolutionCalculator {

    public:
        StandardBrickResolutionCalculator(BrickingInformation& brickingInformation);

        virtual void calculateBrickResolutions();

    protected:

    private:



    }; //end of class



} //namespace




#endif

