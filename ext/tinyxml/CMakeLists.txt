cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(tinyxml)

add_definitions(-fPIC)

file(
	GLOB PROJECT_SOURCES
	./tinystr.h
	./tinyxml.h
	./tinystr.cpp
	./tinyxml.cpp
	./tinyxmlparser.cpp
	./tinyxmlerror.cpp
	)

add_library(
	tinyxml
	${PROJECT_SOURCES}
	)
