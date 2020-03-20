#include "devices/terminal.h"

int main() {

    // If you want to test the emulators just compile this for your target emulator and run it
    // The following string should be printed in terminal 0
    term_putstr(get_terminal(0), "If you're reading this the emulator is working");

    return 0;
}

