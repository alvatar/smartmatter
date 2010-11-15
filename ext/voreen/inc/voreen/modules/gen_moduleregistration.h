// WARNING: This file is auto-generated!
#include "voreen/core/voreenapplication.h"
// module class headers
#include "voreen/modules/base/basemodule.h"
#include "voreen/modules/connectedcomponents/connectedcomponentsmodule.h"
#include "voreen/modules/flowreen/flowreenmodule.h"
#include "voreen/modules/pvm/pvmmodule.h"
#include "voreen/modules/segy/segymodule.h"
#include "voreen/modules/triangle/trianglemodule.h"
// instantiate module classes
namespace voreen {
void addAllModules(VoreenApplication* vapp) {
(*vapp).addModule(new BaseModule());
(*vapp).addModule(new ConnectedComponentsModule());
(*vapp).addModule(new FlowreenModule());
(*vapp).addModule(new PVMModule());
(*vapp).addModule(new SEGYModule());
(*vapp).addModule(new TriangleModule());
}}
