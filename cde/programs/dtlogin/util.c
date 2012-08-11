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
/* (c) Copyright 1997 The Open Group */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 * xdm - display manager daemon
 *
 * $TOG: util.c /main/15 1998/04/06 13:22:20 mgreess $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * util.c
 *
 * various utility routines
 */


#include    <sys/stat.h>
#include    <setjmp.h>
#include    <string.h>
#include    <dirent.h>
#include    <nl_types.h>

# include   <sys/signal.h>

# ifndef NULL
#  define NULL 0
# endif


# include	"dm.h"
# include	"vgmsg.h"
nl_catd		nl_fd = (nl_catd)-1;	/* message catalog file descriptor */

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif

#if !defined (ENABLE_DYNAMIC_LANGLIST)
#define LANGLISTSIZE    2048
char    languageList[LANGLISTSIZE];     /* global list of languages     */
#endif /* ENABLE_DYNAMIC_LANGLIST */

/***************************************************************************
 *
 *  Local procedure declarations
 *
 ***************************************************************************/

static char * makeEnv( 
                        char *name,
                        char *value) ;

static SIGVAL MakeLangAbort(
			int arg	);
			
static int MatchesFileSuffix(
			char * filename,
			char * suffix );

static void   ScanNLSDir(
			char * dirname );

/********    End Local Function Declarations    ********/


/***************************************************************************
 *
 *  ReadCatalog
 *
 *  read a string from the message catalog
 ***************************************************************************/

unsigned char *
ReadCatalog( int set_num, int msg_num, char *def_str )
{
    static Bool alreadyopen = False;
    char *s;

    if (alreadyopen == False)
    {
      char *curNlsPath, *newNlsPath;
      int newNlsPathLen;

      alreadyopen = True;

     /*
      * Desktop message catalogs are in DT directory, so append desktop
      * search paths to current NLSPATH.
      */
      #define NLS_PATH_STRING  CDE_INSTALLATION_TOP "/nls/msg/%L/%N.cat:" \
                         CDE_INSTALLATION_TOP "/lib/nls/msg/%L/%N.cat:" \
                         CDE_INSTALLATION_TOP "/lib/nls/msg/%l/%t/%c/%N.cat:" \
                         CDE_INSTALLATION_TOP "/lib/nls/msg/%l/%c/%N.cat"

      curNlsPath = getenv("NLSPATH");
      if (curNlsPath && strlen(curNlsPath) == 0)
      {
        curNlsPath = NULL;
      }
 
     /*
      * 7 is "NLSPATH"
      * 1 is "="
      * <length of NLS_PATH_STRING>
      * 1 for null byte
      */
      newNlsPathLen = 7 + 1 + strlen(NLS_PATH_STRING) + 1;

      if (curNlsPath != NULL)
      {
       /*
        * 1 is ":"
        * <length of curNlsPath>
        */
        newNlsPathLen += (1 + strlen(curNlsPath));
      }

      newNlsPath = malloc(newNlsPathLen);  /* placed in environ, do not free */
      
      if (curNlsPath != NULL)
      {
        sprintf(newNlsPath, "NLSPATH=%s:%s", curNlsPath, NLS_PATH_STRING);
      }
      else
      {
        sprintf(newNlsPath, "NLSPATH=%s", NLS_PATH_STRING);
      }
     
     /*
      * Store new NLSPATH in environment. Note this memory cannot be freed 
      */
      putenv(newNlsPath);
  
     /*
      * Open message catalog. Note, if invalid descriptor returned (ie
      * msg catalog could not be opened), subsequent call to catgets() using
      * that descriptor will return 'def_str'.
      */
      nl_fd = catopen("dtlogin", NL_CAT_LOCALE);
    }

    s = catgets(nl_fd,set_num,msg_num,def_str);
    
    return((unsigned char *)s);
}

void
printEnv( char **e )
{
	while (*e)
		Debug ("    %s\n", *e++);
}

static char * 
makeEnv( char *name, char *value )
{
	char	*result;

	result = malloc ((unsigned) (strlen (name) + strlen (value) + 2));
	if (!result) {
		LogOutOfMem(
		  ReadCatalog(MC_LOG_SET,MC_LOG_MAKEENV,MC_DEF_LOG_MAKEENV));
		return 0;
	}

        if (value && *value) {
	           sprintf (result, "%s=%s", name, value);
        }
        else {
                sprintf (result, "%s", name);
        }

	return result;
}

char * 
getEnv( char **e, char *name )
{
    int	l = strlen (name);

    while (*e) {
	if ((int) strlen (*e) > l &&
	    !strncmp (*e, name, l) &&
	    (*e)[l] == '=')
	  return (*e) + l + 1;

	++e;
    }
    return 0;
}

char ** 
setEnv( char **e, char *name, char *value )
{
    char	**new, **old;
    char	*newe;
    int		envsize;
    int		l;

    l = strlen (name);
    newe = makeEnv (name, value);
    if (!newe) {
	LogOutOfMem(ReadCatalog(MC_LOG_SET,MC_LOG_SETENV,MC_DEF_LOG_SETENV));
	return e;
    }
    if (e) {
	for (old = e; *old; old++)
	    if ((int) strlen (*old) > l &&
		!strncmp (*old, name, l) &&
		(*old)[l] == '=')
	      break;

	if (*old) {
		free (*old);
		*old = newe;
		return e;
	}
	envsize = old - e;
	new = (char **)
	      realloc((char *) e, (unsigned) ((envsize + 2) * sizeof (char *)));
    } else {
	envsize = 0;
	new = (char **) malloc (2 * sizeof (char *));
    }
    if (!new) {
	LogOutOfMem(ReadCatalog(MC_LOG_SET,MC_LOG_SETENV,MC_DEF_LOG_SETENV));
	free (newe);
	return e;
    }
    new[envsize] = newe;
    new[envsize+1] = 0;
    return new;
}

void
freeEnv (char **env)
{
    char    **e;

    if (env)
    {
        for (e = env; *e; e++)
            free (*e);
        free (env);
    }
}

# define isblank(c)	((c) == ' ' || c == '\t')

char ** 
parseArgs( char **argv, char *string )
{
	char	*word;
	char	*save;
	int	i;

	i = 0;
	while (argv && argv[i])
		++i;
	if (!argv) {
		argv = (char **) malloc (sizeof (char *));
		if (!argv) {
			LogOutOfMem(ReadCatalog(
			  MC_LOG_SET,MC_LOG_PARSEARGS,MC_DEF_LOG_PARSEARGS));
			return 0;
		}
	}
	word = string;
	for (;;) {
		if (!*string || isblank (*string)) {
			if (word != string) {
				argv = (char **) realloc ((char *) argv,
					(unsigned) ((i + 2) * sizeof (char *)));
				save = malloc ((unsigned) (string - word + 1));
				if (!argv || !save) {
					LogOutOfMem(ReadCatalog(MC_LOG_SET,
					  MC_LOG_PARSEARGS,
					  MC_DEF_LOG_PARSEARGS));
					if (argv)
						free ((char *) argv);
					if (save)
						free (save);
					return 0;
				}
				argv[i] = strncpy (save, word, string-word);
				argv[i][string-word] = '\0';
				i++;
			}
			if (!*string)
				break;
			word = string + 1;
		}
		++string;
	}
	argv[i] = 0;
	return argv;
}

void
CleanUpChild( void )
{
/*
 *  On i386/i486 platforms setprrp() functions causes the mouse not
 *  to work.  Since in the daemon mode the parent daemon has already
 *  executed a setpgrp it is a process and session leader. Since it
 *  has also gotten rid of the controlling terminal there is no great
 *  harm in not making the sub-daemons as leaders.
 */
#ifdef __osf__
        setsid();
        sigsetmask(0);
#else
#if defined (SYSV) || defined (SVR4) || defined(linux)
#if !defined (USL) && !defined(__uxp__)
	setpgrp ();
#endif  /* USL */
#else
	setpgrp (0, getpid ());
	sigsetmask (0);
#endif
#endif /* __osf__ */
#ifdef SIGCHLD
	(void) signal (SIGCHLD, SIG_DFL);
#endif
	(void) signal (SIGTERM, SIG_DFL);
	(void) signal (SIGPIPE, SIG_DFL);
	(void) signal (SIGALRM, SIG_DFL);
	(void) signal (SIGHUP, SIG_DFL);
	CloseOnFork ();
}

char * * 
parseEnv( char **e, char *string )
{

    char *s1, *s2, *t1, *t2;
    
    s1 = s2 = strdup(string);
    
    while ((t1 = strtok(s1," \t")) != NULL ) {
	if ( (t2 = strchr(t1,'=')) != NULL ) {
	    *t2++ = '\0';
	    e = setEnv(e, t1, t2);
	}
	
	s1 = NULL;
    }

    free(s2);
    return (e);
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
    char        *bits;
    char        *maskBits;
    unsigned int width;
    unsigned int height;
    unsigned int xHotspot;
    unsigned int yHotspot;
    Pixmap       pixmap;
    Pixmap       maskPixmap;
    XColor       xcolors[2];
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
	bits = (char *)time32_bits;
	maskBits = (char *)time32m_bits;
	xHotspot = time32_x_hot;
	yHotspot = time32_y_hot;
    }
    else
    {
	width = time16_width;
	height = time16_height;
	bits = (char *)time16_bits;
	maskBits = (char *)time16m_bits;
	xHotspot = time16_x_hot;
	yHotspot = time16_y_hot;
    }

    pixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), bits, 
		     width, height);

    maskPixmap = XCreateBitmapFromData (dpy, 
		     DefaultRootWindow(dpy), maskBits, 
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




/*************************************<->*************************************
 *
 *  void SetHourGlassCursor
 *
 *
 *  Description:
 *  -----------
 *  sets the window cursor to an hourglass
 *
 *
 *  Inputs:
 *  ------
 *  dpy	= display
 *  w   = window
 * 
 *  Outputs:
 *  -------
 *  None
 *
 *  Comments:
 *  --------
 *  None. (None doesn't count as a comment)
 * 
 *************************************<->***********************************/

void 
SetHourGlassCursor( Display *dpy, Window w )
{
    Cursor	cursor;
    
    XUndefineCursor(dpy, w);
    
    cursor = GetHourGlass(dpy);

    XDefineCursor(dpy, w, cursor);
    XFreeCursor(dpy, cursor);
    XFlush(dpy);
}

#if !defined (ENABLE_DYNAMIC_LANGLIST)
/***************************************************************************
 *
 *  MakeLangList
 *
 *  Generate the list of languages installed on the host.
 *  Result is stored the global array "languageList"
 *  
 ***************************************************************************/

#define DELIM		" \t"   /* delimiters in language list		   */

static jmp_buf	langJump;

static SIGVAL
MakeLangAbort( int arg )

{
    longjmp (langJump, 1);
}

void
MakeLangList( void )
{
    register int i, j;

    char        *lang[500];             /* sort list for languages         */
    int         nlang;                  /* total number of languages       */
    char        *p, *s;
    char        *savelist;

    /*
     *  build language list from set of languages installed on the host...
     *  Wrap a timer around it so it doesn't hang things up too long.
     *  langListTimeout resource  by default is 30 seconds to scan NLS dir. 
     */

    p = languageList;
    strcpy( p, "C");

    signal (SIGALRM, MakeLangAbort);
    alarm ((unsigned) langListTimeout);

    if (!setjmp (langJump)) {
        ScanNLSDir(DEF_NLS_DIR);
    }
    else {
        LogError(ReadCatalog(MC_LOG_SET,MC_LOG_NO_SCAN,MC_DEF_LOG_NO_SCAN),
                   DEF_NLS_DIR, langListTimeout);
    }

    alarm (0);
    signal (SIGALRM, SIG_DFL);


    /*
     *  sort the list to eliminate duplicates and replace in global array...
     */

    p = savelist = strdup(languageList);
    nlang = 0;

    while ( (s = strtok(p, DELIM)) != NULL ) {

        if ( nlang == 0 ) {
            lang[0] = s;
            lang[++nlang] = 0;
            p = NULL;
            continue;
        }

        for (i = nlang; i > 0 && strcmp(s,lang[i-1]) < 0; i--);

        if (i==0 || strcmp(s,lang[i-1]) != 0 ) {
            for (j = nlang; j > i; j--)
                lang[j] = lang[j-1];

            lang[i] = s;
            lang[++nlang] = 0;
        }

        p = NULL;
    }


    p = languageList;
    strcpy(p,"");

    for ( i = 0; i < nlang; i++) {
        strcat(p, lang[i]);
        strcat(p, " ");
    }

    free(savelist);

}        


static int
MatchesFileSuffix(char *filename, char *suffix)
{
    int		retval = 0;
#if defined(_AIX) || defined(SVR4) || defined (__osf__) || defined(linux) || \
	defined(CSRG_BASED)
    char	*pch;
	     
    /*
     * The assumption here is that the use of strrstr is
     * to determine if "dp->d_name" ends in ".cat".
     */
    pch = filename;
    if ((int) strlen(filename) >= (int) strlen(suffix))
      pch = (char *)
	    strcmp(filename + (strlen(filename) - strlen (suffix)), suffix);

    return (pch == NULL);
#else
    return (strrstr(filename, suffix) != NULL);
#endif
}

/***************************************************************************
 *
 *  ScanNLSDir
 *
 *  Scan a directory structure to see if it contains an installed language.
 *  If so, the name of the language is appended to a global list of languages.
 *
 *  Scan method and scan directory will vary by platform. 
 *
 ***************************************************************************/


static void
ScanNLSDir(char *dirname)

#if defined(_AIX)
/*
 * Search installed locale names for AIX 3.2.5
 */
{
    DIR *dirp;
    struct dirent *dp;

    /*  Search valid locales which are locale database files in
     *  /usr/lib/nls/loc.
     *  File name is "??_??" which can be used as LANG variable.
     */
    if((dirp = opendir(dirname)) != NULL)
    {
        while((dp = readdir(dirp)) != NULL)
	{
            if(strlen(dp->d_name) == 5 && dp->d_name[2] == '_')
	    {
                if((int) strlen(languageList) + 7 < LANGLISTSIZE )
		{
                    strcat(languageList, " ");
                    strcat(languageList, dp->d_name);
                }
            }
        }
        closedir(dirp);
    }
}

#elif defined(hpV4)

#define LOCALE		"locale.inf"
#define LOCALEOLD	"locale.def"
#define COLLATE8	"collate8"
#define MAILX		"mailx"
#define ELM		"elm"
#define MSGCAT		".cat"
#define DOT		"."
#define DOTDOT		".."

/*
 * Scan for installed locales on HP platform.
 */
{
    /***************************************************************************
     *  Scan supplied NLS directory structure to see if it contains an
     *  installed language.  If so, the name of the language is appended
     *  to a global list of languages.
     *
     *  This routine is recursively called as a directory structure is
     *  traversed.
     *
     *************************************************************************/

    DIR *dirp;
    struct dirent *dp;
    struct stat	statb;

    char buf[1024];
    
    /*
     *  Scan input directory, looking for a LOCALE file. If a sub-directory
     *  is found, recurse down into it...
     */
    if ( (dirp = opendir(dirname)) != NULL )
    {
	while ( (dp = readdir(dirp)) != NULL )
	{
	    /*
	     *  ignore files that are known not to be candidates...
	     */
	    if ( MatchesFileSuffix(dp->d_name, MSGCAT) ||
	         (strcmp (dp->d_name, COLLATE8)	== 0 ) ||
	         (strcmp (dp->d_name, MAILX)	== 0 ) ||
	         (strcmp (dp->d_name, ELM)	== 0 ) ||
	         (strcmp (dp->d_name, DOT)	== 0 ) ||
	         (strcmp (dp->d_name, DOTDOT)	== 0 ) )
	      continue;


	    /*
	     *  check to see if this is the locale file...
	     */
	    if ( (strcmp(dp->d_name, LOCALEOLD) == 0 ) ||
	         (strcmp(dp->d_name, LOCALE)    == 0 ) )
	    {
		char *p, *s;

		/*
		 *  Convert directory name to language name...
		 */
		if ( (p = strstr(dirname, DEF_NLS_DIR)) != NULL )
		{
		    p += strlen(DEF_NLS_DIR);
		    if ( *p == '/' )
		      p++;

		    s = p;
		    while ( (p = strchr(s,'/')) != NULL )
		      *p = '.';

		    /*
		     *  append to global list of languages...
		     */
		    if ((int) (strlen(languageList)+strlen(s)+2) < LANGLISTSIZE)
		    {
			strcat(languageList, " ");
			strcat(languageList, s);
		    }
		}

		continue;
	    }
	
	    /*
	     *  if this file is a directory, scan it also...
	     */
	    strcpy(buf, dirname);
	    strcat(buf, "/");
	    strcat(buf, dp->d_name);

	    if  (stat(buf, &statb) == 0  &&  S_ISDIR(statb.st_mode))
	      ScanNLSDir(buf);
	}

	closedir(dirp);
    }
}

#elif defined(__osf__)

#if defined(__osf__obsoleted)
#define LANGUAGE_LIST_CMD	"/bin/locale -a"
/*
 * Scan for installed locales on DEC platform
 */
{
    FILE	*f;
    char	locale[MAXPATHLEN];

    if (NULL == (f = popen(LANGUAGE_LIST_CMD, "r")))
      return;

    while (NULL != fgets(locale, sizeof(locale), f))
    {
	if  (locale[0] != '.' &&
             LANGLISTSIZE > (int) (strlen(languageList) + strlen(locale) + 2))
	{
	    int	len = strlen(locale);

	    if ('\n' == locale[len-1])
	      locale[len-1] = '\0';
	    strcat(languageList, " ");
	    strcat(languageList, locale);
	}
    }

    pclose(f);
}

#else

{
#include <fnmatch.h>
    char *str, *p;
    char **ignore = NULL;
    int num_ignore = 0;
    int max_ignore = 0;
    int listlen = 0;
    const char *delim = " \t";
    DIR *dirp;
    struct dirent *entp;

    /*
     * Convert the string of locale patterns to an array.  It will
     * be easier to loop through when we start matching locales.
     */

    if (!(str = strdup(ignoreLocales)))
	return;

    for (p = strtok(str, delim); p; p = strtok(NULL, delim)) {
	if (num_ignore >= max_ignore) {
	    max_ignore += 16;
	    if (!(ignore = realloc(ignore, max_ignore * sizeof(char *)))) {
		free(str);
		return;
	    }
	}

	ignore[num_ignore++] = p;
    }

    /*
     * Assume that each file of the form ??_??* is a locale.  If
     * the locale doesn't match any of the ignore patterns, add it
     * to the language list with a space separator.
     *
     * Seed the list with C and POSIX.  They're built into libc
     * and don't have locale files.  Checking ignoreLocales for them
     * isn't worth the effort.
     */

    strcpy(languageList, "C POSIX");
    listlen = strlen(languageList);

    if (dirp = opendir(dirname)) {
	while (entp = readdir(dirp)) {
	    int namelen = strlen(entp->d_name);
	    if (namelen >= 5 && entp->d_name[2] == '_') {
		int  i, match = 0;
		for (i = 0; i < num_ignore; i++) {
		    if (!fnmatch(ignore[i], entp->d_name, 0)) {
			match = 1;
			break;
		    }
		}

		if (match)
		    continue;

		/* 1 for space-separator, 1 for null-terminator */
		if (listlen + 1 + namelen + 1 > LANGLISTSIZE)
		    break;

		languageList[listlen++] = ' ';

		strcpy(&languageList[listlen], entp->d_name);
		listlen += namelen;
	    }
	}

	languageList[listlen++] = '\0';
	closedir(dirp);
    }

    free(ignore);
    free(str);
}
#endif /* __osf__obsoleted__ */

#elif defined(sun)
/*
 * Scan for installed locales on Sun platform.
 */
{
    DIR *dirp;
    struct dirent *dp;
    char* filename; 
    char path1[MAXPATHLEN], path2[MAXPATHLEN];
    struct stat stat1, stat2;
    int retval1, retval2;

    /* 
     * To determin the fully installed locale list, check several locations.
     */
    if((dirp = opendir(DEF_X11_NLS_SHARE_DIR)) != NULL)
    {
        while((dp = readdir(dirp)) != NULL)
	{
	    filename = dp->d_name;

	    if  ( filename[0] != '.' &&
                 (int)(strlen(languageList) +
		       strlen(filename) + 2) < LANGLISTSIZE)
	    {
		(void) sprintf(path1, "%s/%s", DEF_X11_NLS_LIB_DIR, filename);
		(void) sprintf(path2, "%s/%s", dirname, filename);
	        retval1 = stat(path1, &stat1);
	        retval2 = stat(path2, &stat2);

	    	if ( retval1==0 && retval2==0 &&
		     S_ISDIR(stat1.st_mode) && S_ISDIR(stat2.st_mode) )
		{
		    strcat(languageList, " ");
		    strcat(languageList, filename);
		}
            }
        }
        closedir(dirp);
    }
}

#elif defined(__uxp__) || defined(USL)

#define LC_COLLATE	"LC_COLLATE"
#define LC_CTYPE	"LC_CTYPE"
#define LC_MESSAGES	"LC_MESSAGES"
#define LC_MONETARY	"LC_MONETARY"
#define LC_NUMERIC	"LC_NUMERIC"
#define LC_TIME		"LC_TIME"

/*
 * Scan for installed locales on Fujitsu and Novell/SCO platforms.
 */
{
    DIR *nls_dirp, *locale_dirp;
    struct dirent *dp;
    char* locale; 
    char locale_path[MAXPATHLEN];
    struct stat locale_stat;
    int retval;

    /* 
     * To determin the fully installed locale list, check several locations.
     */
    if((nls_dirp = opendir(dirname)) != NULL)
    {
        while((dp = readdir(nls_dirp)) != NULL)
	{
	    int	is_locale = 0;

	    locale = dp->d_name;

	    /*
	     * A locale is indicated by a directory which has one or more
	     * of the following subdirectories:
	     *		LC_COLLATE
	     *		LC_CTYPE
	     *		LC_MESSAGES
	     *		LC_MONETARY
	     *		LC_NUMERIC
	     *		LC_TIME
	     */
	    (void) sprintf(locale_path, "%s/%s", dirname, locale);
	    retval = stat(locale_path, &locale_stat);
	    if ( (0 != retval) || (! S_ISDIR(locale_stat.st_mode)) )
	      continue;

	    if (NULL == (locale_dirp = opendir(locale_path)) )
	      continue;
	    
	    is_locale = FALSE;
            while (NULL != (dp = readdir(locale_dirp)) &&
		   FALSE == is_locale)
	    {
		if ((0 == strcmp(dp->d_name, LC_COLLATE)) ||
		    (0 == strcmp(dp->d_name, LC_CTYPE)) ||
		    (0 == strcmp(dp->d_name, LC_MESSAGES)) ||
		    (0 == strcmp(dp->d_name, LC_MONETARY)) ||
		    (0 == strcmp(dp->d_name, LC_NUMERIC)) ||
		    (0 == strcmp(dp->d_name, LC_TIME)) )
		  is_locale = 1;
	    }

            closedir(locale_dirp);

	    if (is_locale &&
                LANGLISTSIZE > (int) (strlen(languageList)+strlen(locale)+2) )
	    {
		strcat(languageList, " ");
		strcat(languageList, locale);
            }
        }

        closedir(nls_dirp);
    }
}

#else /* !_AIX && !hpV4 && !__osf__ !sun && !USL && !__uxp__ */
/*
 * Scan for installed locales on generic platform
 */
{
    DIR *dirp;
    struct dirent *dp;
    char* locale; 
    char locale_path[MAXPATHLEN];
    struct stat locale_stat;
    int retval;

    /* 
     * To determin the fully installed locale list, check several locations.
     */
    if(NULL != (dirp = opendir(dirname)))
    {
        while((dp = readdir(dirp)) != NULL)
	{
	    locale = dp->d_name;

            if ( (strcmp(dp->d_name, ".") == 0) ||
                 (strcmp(dp->d_name, "..") == 0) )
              continue;

	    if (locale[0] != '.' &&
                LANGLISTSIZE > (int) (strlen(languageList)+strlen(locale)+2));
	    {
		(void) sprintf(locale_path, "%s/%s", dirname, locale);
	        retval = stat(locale_path, &locale_stat);

	    	if (0 == retval && S_ISDIR(locale_stat.st_mode))
		{
		    strcat(languageList, " ");
		    strcat(languageList, locale);
		}
            }
        }
        closedir(dirp);
    }
}

#endif

#endif /* ENABLE_DYNAMIC_LANGLIST */

#ifdef _AIX
#define ENVFILE "/etc/environment"

/* Refer to the LANG environment variable, first.
 * Or, search a line which includes "LANG=XX_XX" in /etc/environment.
 * If succeeded, set the value to d->language.
 */
void
SetDefaultLanguage(struct display *d)
{
    FILE *file;
    char lineBuf[160];
    int n;
    char *p;
    char *lang = NULL;

    if((lang = getenv( "LANG" )) == NULL ) {
	if((file = fopen(ENVFILE, "r")) != NULL) {
	    while(fgets(lineBuf, sizeof(lineBuf) - 1, file)) {
		n = strlen(lineBuf);
		if(n > 1 && lineBuf[0] != '#') {
		    if(lineBuf[n - 1] == '\n')
			lineBuf[n - 1] = '\0';
		    if((p = strstr(lineBuf, "LANG=")) != NULL) {
			p += 5;
			if(strlen(p) == 5 && p[2] == '_') {
			    lang = p;
			    break;
			}
		    }
		}
	    }
	    fclose(file);
	}
    }
    if(lang != NULL && strlen(lang) > 0) {
    /*
     * If LANG is set for hft, we need to change it for X.
     * Currently there are four hft LANG variables.
     */
        d->language = (char *)malloc(strlen(lang)+1);
        if(strcmp(lang, "En_JP") == 0)
            strcpy(d->language, "Ja_JP");
        else if(strcmp(lang, "en_JP") == 0)
            strcpy(d->language, "ja_JP");
        else if(strcmp(lang, "en_KR") == 0)
            strcpy(d->language, "ko_KR");
        else if(strcmp(lang, "en_TW") == 0)
            strcpy(d->language, "zh_TW");
        else
            strcpy(d->language, lang);
    }
}
#endif /* _AIX */


char **
setLang( struct display *d, char **env , char *langptr)
{
    char  langlist[LANGLISTSIZE];
    int   s = 0;
    char *element = NULL;
    int   set_def_lang = FALSE;
 
 
    if (NULL != langptr)
      Debug("setLang():  langlist = %s\n", langptr);
    else
      Debug("setLang():  langlist = NULL\n");

    if (langptr)
      strcpy(langlist, langptr);
    else
      strcpy(langlist, getEnv(env, "LANGLIST"));

    if (langlist && (int) strlen(langlist) > 0) {
        element = strtok(langlist, DELIM);
        while(element) {
            set_def_lang = FALSE;
            if (strcmp(element,d->language) == 0){
                env = setEnv(env, "LANG",  d->language);
                break;
            }
            else
              set_def_lang = TRUE;
 
            s += strlen(element) +1;
            element = strtok(langlist+s, DELIM);
        }
    } else
      set_def_lang = TRUE;
 
    if (set_def_lang) {
        env = setEnv(env, "LANG", "C");
        d->language = strdup("C");
    }
    return env;
}

static char localHostbuf[256];
static int  gotLocalHostname;

char *
localHostname ()
{
    if (!gotLocalHostname)
    {
        XmuGetHostname (localHostbuf, sizeof (localHostbuf) - 1);
        gotLocalHostname = 1;
    }
    return localHostbuf;
}

