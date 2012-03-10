/* $XConsortium: include_files.h /main/3 1995/10/31 12:00:37 rswiston $ */
#ifndef DO_NOT_NEED_NOW
#define DO_NOT_NEED_NOW
#endif

#ifndef _HPUX_SOURCE
#define _HPUX_SOURCE
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <ctype.h>
#ifdef sun
#include <string.h>
#else
#include <strings.h>
#endif
#include <unistd.h>

#include "p2c.h"

