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

#include "voreen/core/ports/coprocessorport.h"

namespace voreen {

CoProcessorPort::CoProcessorPort(PortDirection direction, const std::string& name, const bool allowMultipleConnections,
                Processor::InvalidationLevel invalidationLevel)
    : Port(name, direction, allowMultipleConnections, invalidationLevel)
{}

std::vector<Processor*> CoProcessorPort::getConnectedProcessors() const {
    if (isOutport()) {
        tgtAssert(false, "Called CoProcessorPort::getConnectedProcessors on outport!");
        LERRORC("voreen.coprocessorport", "Called CoProcessorPort::getConnectedProcessors on outport!");
        return std::vector<Processor*>();
    }
    else {
        std::vector<Processor*> processors;
        for (size_t i = 0; i < connectedPorts_.size(); ++i) {
            processors.push_back(connectedPorts_[i]->getProcessor());
        }
        return processors;
    }
}

Processor* CoProcessorPort::getConnectedProcessor() const {
    if (isOutport()) {
        tgtAssert(false, "Called CoProcessorPort::getConnectedProcessor on outport!");
        LERRORC("voreen.coprocessorport", "Called CoProcessorPort::getConnectedProcessor on outport!");
        return 0;
    }
    else {
        if (isConnected())
            return connectedPorts_[0]->getProcessor();
        else
            return 0;
    }
}

bool CoProcessorPort::isReady() const {
    return ((getConnectedProcessor() != 0) && getConnectedProcessor()->isReady());
}

} // namespace
