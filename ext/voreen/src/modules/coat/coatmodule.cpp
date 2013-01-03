#include "voreen/modules/coat/coatmodule.h"

#include "voreen/modules/coat/coatvolumereader.h"

namespace voreen {

CoatModule::CoatModule()
    : VoreenModule()
{
    // module name to be used in the GUI
    setName("3d Coat");

    addVolumeReader(new CoatVolumeReader());
}

} // namespace
