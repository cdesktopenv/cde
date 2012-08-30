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
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
#ifdef VERBOSE_REV_INFO
static char rcsid[] = "$TOG: dthello.c /main/9 1998/04/20 12:52:17 mgreess $";
#endif /* VERBOSE_REV_INFO */
/******************************<+>*************************************
 **********************************************************************
 **
 **  File:        dthello.c
 **
 **  Project:     HP Visual User Environment
 **
 **  Description:
 **  -----------
 **  This file implements the startup transition for the DT 
 **  environment
 **
 **
 **********************************************************************
 **
 **     (c) Copyright 1990, 1991, 1992 Hewlett-Packard Company
 **     All rights reserved
 **
 **********************************************************************
 ******************************<+>*************************************/

/*
 * Notes on conditional compilation:
 *
 * AUTO_TOP	If defined, the dthello window will automatically
 * 		raise itself over any window that obscures it.
 *
 * BLOCK_CENTER_FILES
 *		If defined, dthello will center the text from
 *		input files as a block as opposed to line-by-line.
 */

/*
 * Included Files:
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <Xm/MwmUtil.h>
#include <sys/signal.h>
#include <Xm/Xm.h>
#include <Dt/GetDispRes.h>
#ifdef __apollo
#include </sys5/usr/include/limits.h>
#else
#include <limits.h>
#endif
#include <locale.h>
#ifndef NO_MESSAGE_CATALOG
# include <nl_types.h>
#endif

#include <Dt/EnvControlP.h>
#include "dthello.h"

#ifndef NO_MESSAGE_CATALOG
# define GETMESSAGE(set, number, string)    GetMessage(set, number, string)
#else
# define GETMESSAGE(set, number, string)    string
#endif

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif

/* 
 * Globals
 */
Window              welcome;            /* Welcome window ID */
Display            *dpy;                /* X server connection */
struct globalStruct vhGD;
int                 x_offset = 0;       /* for left-justifying text */
int                 box_line_width = 0; /* for drawing a box */
XFontSet            fontset;            /* Font descriptor for ILS */
unsigned long       textHeight;         /* Font size parameters */
unsigned long       fg, bg;             /* Pixel values */
Boolean             colorSuccess = True; /* Success at allocating colors */
XGCValues           gcv;                /* Struct for creating GC */
unsigned int        totalHeight;        /* total Height used for text */
unsigned int        displayHeight;      /* height of display in pixels */
int                 maxWidth;           /* max width of lines read in from file */
unsigned int        displayWidth;       /* width of display in pixels */
GC                  gc;                 /* GC to draw with */

static XrmOptionDescRec optTable[] = {
{"-font",	"*vfont",	XrmoptionSepArg,	(XtPointer) NULL},
{"-fnt",	"*vfont",	XrmoptionSepArg,	(XtPointer) NULL},
};
static int noptTable = sizeof(optTable)/sizeof(optTable[0]);

ArgSpec argSpecs[] =
{
    {"-bground", 8},	/* background color */
#define BG_ARG 		0

    {"-fground", 8},	/* foreground color */
#define FG_ARG 		1

    {"-string", 7},	/* string to display */
#define STRING_ARG 	2

    {"-fnt", 4},	/* font to use */
#define FONT_ARG 	3

    {"-timeout", 8},	/* timeout amount in seconds*/
#define TIME_ARG 	4

    {"-file", 5},	/* file */
#define FILE_ARG 	5

};

char          *ppchFileNames[MAX_FILES]; /* names of files to print */
unsigned char *ppchText[MAX_LINES]; /* text lines to print out */
int            numFiles;            /* number of files to print */
int numLines;                       /* number of text lines to print out */
#ifdef BLOCK_CENTER_FILES
int            centerLines;         /* number of text lines to print centered */
#endif /* BLOCK_CENTER_FILES */
char *progName;                     /* who we are */
char          *fontArg;             /* font argument */
int            sizeFontArg;
char          *stringArg;           /* string argument */
int            sizeStringArg;
char          *bgArg;               /* background argument */
int            sizeBgArg;
char          *fgArg;               /* foreground argument */
int            sizeFgArg;
char          *xoffsetArg;          /* text x_offset argument */
int            sizeXoffsetArg;
char          *timeArg;             /* timeout argument, in seconds */
int            sizeTimeArg;
int            sizeFileArg;
Boolean        Done;                /* while painting text */

static VhResourceEntry restable[] = {
    { vNbackground,  vCBackground, &bgArg,              &sizeBgArg      },
    { vNforeground,  vCForeground, &fgArg,              &sizeFgArg      },
    { vNfont,        vCFont,       &fontArg,            &sizeFontArg    },
    { vNxoffset,     vCXoffset,    &xoffsetArg,         &sizeXoffsetArg },
    { vNstring,      vCString,     &stringArg,          &sizeStringArg  },
    { vNfile,        vCFile,       &ppchFileNames[0],   &sizeFileArg    },
    { vNtimeout,     vCTimeout,    &timeArg,            &sizeTimeArg    },
};

void
Usage(void)
{
      fprintf(stderr, (char *) GETMESSAGE (4, 4, 
"usage: %s [-display <display>] [-bground <color>] [-fground <color>]\n"),
		  progName);
      fprintf(stderr, "%s", (char *) GETMESSAGE (4, 5, 
"\t[-font <font>] [-string <message>] [-timeout <seconds>] [-file <name>]\n"));
}


/*************************************<->*************************************
 *
 *  main (argc, argv)
 *
 *
 *  Description:
 *  -----------
 *  This is the main routine that does almost all the work.
 *
 *  Inputs:
 *  ------
 *  argc = Std C argument count
 *  argv = Std C argument vector
 *
 *  Outputs:
 *  --------
 *  Exit code
 *
 *  Comments:
 *  ---------
 *  This works by creating an override-redirect window the size of
 *  the screen and painting a message on it. At the same time, a 
 *  1x1 window is created that will be picked up by the window 
 *  manager.  When the window manager reparents the little window,
 *  this program exits.
 *
 *  Synopsis:
 *  ---------
 *  dthello [-display <display>] [-fground <color>] [-bground <color>]
 *           [-font <fontname>] [-string <message>] [-file <filename>]
 *           [-timeout <seconds>]
 * 
 *************************************<->***********************************/
main (int argc, char **argv)
{
    Window      wmwin;		/* Window ID for wm */
    XEvent      event;		/* Event received */
    unsigned long mask;		/* mask for window attribs */
    XSetWindowAttributes xwa;	/* Set Window Attribute struct */
    int 	argn;		/* temp for parsing args */
    XColor	colorDef;	/* for parsing/allocating colors */
    Colormap	colormap;	/* color map of screen */
    Atom	xaMwmHints;	/* mwm hints atom */
    PropMotifWmHints  mwmHints;	/* mwm hints structure */
    Visual *pdv;		/* X visual structure */
    FILE 	*fp;		/* file pointer */
    int 	i;		/* loop index */
    char	*default_string;	/* default message */
    XtAppContext appcontext;
    int         n;              /* for Arglist processing */
    Arg         args[10];
    Widget              	topLevelWid;
    char        *def_str;
    char        **missing_clist;
    int         missing_count;
    XFontSetExtents *extents;

    XtSetLanguageProc( NULL, NULL, NULL );

    /*
     * Initialization
     */
    /*
     * Set up NLSPATH, app-defaults, etc. for this DT client.
     */
    _DtEnvControl(DT_ENV_SET);

    /*
     * Process command line arguments
     */
    progName = argv[0];
    /* fontArg = DEFAULT_FONT; */
    bgArg = NULL;
    fgArg = NULL;
    timeArg = DEFAULT_TIME;
    numFiles = 0;
    sizeFileArg = 0;

    /*
     * Initialize  Toolkit, open display
     */
    XtToolkitInitialize();
    appcontext = XtCreateApplicationContext();

    dpy = XtOpenDisplay (appcontext, NULL, argv[0], DTHELLO_CLASS_NAME, 
			 optTable, noptTable, (int *)(&argc), argv);

    if (dpy == NULL) 
    {
	setlocale(LC_ALL, "");
	fprintf(stderr, (char *) 
	    GETMESSAGE(4, 1, "%s: can't open display\n"), progName);
	exit(1);
    }

    default_string = strdup (
    ((char *)GETMESSAGE(2, 3, 
			"Starting the\nCommon Desktop Environment\n\n")));

    stringArg = default_string;

    /*
     * Find appropriate default font
     * and offset from the left side of the screen
     */

    switch (_DtGetDisplayResolution(dpy, XDefaultScreen(dpy)))
    {
	case LOW_RES_DISPLAY:
		fontArg = DEFAULT_FONT_SMALL;
		x_offset = DEFAULT_XOFFSET_SMALL;
		box_line_width = BOX_LINE_WIDTH_SMALL;
		break;
	
	case MED_RES_DISPLAY:
		fontArg = DEFAULT_FONT_MEDIUM;
		x_offset = DEFAULT_XOFFSET_MEDIUM;
		box_line_width = BOX_LINE_WIDTH_MEDIUM;
		break;

	case HIGH_RES_DISPLAY:
		fontArg = DEFAULT_FONT_LARGE;
		x_offset = DEFAULT_XOFFSET_LARGE;
		box_line_width = BOX_LINE_WIDTH_LARGE;
		break;
	default:
		fontArg = DEFAULT_FONT_SMALL;
		x_offset = DEFAULT_XOFFSET_SMALL;
		box_line_width = BOX_LINE_WIDTH_SMALL;
		break;
    }

    /*
     * Fetch resources
     */
    VhGetResources (dpy, progName, DTHELLO_CLASS, 
			restable, XtNumber(restable));

    /* assign the x_offset to the value set in the resource list if given by the user */
    /* RK 11.06.93								      */
    if( xoffsetArg != NULL)
	x_offset = atoi(xoffsetArg);
    if (x_offset < 0)
    {
	x_offset = -x_offset;
    }

    if (sizeFileArg != 0)
    {
	numFiles = 1;
    }

    /*
     * Parse remaining command line arguments
     */
    for (argn = 1; argn < argc; argn++)
    {
	if ((*argv[argn] == '-') &&
	    (argn+1 < argc))
	{
	    if (ArgMatch (argv[argn], BG_ARG))
	    {
		bgArg = argv[++argn];
		continue;
	    }

	    else if (ArgMatch (argv[argn], FG_ARG))
	    {
		fgArg = argv[++argn];
		continue;
	    }

	    else if (ArgMatch (argv[argn], STRING_ARG))
	    {
		stringArg = argv[++argn];
		continue;
	    }

	    else if (ArgMatch (argv[argn], FONT_ARG))
	    {
		fontArg = argv[++argn];
		continue;
	    }

	    else if (ArgMatch (argv[argn], TIME_ARG))
	    {
		timeArg = argv[++argn];
		if (atoi(timeArg) <= 0)
		{
		    fprintf (stderr, (char *) 
			GETMESSAGE(4, 2, "%s: timeout must be positive\n"),
			argv[0]);
		    timeArg = DEFAULT_TIME;
		}
		continue;
	    }

	    else if (ArgMatch (argv[argn], FILE_ARG))
	    {
		argn++;
		if (numFiles < MAX_FILES)
		{
		    ppchFileNames[numFiles] = (char *) argv[argn];
		    numFiles++;
		}
		else
		{
		  fprintf(stderr, (char *) GETMESSAGE(4, 3, 
		      "%1$s: Maxiumum of %2$d files allowed, skipping %3$s\n"), 
		      argv[0], MAX_FILES, argv[argn]);
		}
		continue;
	    }

	    else
	    {
		Usage();
	        exit(1);
	    }
	}
	else
	{
	    Usage();
	    exit(1);
	}
    }


    displayHeight = XDisplayHeight (dpy, XDefaultScreen(dpy));
    displayWidth = XDisplayWidth (dpy, XDefaultScreen(dpy));

    /*
     * Set default fg/bg colors if not specified.
     * (adjust for low-color systems)
     */

    pdv = XDefaultVisual(dpy, XDefaultScreen(dpy));

    /*
     * Set default colors if not specified on command line.
     */
    if ((XDefaultDepth(dpy, XDefaultScreen(dpy)) <= 4) ||
	(pdv->class == StaticGray) ||
	(pdv->class == GrayScale))
    {
	/*
	 * B&W, GrayScale, or low-color systems
	 */
	if (!bgArg)
	{
	    bgArg = DEFAULT_LOW_BG;
	}

	if (!fgArg)
	{
	    fgArg = DEFAULT_LOW_FG;
	}
    }
    else
    {
	/*
	 * Medium- to High-color systems
	 */
	if (!bgArg)
	{
	    bgArg = DEFAULT_BG;
	}

	if (!fgArg)
	{
	    fgArg = DEFAULT_FG;
	}
    }

    /*
     * Load the font.
     */
    if ((fontset = XCreateFontSet(dpy, fontArg, &missing_clist, &missing_count,
                                  &def_str)) == NULL)
    {
	fprintf(stderr, (char *) 
	    GETMESSAGE (4, 6, "%1$s: display %2$s doesn't know font %3$s\n"),
		argv[0], DisplayString(dpy), fontArg);

        if ((fontset = XCreateFontSet(dpy, DEFAULT_FONT, &missing_clist,
                                        &missing_count, &def_str)) == NULL)
        {
            fprintf(stderr, (char *)
            GETMESSAGE (4, 6, "%1$s: display %2$s doesn't know font %3$s\n"),
                    argv[0], DisplayString(dpy), DEFAULT_FONT);
        }
  
        if ((NULL == fontset) &&
            (fontset = XCreateFontSet(dpy, FIXED_FONT, &missing_clist,
                                      &missing_count, &def_str)) == NULL)
	{
	    fprintf(stderr, (char *)
	    GETMESSAGE (4, 6, "%1$s: display %2$s doesn't know font %3$s\n"),
		    argv[0], DisplayString(dpy), FIXED_FONT);
	    exit(1);
	}
    }
    extents = XExtentsOfFontSet(fontset);
    textHeight = extents->max_ink_extent.height;

    /*
     * Print the copyright file by default if no other file
     * specified
     */
#ifdef DEFAULT_FILE
    if (numFiles == 0)
    {
	ppchFileNames[numFiles] = (char *) DEFAULT_FILE;
	numFiles = 1;
    }
#endif

    /*
     * Break the text string up into separate lines
     * and place into message buffer.
     */
    SeparateTextLines ((unsigned char *)stringArg);

#ifdef BLOCK_CENTER_FILES
    centerLines = numLines;
#endif /* BLOCK_CENTER_FILES */

    for (i = 0; i < numFiles; i++)
    {
	maxWidth = 0;
	if (!(fp = fopen ((char *)ppchFileNames[i], "r")))
	{
	    fprintf (stderr, (char *) 
		GETMESSAGE (4, 7, "%1$s: unable to open file %2$s\n"),
		argv[0], ppchFileNames[i]);
	}
	else
	{
	    /* 
	     * read in lines
	     */
	    ReadInTextLines (fp, fontset, (unsigned int *)&maxWidth);
	}
    }

    /*
     * Get the colors 
     */
    colormap = XDefaultColormap (dpy, XDefaultScreen(dpy));

    /* 
     * Get background color
     */
    if (!XParseColor (dpy, colormap, bgArg, &colorDef))
    {
	/* failed to get background color, try low color default */
        colorSuccess = False; 
	fprintf(stderr, (char *) GETMESSAGE (4, 8, 
	    "%1$s: can't set background to %2$s, using %3$s.\n"),
		    argv[0], bgArg, DEFAULT_LOW_BG);
	XParseColor (dpy, colormap, DEFAULT_LOW_BG, &colorDef);
    }
    XAllocColor (dpy, colormap, &colorDef);

    bg = colorDef.pixel;

    /* 
     * Get foreground color
     */
    if (!XParseColor (dpy, colormap, fgArg, &colorDef))
    {
	/* failed to get foreground color, try low color default */
        colorSuccess = False; 
	fprintf(stderr, (char *) GETMESSAGE (4, 9, 
	    "%1$s: can't set foreground to %2$s, using %3$s.\n"),
		    argv[0], fgArg, DEFAULT_LOW_FG);
	XParseColor (dpy, colormap, DEFAULT_LOW_FG, &colorDef);
    }
    XAllocColor (dpy, colormap, &colorDef);

    fg = colorDef.pixel;

    /* 
     * Create 1x1 window to catch reparenting action of window manager
     * Request no mwm decoration to reduce flash.
     * Request no mwm functions to avoid icon in icon box.
     */
    wmwin = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
			      0, 0, 1, 1, 0, bg, bg);

    xaMwmHints = XInternAtom (dpy, _XA_MOTIF_WM_HINTS, 0);

    mwmHints.flags = MWM_HINTS_DECORATIONS | MWM_HINTS_FUNCTIONS;
    mwmHints.decorations = 0;
    mwmHints.functions = 0;

    XChangeProperty (dpy, wmwin, xaMwmHints, xaMwmHints, 32, 
	PropModeReplace, (unsigned char *) &mwmHints, 
	sizeof(PropMotifWmHints)/sizeof(long));

    XSelectInput (dpy, wmwin, StructureNotifyMask);

    XMapWindow(dpy, wmwin);

    /* 
     * Create override-redirect window for display of transition
     * message.
     */

    welcome = XCreateSimpleWindow (dpy, DefaultRootWindow (dpy), 0, 0,
	displayWidth, displayHeight, 0, bg, bg);

    mask = CWOverrideRedirect | CWEventMask | CWCursor;

    xwa.override_redirect = 1;
#ifdef AUTO_TOP
    xwa.event_mask = ExposureMask | VisibilityChangeMask;
#else /* AUTO_TOP */
    xwa.event_mask = ExposureMask;
#endif /* AUTO_TOP */
    xwa.cursor = GetHourGlass (dpy);

    XChangeWindowAttributes (dpy, welcome, mask, &xwa);

    XMapWindow(dpy, welcome);
    /*
     * Event loop for painting text
     */

    Done = True;

    while (Done) 
    {
	/*
	 * Get the next event
	 */
	/* XNextEvent(dpy, &event); */

	XtAppNextEvent(appcontext, &event);

	if (event.type == Expose && 
	    event.xexpose.window == welcome &&
	    event.xexpose.count == 0) 
	{

	    /*
	     * Remove any other pending Expose events from the queue 
	     */
	    while (XCheckTypedEvent(dpy, Expose, &event));

	    /*
	     * Create the GC for drawing the box and painting the text.
	     */
	    gcv.foreground = fg;
	    gcv.background = bg;
	    gc = XCreateGC(dpy, welcome, (GCForeground | GCBackground), &gcv);
	    XClearWindow(dpy, welcome);
	    DrawBox();
	    PaintText();
	    XFlush(dpy);
	    Done = False;
	}
    } /* end while */

    /* 
     * set up the timeout
     */
    signal (SIGALRM, (void (*)()) CatchAlarm);
    alarm (atoi(timeArg));

    /*
     * Event loop
     */
    while (True) 
    {
	/*
	 * Get the next event
	 */
	/* XNextEvent(dpy, &event); */

	XtAppNextEvent(appcontext, &event);

	if (event.type == ReparentNotify &&
	    event.xany.window == wmwin)
	{
	    /*
	     * this is our cue...exit, stage left
	     */
	    alarm(0);
	    break;
	}
	else
	{
	        /* normal color serving process */
		XtDispatchEvent(&event);		
	}

    } /* end while */

    exit(0);
}


/*************************************<->*************************************
 *
 *  ArgMatch (pch, arn)
 *
 *
 *  Description:
 *  -----------
 *  This function determines if a string matches a predefined switch.
 *
 *  Inputs:
 *  ------
 *  pch = candidate string
 *  arn = number of switch we are trying to match.
 *
 *  Outputs:
 *  --------
 *  Return = True if match succeeds.
 *
 *  Comments:
 *  ---------
 *  Would need work to be localized.
 *
 *************************************<->***********************************/
int
ArgMatch (char *pch, int arn)
{
    int rval = False;

    if (!strncmp(pch, argSpecs[arn].name, argSpecs[arn].len))
    {
	rval = True;
    }

    return (rval);
}


/*************************************<->*************************************
 *
 *  SkipWhitespace (*pch)
 *
 *
 *  Description:
 *  -----------
 *  This procedure scans a string and returns a pointer to the first
 *  non-whitespace character.
 *
 *  Inputs:
 *  ------
 *  pch = pointer to string to scan
 *
 *  Outputs:
 *  --------
 *  Return = pointer to first non-white character
 *
 *  Comments:
 *  ---------
 *  Skips blanks and horizontal tabs.
 *
 *************************************<->***********************************/
unsigned char *
SkipWhitespace (unsigned char *pch)
{
    int chlen;

    if (pch)
    {
	while ((*pch != '\0') &&
	       ((chlen = mblen ((char *)pch, MB_CUR_MAX)) == 1) &&
	       ((*pch == '\t') || (*pch == ' ')))
	{
		pch += chlen;
	}
    }
    return (pch);
}



/*************************************<->*************************************
 *
 *  KillNewlines (*pch)
 *
 *
 *  Description:
 *  -----------
 *  This procedure scans a string and replaces the first newline
 *  with a NULL.
 *
 *  Inputs:
 *  ------
 *  pch = pointer to string to scan
 *
 *  Outputs:
 *  --------
 *  pch = modified
 *
 *  Comments:
 *  ---------
 *
 *************************************<->***********************************/
void
KillNewlines (unsigned char *pch)
{
    int chlen;

    if (pch)
    {
	while (*pch != '\0')
	{
	    if (((chlen = mblen ((char *)pch, MB_CUR_MAX)) == 1) &&
	        (*pch == '\n'))
	    {
		*pch = '\0';
		break;
	    }
	    pch += chlen;
	}
    }
}


/*************************************<->*************************************
 *
 *  ReadInTextLines (fp, font, pMaxWidth)
 *
 *
 *  Description:
 *  -----------
 *  This procedure reads in lines from a file for the message to 
 *  display.
 *
 *  Inputs:
 *  ------
 *  pchIn = pointer to message string
 *  font = font structure to be used to print with
 *  pMaxWidth = width of largest line
 *
 *  Outputs:
 *  --------
 *  Return = none
 *  modifies global numLines
 *
 *  Comments:
 *  ---------
 *  Global data ppchText is modified by this routine to contain 
 *  copies of the text lines from pchIn. pchIn is not modified.
 *
 *************************************<->***********************************/
void
ReadInTextLines (FILE *fp, XFontSet fontset, unsigned int *pMaxWidth)
{
    unsigned int width;
    int allowedLines;

    /* count the number of lines in the file */
    allowedLines = (displayHeight - (2 * box_line_width))/ textHeight;

    while (numLines < allowedLines)
    {
        ppchText[numLines] = (unsigned char *) malloc (1+MAX_COLUMNS);
	if (!ppchText[numLines] ||
	    !fgets ((char *)ppchText[numLines], MAX_COLUMNS, fp))
	{
	    break;
	}
	KillNewlines (ppchText[numLines]);
#ifndef BLOCK_CENTER_FILES
        ppchText[numLines] = SkipWhitespace (ppchText[numLines]);
#endif /* not BLOCK_CENTER_FILES */
	width = XmbTextEscapement(fontset, (char *)(ppchText[numLines]),
			strlen((char *)ppchText[numLines]));
	if (width > *pMaxWidth)
	{
	    *pMaxWidth = width;
	}
	numLines++;
    }
}


/*************************************<->*************************************
 *
 *  SeparateTextLines (pchIn)
 *
 *
 *  Description:
 *  -----------
 *  This procedure takes a message string and separates it into
 *  text lines based on the presence of new line characters.
 *
 *  Inputs:
 *  ------
 *  pchIn = pointer to message string
 *
 *  Outputs:
 *  --------
 *  Return = none
 *
 *  Comments:
 *  ---------
 *  Global data ppchText is modified by this routine to contain 
 *  copies of the text lines from pchIn. pchIn is not modified.
 *
 *************************************<->***********************************/
void
SeparateTextLines (unsigned char *pchIn)
{
    unsigned char *pch, *pch1, *pch2;
    unsigned char *pchInEnd;
    int i, chlen;

    /* count the number of new line characters in the string */

    numLines = 1;
    for (pch = pchIn; *pch; )
    {
	if (((chlen = mblen ((char *)pch, MB_CUR_MAX)) == 1) &&
	    (*pch == '\n'))
	{
	    numLines++;
	}
	pch += chlen;
    }

    if ((chlen == 1) && (*(pch-1) == '\n'))
    {
	numLines--;		/* don't count terminating newline */
    }

    pch2 = pch1 = pchIn;
    pchInEnd = pchIn + strlen((char *)pchIn);

    for (i = 0; (i < numLines) && (pch1 < pchInEnd); i++)
    {
	while ((*pch2 != '\0') &&
               !(((chlen = mblen ((char *)pch2, MB_CUR_MAX)) == 1) &&
	         (*pch2 == '\n')))
	{
	    pch2 += chlen;
        }
	if (*pch2 == '\n')
	{
	    *pch2 = '\0';
	}

        ppchText[i] = (unsigned char *) malloc (1+strlen ((char *)pch1));
	if (ppchText[i])
	{
	    strcpy ((char *)ppchText[i], (char *)pch1);
	}
	else
	{
	    fprintf (stderr, (char *) GETMESSAGE (4, 10, 
		"%s: Insufficient memory (SeparateTextLines)\n"), 
		progName);
	    exit (1);
	}

	/* advance pointers */
	pch1 = ++pch2;
  
    }
}



/*************************************<->*************************************
 *
 *  CatchAlarm (sig)
 *
 *
 *  Description:
 *  -----------
 *  This function catches the SIG_ALRM signal generated when the
 *  timer expires.
 *
 *  Inputs:
 *  ------
 *  sig = signal number
 *
 *  Outputs:
 *  --------
 *  Return = none
 *
 *  Comments:
 *  ---------
 *
 *************************************<->***********************************/
void
CatchAlarm ( int sig)
{
    /* timer expired, exit */
    exit(0);
}


/*************************************<->*************************************
 *
 *  Cursor GetHourGlass ()
 *
 *
 *  Description:
 *  -----------
 *  Builds and returns the appropriate Hourglass cursor
 *
 *
 *  Inputs:
 *  ------
 *  dpy	= display
 * 
 *  Outputs:
 *  -------
 *  Return = cursor.
 *
 *  Comments:
 *  --------
 *  None. (None doesn't count as a comment)
 * 
 *************************************<->***********************************/

#define time32_width 32
#define time32_height 32
#define time32_x_hot 15
#define time32_y_hot 15
static unsigned char time32_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0xff, 0xff, 0x7f,
   0x8c, 0x00, 0x00, 0x31, 0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32,
   0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32, 0x4c, 0x00, 0x00, 0x32,
   0x8c, 0x00, 0x00, 0x31, 0x0c, 0x7f, 0xfe, 0x30, 0x0c, 0xfe, 0x7f, 0x30,
   0x0c, 0xfc, 0x3f, 0x30, 0x0c, 0xf8, 0x1f, 0x30, 0x0c, 0xe0, 0x07, 0x30,
   0x0c, 0x80, 0x01, 0x30, 0x0c, 0x80, 0x01, 0x30, 0x0c, 0x60, 0x06, 0x30,
   0x0c, 0x18, 0x18, 0x30, 0x0c, 0x04, 0x20, 0x30, 0x0c, 0x02, 0x40, 0x30,
   0x0c, 0x01, 0x80, 0x30, 0x8c, 0x00, 0x00, 0x31, 0x4c, 0x80, 0x01, 0x32,
   0x4c, 0xc0, 0x03, 0x32, 0x4c, 0xf0, 0x1f, 0x32, 0x4c, 0xff, 0xff, 0x32,
   0xcc, 0xff, 0xff, 0x33, 0x8c, 0xff, 0xff, 0x31, 0xfe, 0xff, 0xff, 0x7f,
   0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};

#define time32m_width 32
#define time32m_height 32
static unsigned char time32m_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xcf, 0x00, 0x00, 0xf3, 0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76,
   0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76, 0x6e, 0x00, 0x00, 0x76,
   0xce, 0x00, 0x00, 0x73, 0x8e, 0x7f, 0xfe, 0x71, 0x0e, 0xff, 0xff, 0x70,
   0x0e, 0xfe, 0x7f, 0x70, 0x0e, 0xfc, 0x3f, 0x70, 0x0e, 0xf8, 0x1f, 0x70,
   0x0e, 0xe0, 0x07, 0x70, 0x0e, 0xe0, 0x07, 0x70, 0x0e, 0x78, 0x1e, 0x70,
   0x0e, 0x1c, 0x38, 0x70, 0x0e, 0x06, 0x60, 0x70, 0x0e, 0x03, 0xc0, 0x70,
   0x8e, 0x01, 0x80, 0x71, 0xce, 0x00, 0x00, 0x73, 0x6e, 0x80, 0x01, 0x76,
   0x6e, 0xc0, 0x03, 0x76, 0x6e, 0xf0, 0x1f, 0x76, 0x6e, 0xff, 0xff, 0x76,
   0xee, 0xff, 0xff, 0x77, 0xcf, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


#define time16_x_hot 7
#define time16_y_hot 7
#define time16_width 16
#define time16_height 16
static unsigned char time16_bits[] = {
   0x00, 0x00, 0xfe, 0x7f, 0x14, 0x28, 0x14, 0x28, 0x14, 0x28, 0x24, 0x24,
   0x44, 0x22, 0x84, 0x21, 0x84, 0x21, 0x44, 0x22, 0x24, 0x24, 0x14, 0x28,
   0x94, 0x29, 0xd4, 0x2b, 0xfe, 0x7f, 0x00, 0x00};

#define time16m_width 16
#define time16m_height 16
static unsigned char time16m_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f,
   0xfe, 0x7f, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff};


Cursor GetHourGlass (Display *dpy)
{
    unsigned char *bits;
    unsigned char *maskBits;
    unsigned int width;
    unsigned int height;
    unsigned int xHotspot;
    unsigned int yHotspot;
    Pixmap       pixmap;
    Pixmap       maskPixmap;
    XColor       xcolors[2];
    int          scr;
    unsigned int cWidth;
    unsigned int cHeight;
    int		 useLargeCursors = 0;
    Cursor	 waitCursor;

    if (XQueryBestCursor (dpy, DefaultRootWindow(dpy), 
	32, 32, &cWidth, &cHeight))
    {
	if ((cWidth >= 32) && (cHeight >= 32))
	{
	    useLargeCursors = 1;
	}
    }

    if (useLargeCursors)
    {
	width = time32_width;
	height = time32_height;
	bits = time32_bits;
	maskBits = time32m_bits;
	xHotspot = time32_x_hot;
	yHotspot = time32_y_hot;
    }
    else
    {
	width = time16_width;
	height = time16_height;
	bits = time16_bits;
	maskBits = time16m_bits;
	xHotspot = time16_x_hot;
	yHotspot = time16_y_hot;
    }

    pixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), (char *)bits, 
		     width, height);

    maskPixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), (char *)maskBits, 
		     width, height);

    xcolors[0].pixel = BlackPixelOfScreen(DefaultScreenOfDisplay(dpy));
    xcolors[1].pixel = WhitePixelOfScreen(DefaultScreenOfDisplay(dpy));

    XQueryColors (dpy, 
		  DefaultColormapOfScreen(DefaultScreenOfDisplay
					  (dpy)), xcolors, 2);

    waitCursor = XCreatePixmapCursor (dpy, pixmap, maskPixmap,
				      &(xcolors[0]), &(xcolors[1]),
				      xHotspot, yHotspot);
    XFreePixmap (dpy, pixmap);
    XFreePixmap (dpy, maskPixmap);

    return (waitCursor);
}

#ifndef NO_MESSAGE_CATALOG
/*****************************************************************************
 *
 * Function: GetMessage
 *
 * Parameters:
 *
 *   int	set -		The message catalog set number.
 *
 *   int	n - 		The message number.
 *
 *   char	*s -		The default message if the message is not
 *				retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 *
 *****************************************************************************/

char * 
GetMessage(
        int set,
        int n,
        char *s )
{
        char *msg;
	char *lang;
	nl_catd catopen();
	char *catgets();
	static int first = 1;
	static nl_catd nlmsg_fd;
	if ( first ) 
        {
		first = 0;

		lang = (char *) getenv ("LANG");

		if (!lang || !(strcmp (lang, "C"))) 
			/*
			 * If LANG is not set or if LANG=C, then there
			 * is no need to open the message catalog - just
			 * return the built-in string "s".
			 */
			nlmsg_fd = (nl_catd)-1;
		else
			nlmsg_fd = catopen("dthello", NL_CAT_LOCALE);
	}
	msg=catgets(nlmsg_fd,set,n,s);
	return (msg);
}
#endif


/*************************************<->*************************************
 *
 *  void VhGetResources
 *
 *
 *  Description:
 *  -----------
 *  Gets the resources for the client
 *
 *
 *  Inputs:
 *  ------
 * 
 *  Outputs:
 *  -------
 *
 *  Comments:
 *  --------
 * 
 *************************************<->***********************************/

void 
VhGetResources(Display *dpy, char *name, char *class, 
    VhResourceEntry *res, int num)
{
#define INIT_SIZE   256
#define SRCH_SIZE   20
    int i;
    XrmValue xrmv;
    XrmQuark  Qtype, Qstring, Qname, Qclass;
    XrmQuark Qnres[4], Qcres[4];
    XrmHashTable searchList[SRCH_SIZE];

    /*
     * We only deal with string-type resources
     */

    Qstring = XrmStringToQuark (XtRString);

    /*
     * Get resource search list for "dthello"
     */
    XrmStringToQuarkList (name, Qnres);
    XrmStringToQuarkList (class, Qcres);

    if (XrmQGetSearchList(XtDatabase(dpy), Qnres, Qcres, searchList,
			SRCH_SIZE))
    {
	/*
	 * Look for all resources at this level
	 */

	for (i = 0; i < num; i++)
	{
	    Qname = XrmStringToQuark (res[i].resname);
	    Qclass = XrmStringToQuark (res[i].resclass);

	    if ((XrmQGetSearchResource (searchList, Qname, Qclass, 
				  &Qtype, &xrmv)) &&
		(Qtype == Qstring))
	    {
		*(res[i].ppvalue) = (char *) xrmv.addr;
		*res[i].size = (int) xrmv.size;
	    }
	}
    }
}

void
PaintText( void )
{
	    int i, x, y;
	    XFontSetExtents *extents;

	    /* 
	     * Paint the string onto the screen
	     */


	    y = (displayHeight - totalHeight) / 2;
	    if (y < 0)
	    {
	       y = 0;
	    }

	    /* adjust origin by font metric */
	    extents = XExtentsOfFontSet(fontset);
	    y += -(extents->max_logical_extent.y);

	    x = box_line_width + x_offset;

	    for (i = 0; i < numLines; i++)
	    {
		/* draw the string */
		XmbDrawString (dpy, welcome, fontset, gc, x, y, 
			(char *)(ppchText[i]), strlen((char *)ppchText[i]));

		/* move to next "line" */
		y += textHeight;
	    }
}
void
DrawBox( void )
{
	int LTX, LTY, RTX, RTY, LBX, LBY, RBX, RBY;
	int L_middle;  /* pixels to the midpoint of the line width */

	Boolean useDecoration = True;

	/* compute the height of the font */
	totalHeight = textHeight * numLines;

	/* 
	 * Set limits
	 */
	if ((( 2 * box_line_width ) + x_offset + maxWidth) > displayWidth)
	{
		useDecoration = False;
		x_offset = 0;
	}

	if (!useDecoration)
		return;

	L_middle = box_line_width / 2;
	/********************************************************
	+----------------------------------------------+
	| (LTX, LTY)                        (RTX, RTY) |
	|                                              |
	|   (Draw counterclockwise, beginning from     |
	|    top left.)                                |
	|                                              |
	|                                              |
	|                                              |
	|                                              |
	|                                              |
	|                                              |
	| (LBX, LBY)                        (RBX, RBY) |
	+----------------------------------------------+
	********************************************************/
	LTX = 0;
	LTY = 0;

	RTX = displayWidth;
	RTY = 0;

	LBX = 0;
	LBY = displayHeight;

	RBX = displayWidth ;
	RBY = displayHeight;

	XSetLineAttributes(dpy, gc, box_line_width, LineSolid, CapButt, JoinMiter);
	XDrawLine(dpy, welcome, gc, 
				  LTX, LTY + L_middle,
				  RTX, RTY + L_middle);

	XDrawLine(dpy, welcome, gc, 
				RTX - L_middle, RTY, 
				RBX - L_middle, RBY);

	XDrawLine(dpy, welcome, gc, 
				RBX, RBY - L_middle, 
				LBX, LBY - L_middle);

	XDrawLine(dpy, welcome, gc, 
				LBX + L_middle, LBY, 
				LTX + L_middle, LTY);

}

int
Xestrcmp(const char * const s1, const char * const s2)
{
    if (s1 == s2) return 0;
    {
        const char * p1 = (s1) ? s1 : "";
        const char * p2 = (s2) ? s2 : "";

        return strcmp((char *)p1, (char *)p2);
    }
}

/**************         eof          ******************/
