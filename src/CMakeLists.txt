target_sources(kernel PRIVATE                   # Adds this files as sources to our target
        main.c
        testing/p1.5test_bikaya_v0.c
        testing/our_tests.c

        bikaya.c

        core/processes/scheduler.c
        core/exceptions/interrupts.c
        core/exceptions/handler.c
        core/exceptions/syscalls.c

        core/processes/asl.c
        core/processes/pcb.c

        utils/utils.c
        utils/debug.c

        devices/printer.c
        devices/terminal.c

        )

add_subdirectory(core/system)

