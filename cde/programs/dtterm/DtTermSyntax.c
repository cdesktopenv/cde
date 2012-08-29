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
#ifndef lint
#ifdef  VERBOSE_REV_INFO
static char rcs_id[] = "$XConsortium: DtTermSyntax.c /main/4 1996/05/16 11:22:57 ageorge $";
#endif  /* VERBOSE_REV_INFO */
#endif  /* lint */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#include "TermHeader.h"
#include "TermPrimMessageCatI.h"
struct _options {
    char *opt;
    char *desc;
    struct _options *next;
};

static struct _options *optHead;
static struct _options *messageHead;

static char *options_default[] = {
"-/+132				enable/disable 80<->132 column escape seq",
"-/+aw				enable/disable autowrap",
"-/+bs				turn off/on Term background is select color",
"-display displayname		X server to contact",
"-e command args		command to execute",
"-fb fontset			bold text font",
"-fn fontset			normal text font",
"-geometry geom			size (in characters) and position of window",
"-help				print out this message",
"-/+iconic			start/do not start iconic",
"-/+j				enable/disable jump scroll",
"-/+kshMode			enable/disable ksh mode",
"-/+l				enable/disable logging",
"-lf filename			logging filename",
"-/+ls				enable/disable login shell",
"-/+map				enable/disable map window on pty output",
"-/+mb				enable/disable margin bell",
"-ms color			pointer color",
"-n string			specify icon name",
"-name string			client instance, icon, and title strings",
"-nb distance			specify distance for right margin bell",
"-/+rw				enable/disable reverse wrap",
"-/+sb				enable/disable scroll bar",
"-/+sf				enable/disable SUN function keys",
"-sl number[s]			number of scrolled lines [screens] to save",
"-ti name			string used for programmatic identification",
"-title string			title string for window",
"-tm string			terminal mode keywords and characters",
"-tn name			TERM environment variable name",
"-usage				print out this message",
"-/+vb				enable/disable visual bell",
"-xrm resourcestring		additional resource specifications",
"-C				console mode",
"-Sxxd				slave mode on \"ttyxx\" file descriptor \"d\"",
"-Sxxx.d			slave mode on \"ttyxxx\" file descriptor \"d\"",
"End-Of-List",
};

static char *message_defaults[] = {
"The -e option, if given must appear at the end of the command line,",
"otherwise the user's default shell will be started.  Options that start",
"with a plus sign (+) restore the default.",
"End-Of-List",
};

static void GetUsage()
{
    register struct _options *optPtr;
    register int i;
    register char *c;
    char *c2;
    int num_messages;
    optHead = (struct _options *) 0;
    optPtr  = (struct _options *) 0;

    for (i = 1; ; i++) {
        /* 
        ** get the next option...
        */
   	num_messages = i;
        c2 = GETMESSAGE(NL_SETN_Syntax, i, options_default[i-1]);
	c = XtMalloc(strlen(c2) + 1);
	(void) strcpy(c, c2);
        /* 
        ** check and see if we are at the end of the list...
        */
        if (!strcmp(c, "End-Of-List"))
            break;

        /* 
        ** allocate the next entry...
        */
        if (!optHead) {
            optHead = (struct _options *) malloc(sizeof(struct _options));
            optPtr = optHead;
        } else {
            optPtr->next = (struct _options *) malloc(sizeof(struct _options));
            optPtr = optPtr->next;
        }

#ifdef	DKS
        /* 
        ** did we run out of malloc space...
        */
        if (!optPtr) {
            errno = 0;
#ifdef    _VUE_NO_PROTO
            SysError(HPT_MALLOC4);
#else  /* _VUE_NO_PROTO */
            SysError(HPT_MALLOC4, NULL);
#endif /* _VUE_NO_PROTO */
        }
#endif	/* DKS */

        /* 
        ** there is no next element yet...
        */
        optPtr->next = (struct _options *) 0;

        /* 
        ** the first part of the string is the option...
        */
        optPtr->opt = c;
        /* 
        ** find a tab...
        */
        while (*c && ('\t' != *c))
            (void) c++;
        /* 
        ** this marks the end of the option...
        */
        if (*c)
            *c++ = '\0';
        /* 
        ** skip over any more tabs...
        */
        while (*c && ('\t' == *c))
            (void) c++;
        /* 
        ** and this is the beginning of the option desc..
        */
        optPtr->desc = c;
    }

    messageHead = (struct _options *) 0;
    optPtr      = (struct _options *) 0;

    for (i = num_messages + 1; ; i++) {
        /* 
        ** get the next message string...
        */
        c2 = GETMESSAGE(NL_SETN_Syntax, i,message_defaults[i - num_messages - 1]);
	c = XtMalloc(strlen(c2) + 1);
	(void) strcpy(c, c2);
        /* 
        ** check and see if we are at the end of the list...
        */
        if (!strcmp(c, "End-Of-List"))
            break;

        /* 
        ** allocate the next entry...
        */
        if (!messageHead) {
            messageHead = (struct _options *) malloc(sizeof(struct _options));
            optPtr = messageHead;
        } else {
            optPtr->next = (struct _options *) malloc(sizeof(struct _options));
            optPtr = optPtr->next;
        }

#ifdef	DKS
        /* 
        ** did we run out of malloc space...
        */
        if (!optPtr) {
            errno = 0;
#ifdef    _VUE_NO_PROTO
            SysError(HPT_MALLOC5);
#else  /* _VUE_NO_PROTO */
            SysError(HPT_MALLOC5, NULL);
#endif /* _VUE_NO_PROTO */
        }
#endif	/* DKS */

        /* 
        ** there is no next element yet...
        */
        optPtr->next = (struct _options *) 0;

        /* 
        ** the entire string is the "desc"...
        */
        optPtr->desc = c;
    }
}

void Syntax(char *programName, char *badOption)
{
    register struct _options    *optPtr;
    int                          col;
    int                          cols;
    char                        *c;
    char                        *fmt;
    int                          fmtlen;
    char                         buffer[BUFSIZ];

    /* 
    ** get the usage message string...
    */
    GetUsage();

    /* suppress codecenter "Assignment in conditional 'if' expression."
     * warning...
     */
    /*SUPPRESS 624*/
    if (c = getenv("COLUMNS"))  cols = atoi(c);
    else                        cols = 80;

    (void) fprintf(stderr, (GETMESSAGE(NL_SETN_Syntax,47,
                   "%s:  bad command line option \"%s\"\r\n\n")),
                   programName, badOption);

    (void) fprintf(stderr, (GETMESSAGE(NL_SETN_Syntax,48, "usage:  %s")),
                   programName);
    col = 8 + strlen(programName);
    /* 
    ** now that we are NLSized, we need to figure out the width that the
    ** format string adds to each option...
    */

    c = GETMESSAGE(NL_SETN_Syntax,50, " [%s]");
    fmt = XtMalloc(strlen(c) + 1);
    (void) strcpy(fmt, c);
    (void) sprintf(buffer, fmt, "");
    fmtlen = strlen(buffer);

    for (optPtr = optHead; optPtr; optPtr = optPtr->next) {
        /*DKS*DKS*DKS*
        ** the following 3 assumes that the msg_catalog doesn't add more than
        ** 2 characters to the string...
        */
        int len = fmtlen + strlen(optPtr->opt);
        if (col + len >= cols) {
            (void) fprintf(stderr, "%s", (GETMESSAGE(NL_SETN_Syntax,49, "\r\n   ")));
            col = 3;
        }
        (void) fprintf(stderr, fmt, optPtr->opt);
        col += len;
    }
    (void) fprintf(stderr, (GETMESSAGE(NL_SETN_Syntax,51,
                   "\r\n\nType \"%s -help\" for a full description.\r\n\n")),
                   programName);

    (void) exit(1);
}

void Help(char *programName)
{
    register struct _options     *optPtr;
    int                           width = 0;

    /* 
    ** get the usage message string...
    */
    GetUsage();

    (void) fprintf(stderr, "%s", (GETMESSAGE(NL_SETN_Syntax,52, "usage:\n")));
    (void) fprintf(stderr, (GETMESSAGE(NL_SETN_Syntax,53,
                   "\t%s [-options ...] [-e command args]\n\n")),
                   programName);
    (void) fprintf(stderr, "%s", (GETMESSAGE(NL_SETN_Syntax,54,
                   "where options include:\n")));
    for (optPtr = optHead; optPtr; optPtr = optPtr->next)
        if (strlen(optPtr->opt) > (size_t) width) width = strlen(optPtr->opt);

    for (optPtr = optHead; optPtr; optPtr = optPtr->next)
        (void) fprintf(stderr, "    %-*s  %s\n", width, optPtr->opt,
                       optPtr->desc);
    
    (void) fprintf(stderr, "\n");
    for (optPtr = messageHead; optPtr; optPtr = optPtr->next)
        (void) fprintf(stderr, "%s\n", optPtr->desc);

    (void) fprintf(stderr, "\n");
    (void) exit(0);
}
