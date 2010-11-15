cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(SET CMP0015 NEW)
project(ipvr)

include_directories(
	"ext"
	"ext/voreen/inc/"
	)

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
	"src/main.cpp"
	)

add_executable(
	ipvr
	${PROJECT_SOURCES}
	)

target_link_libraries(
	ipvr
	GL
	GLU
	glut
	GLEW
	tgt
	tinyxml
	voreen
	)
