set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(CROSS_GCC arm-none-eabi-gcc)
if (NOT EXISTS ${CROSS_GCC})
    message(FATAL_ERROR "Couldn't find an ARM C compiler")
endif()
set(CMAKE_C_COMPILER ${CROSS_GCC})

# adding appropriate compiler option and definitions, they will be applied to all targets
add_compile_options(-O0 -mcpu=arm7tdmi)
add_compile_definitions(TARGET_UARM)


# Linker setup
find_program(CROSS_LD arm-none-eabi-ld)
if (NOT EXISTS ${CROSS_LD})
    message(FATAL_ERROR "Couldn't find an ARM linker")
endif()
set(CMAKE_C_LINK_EXECUTABLE "${CROSS_LD}  <FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

# adding appropriate compiler option and definitions, they will be applied to all targets
add_link_options(-T/usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -nostdlib -G0)


# Finding relevant files and directories (include directory and lib object files for linking)
find_path(CORE_INCLUDE_PATH libuarm.h REQUIRED
        HINTS /usr/include/uarm)
if (NOT EXISTS ${CORE_INCLUDE_PATH})
    message(SEND_ERROR "Couldn't find the core include directory.")
endif()

find_file(CORE_OBJ_UMPS libuarm.o REQUIRED
        HINTS /usr/include/uarm)
if (NOT EXISTS ${CORE_OBJ_UMPS})
    message(SEND_ERROR "Couldn't find the core libumps object file.")
endif()

find_file(CORE_OBJ_CRTSO crtso.o REQUIRED
        HINTS /usr/include/uarm)
if (NOT EXISTS ${CORE_OBJ_CRTSO})
    message(SEND_ERROR "Couldn't find the core crtso object file.")
endif()

find_file(CORE_OBJ_LIBDIV libdiv.o REQUIRED                  # We need to link libdiv in order to be able to do divisions
        HINTS /usr/include/uarm)
if (NOT EXISTS ${CORE_OBJ_LIBDIV})
    message(SEND_ERROR "Couldn't find the core libdiv object file.")
endif()

set(CORE_LIBS ${CORE_OBJ_CRTSO} ${CORE_OBJ_UMPS} ${CORE_OBJ_LIBDIV})


# Macro for executable targets that might need a conversion in order to be run on their emulator
macro (executable_post_build_conversion TAR)
    get_target_property(TARGET_TYPE ${TAR} TYPE)
    if (TARGET_TYPE STREQUAL "EXECUTABLE")
        # Nothing to be done for this architecture
    else()
        message (FATAL_ERROR "The macro executable_post_build_conversion only accepts executable targets")
    endif()
endmacro(executable_post_build_conversion)



