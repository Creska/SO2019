target_sources(kernel PRIVATE system.c )

if(CMAKE_SYSTEM_PROCESSOR MATCHES mips)
    target_sources(kernel PRIVATE system_mps.c )
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES arm)
    target_sources(kernel PRIVATE system_arm.c )
endif()

