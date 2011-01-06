cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(tgt)

find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)
set( QT_USE_QTOPENGL TRUE )  
include( ${QT_USE_FILE} )

add_definitions(-DLINUX)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_NO_DEBUG)
add_definitions(-fPIC)

include_directories("..")

file(
	GLOB PROJECT_SOURCES
	./bounds.cpp
	./tesselator.cpp
	./stopwatch.cpp
	./gpucapabilitieswindows.cpp
	./frustum.cpp
	./framecounter.cpp
	./camera.cpp
	./tgt_gl.cpp
	./filesystem.cpp
	./curve.cpp
	./event/eventlistener.cpp
	./event/mouseevent.cpp
	./event/eventhandler.cpp
	./event/timeevent.cpp
	./event/keyevent.cpp
	./texturemanager.cpp
	./exception.cpp
	./spline.cpp
	./memorymanager.cpp
	./init.cpp
	./gpucapabilities.cpp
	./modelreader.cpp
	./logmanager.cpp
	./textureunit.cpp
	./light.cpp
	./submesh.cpp
	./guiapplication.cpp
	./model.cpp
	./modelmanager.cpp
	./stringutil.cpp
	./font.cpp
	./texturereaderdevil.cpp
	./texturereader.cpp
	./texturereadertga.cpp
	./mesh.cpp
	./timer.cpp
	#./glut/gluttimer.cpp
	#./glut/glutcanvas.cpp
	#./glut/glutmouse.cpp
	#./glut/glutapplication.cpp
	#./glut/toolkitfactory.cpp
	./texture.cpp
	./framebufferobject.cpp
	#./vertexarray.cpp
	./painter.cpp
	./skybox.cpp
	#./stereopainter.cpp
	./shadermanager.cpp
	./assert.cpp
	./navigation/navigation.cpp
	./navigation/trackball.cpp
	./ziparchive.cpp
	./ffmpeg/videoencoder.cpp
	./glcanvas.cpp
	./animation/circleanimation.cpp
	./animation/bsplineanimation.cpp

	# Qt
	./qt/qttimer.cpp
	./qt/qtcanvas.cpp
	./qt/qtapplication.cpp
	)

add_library(
	tgt
	${PROJECT_SOURCES}
	)
