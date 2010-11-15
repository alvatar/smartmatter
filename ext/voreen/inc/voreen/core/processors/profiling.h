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

#ifndef VRN_PROFILING_H
#define VRN_PROFILING_H

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <string>
#include <vector>
#include <stack>
#include <time.h>

namespace voreen {

class PerformanceSample  {
public:
    PerformanceSample(PerformanceSample* parent, std::string name);

    PerformanceSample* addChild(std::string name);
    void print(int level = 0) const;

    void setTime(float time);
    float getTime() const;

    float getChildrenTime() const;
    PerformanceSample* getParent() const { return parent_; }

protected:
    PerformanceSample* parent_;
    std::vector<PerformanceSample> children_;
    std::string name_;
    float time_;

    static const std::string loggerCat_;
};

class ProfilingBlock;

/**
 * @brief Holds profiling info for an object.
 */
class PerformanceRecord {
    friend class ProfilingBlock;
public:
    PerformanceRecord();
    ~PerformanceRecord();

    const std::vector<PerformanceSample*> getSamples() const;
    PerformanceSample* getLastSample() const;

protected:
    void startBlock(const ProfilingBlock* const pb);
    void endBlock(const ProfilingBlock* const pb);

    PerformanceSample* current_;

    //history:
    std::vector<PerformanceSample*> samples_;
};

/**
 * @brief The constructor/destructor of this class is used to time blocks.
 *
 * Use macro PROFILING_BLOCK("NAME") to measure runtime of a block.
 */
class ProfilingBlock {
public:
    ProfilingBlock(std::string name, PerformanceRecord& pr);
    ~ProfilingBlock();

    float getTime() const;
    std::string getName() const;
protected:
    std::string name_;
    PerformanceRecord& pr_;

    clock_t start_;
    clock_t end_;

#ifdef _MSC_VER  // use high-performance counter on windows systems
    bool useHpf_;
    LARGE_INTEGER hpfTicksPerSecond_;
    LARGE_INTEGER hpfStart_;
    LARGE_INTEGER hpfEnd_;
#endif

    //static const std::string loggerCat_;
};

#define PROFILING_BLOCK(name) \
    ProfilingBlock block(name, performanceRecord_);

} // namespace

#endif //VRN_PROFILING_H
