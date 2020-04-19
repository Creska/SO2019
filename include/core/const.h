#ifndef _CONST_H
#define _CONST_H

/**************************************************************************** 
 *
 * This header file contains the global constant & macro definitions.
 * 
 ****************************************************************************/

/* Maxi number of overall (eg, system, daemons, user) concurrent processes */
#define MAXPROC 20

#define UPROCMAX 3  /* number of usermode processes (not including master proc
		       and system daemons */

#define	HIDDEN static
#define	TRUE 	1
#define	FALSE	0
#define ON 	1
#define OFF 	0
#define EOS '\0'

#define DEV_PER_INT 8 /* Maximum number of devices per interrupt line */

#define CR 0x0a   /* carriage return as returned by the terminal */



// From phase 2 const -------------------------------------------------------------------------------------------------

#ifdef TARGET_UMPS


/* Values for CP0 Cause.ExcCode */
#define EXC_INTERRUPT      0
#define EXC_TLBMOD         1
#define EXC_TLBINVLOAD     2
#define EXC_TLBINVSTORE    3
#define EXC_ADDRINVLOAD    4
#define EXC_ADDRINVSTORE   5
#define EXC_BUSINVFETCH    6
#define EXC_BUSINVLDSTORE  7
#define EXC_SYSCALL        8
#define EXC_BREAKPOINT     9
#define EXC_RESERVEDINSTR  10
#define EXC_COPROCUNUSABLE 11
#define EXC_ARITHOVERFLOW  12
#define EXC_BADPTE         13
#define EXC_PTEMISS        14

/* Interrupt lines used by the devices */
#define INT_T_SLICE  1 /* time slice interrupt */
#define INT_TIMER    2 /* timer interrupt */
#define INT_LOWEST   3 /* minimum interrupt number used by real devices */
#define INT_DISK     3
#define INT_TAPE     4
#define INT_UNUSED   5 /* network? */
#define INT_PRINTER  6
#define INT_TERMINAL 7

#define DEV_USED_INTS 5 /* Number of ints reserved for devices: 3,4,5,6,7 */
#define DEV_PER_INT   8 /* Maximum number of devices per interrupt line */

#endif

#ifdef TARGET_UARM
#include "uARMconst.h"
#endif


/* nucleus (phase2)-handled SYSCALL values */
#define GETCPUTIME       1
#define CREATEPROCESS    2
#define TERMINATEPROCESS 3
#define VERHOGEN         4
#define PASSEREN         5
#define WAITIO           6
#define SPECPASSUP       7
#define GETPID           8

#define DEFAULT_PRIORITY 1



#endif
