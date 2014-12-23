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
/* $XConsortium: fontpath.c /main/4 1995/10/27 16:13:29 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
**  fontpath.c - font path modification routines
**
**  $fontpath.c,v 1.1 93/06/24 14:52:10 bill Exp $
**
**  Copyright 1993 Hewlett-Packard Company
*/

# include "dm.h"
# include <signal.h>
# include <X11/Xatom.h>
# include <setjmp.h>
#if defined(__FreeBSD__) && OSMAJORVERSION > 8
# include <utmpx.h>
#else
# include <utmp.h>
#endif
# include "vgproto.h"

/* 
**  Prototypes
*/


int ApplyFontPathMods( struct display *d, Display *dpy );
void GetSysParms( char **tzpp, char **fhpp, char **ftpp );
static int PathInPList(char *path, char **fplist, int listlen);
static int PathInZList(char *path, char *fplist, int listlen);
static int SeparateParts( char **path );


static void
DebugFontPath(char *note, char **path, int nelems)
{
    int i;
    Debug("  %s: %d elements\n",note,nelems);
    for (i=0; i<nelems; i++)
        Debug("    %s\n",path[i]);
}

static int 
ErrorHandler(Display *dpy, XErrorEvent *event)
{
    XmuPrintDefaultErrorMessage(dpy, event, stderr);
    return 0; /* nonfatal */
}

/* ________________________________________________________________
**|                                                                |
**| ApplyFontPathMods(d) -                                         |
**|                                                                |
**|     If new font path mods are mandated, fabricate and apply    |
**|     an appropriate new complete X server font path.            |
**|								   |
**| Specify: d = pointer to display structure			   |
**|								   |
**| Returns: nothing						   |
**|________________________________________________________________|
*/

int 
ApplyFontPathMods( struct display *d, Display *dpy )
{
    FILE *fin;
    char *s,*t;
    char *fph = NULL;
    char *fpt = NULL;
    char **fontPath,**newList;
    int numHeads = 0;
    int numTails = 0;
    int i,j,k,numPaths;

    Debug("ApplyFontPathMods() for %s\n",d->name);

    if (d->displayType.location == Foreign) {
        Debug("  Foreign display\n");
        return 0;
    }

    if (!(fontPath=XGetFontPath(dpy, &numPaths))) {
        Debug("  Can't get font path\n");
        return 0;
    }

    /*
    **  Font path mods can come from (in priority order):
    **
    **    1. FONT_PATH_HEAD/TAIL definitions in /etc/src.sh
    **    2. fontPathHead/Tail resources
    */

    GetSysParms(0,&fph,&fpt);

    if (fph)          { s = "sys parm file"; }
    else if (fpHead)  { s = "resource"; fph = strdup(fpHead); }
    if (fph && !*fph) { free(fph); fph = NULL; }
    if (fph)          Debug("  +fp (%s) %s\n",s,fph);

    if (fpt)          { s = "sys parm file"; }
    else if (fpTail)  { s = "resource"; fpt = strdup(fpTail); }
    if (fpt && !*fpt) { free(fpt); fpt = NULL; }
    if (fpt)          Debug("  fp+ (%s) %s\n",s,fpt);

    /* 
    **  Break up fph and fpt into constituent parts and 
    **  then reconstruct the complete, modified font path.
    **  During reconstruction we also eliminate redundancies.
    */

    numHeads = SeparateParts(&fph);
    numTails = SeparateParts(&fpt);
    if (numHeads || numTails) {
        newList = (char **) malloc((numHeads+numPaths+numTails)
            * sizeof(char *));
        if (newList) {
            for (s=fph, i=j=0; j<numHeads; j++) {
                if (!PathInPList(s,newList,i))
                    newList[i++] = s;
                while (*s) s++; s++;
            }
            for (j=0; j<numPaths; j++) {
                if (!PathInPList(fontPath[j],newList,i) &&
                    !PathInZList(fontPath[j],fpt,numTails)) {
                    newList[i++] = fontPath[j];
                }
            }
            for (s=fpt, j=0; j<numTails; j++) {
                if (!PathInPList(s,newList,i))
                    newList[i++] = s;   
                while (*s) s++; s++;
            }
            if (debugLevel > 0)
                DebugFontPath("Request (XSetFontPath)",newList,i);

            /*
            **  Tell X server to set new font path now.  Log failure,
            **  but don't let it be fatal.  (Note that caller should
            **  reset error handler to elsewhere when we return.)
            */

            (void)XSetErrorHandler(ErrorHandler);
            XSetFontPath(dpy, newList, i);
            XSync(dpy, True);
            free(newList);
            if (debugLevel > 0) {
                newList = XGetFontPath(dpy, &i);
                DebugFontPath("Confirm (XGetFontPath)",newList,i);
                XFreeFontPath(newList);
            }
        }
    }

    if (fph) free(fph);
    if (fpt) free(fpt);
    XFreeFontPath(fontPath);

    return 1;
}

/* ___________________________________________________________________
**|                                                                   |
**| PathInPList(path,fplist,listlen) -                                |
**|								      |
**|     Determine if a specific fontpath element is in a list,        |
**|     taking into account that identical elements may be formed     |
**|     differently (with multiple embedded and trailing slashes).    |
**|                                                                   |
**| Specify: (char *)path = the single element to be tested           |
**|          (char **)fplist = list of ptrs to asciz elements         |
**|          (int)listlen = number of pointers in the list            |
**|                                                                   |
**| Returns: TRUE if element is in the list                           |
**|___________________________________________________________________|
*/

static int
PathInPList(char *path, char **fplist, int listlen)
{
    char *s,*t;
    while (listlen-- > 0) {
        for (s=path, t=fplist[listlen]; *s && (*s == *t); ) {
            t++; while (*t == '/') t++;
            s++; while (*s == '/') s++;
        }
        if (!*s && !*t) return 1;
    }
    return 0;    
}

/* ___________________________________________________________________
**|                                                                   |
**| PathInZList(path,fplist,listlen) -                                |
**|								      |
**|     Determine if a specific fontpath element is in a list,        |
**|     taking into account that identical elements may be formed     |
**|     differently (with multiple embedded and trailing slashes).    |
**|                                                                   |
**| Specify: (char *)path = the single element to be tested           |
**|          (char *)fplist = list of concatenated asciz elements     |
**|          (int)listlen = number of elements in the list            |
**|                                                                   |
**| Returns: TRUE if element is in the list                           |
**|___________________________________________________________________|
*/

static int
PathInZList(char *path, char *fplist, int listlen)
{
    char *s,*t;
    for (t=fplist; listlen > 0; listlen--) {
        for (s=path; *s && (*s == *t); ) {
            t++; while (*t == '/') t++;
            s++; while (*s == '/') s++;
        }
        if (!*s && !*t) return 1;
        while (*t) t++; t++;
    }
    return 0;    
}

/* ________________________________________________________________
**|                                                                |
**| SeparateParts(path)					           |
**|								   |
**|     Break a comma-delimited asciz path string into its	   |
**|     separate asciz constituent parts.			   |
**|								   |
**| Specify: path = ptr to asciz path string (e.g., "as,df,jk")	   |
**|								   |
**| Returns: number of constituent parts, with path string         |
**|          converted into as many sequential asciz strings       |
**|          (e.g., "as\0df\0jk").				   |
**|________________________________________________________________|
*/

static int
SeparateParts( char **path )
{
    char *t,*s;
    int nparts = 0;
    if (path && *path)
	for (s=*path; t=strtok(s,","); s=NULL, nparts++);
    return nparts;
}

/* ___________________________________________________________________
**|                                                                   |
**| GetSysParms(tzpp,fhpp,ftpp) -				      |
**|								      |
**|     Extract TZ, FONT_PATH_HEAD, and FONT_PATH_TAIL definitions    |
**|     from the sys parms file (typically /etc/src.sh).              |
**|                                                                   |
**| Specify: (char **)tzpp = where to put ptr to TZ string            |
**|          (char **)fhpp = where to put ptr to FONT_PATH_HEAD       |
**|          (char **)ftpp = where to put ptr to FONT_PATH_TAIL       |
**|                                                                   |
**|     Specify a NULL (char **) for any undesired strings.           |
**|                                                                   |
**| Returns: Appropriate pointers to malloc'ed strings (NULL pointer  |
**|          if a string is neither requested nor defined).           |
**|___________________________________________________________________|
*/

void
GetSysParms( char **tzpp, char **fhpp, char **ftpp )
{
    FILE *fin;
    char *s,*t,buf[256];

    if (tzpp) *tzpp = NULL;
    if (fhpp) *fhpp = NULL;
    if (ftpp) *ftpp = NULL;
    if ((*sysParmsFile != '/') || !(fin=fopen(sysParmsFile,"r"))) {
        Debug("(GetSysParms) Can't open sys parms file\n");
        return;
    }
    Debug("(GetSysParms) Reading sys parms file\n");
    while (fgets(buf,255,fin)) {
        for (t=buf; *t && *t<=' '; t++); /* t -> EOS or nonblank */
        if (!*t || *t=='#') continue;    /* ignore comment lines */
        while (*t && *t!='\n') t++;      /* t -> EOS or newline  */
        if (*t) *t = '\0';               /* discard newline char */
        if (tzpp && !*tzpp)
            if ((s=strstr(buf,"TZ=")) && (t=strtok(s+3,"; \t")))
                *tzpp = (char *) strdup(t);
        if (fhpp && !*fhpp)
            if ((s=strstr(buf,"FONT_PATH_HEAD=")) && (t=strtok(s+15,"; \t")))
                *fhpp = (char *) strdup(t);
        if (ftpp && !*ftpp)
            if ((s=strstr(buf,"FONT_PATH_TAIL=")) && (t=strtok(s+15,"; \t")))
                *ftpp = (char *) strdup(t);
    }
    fclose(fin);
}    

