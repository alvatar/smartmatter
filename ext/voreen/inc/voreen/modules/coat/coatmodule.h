#ifndef VRN_COATMODULE_H
#define VRN_COATMODULE_H

#include "voreen/core/voreenmodule.h"

namespace voreen {

class CoatModule : public VoreenModule {

public:
    CoatModule();

    virtual std::string getDescription() const {
        return "Provides 3d-Coat volume reader.";
    }
};

} // namespace

#endif // VRN_COATMODULE_H
