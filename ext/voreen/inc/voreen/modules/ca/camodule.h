#ifndef VRN_CAMODULE_H
#define VRN_CAMODULE_H

#include "voreen/core/voreenmodule.h"

namespace voreen {

class CAModule : public VoreenModule {

public:
    CAModule();

    virtual std::string getDescription() const { 
        return "Cellular Automata Visualization Module"; 
    }
};

} // namespace

#endif
