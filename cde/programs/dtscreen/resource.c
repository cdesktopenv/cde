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
/* $XConsortium: resource.c /main/4 1996/06/19 09:47:56 mustafa $ */
/*
 */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*-
 * resource.c - resource management for dtscreen, the X Window System lockscreen.
 *
 * Copyright (c) 1991 by Patrick J. Naughton.
 *
 * See dtscreen.c for copying information.
 *
 * Revision History:
 * 25-Sep-91: added worm mode.
 * 06-Jun-91: Added flame mode.
 * 16-May-91: Added random mode and pyro mode.
 * 26-Mar-91: CheckResources: delay must be >= 0.
 * 29-Oct-90: Added #include <ctype.h> for missing isupper() on some OS revs.
 *	      moved -mode option, reordered Xrm database evaluation.
 * 28-Oct-90: Added text strings.
 * 26-Oct-90: Fix bug in mode specific options.
 * 31-Jul-90: Fix ':' handling in parsefilepath
 * 07-Jul-90: Created from resource work in dtscreen.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "dtscreen.h"
#include <netdb.h>
#include <math.h>
#include <ctype.h>

#include <X11/Xresource.h>

/*              include file for message texts          */
#include <limits.h>
#include <nl_types.h>
#define MF_DTSCREEN "dtscreen.cat"

#include <locale.h>
extern nl_catd  scmc_catd;   /* Cat descriptor for scmc conversion */

/*
 * Declare external interface routines for supported screen savers.
 */

extern void inithop();
extern void drawhop();

extern void initlife();
extern void drawlife();

extern void initqix();
extern void drawqix();

extern void initimage();
extern void drawimage();

extern void initblank();
extern void drawblank();

extern void initswarm();
extern void drawswarm();

extern void initrotor();
extern void drawrotor();

extern void initpyro();
extern void drawpyro();

extern void initflame();
extern void drawflame();

extern void initworm();
extern void drawworm();

void CheckResources(void);

typedef struct {
    char       *cmdline_arg;
    void        (*lp_init) ();
    void        (*lp_callback) ();
    int         def_delay;
    int         def_batchcount;
    float       def_saturation;
    char       *desc;
}           LockStruct;

static char randomstring[] = "random";

static LockStruct LockProcs[] = {
    {"hop", inithop, drawhop, 0, 1000, 1.0, "Hopalong iterated fractals"},
    {"qix", initqix, drawqix, 30000, 64, 1.0, "Spinning lines a la Qix(tm)"},
    {"image", initimage, drawimage, 2000000, 8, 0.3, "Random bouncing image"},
    {"life", initlife, drawlife, 1000000, 100, 1.0, "Conway's game of Life"},
    {"swarm", initswarm, drawswarm, 10000, 100, 1.0, "Swarm of bees"},
    {"rotor", initrotor, drawrotor, 10000, 4, 0.4, "Rotor"},
    {"pyro", initpyro, drawpyro, 15000, 40, 1.0, "Fireworks"},
    {"flame", initflame, drawflame, 10000, 20, 1.0, "Cosmic Flame Fractals"},
    {"worm", initworm, drawworm, 10000, 20, 1.0, "Wiggly Worms"},
    {"blank", initblank, drawblank, 5000000, 1, 1.0, "Blank screen"},
    {randomstring, NULL, NULL, 0, 0, 0.0, "Random mode"},
};
#define NUMPROCS (sizeof LockProcs / sizeof LockProcs[0])


extern char *getenv();

#ifndef DEF_FILESEARCHPATH
#define DEF_FILESEARCHPATH "/usr/lib/X11/%T/%N%S"
#endif
#define DEF_DISPLAY	":0"
#define DEF_MODE	"swarm"
#define DEF_BG		"White"
#define DEF_FG		"Black"
#define DEF_BC		"100"	/* vectors (or whatever) per batch */
#define DEF_DELAY	"200000"/* microseconds between batches */
#define DEF_NICE	"10"	/* dtscreen process nicelevel */
#define DEF_SAT		"1.0"	/* color ramp saturation 0->1 */
#define DEF_CLASSNAME	"Dtscreen"

static char *classname;
static char modename[1024];
static char modeclass[1024];

static XrmOptionDescRec genTable[] = {
    {"-mode", ".mode", XrmoptionSepArg, (caddr_t) NULL},
    {"-mono", ".mono", XrmoptionNoArg, (caddr_t) "on"},
    {"+mono", ".mono", XrmoptionNoArg, (caddr_t) "off"},
    {"-nice", ".nice", XrmoptionSepArg, (caddr_t) NULL},
    {"-create",  ".create", XrmoptionNoArg, (caddr_t) "on"},
};
#define genEntries (sizeof genTable / sizeof genTable[0])

/*************************************************************/
/** This table was changed for AIX.  In order to read these **/
/** command line options properly, the specifier field must **/
/** be built on the fly.                                    **/
/*************************************************************/
static XrmOptionDescRec modeTable[] = {
    {"-delay",      NULL, XrmoptionSepArg, (caddr_t) NULL},
    {"-batchcount", NULL, XrmoptionSepArg, (caddr_t) NULL},
    {"-saturation", NULL, XrmoptionSepArg, (caddr_t) NULL},
};
#define modeEntries (sizeof modeTable / sizeof modeTable[0])

static XrmOptionDescRec cmdlineTable[] = {
    {"-display", ".display", XrmoptionSepArg, (caddr_t) NULL},
    {"-xrm",     NULL,       XrmoptionResArg, (caddr_t) NULL},
};
#define cmdlineEntries (sizeof cmdlineTable / sizeof cmdlineTable[0])

static XrmOptionDescRec nameTable[] = {
    {"-name", ".name", XrmoptionSepArg, (caddr_t) NULL},
};


typedef struct {
    char       *opt;
    char       *desc;
}           OptionStruct;

static OptionStruct opDesc[] = {
    {"-help", "print out this message"},
    {"-resources", "print default resource file to standard output"},
    {"-display displayname", "X server to contact"},
    {"-/+mono", "turn on/off monochrome override"},
    {"-delay usecs", "microsecond delay between screen updates"},
    {"-batchcount num", "number of things per batch"},
    {"-nice level", "nice level for dtscreen process"},
    {"-saturation value", "saturation of color ramp"},
    {"-create", "create a window in which to draw"},
};
#define opDescEntries (sizeof opDesc / sizeof opDesc[0])

char       *display;
char       *mode;
float       saturation;
int         nicelevel;
int         delay;
int         batchcount;
Bool        mono;
Bool        create;


#define t_String	0
#define t_Float		1
#define t_Int		2
#define t_Bool		3

typedef struct {
    caddr_t    *var;
    char       *name;
    char       *class;
    char       *def;
    int         type;
}           argtype;

static argtype genvars[] = {
    {(caddr_t *) &nicelevel, "nice", "Nice", DEF_NICE, t_Int},
    {(caddr_t *) &mono, "mono", "Mono", "off", t_Bool},
    {(caddr_t *) &create, "create", "Create", "off", t_Bool},
};
#define NGENARGS (sizeof genvars / sizeof genvars[0])

static argtype modevars[] = {
    {(caddr_t *) &delay, "delay", "Delay", DEF_DELAY, t_Int},
    {(caddr_t *) &batchcount, "batchcount", "BatchCount", DEF_BC, t_Int},
    {(caddr_t *) &saturation, "saturation", "Saturation", DEF_SAT, t_Float},
};
#define NMODEARGS (sizeof modevars / sizeof modevars[0])


static void
Syntax(badOption)
    char       *badOption;
{
    int         col, len, i;

#ifdef MIT_R5
    fprintf(stderr, "%s:  bad command line option:  %s.\n\n",
            ProgramName, badOption);
#else
    fprintf(stderr, catgets(scmc_catd, 2, 1,
        "%s:  Bad command line option:  %s.\n\n"), 
        ProgramName, badOption);
#endif

    fprintf(stderr, "usage:  %s", ProgramName);
    col = 8 + strlen(ProgramName);
    for (i = 0; i < opDescEntries; i++) {
	len = 3 + strlen(opDesc[i].opt);	/* space [ string ] */
	if (col + len > 79) {
	    fprintf(stderr, "\n   ");	/* 3 spaces */
	    col = 3;
	}
	fprintf(stderr, " [%s]", opDesc[i].opt);
	col += len;
    }

    len = 8 + strlen(LockProcs[0].cmdline_arg);
    if (col + len > 79) {
	fprintf(stderr, "\n   ");	/* 3 spaces */
	col = 3;
    }
    fprintf(stderr, " [-mode %s", LockProcs[0].cmdline_arg);
    col += len;
    for (i = 1; i < NUMPROCS; i++) {
	len = 3 + strlen(LockProcs[i].cmdline_arg);
	if (col + len > 79) {
	    fprintf(stderr, "\n   ");	/* 3 spaces */
	    col = 3;
	}
	fprintf(stderr, " | %s", LockProcs[i].cmdline_arg);
	col += len;
    }
    fprintf(stderr, "]\n");

#ifdef MIT_R5
    fprintf(stderr, "\nType %s -help for a full description.\n\n",
            ProgramName);
#else
    fprintf(stderr, catgets(scmc_catd, 2, 2,
        "\nType %s -help for a full description.\n\n"), 
        ProgramName);
#endif
    exit(1);
}

static void
Help()
{
    int         i;

#ifdef MIT_R5
    fprintf(stderr, "usage:\n        %s [-options ...]\n\n", ProgramName);
    fprintf(stderr, "where options include:\n");

#else
    fprintf(stderr, catgets(scmc_catd, 2, 3,
        "Usage:\n        %s [-options ...]\n\n\
        where options include:\n"), ProgramName);
#endif

    for (i = 0; i < opDescEntries; i++) {
	fprintf(stderr, "    %-28s %s\n", opDesc[i].opt, opDesc[i].desc);
    }

#ifdef MIT_R5
    fprintf(stderr, "    %-28s %s\n", "-mode mode", "animation mode");
    fprintf(stderr, "    where mode is one of:\n");
#else
    fprintf(stderr, catgets(scmc_catd, 2, 5, 
         "    %-28s %s\n\t where mode is one of:\n"), 
         "-mode mode", "animation mode");
#endif
    for (i = 0; i < NUMPROCS; i++) {
	fprintf(stderr, "          %-23s %s\n",
		LockProcs[i].cmdline_arg, LockProcs[i].desc);
    }
    putc('\n', stderr);

    exit(0);
}

static void
DumpResources()
{
    int         i;

    printf("%s.mode: %s\n", classname, DEF_MODE);

    for (i = 0; i < NGENARGS; i++)
	printf("%s.%s: %s\n",
	       classname, genvars[i].name, genvars[i].def);

    for (i = 0; i < NUMPROCS - 1; i++) {
	printf("%s.%s.%s: %d\n", classname, LockProcs[i].cmdline_arg,
	       "delay", LockProcs[i].def_delay);
	printf("%s.%s.%s: %d\n", classname, LockProcs[i].cmdline_arg,
	       "batchcount", LockProcs[i].def_batchcount);
	printf("%s.%s.%s: %g\n", classname, LockProcs[i].cmdline_arg,
	       "saturation", LockProcs[i].def_saturation);
    }
    exit(0);
}


static void
LowerString(s)
    char       *s;
{

    while (*s) {
	if (isupper(*s))
	    *s += ('a' - 'A');
	s++;
    }
}

static void
GetResource(database, parentname, parentclass,
	    name, class, valueType, def, valuep)
    XrmDatabase database;
    char       *parentname;
    char       *parentclass;
    char       *name;
    char       *class;
    int         valueType;
    char       *def;
    caddr_t    *valuep;		/* RETURN */
{
    char       *type;
    XrmValue    value;
    char       *string;
    char        buffer[1024];
    char        fullname[1024];
    char        fullclass[1024];
    int         len;

    sprintf(fullname, "%s.%s", parentname, name);
    sprintf(fullclass, "%s.%s", parentclass, class);
    if (XrmGetResource(database, fullname, fullclass, &type, &value)) {
	string = value.addr;
	len = value.size;
    } else {
	string = def;
	len = strlen(string);
    }
    (void) strncpy(buffer, string, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    switch (valueType) {
    case t_String:
	{
	    char       *s;
	    s = (char *) malloc(len + 1);
	    if (s == (char *) NULL)
#ifdef MIT_R5
                error("%s: GetResource - couldn't allocate memory");
#else
	    {
                fprintf(stderr, catgets(scmc_catd, 2, 18, 
                     "%s: GetResource - couldn't allocate memory.\n"),ProgramName);
                exit(1);
	    }
#endif
	    (void) strncpy(s, string, len);
	    s[len] = '\0';
	    *((char **) valuep) = s;
	}
	break;
    case t_Bool:
	LowerString(buffer);
	*((int *) valuep) = (!strcmp(buffer, "true") ||
			     !strcmp(buffer, "on") ||
			     !strcmp(buffer, "enabled") ||
			     !strcmp(buffer, "yes")) ? True : False;
	break;
    case t_Int:
	*((int *) valuep) = atoi(buffer);
	break;
    case t_Float:
	*((float *) valuep) = (float) atof(buffer);
	break;
    }
}


static      XrmDatabase
parsefilepath(xfilesearchpath, TypeName, ClassName)
    char       *xfilesearchpath;
    char       *TypeName;
    char       *ClassName;
{
    XrmDatabase database = NULL;
    char        appdefaults[1024];
    char       *src;
    char       *dst;

    src = xfilesearchpath;
    appdefaults[0] = '\0';
    dst = appdefaults;
    while (1) {
	if (*src == '%') {
	    src++;
	    switch (*src) {
	    case '%':
	    case ':':
		*dst++ = *src++;
		*dst = '\0';
		break;
	    case 'T':
		(void) strcat(dst, TypeName);
		src++;
		dst += strlen(TypeName);
		break;
	    case 'N':
		(void) strcat(dst, ClassName);
		src++;
		dst += strlen(ClassName);
		break;
	    case 'S':
		src++;
		break;
	    default:
		src++;
		break;
	    }
	} else if (*src == ':') {
	    database = XrmGetFileDatabase(appdefaults);
	    if (database == NULL) {
		dst = appdefaults;
		src++;
	    } else
		break;
	} else if (*src == '\0') {
	    database = XrmGetFileDatabase(appdefaults);
	    break;
	} else {
	    *dst++ = *src++;
	    *dst = '\0';
	}
    }
    return database;
}

/*******************************************************************/
/** screenIOErrorHandler                                          **/
/**                                                               **/
/** this function will exit cleanly when the connection is broken **/
/*******************************************************************/
static int screenIOErrorHandler(dpy)
	Display *dpy;
{
	exit(1);
	return 1;
}

static void
open_display()
{
    if (display != NULL) {
	char       *colon = strchr(display, ':');

	if (colon == NULL)
#ifdef MIT_R5
            error("%s: Malformed -display argument, \"%s\"\n", display);
#else
	{
            fprintf(stderr, catgets(scmc_catd, 2, 19,
                "%s: Malformed -display argument:  %s.\n"), ProgramName,display);
            exit(1);
	}
#endif

    } else
	display = ":0.0";
    if (!(dsp = XOpenDisplay(display)))
#ifdef MIT_R5
        error("%s: unable to open display %s.\n", display);
#else
    {
        fprintf(stderr, catgets(scmc_catd, 2, 17,
                "%s: Unable to open display %s.\n"),ProgramName, display);
        exit(1);
    }
#endif

    XSetIOErrorHandler(screenIOErrorHandler);
}

void
printvar(class, var)
    char       *class;
    argtype     var;
{
    switch (var.type) {
    case t_String:
	fprintf(stderr, "%s.%s: %s\n",
		class, var.name, *((char **) var.var));
	break;
    case t_Bool:
	fprintf(stderr, "%s.%s: %s\n",
		class, var.name, *((int *) var.var)
		? "True" : "False");
	break;
    case t_Int:
	fprintf(stderr, "%s.%s: %d\n",
		class, var.name, *((int *) var.var));
	break;
    case t_Float:
	fprintf(stderr, "%s.%s: %g\n",
		class, var.name, *((float *) var.var));
	break;
    }
}


void
GetResources(argc, argv)
    int         argc;
    char       *argv[];
{
    XrmDatabase RDB = NULL;
    XrmDatabase modeDB = NULL;
    XrmDatabase nameDB = NULL;
    XrmDatabase cmdlineDB = NULL;
    XrmDatabase generalDB = NULL;
    XrmDatabase homeDB = NULL;
    XrmDatabase applicationDB = NULL;
    XrmDatabase serverDB = NULL;
    XrmDatabase userDB = NULL;
    char        userfile[1024];
    char       *homeenv;
    char       *userpath;
    char       *env;
    char       *serverString;
    int         i;
    /***************************/
    /** new variables for AIX **/
    /***************************/
    char        delaySpecifier[64];
    char        batchcountSpecifier[64];
    char        saturationSpecifier[64];

    XrmInitialize();

    for (i = 0; i < argc; i++) {
	if (!strncmp(argv[i], "-help", strlen(argv[i])))
	    Help();
	/* NOTREACHED */
    }

    /*
     * get -name arg from command line so you can have different resource
     * files for different configurations/machines etc...
     */
    XrmParseCommand(&nameDB, nameTable, 1, ProgramName,
                    &argc, argv);
    GetResource(nameDB, ProgramName, "*", "name", "Name", t_String,
                DEF_CLASSNAME, &classname);

    homeenv = getenv("HOME");
    if (!homeenv)
	homeenv = "";

    env = getenv("XFILESEARCHPATH");
    applicationDB = parsefilepath(env ? env : DEF_FILESEARCHPATH,
				  "app-defaults", classname);

    XrmParseCommand(&cmdlineDB, cmdlineTable, cmdlineEntries, ProgramName,
		    &argc, argv);

    userpath = getenv("XUSERFILESEARCHPATH");
    if (!userpath) {
	env = getenv("XAPPLRESDIR");
	if (env)
	    sprintf(userfile, "%s/%%N:%s/%%N", env, homeenv);
	else
	    sprintf(userfile, "%s/%%N", homeenv);
	userpath = userfile;
    }
    userDB = parsefilepath(userpath, "app-defaults", classname);

    (void) XrmMergeDatabases(applicationDB, &RDB);
    (void) XrmMergeDatabases(userDB, &RDB);
    (void) XrmMergeDatabases(cmdlineDB, &RDB);

    env = getenv("DISPLAY");
    GetResource(RDB, ProgramName, classname, "display", "Display", t_String,
		env ? env : DEF_DISPLAY, &display);
    open_display();
    serverString = XResourceManagerString(dsp);
    if (serverString) {
	serverDB = XrmGetStringDatabase(serverString);
	(void) XrmMergeDatabases(serverDB, &RDB);
    } else {
	char        buf[1024];
	sprintf(buf, "%s/.Xdefaults", homeenv);
	homeDB = XrmGetFileDatabase(buf);
	(void) XrmMergeDatabases(homeDB, &RDB);
    }

    XrmParseCommand(&generalDB, genTable, genEntries, ProgramName, &argc, argv);
    (void) XrmMergeDatabases(generalDB, &RDB);

    GetResource(RDB, ProgramName, classname, "mode", "Mode", t_String,
		DEF_MODE, (caddr_t *) &mode);

    /*
     * if random< mode, then just grab a random entry from the table
     */
    if (!strcmp(mode, randomstring))
	mode = LockProcs[random() % (NUMPROCS - 2)].cmdline_arg;

    sprintf(modename, "%s.%s", ProgramName, mode);
    sprintf(modeclass, "%s.%s", classname, mode);


    /*********************************************************************/
    /** New code for AIX                                                **/
    /** We must build the specifier fields of the modeTable on the fly. **/
    /*********************************************************************/
    sprintf(delaySpecifier,      ".%s.delay",      mode);
    sprintf(batchcountSpecifier, ".%s.batchcount", mode);
    sprintf(saturationSpecifier, ".%s.saturation", mode);
    modeTable[0].specifier = delaySpecifier;
    modeTable[1].specifier = batchcountSpecifier;
    modeTable[2].specifier = saturationSpecifier;


    XrmParseCommand(&modeDB, modeTable, modeEntries, ProgramName, &argc, argv);
    (void) XrmMergeDatabases(modeDB, &RDB);

    /* Parse the rest of the command line */
    for (argc--, argv++; argc > 0; argc--, argv++) {
	if (**argv != '-')
	    Syntax(*argv);
	switch (argv[0][1]) {
	case 'r':
	    DumpResources();
	    /* NOTREACHED */
	default:
	    Syntax(*argv);
	    /* NOTREACHED */
	}
    }

    /* the RDB is set, now query load the variables from the database */

    for (i = 0; i < NGENARGS; i++)
	GetResource(RDB, ProgramName, classname,
		    genvars[i].name, genvars[i].class,
		    genvars[i].type, genvars[i].def, genvars[i].var);

    for (i = 0; i < NMODEARGS; i++)
	GetResource(RDB, modename, modeclass,
		    modevars[i].name, modevars[i].class,
		    modevars[i].type, modevars[i].def, modevars[i].var);

    (void) XrmDestroyDatabase(RDB);

}


void CheckResources(void)
{
    int         i;

    if (batchcount < 1)
        Syntax("-batchcount argument must be positive.");
    if (saturation < 0.0 || saturation > 1.0)
        Syntax("-saturation argument must be between 0.0 and 1.0.");
    if (delay < 0)
	Syntax("-delay argument must be positive.");

    for (i = 0; i < NUMPROCS; i++) {
	if (!strncmp(LockProcs[i].cmdline_arg, mode, strlen(mode))) {
	    init = LockProcs[i].lp_init;
	    callback = LockProcs[i].lp_callback;
	    break;
	}
    }
    if (i == NUMPROCS) {
#ifdef MIT_R5
        fprintf(stderr, "Unknown mode: ");
#else
        fprintf(stderr, "%s", catgets(scmc_catd, 2, 7, 
             "Unknown mode: "));
#endif
	Syntax(mode);
    }
}
