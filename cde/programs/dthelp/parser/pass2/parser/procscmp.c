/* $XConsortium: procscmp.c /main/3 1995/11/08 10:57:41 rswiston $ */
/*
              Copyright 1986 Tandem Computers Incorporated.
This product and information is proprietary of Tandem Computers Incorporated.
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Procscmp.c prefixes the interface's procs.c file with appropriate
   declarations so it can be compiled. */

#include "userinc.h"
#include "globdec.h"

/* Include any procedures specified by the interface designer */
#include "procs.c"


