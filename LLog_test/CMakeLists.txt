# CMakeList.txt: ChatClient's CMake project, 
#	where source code is included and project-specific logic is defined.
#
CMAKE_MINIMUM_REQUIRED (VERSION 3.8)

# LLog
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/LLog)

FILE(GLOB PROJECT_SOURCES "*.cpp")

# Add the source code to the project's executable。
ADD_EXECUTABLE (LLog_test   ${PROJECT_SOURCES}
 				            )
ADD_DEPENDENCIES(LLog_test  LLog)

TARGET_LINK_LIBRARIES(LLog_test PRIVATE
                                LLog
                                )