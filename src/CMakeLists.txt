CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(MatlabFileTestApp)

SET (MatlabFileTestApp_VERSION_MAJOR 1)
SET (MatlabFileTestApp_VERSION_MINOR 0)

FIND_PACKAGE(HDF5 REQUIRED)

INCLUDE_DIRECTORIES(
	${HDF5_INCLUDE_DIRS}
	"example"
    "libMatlabFile"
)

ADD_LIBRARY(libMatlabFile
	libMatlabFile/MatlabFile.cpp
)
TARGET_LINK_LIBRARIES(libMatlabFile
	${HDF5_CXX_LIBRARIES}
	${HDF5_LIBRARIES}
)

ADD_EXECUTABLE(MatlabFileTestApp
	example/matlab_file_test.cpp
)
TARGET_LINK_LIBRARIES(MatlabFileTestApp
	libMatlabFile
)
