# CMakeList.txt: LLog's CMake project, 
#	where source code is included and project-specific logic is defined.
#
CMAKE_MINIMUM_REQUIRED (VERSION 3.8)

FILE(GLOB PROJECT_SOURCES "src/*.cpp")

# Add the source code to LLog。
ADD_LIBRARY (LLog STATIC ${PROJECT_SOURCES})
