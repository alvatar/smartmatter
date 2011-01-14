#include "voreen/modules/ipc/ipcmodule.h"

#include "voreen/modules/ipc/ipcvolumesource.h"

namespace voreen {

IPCModule::IPCModule() 
	: VoreenModule() {
    // module name to be used in the GUI
    setName("Inter-process Communication Module");

    // each module processor needs to be registered
    addProcessor(new IPCVolumeSource());

    // adds the module source dir <VOREEN_ROOT>/src/modules/test
    // to the shader search path
    addShaderPath(getModulesPath("ipc"));
}

} // namespace
