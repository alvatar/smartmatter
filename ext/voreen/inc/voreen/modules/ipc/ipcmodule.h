#ifndef VRN_IPCMODULE_H
#define VRN_IPCMODULE_H

#include "voreen/core/voreenmodule.h"

namespace voreen {

class IPCModule : public VoreenModule {

public:
    IPCModule();

    virtual std::string getDescription() const { 
        return "Inter-process Communication Module"; 
    }
};

} // namespace

#endif
