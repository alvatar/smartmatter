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

#include "voreen/core/plotting/aggregationfunction.h"

#include <algorithm>
#include <limits>
#include <math.h>

namespace voreen {



// AggregationFunctionCountHistogramm methods -------------------------------------------------------

plot_t AggregationFunctionCountHistogram::evaluate(std::vector<plot_t>& values) const {
    plot_t result = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        result += values.at(i);
    }
    return result;
}

AggregationFunction* AggregationFunctionCountHistogram::clone() const {
    return new AggregationFunctionCountHistogram();
}

std::string AggregationFunctionCountHistogram::toString() const {
    return "Count Histogramm";
}

void AggregationFunctionCountHistogram::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionCountHistogram::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionCount methods -------------------------------------------------------

plot_t AggregationFunctionCount::evaluate(std::vector<plot_t>& values) const {
    return values.size();
}

AggregationFunction* AggregationFunctionCount::clone() const {
    return new AggregationFunctionCount();
}

std::string AggregationFunctionCount::toString() const {
    return "Count";
}

void AggregationFunctionCount::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionCount::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionHistogrammMin methods -------------------------------------------------------

plot_t AggregationFunctionMinHistogram::evaluate(std::vector<plot_t>& values) const {
    if (values.size() == 0)
        return 0;
    plot_t min = 0;
    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i] > 0) {
            min = i;
            break;
        }
    }
    return min;
}

AggregationFunction* AggregationFunctionMinHistogram::clone() const {
    return new AggregationFunctionMinHistogram();
}

std::string AggregationFunctionMinHistogram::toString() const {
    return "Min Histogramm";
}


void AggregationFunctionMinHistogram::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMinHistogram::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionMin methods -------------------------------------------------------

plot_t AggregationFunctionMin::evaluate(std::vector<plot_t>& values) const {
    if (values.size() == 0)
        return 0;
    plot_t min = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        if((values[i] < min && values[i] == values[i]) || (min != min))
            min = values[i];
    }
    return min;
}

AggregationFunction* AggregationFunctionMin::clone() const {
    return new AggregationFunctionMin();
}

std::string AggregationFunctionMin::toString() const {
    return "Min";
}


void AggregationFunctionMin::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMin::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionHistogrammMax methods -------------------------------------------------------

plot_t AggregationFunctionMaxHistogram::evaluate(std::vector<plot_t>& values) const {
    if (values.size() == 0)
        return 0;

    plot_t max = 0;
    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i] > 0)
            max = i;
    }
    return max;
}

AggregationFunction* AggregationFunctionMaxHistogram::clone() const {
    return new AggregationFunctionMaxHistogram();
}

std::string AggregationFunctionMaxHistogram::toString() const {
    return "Max Histogramm";
}

void AggregationFunctionMaxHistogram::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMaxHistogram::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionMax methods -------------------------------------------------------

plot_t AggregationFunctionMax::evaluate(std::vector<plot_t>& values) const {
    if (values.size() == 0)
        return 0;

    plot_t max = values[0];
    for (size_t i = 1; i < values.size(); ++i) {
        if ((values[i] > max && values[i] == values[i])|| (max != max))
            max = values[i];
    }
    return max;
}

AggregationFunction* AggregationFunctionMax::clone() const {
    return new AggregationFunctionMax();
}

std::string AggregationFunctionMax::toString() const {
    return "Max";
}

void AggregationFunctionMax::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMax::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionSum methods -------------------------------------------------------

plot_t AggregationFunctionSum::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    for (size_t i = 0; i < values.size(); ++i)
        if (values[i] == values[i])
            sum += values[i];
    return sum;
}

AggregationFunction* AggregationFunctionSum::clone() const {
    return new AggregationFunctionSum();
}

std::string AggregationFunctionSum::toString() const {
    return "Sum";
}

void AggregationFunctionSum::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionSum::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionSum methods -------------------------------------------------------

plot_t AggregationFunctionSumHistogram::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    for (size_t i = 0; i < values.size(); ++i)
        sum += values[i]*i;
    return sum;
}

AggregationFunction* AggregationFunctionSumHistogram::clone() const {
    return new AggregationFunctionSumHistogram();
}

std::string AggregationFunctionSumHistogram::toString() const {
    return "Sum Histogramm";
}

void AggregationFunctionSumHistogram::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionSumHistogram::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionAverage methods -------------------------------------------------------

plot_t AggregationFunctionAverage::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sum += values[i];
    }
    return sum/values.size();
}

AggregationFunction* AggregationFunctionAverage::clone() const {
    return new AggregationFunctionAverage();
}

std::string AggregationFunctionAverage::toString() const {
    return "Average";
}

void AggregationFunctionAverage::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionAverage::deserialize(XmlDeserializer& /*s*/) {
}


// AggregationFunctionGeometricAverage methods -------------------------------------------------------

plot_t AggregationFunctionGeometricAverage::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 1;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sum *= values[i];
    }
    return std::pow(sum,1.0/values.size());
}

AggregationFunction* AggregationFunctionGeometricAverage::clone() const {
    return new AggregationFunctionGeometricAverage();
}

std::string AggregationFunctionGeometricAverage::toString() const {
    return "Geometric Average";
}

void AggregationFunctionGeometricAverage::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionGeometricAverage::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionHarmonicAverage methods -------------------------------------------------------

plot_t AggregationFunctionHarmonicAverage::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sum += 1.f/values[i];
    }
    return (values.size()+1.f)/sum;
}

AggregationFunction* AggregationFunctionHarmonicAverage::clone() const {
    return new AggregationFunctionHarmonicAverage();
}

std::string AggregationFunctionHarmonicAverage::toString() const {
    return "Harmonic Average";
}

void AggregationFunctionHarmonicAverage::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionHarmonicAverage::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionMedian methods -------------------------------------------------------

plot_t AggregationFunctionMedian::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    std::sort(values.begin(),values.end());
    if (values.size() % 2 == 0) {
        return (values[values.size()/2-1] + values[values.size()/2]) /2;
    }
    return values[(values.size()+1)/2-1];
}

AggregationFunction* AggregationFunctionMedian::clone() const {
    return new AggregationFunctionMedian();
}

std::string AggregationFunctionMedian::toString() const {
    return "Median";
}

void AggregationFunctionMedian::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMedian::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionMedianHistogram methods -------------------------------------------------------

plot_t AggregationFunctionMedianHistogram::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    plot_t med = 0;
    for (size_t i = 0; i < values.size(); ++i)
        sum += values[i];
    for (size_t i = 0; i < values.size(); ++i){
        med += values[i];
        if (med >= sum/2)
            return i;
    }
    return 0;
}

AggregationFunction* AggregationFunctionMedianHistogram::clone() const {
    return new AggregationFunctionMedianHistogram();
}

std::string AggregationFunctionMedianHistogram::toString() const {
    return "Median Histogramm";
}

void AggregationFunctionMedianHistogram::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMedianHistogram::deserialize(XmlDeserializer& /*s*/) {
}

// AggregationFunctionStandardDeviation methods -------------------------------------------------------

plot_t AggregationFunctionStandardDeviation::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sum += values[i];
    }
    plot_t average = sum/values.size();
    plot_t sqrtsum = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sqrtsum += std::pow(values[i]-average,2);
    }
    return std::sqrt(sqrtsum/values.size());
}

AggregationFunction* AggregationFunctionStandardDeviation::clone() const {
    return new AggregationFunctionStandardDeviation();
}

std::string AggregationFunctionStandardDeviation::toString() const {
    return "Standard Deviation";
}

void AggregationFunctionStandardDeviation::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionStandardDeviation::deserialize(XmlDeserializer& /*s*/) {
}


// AggregationFunctionVariance methods -------------------------------------------------------

plot_t AggregationFunctionVariance::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;

    plot_t sum = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sum += values[i];
    }
    plot_t average = sum/values.size();
    plot_t sqrtsum = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] == values[i])
            sqrtsum += std::pow(values[i]-average,2);
    }
    return sqrtsum/values.size();
}

AggregationFunction* AggregationFunctionVariance::clone() const {
    return new AggregationFunctionVariance();
}

std::string AggregationFunctionVariance::toString() const {
    return "Variance";
}

void AggregationFunctionVariance::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionVariance::deserialize(XmlDeserializer& /*s*/) {
}



// AggregationFunctionMode methods -------------------------------------------------------

plot_t AggregationFunctionMode::evaluate(std::vector<plot_t>& values) const {
    if (values.size() <= 0)
        return 0;
    std::stable_sort(values.begin(),values.end());

    std::vector<std::pair<plot_t,int> > value;
    std::pair<plot_t,int> mode =  std::pair<plot_t,int>(0,0);
    bool match;
    for (size_t i = 0; i < values.size(); ++i) {
        match = false;
        for (size_t j = 0; j < value.size(); ++j) {
            if (values[i] == value[j].first) {
                ++value[j].second;
                match = true;
                break;
            }
        }
        if (!match)
            value.push_back(std::pair<plot_t,int>(values[i],1));
    }
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i].second > mode.second) {
            mode = value[i];
        }
    }

    return mode.first;
}

AggregationFunction* AggregationFunctionMode::clone() const {
    return new AggregationFunctionMode();
}

std::string AggregationFunctionMode::toString() const {
    return "Mode";
}

void AggregationFunctionMode::serialize(XmlSerializer& /*s*/) const {
}

void AggregationFunctionMode::deserialize(XmlDeserializer& /*s*/) {
}

}

