cmake_minimum_required(VERSION 3.16)
project(MultiCast_Module VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type." FORCE)
endif()

option(BUILD_SHARED_LIBS "Build shared libraries by default." OFF)
option(ENABLE_UNIT_TESTS "Enable unit tests." OFF)

# Enable compiler warnings for GCC/Clang and MSVC.
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(ThirdParty/spdlog)
add_subdirectory(src)
