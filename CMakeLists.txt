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

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include the headers (optional, if needed)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/src/include
)
