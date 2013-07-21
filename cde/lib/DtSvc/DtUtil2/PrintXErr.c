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
/*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/* Copyright    Massachusetts Institute of Technology    1985, 1986, 1987 */

/*
 * $TOG: PrintXErr.c /main/7 1998/04/10 07:46:38 mgreess $
 */

/* **  (c) Copyright Hewlett-Packard Company, 1990.*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*Lifted from xlib code.  How to print a reasonably complete message */
/*and NOT exit.*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*$TOG: PrintXErr.c /main/7 1998/04/10 07:46:38 mgreess $*/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>

#include <Dt/UserMsg.h>

int 
_DtPrintDefaultError(
        Display *dpy,
        XErrorEvent *event,
        char *msg )
{
    _DtPrintDefaultErrorSafe(dpy, event, msg, BUFSIZ);
    /* XXX retval? */
}

#define _DTP_STRNCAT(s1, s2, nb, ec) \
{ \
 strncat((s1),(s2),(nb)); \
 (nb)-=strlen(s2); \
 if (0>=(nb)) return (ec); \
}

int 
_DtPrintDefaultErrorSafe(
        Display *dpy,
        XErrorEvent *event,
        char *msg,
	int bytes)
{
    char buffer[BUFSIZ];
    char fpBuf[BUFSIZ];
    char *fp;
    char mesg[BUFSIZ];
    char number[32];
    char *mtype = "XlibMessage";
    int nbytes = bytes-1;
    register _XExtension *ext = (_XExtension *)NULL;

    memset(msg, 0, bytes);


    {
      XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
      XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);

      _DTP_STRNCAT(msg, mesg,   nbytes, event->error_code);
      _DTP_STRNCAT(msg, ":  ",  nbytes, event->error_code);
      _DTP_STRNCAT(msg, buffer, nbytes, event->error_code);
      _DTP_STRNCAT(msg, "\n  ", nbytes, event->error_code);
    }

    {
      XGetErrorDatabaseText(
		dpy, mtype, "MajorCode", "Request Major code %d", mesg, BUFSIZ);

      if (strlen(mesg) < BUFSIZ-10)
        fp = fpBuf;
      else
        fp = malloc(strlen(mesg) + 10);

      (void) sprintf(fp, mesg, event->request_code);

      _DTP_STRNCAT(msg, fp,     nbytes, event->error_code);

      if (fp != fpBuf) free(fp);
    }

    {
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
	  {
              strncpy(buffer, ext->name, BUFSIZ-1);
	      buffer[BUFSIZ-1] = '\0';
	  }
          else
            buffer[0] = '\0';
      }

      _DTP_STRNCAT(msg, " (",    nbytes, event->error_code);
      _DTP_STRNCAT(msg, buffer,  nbytes, event->error_code);
      _DTP_STRNCAT(msg, ")\n  ", nbytes, event->error_code);
    }

    {
      if (event->request_code >= 128)
      {
          XGetErrorDatabaseText(
		dpy, mtype, "MinorCode", "Request Minor code %d", mesg, BUFSIZ);
          if (strlen(mesg) < BUFSIZ-10)
            fp = fpBuf;
          else
            fp = malloc(strlen(mesg) + 10);

          (void) sprintf(fp, mesg, event->minor_code);
         
          _DTP_STRNCAT(msg, fp,     nbytes, event->error_code);

          if (fp != fpBuf) free(fp);

          if (ext)
	  {
              if (strlen(ext->name) < BUFSIZ-10)
                fp = fpBuf;
              else
                fp = malloc(strlen(ext->name) + 10);

              sprintf(fp, "%s.%d", ext->name, event->minor_code);

              XGetErrorDatabaseText(dpy, "XRequest", fp, "", buffer, BUFSIZ);

              if (fp != fpBuf) free(fp);
          }

          _DTP_STRNCAT(msg, "\n  (", nbytes, event->error_code);
          _DTP_STRNCAT(msg, buffer,  nbytes, event->error_code);
          _DTP_STRNCAT(msg, ")",     nbytes, event->error_code);
      }
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
                break;
        }
        if (buffer[0])
	{
	    if (strlen(buffer) < BUFSIZ-10)
              sprintf(buffer,
		"%s.%d",
		ext->name,
		event->error_code - ext->codes.first_error);
	}
        else
            strcpy(buffer, "Value");
        XGetErrorDatabaseText(dpy, mtype, buffer, "Value 0x%x", mesg, BUFSIZ);
        if (*mesg)
	{
            if (strlen(mesg) < BUFSIZ-10)
              fp = fpBuf;
            else
              fp = malloc(strlen(mesg) + 10);

            (void) sprintf(fp, mesg, event->resourceid);

            _DTP_STRNCAT(msg, fp,     nbytes, event->error_code);
            _DTP_STRNCAT(msg, "\n  ", nbytes, event->error_code);

            if (fp != fpBuf) free(fp);
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
            XGetErrorDatabaseText(dpy, mtype, "Value", "Value 0x%x",
                                  mesg, BUFSIZ);
        else if (event->error_code == BadAtom)
            XGetErrorDatabaseText(dpy, mtype, "AtomID", "AtomID 0x%x",
                                  mesg, BUFSIZ);
        else
            XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
                                  mesg, BUFSIZ);

        if (strlen(mesg) < BUFSIZ-10)
          fp = fpBuf;
        else
          fp = malloc(strlen(mesg) + 10);

        (void) sprintf(fp, mesg, event->resourceid);

        _DTP_STRNCAT(msg, fp,     nbytes, event->error_code);
        _DTP_STRNCAT(msg, "\n  ", nbytes, event->error_code);

        if (fp != fpBuf) free(fp);
    }    

    {
        XGetErrorDatabaseText(
	  dpy, mtype, "ErrorSerial", "Error Serial #%d", mesg, BUFSIZ);

        if (strlen(mesg) < BUFSIZ-10)
          fp = fpBuf;
        else
          fp = malloc(strlen(mesg) + 10);

        (void) sprintf(fp, mesg, event->serial);

        _DTP_STRNCAT(msg, fp,     nbytes, event->error_code);
        _DTP_STRNCAT(msg, "\n  ", nbytes, event->error_code);

        if (fp != fpBuf) free(fp);
    }    

    {
        XGetErrorDatabaseText(
	  dpy, mtype, "CurrentSerial", "Current Serial #%d", mesg, BUFSIZ);

        if (strlen(mesg) < BUFSIZ-10)
          fp = fpBuf;
        else
          fp = malloc(strlen(mesg) + 10);

        (void) sprintf(fp, mesg, dpy->request);

        _DTP_STRNCAT(msg, fp,     nbytes, event->error_code);
        _DTP_STRNCAT(msg, "\n  ", nbytes, event->error_code);

        if (fp != fpBuf) free(fp);
    }    

    if (event->error_code == BadImplementation) return 0;
    return 1;
}
