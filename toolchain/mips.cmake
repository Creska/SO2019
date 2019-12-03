set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mips)

# C cross compiler setup
find_program(CROSS_GCC mipsel-linux-gnu-gcc)
if (NOT EXISTS ${CROSS_GCC})
    message(FATAL_ERROR "Couldn't find a mipsel C compiler")
endif()
set(CMAKE_C_COMPILER ${CROSS_GCC})

# adding appropriate compiler option and definitions, they will be applied to all targets
add_compile_options(-ansi -mips1 -mfp32)
add_compile_definitions(TARGET_UMPS)


# Linker setup
find_program(CROSS_LD mipsel-linux-gnu-ld)
if (NOT EXISTS ${CROSS_LD})
    message(FATAL_ERROR "Couldn't find a mipsel linker")
endif()
set(CMAKE_C_LINK_EXECUTABLE "${CROSS_LD}  <FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")

# adding appropriate compiler option and definitions, they will be applied to all targets
add_link_options(-nostdlib -T /usr/local/share/umps2/umpscore.ldscript)


# Finding relevant files and directories (include directory and lib object files for linking)
find_path(CORE_INCLUDE_PATH libumps.h REQUIRED
        HINTS /usr/local/include/umps2/umps)
if (NOT EXISTS ${CORE_INCLUDE_PATH})
    message(SEND_ERROR "Couldn't find the core include directory.")
endif()

find_file(CORE_OBJ_UMPS libumps.o REQUIRED
        HINTS /usr/local/lib/umps2)
if (NOT EXISTS ${CORE_OBJ_UMPS})
    message(SEND_ERROR "Couldn't find the core libumps object file.")
endif()

find_file(CORE_OBJ_CRTSO crtso.o REQUIRED
        HINTS /usr/local/lib/umps2)
if (NOT EXISTS ${CORE_OBJ_CRTSO})
    message(SEND_ERROR "Couldn't find the core crtso object file.")
endif()

set(CORE_LIBS ${CORE_OBJ_CRTSO} ${CORE_OBJ_UMPS})


# Macro for executable targets that might need a conversion in order to be run on their emulator
macro (executable_post_build_conversion TAR)
    get_target_property(TARGET_TYPE ${TAR} TYPE)
    if (TARGET_TYPE STREQUAL "EXECUTABLE")
        add_custom_command(TARGET ${TAR} POST_BUILD COMMAND /usr/local/bin/umps2-elf2umps -k kernel)
    else()
        message (FATAL_ERROR "The macro executable_post_build_conversion only accepts executable targets")
    endif()
endmacro(executable_post_build_conversion)







