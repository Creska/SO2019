#include "devices/terminal.h"
#include "core/system.h"


#define TERM_TX_ST_SPECIFIC             5

#define TERM_CHAR_OFFSET                8
#define TERM_STATUS_MASK                0xFF


struct terminal {
    termreg_t *reg;
};

static terminal terminals[N_DEV_PER_IL];


terminal* get_terminal(unsigned int device_index) {
    if (terminals[device_index].reg==NULL) {
        terminals[device_index].reg = ((termreg_t *) DEV_REG_ADDR(IL_TERMINAL, device_index));
    }
    return &terminals[device_index];
}

static unsigned int term_tx_status(terminal * tp) {
    return ((tp->reg->transm_status) & TERM_STATUS_MASK);
}

static unsigned int term_rx_status(terminal* tp) {
    return ((tp->reg->recv_status) & TERM_STATUS_MASK);
}

int term_putchar(terminal* term, char c) {
    unsigned int stat = term_tx_status(term);
    if (stat != DEVICE_ST_READY && stat != TERM_TX_ST_SPECIFIC)
        return -1;

    term->reg->transm_command = ((c << TERM_CHAR_OFFSET) | DEVICE_CMD_FUNC_SPECIFIC);
    while ((stat = term_tx_status(term)) == DEVICE_ST_BUSY)
        ;

    term->reg->transm_command = DEVICE_CMD_ACK;

    if (stat != TERM_TX_ST_SPECIFIC) return -1;
    else return 0;
}

char term_getchar(terminal* term) {
    unsigned int stat = term_tx_status(term);
    if (stat != DEVICE_ST_READY && stat != TERM_TX_ST_SPECIFIC)
        return TERM_READ_ERROR_RETURN_CHAR;

    term->reg->recv_command = DEVICE_CMD_FUNC_SPECIFIC;

    while ((stat = term_rx_status(term)) == DEVICE_ST_BUSY)
        ;

    char c = (char)(term->reg->recv_status >> TERM_CHAR_OFFSET);
    term->reg->recv_command = DEVICE_CMD_ACK;

    if (stat != TERM_TX_ST_SPECIFIC) return TERM_READ_ERROR_RETURN_CHAR;
    else return c;
}

void term_putstr(terminal* term_reg, const char *str) {
    while (*str)
        if (term_putchar(term_reg, *str++))
            return;
}

/* This function places the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp) {
    term_putstr(get_terminal(0), strp);
}


/* This function places the specified character string in errbuf and
 *	causes the string to be written out to terminal0.  After this is done
 *	the system shuts down with a panic message */
void adderrbuf(char *strp) {

    term_putstr(get_terminal(0), strp);

    PANIC();
}

char* itoa(int i, char b[]) {
    char const digit[] = "0123456789";
    char* p = b;

    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
