#include <QObject>
#include <QEvent>
#include <QApplication>
#include <QMainWindow>

#include "tgt/init.h"
#include "tgt/qt/qtcanvas.h"
#include "tgt/camera.h"
#include "tgt/shadermanager.h"

#include "voreen/core/utils/voreenpainter.h"
#include "voreen/core/network/networkevaluator.h"
#include "voreen/core/network/workspace.h"
#include "voreen/core/network/processornetwork.h"
#include "voreen/core/processors/canvasrenderer.h"
#include "voreen/qt/voreenapplicationqt.h"

using namespace voreen;

class MyEventFilter : public QObject {
	protected:
		bool eventFilter(QObject *obj, QEvent *ev) {
			if(ev->type() == QEvent::KeyPress) {
				QKeyEvent *ke = (QKeyEvent *)ev;
				if ( ke->key() == Qt::Key_Escape ) {
					ke->accept();
					QApplication::quit();
				}
			}
			return QObject::eventFilter(obj, ev);
		}
};

int main(int argc, char* argv[]) {
    // init both Qt and Voreen application (does not require OpenGL context)
    QApplication myapp(argc, argv);
    VoreenApplicationQt vapp("Cellular Automata Inter-process visualization"
			, "Cellular Automata Inter-process visualization"
			, argc
			, argv
			, VoreenApplication::APP_ALL);
    vapp.init();

    // create the mainwindow and assign a canvas to it as central widget
    QMainWindow* mainwindow = new QMainWindow();

    VoreenApplicationQt::qtApp()->setMainWindow(mainwindow);
    tgt::QtCanvas* canvas = new tgt::QtCanvas("Voreen - The Volume Rendering Engine");
	MyEventFilter filter;
	canvas->installEventFilter(&filter);
    mainwindow->setCentralWidget(canvas);
    mainwindow->resize(512, 512);
    mainwindow->show();

    // must be called *after* a OpenGL context has been created (canvas)
    // and *before* any further OpenGL access
    vapp.initGL();

    ShdrMgr.addPath("../ext/voreen/src/core/glsl/");
    ShdrMgr.addPath("../ext/voreen/src/core/glsl/utils");
    ShdrMgr.addPath("../ext/voreen/src/modules/base/glsl");
    ShdrMgr.addPath("../ext/voreen/src/modules/base/glsl/image");
    // load workspace from disc
    Workspace* workspace = new Workspace();
    //workspace->load(VoreenApplication::app()->getWorkspacePath("/standard.vws"));
    workspace->load("../data/workspaces/ca-standard.vws");

    // initialize the network evaluator
    NetworkEvaluator* networkEvaluator = new NetworkEvaluator();
    ProcessorNetwork* network = workspace->getProcessorNetwork();
    std::vector<CanvasRenderer*> canvasRenderer = network->getProcessorsByType<CanvasRenderer>();

    // init painter and connect it to canvas, evaluator and canvas renderer
    VoreenPainter* painter = new VoreenPainter(canvas, networkEvaluator, canvasRenderer[0]);
    canvas->setPainter(painter);
    canvasRenderer[0]->setCanvas(canvas);

    // pass the network to the network evaluator, which also initializes the processors
    networkEvaluator->setProcessorNetwork(network);

    // start the event process; the program runs as long as theres no exit-event
    myapp.exec();

    // we're done as soon as myapp.exec() returns, so we can delete everything
    delete painter;
    delete workspace;
    delete networkEvaluator;
    delete mainwindow;

    vapp.deinitGL();
    vapp.deinit();
}
