cmake_minimum_required(VERSION 3.31)
project(BumbleBee)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

# Include Third party
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Gather source files
file(GLOB_RECURSE SRC_FILES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES}
        src/BumbleBeeDB.cpp
        src/include/bumblebee/BumbleBeeDB.h
        src/common/Log.cpp
        src/include/bumblebee/common/Log.h)

# Include the headers (optional, if needed)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/src/include
)

#
#add_executable(BumbleBee
#        src/main/main.cpp
#        src/include/parser/InputBuilder.h
#        src/parser/InputBuilder.cpp
#        src/parser/InputDirector.cpp
#        src/include/parser/InputDirector.h
#        src/include/common/Assert.cpp
#        src/include/common/Assert.h
#        src/common/ErrorMessage.cpp
#        src/include/common/ErrorMessage.h
#        src/include/common/Constants.h
#)
