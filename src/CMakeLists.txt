target_sources(kernel PRIVATE                   # Adds this files as sources to our target
        main.c
        testing/p1.5test_bikaya_v0.c
        testing/our_tests.c

        core/scheduler.c
        core/interrupts.c
        core/handler.c

        asl.c
        pcb.c
        utils.c

        devices/printer.c
        devices/terminal.c

        )

add_subdirectory(core/arch)

