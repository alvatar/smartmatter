cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0015 NEW)

###############################################################################
## IPCC
###############################################################################

project(ipcc)

# Options
set(FORK_IPVR FALSE)

find_package(Boost REQUIRED)

include_directories(
    "common"
	"ext"
	)

file(
	GLOB PROJECT_SOURCES
    "ipcc/ca_algorithms/basic.cpp"
	"ipcc/ipcc.cpp"
	)

add_executable(
	ipcc
	${PROJECT_SOURCES}
	)

if(FORK_IPVR)
	message("IPVR forking activated")
	set_property(
		TARGET
		ipcc
		PROPERTY
		COMPILE_DEFINITIONS
		_FORK_IPVR
		)
endif(FORK_IPVR)

target_link_libraries(
	ipcc
    rt
	voreen
	)

###############################################################################
## IPVR
###############################################################################

project(ipvr)

include_directories(
	"common"
	"ext"
	"ext/voreen/inc/"
	"ipvr"
	"ipcc"
	)

find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)
find_package(Boost REQUIRED)
set( QT_USE_QTOPENGL TRUE )  
include( ${QT_USE_FILE} )

link_directories(
	"ext/tinyxml/dep"
	"ext/tgt/dep"
	"ext/voreen/dep"
	)

add_subdirectory(
	"ext/tgt"
	)
add_subdirectory(
	"ext/tinyxml"
	)
add_subdirectory(
	"ext/voreen"
	)

file(
	GLOB PROJECT_SOURCES
	"ipvr/ipvr.cpp"
	)

add_executable(
	ipvr
	${PROJECT_SOURCES}
	)

target_link_libraries(
	ipvr
	GL
	GLU
	#glut
	${QT_LIBRARIES}
	GLEW
	tgt
	tinyxml
	voreen
	)
