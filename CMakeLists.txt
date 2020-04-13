# TOOLCHAIN FILE MANAGEMENT -------------------------------------------------------------------------------------------

if (CMAKE_TOOLCHAIN_FILE MATCHES "")                                                # If no toolchain file was directly specified we can check if our custom ARCH variable
    if(ARCH MATCHES "uarm" OR ARCH MATCHES "arm")                                   # was set to supported emulators/architectures names
        message("UARM set")
        SET(CMAKE_TOOLCHAIN_FILE ./toolchain/uarm.cmake)                            # this needs to happen before the call to project(bikaya...) because at that point the toolchain file is executed
    elseif(ARCH MATCHES "umps" OR ARCH MATCHES "umps2" OR ARCH MATCHES "mips")
        message("UMPS set")
        SET(CMAKE_TOOLCHAIN_FILE ./toolchain/umps2.cmake)
    endif()
endif()


# BUILD TYPE MANAGEMENT -----------------------------------------------------------------------------------------------

# For now defaults to debug build type if not set through the cmake cache or cmake commands flags
if(NOT CMAKE_BUILD_TYPE)
    message("Since CMAKE_BUILD_TYPE is not defined, defaulting to Debug, you can change it through ccmake or cmake-gui")
    set( CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_compile_definitions(DEBUG)
    set(CMAKE_VERBOSE_MAKEFILE TRUE)                                                    # forces make to show the executed commands at build time
elseif(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelMixSize)
    SET(CMAKE_BUILD_TYPE Custom)                                                        # with build type set to Release or similar cmake sets some flags that are incompatible with our cross compilers
endif()                                                                                 # setting the build type to custom prevents that



# PROJECT SETUP -------------------------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13...3.17)                             # Defines a range of tested CMake versions, important since the language and paradigms have changed a lot between versions
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")                     # This allows our cross compilers to pass CMake tests even without stdlib and standard flags

message("Pre project setup")
project(bikaya-phase0 LANGUAGES C)                                      # defines the project name and the enabled languages

add_executable(kernel)                                                  # Creates an executable target
target_compile_options(kernel PUBLIC -std=gnu99)

target_include_directories(kernel PUBLIC include)                       # Our kernel target needs to include the ./include directory on compilation...
target_include_directories(kernel PUBLIC ${CORE_INCLUDE_PATH})          # ...and the directory containing the headers for the core libs
                                                                        # variables like CORE_INCLUDE_PATH and CORE_LIBS are defined by toolchain files
target_link_libraries(kernel PUBLIC ${CORE_LIBS})                       # Link our target to the core libs
executable_post_build_conversion(kernel)                                # Run a macro that needs to be declared by the toolchain files
                                                                        # for setting post-build conversion operations on an executable target

add_subdirectory(src)                                                   # runs the CMake code in the CMakeLists.txt in the given dir