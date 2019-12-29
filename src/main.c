#include "printer.h"
#include "terminal.h"
#include "system.h"


int main(void)
{
    terminal* target_term = get_terminal(0);
    printer* target_printer = get_printer(0);

    if (!is_printer_installed(target_printer)) {
        term_putstr(target_term, "The targeted printer isn't installed\n");
        return 1;
    }

    char c;
    while ((c = term_getchar(target_term)) != TERM_READ_ERROR_RETURN_CHAR) {
        printer_putchar(target_printer, c);
        if (c=='\n') break;
    }

    while (1)
        WAIT();
    return 0;
}

