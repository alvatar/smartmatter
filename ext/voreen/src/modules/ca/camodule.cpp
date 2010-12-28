#include "voreen/modules/ca/camodule.h"

#include "voreen/modules/ca/cavolumeprocessor.h"

namespace voreen {

CAModule::CAModule() 
	: VoreenModule() {
    // module name to be used in the GUI
    setName("Cellular Automata Volume Visualization");

    // each module processor needs to be registered
    addProcessor(new CAVolumeProcessor());

    // adds the module source dir <VOREEN_ROOT>/src/modules/test
    // to the shader search path
    addShaderPath(getModulesPath("ca"));
}

} // namespace
