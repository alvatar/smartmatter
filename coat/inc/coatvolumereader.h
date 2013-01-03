#ifndef VRN_COATVOLUMEREADER_H
#define VRN_COATVOLUMEREADER_H

#include <string>

/*
#include "tgt/vector.h"
#include "tgt/matrix.h"

#include "voreen/core/datastructures/volume/modality.h"
*/
#include "voreen/core/io/volumereader.h"

namespace voreen {

//!
//! Reader for <tt>.3d Coat</tt> file containing a modeled volume dataset.
//!
class CoatVolumeReader : public VolumeReader
{

    public:

        CoatVolumeReader(ProgressBar* progress = 0);

        virtual VolumeReader* create(ProgressBar* progress = 0) const;

        virtual VolumeCollection* read(const std::string& fileName)
            throw(tgt::CorruptedFileException, tgt::IOException, std::bad_alloc);

    private:

        static const std::string loggerCat_;
};


}

#endif    // VRN_COATVOLUMEREADER_H
