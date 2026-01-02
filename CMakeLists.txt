cmake_minimum_required(VERSION 3.31)
project(BumbleBee)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Enable AddressSanitizer only for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building with AddressSanitizer in Debug mode")
    add_compile_options(-fsanitize=address -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# Third-party includes
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# ----------------------------------------
# Build project code as a library
# ----------------------------------------
file(GLOB_RECURSE SRC_FILES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/third_party/*.cpp
)

# Exclude main.cpp from the library
list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp")

add_library(bumblebee_lib ${SRC_FILES})
target_include_directories(bumblebee_lib PUBLIC
        ${PROJECT_SOURCE_DIR}/src/include
)

# Disable asserts in Release
target_compile_definitions(bumblebee_lib
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

# ----------------------------------------
# Main application executable
# ----------------------------------------
add_executable(${PROJECT_NAME} src/main/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE bumblebee_lib)

# ----------------------------------------
# Google Test setup
# ----------------------------------------
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Unit test sources
file(GLOB_RECURSE UNIT_TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/test/unit/*.cpp
)

add_executable(unit_tests ${UNIT_TEST_SOURCES} )

target_link_libraries(unit_tests
        bumblebee_lib
        gtest_main
)

target_include_directories(unit_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/src/include
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
