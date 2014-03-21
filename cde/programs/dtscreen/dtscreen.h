/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: dtscreen.h /main/5 1996/05/13 16:54:19 rswiston $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 *
 * xlock.h - external interfaces for new modes and SYSV OS defines.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <Dt/Dt.h>

#define MAXSCREENS 3
#define NUMCOLORS 64

typedef struct {
    Screen     *screen;
    int         npixels;	/* number of valid entries in pixels */
    u_long      pixels[NUMCOLORS];	/* pixel values in the colormap */
}           perscreen;

typedef struct {
    Window      w; /* window id */
    GC          gc; /* graphics context for animation */
    perscreen  *perscreen; /* screen info for this window */
    void       *data; /* mode data */
}           perwindow;

extern Display *dsp;

extern char *ProgramName;
extern char *display;
extern char *mode;
extern float saturation;
extern int  nicelevel;
extern int  delay;
extern int  batchcount;
extern int  reinittime;
extern Bool usefirst;
extern Bool mono;
extern Bool create;
extern void (*callback) ();
extern void (*init) ();

extern void GetResources();
extern void CheckResources();
extern void hsbramp();
extern void error();
extern long seconds();
extern void usage();
extern void hide_hft_ring();
extern void unhide_hft_ring();


/* System V Release 4 redefinitions of BSD functions and structures */

#if defined (SYSV) || defined (SVR4)

#include <sys/time.h>
#define passwd spwd
#define pw_passwd sp_pwdp
#if defined(AIXV3) || defined(hpux)
#undef NULL
#define NULL 0
#include <sys/poll.h>

struct passwd {
        char    *pw_name;
        char    *pw_passwd;
        uid_t   pw_uid;
        gid_t   pw_gid;
        char    *pw_gecos;
        char    *pw_dir;
        char    *pw_shell;
};
#else
#include <poll.h>
#include <shadow.h>
#define getpwnam getspnam
#endif
#if !(defined(_AIX) || defined(hpux) || defined(sun))
# define srandom srand
# define random rand
# define MAXRAND (32767.0)
#else
# define MAXRAND (2147483648.0)
#endif

#else

#define MAXRAND (2147483648.0)

#endif
