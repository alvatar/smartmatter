#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glut.h>

#include "tgt/shadermanager.h"
#include "tgt/glut/glutcanvas.h"

#include "voreen/core/voreenapplication.h"
#include "voreen/core/utils/voreenpainter.h"
#include "voreen/core/network/networkevaluator.h"
#include "voreen/core/network/workspace.h"
#include "voreen/core/network/processornetwork.h"
#include "voreen/core/processors/canvasrenderer.h"

#include "voreen/modules/ca/cavolumeprocessor.h"

//---------------------------------------------------------------------------

using namespace voreen;

VoreenApplication* app = 0;

NetworkEvaluator* networkEvaluator = 0;
ProcessorNetwork* network = 0;
VoreenPainter* painter = 0;


/*
namespace tgt {

class SyncGLUTCanvas : public tgt::GLUTCanvas {
	public:
		SyncGLUTCanvas(const std::string& title = "",
				const ivec2& size = ivec2(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT),
				const GLCanvas::Buffers buffers = RGBADD)
			: GLUTCanvas(title, size, buffers) {}

		void update() {
			tgt::GLUTCanvas::update();
			std::cout << "update();" << std::endl;
			std::vector<CAVolumeProcessor*> cap = network->getProcessorsByType<CAVolumeProcessor>();
			if(cap.size() > 0) {
				cap[0]->invalidate(Processor::INVALID_RESULT);
			}
		}
};

}
*/

tgt::GLUTCanvas* canvas = 0;

//---------------------------------------------------------------------------

void initialize() {
    ShdrMgr.addPath("../ext/voreen/src/core/glsl/");
    ShdrMgr.addPath("../ext/voreen/src/core/glsl/utils");
    ShdrMgr.addPath("../ext/voreen/src/modules/base/glsl");
    ShdrMgr.addPath("../ext/voreen/src/modules/base/glsl/image");

    Workspace* workspace = new Workspace();
    //workspace->load(VoreenApplication::app()->getWorkspacePath("/standard.vws"));
    workspace->load("../data/workspaces/ca-standard.vws");

    // initialize the network evaluator
    networkEvaluator = new NetworkEvaluator();
    network = workspace->getProcessorNetwork();
    std::vector<CanvasRenderer*> canvasRenderer = network->getProcessorsByType<CanvasRenderer>();

    if (canvasRenderer.size() > 0) {
        // init painter and connect it to the canvas
        painter = new VoreenPainter(canvas, networkEvaluator, canvasRenderer[0]);
        canvas->setPainter(painter);
        canvasRenderer[0]->setCanvas(canvas);
        // give the network to the network evaluator
        networkEvaluator->setProcessorNetwork(network);
    }
}

void finalize() {
    delete painter;
    painter = 0;
    delete network;
    network = 0;
    delete networkEvaluator;
    networkEvaluator = 0;

    if (app) {
        app->deinitGL();
        app->deinit();
    }
    delete app;
    app = 0;
}

void keyPressed(unsigned char key, int /*x*/, int /*y*/) {
    switch (key) {
        case '\033': // = ESC
            finalize();
            exit(0);
            break;
        case '1':
            glutReshapeWindow(128,128);
            break;
        case '2':
            glutReshapeWindow(256,256);
            break;
        case '3':
            glutReshapeWindow(512,512);
            break;
        case '4':
            glutReshapeWindow(1024, 1024);
            break;
    }
}



int main(int argc, char** argv) {
	VoreenApplication* app = new VoreenApplication("simple-GLUT", "simple-GLUT", argc, argv,
			VoreenApplication::APP_ALL);
	app->init();

	glutInit(&argc, argv);

	canvas = new tgt::GLUTCanvas("Voreen - The Volume Rendering Engine",
			tgt::ivec2(512, 512), tgt::GLCanvas::RGBADD);
	canvas->init();

	glutKeyboardFunc(keyPressed);

	app->initGL();
	initialize();

	glutMainLoop();
	return 0; // will never be reached for standard GLUT
}
