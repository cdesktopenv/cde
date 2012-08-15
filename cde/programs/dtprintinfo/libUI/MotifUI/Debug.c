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
/* $TOG: Debug.c /main/5 1998/04/06 13:32:19 mgreess $ */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <X11/Intrinsic.h>
#include <X11/Xlibint.h>

#define INT_MESSAGE3    "XIO:  fatal IO error %d (%s) on X server \"%s\"\r\n"
#define INT_MESSAGE4    "      after %lu requests (%lu known processed) with %d events remaining.\r\n"
#define INT_MESSAGE5    "    The connection was probably broken by a server shudown or KillClient.\r\n"

static int PrintXError(Display *dpy, XErrorEvent *event, FILE *fp);
static  char *SysErrorMsg(int n);

/* Error Handlers */
static  int XIOError(Display *dpy);
static  int XError(Display *dpy, XErrorEvent *event);
static  void _XtError(String);

static Boolean G_DumpCore;

void
Debug(
    XtAppContext app_context
    )
{
    XSetErrorHandler(XError);
    XSetIOErrorHandler(XIOError);
    XtAppSetErrorHandler(app_context, _XtError);
    G_DumpCore = True;
}

/*
 *   NAME:     XError
 *   FUNCTION:
 *   RETURNS:
 */
static int
XError(
   Display *dpy,
   XErrorEvent *event
   )
{
    if (PrintXError(dpy, event, stderr) == 0)
        return 0;
    if (G_DumpCore)
        kill(getpid(), SIGQUIT);
    else
        exit(1);
}

/*
 *   NAME:     XIOError
 *   FUNCTION:
 *   RETURNS:
 */
static int
XIOError(
   Display *dpy
   )
{
    fprintf(stderr, INT_MESSAGE3,
        errno, SysErrorMsg (errno), DisplayString (dpy));
    fprintf(stderr, INT_MESSAGE4,
        NextRequest(dpy) - 1, LastKnownRequestProcessed(dpy), QLength(dpy));

    if (errno == EPIPE)
        fprintf(stderr, INT_MESSAGE5);
    if (G_DumpCore)
        kill(getpid(), SIGQUIT);
    else
        exit(1);
}

/*
 *   NAME:     SysErrorMsg
 *   FUNCTION:
 *   RETURNS:
 */
static char *
SysErrorMsg(
   int n
   )
{
#if !defined(linux) && !defined(__FreeBSD__)
    extern char *sys_errlist[];
    extern int sys_nerr;
#endif
    char *s = ((n >= 0 && n < sys_nerr) ? sys_errlist[n] : "unknown error");
    return (s ? s : "no such error");
}

/*
 *   NAME:     PrintXError
 *   FUNCTION:
 *   RETURNS:
 */
static int
PrintXError(
   Display *dpy,
   XErrorEvent *event,
   FILE *fp
   )
{
    char buffer[BUFSIZ];
    char mesg[BUFSIZ];
    char number[32];
    char *mtype = "XlibMessage";
    register _XExtension *ext = (_XExtension *)NULL;
    _XExtension *bext = (_XExtension *)NULL;
    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
    fprintf(fp, "%s:  %s\n  ", mesg, buffer);
    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code %d",
        mesg, BUFSIZ);
    fprintf(fp, mesg, event->request_code);
    if (event->request_code < 128)
      {
        sprintf(number, "%d", event->request_code);
        XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer, BUFSIZ);
      }
    else
      {
        for (ext = dpy->ext_procs;
            ext && (ext->codes.major_opcode != event->request_code);
            ext = ext->next)
            ;
        if (ext)
            strcpy(buffer, ext->name);
        else
            buffer[0] = '\0';
      }
    fprintf(fp, " (%s)\n", buffer);
    if (event->request_code >= 128)
      {
        XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Request Minor code %d",
            mesg, BUFSIZ);
        fputs("  ", fp);
        fprintf(fp, mesg, event->minor_code);
        if (ext)
          {
            sprintf(mesg, "%s.%d", ext->name, event->minor_code);
            XGetErrorDatabaseText(dpy, "XRequest", mesg, "", buffer, BUFSIZ);
            fprintf(fp, " (%s)", buffer);
          }
        fputs("\n", fp);
      }
    if (event->error_code >= 128)
      {
        /* kludge, try to find the extension that caused it */
        buffer[0] = '\0';
        for (ext = dpy->ext_procs; ext; ext = ext->next)
          {
            if (ext->error_string)
                (*ext->error_string)(dpy, event->error_code, &ext->codes,
                    buffer, BUFSIZ);
            if (buffer[0])
              {
                bext = ext;
                break;
              }
            if (ext->codes.first_error &&
                ext->codes.first_error < (int)event->error_code &&
                (!bext || ext->codes.first_error > bext->codes.first_error))
                bext = ext;
          }
        if (bext)
            sprintf(buffer, "%s.%d", bext->name,
                event->error_code - bext->codes.first_error);
        else
            strcpy(buffer, "Value");
        XGetErrorDatabaseText(dpy, mtype, buffer, "", mesg, BUFSIZ);
        if (mesg[0])
          {
            fputs("  ", fp);
            fprintf(fp, mesg, event->resourceid);
            fputs("\n", fp);
          }
        /* let extensions try to print the values */
        for (ext = dpy->ext_procs; ext; ext = ext->next)
          {
            if (ext->error_values)
                (*ext->error_values)(dpy, event, fp);
          }
      }
    else if ((event->error_code == BadWindow) ||
        (event->error_code == BadPixmap) ||
        (event->error_code == BadCursor) ||
        (event->error_code == BadFont) ||
        (event->error_code == BadDrawable) ||
        (event->error_code == BadColor) ||
        (event->error_code == BadGC) ||
        (event->error_code == BadIDChoice) ||
        (event->error_code == BadValue) ||
        (event->error_code == BadAtom))
      {
        if (event->error_code == BadValue)
            XGetErrorDatabaseText(dpy, mtype, "Value", "Value 0x%lx",
                mesg, BUFSIZ);
        else if (event->error_code == BadAtom)
            XGetErrorDatabaseText(dpy, mtype, "AtomID", "AtomID 0x%lx",
                mesg, BUFSIZ);
        else
            XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%lx",
                mesg, BUFSIZ);
        fputs("  ", fp);
        fprintf(fp, mesg, event->resourceid);
        fputs("\n", fp);
      }
    XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Error Serial #%ld",
        mesg, BUFSIZ);
    fputs("  ", fp);
    fprintf(fp, mesg, event->serial);
    XGetErrorDatabaseText(dpy, mtype, "CurrentSerial", "Current Serial #%ld",
        mesg, BUFSIZ);
    fputs("\n  ", fp);
    fprintf(fp, mesg, dpy->request);
    fputs("\n", fp);
    if (event->error_code == BadImplementation)
        return 0;
    return 1;
}

/*
 *   NAME:     XtError
 *   FUNCTION:
 *   RETURNS:
 */
static void
_XtError(
    String string
    )
{
    (void)fprintf(stderr, "XtToolkit Error: %s\n", string);
    if (G_DumpCore)
        kill(getpid(), SIGQUIT);
    else
        exit(1);
}
