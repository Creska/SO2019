#include "printer.h"
#include "terminal.h"

#define TARGET_TERM_INDEX       0
#define TARGET_PRINTER_INDEX    0



int main(void)
{
    /* Get the addresses of the target devices' registers */
    termreg_t* target_term = get_terminal(TARGET_TERM_INDEX);
    dtpreg_t* target_printer = get_printer(TARGET_PRINTER_INDEX);

    if (!is_printer_installed(target_printer)) {
        term_putstr(target_term, "The targeted printer isn't installed\n");
        return 1;
    }

    char c;
    while ((c = term_getchar(target_term)) != '\a') {
        printer_putchar(target_printer, c);
        if (c=='\n') break;
    }

    while (1)
        WAIT();
}

