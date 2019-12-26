#include "terminal.h"

#define TERM_TX_ST_TRANSMITTED  5
#define TERM_RX_ST_RECEIVED     5

#define TERM_CMD_TRANSMIT       2
#define TERM_CMD_RECEIVE        2

#define TERM_CHAR_OFFSET        8
#define TERM_STATUS_MASK        0xFF

#define TERM_READ_ERROR_RETURN_CHAR    '\a'

termreg_t* get_terminal(unsigned int device_index) {
    return ((termreg_t *)DEV_REG_ADDR(IL_TERMINAL, device_index));
}

static unsigned int term_tx_status(termreg_t* tp) {
    return ((tp->transm_status) & TERM_STATUS_MASK);
}

static unsigned int term_rx_status(termreg_t* tp) {
    return ((tp->recv_status) & TERM_STATUS_MASK);
}

int term_putchar(termreg_t* term_reg, char c) {
    unsigned int stat = term_tx_status(term_reg);
    if (stat != DEVICE_ST_READY && stat != TERM_TX_ST_TRANSMITTED)
        return -1;

    term_reg->transm_command = ((c << TERM_CHAR_OFFSET) | TERM_CMD_TRANSMIT);

    while ((stat = term_tx_status(term_reg)) == DEVICE_ST_BUSY)
        ;

    term_reg->transm_command = DEVICE_CMD_ACK;

    if (stat != TERM_TX_ST_TRANSMITTED) return -1;
    else return 0;
}

char term_getchar(termreg_t* term_reg) {
    unsigned int stat = term_tx_status(term_reg);
    if (stat != DEVICE_ST_READY && stat != TERM_RX_ST_RECEIVED)
        return TERM_READ_ERROR_RETURN_CHAR;

    term_reg->recv_command = TERM_CMD_RECEIVE;

    while ((stat = term_rx_status(term_reg)) == DEVICE_ST_BUSY)
        ;

    char c = (char)(term_reg->recv_status >> TERM_CHAR_OFFSET);
    term_reg->recv_command = DEVICE_CMD_ACK;

    if (stat != TERM_TX_ST_TRANSMITTED) return TERM_READ_ERROR_RETURN_CHAR;
    else return c;
}

void term_putstr(termreg_t* term_reg, const char *str) {
    while (*str)
        if (term_putchar(term_reg, *str++))
            return;
}