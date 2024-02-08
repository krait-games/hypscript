cmake_minimum_required(VERSION 3.5)

set(BUILD_NAME hypscript)

set(vcpkg_path "$ENV{VCPKG_INSTALLED_DIR}/../scripts/buildsystems/vcpkg.cmake")
project(${BUILD_NAME} CXX C)

# set HYP_PLATFORM_NAME
if(WIN32)
    set(HYP_PLATFORM_NAME "windows")
elseif(APPLE)
    set(HYP_PLATFORM_NAME "macos")
elseif(UNIX)
    set(HYP_PLATFORM_NAME "linux")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(MSVC)
    message(STATUS "Building on MSVC, checking toolchain is set (should be set to `<vcpkg root path>/scripts/buildsystems/vcpkg.cmake`)")

    if (DEFINED CMAKE_TOOLCHAIN_FILE)
        message(STATUS "Using CMake toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
    else()
        message(SEND_ERROR "CMake toolchain file not specified -- should be set to vcpkg cmake file! \
            If you don't have vcpkg set up, download vcpkg from Microsoft and set the `-DCMAKE_TOOLCHAIN_FILE` CMake argument to \
            `<vcpkg root path>/scripts/buildsystems/vcpkg.cmake`.")
    endif()

    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /MD")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fsanitize=address")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

find_package(Threads REQUIRED)

add_definitions(-DHYP_ROOT_DIR="${CMAKE_CURRENT_LIST_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHYP_ROOT_DIR=\\\"${CMAKE_CURRENT_LIST_DIR}\\\"")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp" "src/*.inl")

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to be used")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HYPERION_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(${BUILD_NAME} ${SOURCES} ${HEADERS})

set(COMMON_LIBRARIES
    ${COMMON_LIBRARIES}
    ${CMAKE_THREADS_LIBS_INIT}
)

target_link_libraries(${BUILD_NAME} PUBLIC ${COMMON_LIBRARIES})

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})