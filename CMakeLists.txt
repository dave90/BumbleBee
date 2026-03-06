cmake_minimum_required(VERSION 3.31)

# When building via pip (scikit-build-core sets SKBUILD) on macOS, prefer
# Apple's system Clang over any Homebrew LLVM that may appear first in PATH.
# Must be set before the first project() call.
if(SKBUILD AND APPLE)
    if(NOT DEFINED CMAKE_C_COMPILER)
        set(CMAKE_C_COMPILER /usr/bin/clang CACHE FILEPATH "" FORCE)
    endif()
    if(NOT DEFINED CMAKE_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE FILEPATH "" FORCE)
    endif()
endif()

project(BumbleBee)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# Enable AddressSanitizer only for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building with AddressSanitizer in Debug mode")
    add_compile_options(-fsanitize=address -O0 -g -fno-omit-frame-pointer)
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
        ${PROJECT_SOURCE_DIR}/third_party/*.cc
)

# Exclude main.cpp from the library
list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp")

add_library(bumblebee_lib ${SRC_FILES})
target_include_directories(bumblebee_lib PUBLIC
        ${PROJECT_SOURCE_DIR}/src/include
)

# Disable asserts in Release; set log level to WARNING in Release
target_compile_definitions(bumblebee_lib
        PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:Release>:LOG_LEVEL=2>
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

# ----------------------------------------
# Python binding module (optional)
# ----------------------------------------


# If pybind11_DIR was not provided via command line, auto-detect it from Python.
#if(NOT pybind11_DIR OR pybind11_DIR STREQUAL "pybind11_DIR-NOTFOUND")
foreach(_python_cmd python3 python)
    execute_process(
        COMMAND ${_python_cmd} -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE _pybind11_cmake_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE _pybind11_result
    )
    if(_pybind11_result EQUAL 0 AND _pybind11_cmake_dir)
        set(pybind11_DIR "${_pybind11_cmake_dir}" CACHE PATH "pybind11 cmake dir (auto-detected)" FORCE)
        break()
    endif()
endforeach()
#endif()
find_package(pybind11 CONFIG QUIET)

if(pybind11_FOUND)
    file(GLOB_RECURSE PYTHON_SRC_FILES ${PROJECT_SOURCE_DIR}/python/src/*.cpp)
    pybind11_add_module(pyBumbleBee ${PYTHON_SRC_FILES})
    set_target_properties(pyBumbleBee PROPERTIES OUTPUT_NAME bumblebee)
    target_link_libraries(pyBumbleBee PRIVATE bumblebee_lib)
    target_include_directories(pyBumbleBee PRIVATE
        ${PROJECT_SOURCE_DIR}/src/include
        ${PROJECT_SOURCE_DIR}/python/src
        ${PROJECT_SOURCE_DIR}/third_party   # resolves <pybind11/pybind11.h>
    )
    message(STATUS "Python bindings target 'pyBumbleBee' configured (pybind11 ${pybind11_VERSION})")
    install(TARGETS pyBumbleBee DESTINATION . COMPONENT python)
endif()
