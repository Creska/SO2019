if (CMAKE_TOOLCHAIN_FILE MATCHES "")                                                # If no toolchain file was directly specified we can check if our custom ARCH variable
    if(ARCH MATCHES "uarm" OR ARCH MATCHES "arm")                                   # was set to supported emulators/architectures names
        SET(CMAKE_TOOLCHAIN_FILE ./toolchain/uarm.cmake)                            # this needs to happen before the call to project(bikaya...) because at that point the toolchain file is executed
    elseif(ARCH MATCHES "umps" OR ARCH MATCHES "umps2" OR ARCH MATCHES "mips")
        SET(CMAKE_TOOLCHAIN_FILE ./toolchain/umps2.cmake)
    endif()
endif()


cmake_minimum_required(VERSION 3.13...3.16)                             # Defines a range of tested CMake versions, important since the language and paradigms have changed a lot between versions
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")                     # This allows our cross compilers to pass CMake tests even without stdlib and standard flags


project(bikaya-phase0 LANGUAGES C)                                      # defines the project name and the enabled languages


# set(CMAKE_VERBOSE_MAKEFILE TRUE)                                        # TEMPORARY - forces make to show the executed commands

add_executable(kernel)                                                  # Creates an executable target
target_compile_options(kernel PUBLIC -std=gnu99)

target_include_directories(kernel PUBLIC include)                       # Our kernel target needs to include the ./include directory on compilation...
target_include_directories(kernel PUBLIC ${CORE_INCLUDE_PATH})          # ...and the directory containing the headers for the core libs
                                                                        # variables like CORE_INCLUDE_PATH and CORE_LIBS are defined by toolchain files
target_link_libraries(kernel PUBLIC ${CORE_LIBS})                       # Link our target to the core libs
executable_post_build_conversion(kernel)                                # Run a macro that needs to be declared by the toolchain files
                                                                        # for setting post-build conversion operations on an executable target

add_subdirectory(src)                                                   # runs the CMake code in the CMakeLists.txt in the given dir