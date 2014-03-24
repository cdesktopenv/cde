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
/* $XConsortium: fileIO.c /main/8 1996/10/21 17:31:11 mgreess $ */
/*********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/******************************************************************************
 **  Program:           dticon
 **
 **  Description:       X11-based multi-color icon editor
 **
 **  File:              fileIO.c, which contains the following subroutines or
 **                     functions:
 **                       Do_FileIO()
 **                       Read_File()
 **                       Write_File()
 **                       Display_XPMFile()
 **                       Display_XBMFile()
 **                       Dump_AttribStruct()
 **
 ******************************************************************************
 **
 **  Copyright Hewlett-Packard Company, 1990, 1991, 1992.
 **  All rights are reserved.  Copying or reproduction of this program,
 **  except for archival purposes, is prohibited without prior written
 **  consent of Hewlett-Packard Company.
 **
 **  Hewlett-Packard makes no representations about the suitibility of this
 **  software for any purpose.  It is provided "as is" without express or
 **  implied warranty.
 **
 ******************************************************************************/
#ifdef __osf__
#include <stdlib.h>   /* for getenv() function prototype */
#endif
#include <sys/param.h>
#include <sys/stat.h>
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/FileSB.h>
#include <stdio.h>
#include <string.h>
#include "externals.h"
#include "main.h"

#ifdef __TOOLTALK
#include <Tt/tttk.h>
extern void ReplyToMessage( );
extern Tt_message replyMsg;
#endif

extern XtPointer _XmStringUngenerate (
				XmString string,
				XmStringTag tag,
				XmTextType tag_type,
				XmTextType output_type);

extern Widget   formatMenu_xpm_tb, formatMenu_xbm_tb;
extern Window tablet_win;
extern Widget editMenu_deleteHS_pb;
Boolean Read_File(), Write_File();
Pixmap pix_ret, shape_ret, mask_ret;
int successFormat, x_hot, y_hot;
unsigned int width_ret, height_ret;
extern GC scratch_gc;

extern void PixelTableClear();
extern int PixelTableLookup();
extern void send_tt_saved();
extern void Display_XPMFile(int, int);
extern void Display_XBMFile(int, int);


char  *tmpSave;            /* Save the file path being saved */
char  dummy[256];           /* mask file for use in main */
extern int SaveMeNot;
extern int SavedOnce;

/***************************************************************************
 *                                                                         *
 * Routine:   Do_FileIO                                                    *
 *                                                                         *
 * Purpose:   To read/write the current icon from/to the filename          *
 *            selected in the file selection box.                          *
 *                                                                         *
 *            If fileIOMode is FILE_READ, this file should be read in.     *
 *            If the read attempt fails, an error dialog pops up to        *
 *            inform the user.                                             *
 *                                                                         *
 *            If fileIOMode is FILE_WRITE, the currently loaded icon       *
 *            should be written to the named file, using the current file  *
 *            format (specified by 'fileFormat').  As with FILE_READ, if   *
 *            the write attempt fails, an error dialog pops up to inform   *
 *            the user.                                                    *
 *                                                                         *
 ***************************************************************************/

void
Do_FileIO(
        Widget wid,
        XtPointer client_unused,
        XmFileSelectionBoxCallbackStruct *callback_data )
{
  int   unmanageFileIO = True;
  struct stat statbuf;        /* Information on a file. */

#ifdef DEBUG
  if (debug)
    stat_out("Entering Do_FileIO\n");
#endif
  pix_ret = 0;
  shape_ret = 0;
  mask_ret = 0;

  /* get file name */
  if (SaveMeNot){
  tmpSave = (char *) _XmStringUngenerate(
					callback_data->value, NULL,
					XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT);
  }

  if (fileIOMode == FILE_READ) {
    if (!Read_File(tmpSave))
    {
      unmanageFileIO = False;
      DoErrorDialog( (GETSTR(16,2, "The file cannot be accessed\nor contains invalid data")) );
    }
    else {
      if (successFormat == FORMAT_XPM) {
        X_Hot = xpm_ReadAttribs.x_hotspot;
        Y_Hot = xpm_ReadAttribs.y_hotspot;
        Display_XPMFile(xpm_ReadAttribs.width, xpm_ReadAttribs.height);
       }
      else if (successFormat == FORMAT_XBM) {
        X_Hot = x_hot;
        Y_Hot = y_hot;
        Display_XBMFile(width_ret, height_ret);
       }
      Dirty = False;
      SavedOnce = True;   /* Implicitly saved, since we loaded the file */
      /*
         Turn off the HotSpot thing in the File menu.
      */
      if ( X_Hot == -1 )
        XtSetSensitive((Widget) editMenu_deleteHS_pb, False );

      if (unmanageFileIO)
         XtUnmanageChild(fileIODialog);
    } /* else */
  } /* if(FileIOMode...) */

  if (fileIOMode == FILE_WRITE) {
    if (stat(tmpSave, &statbuf) == 0 && SaveMeNot)
    {
      DialogFlag=SAVE_AS;
      DoQueryDialog( GETSTR(16,25, "File already exists.\n\nOK to overwrite the file?") );
    }
    else
    {
      if (!Write_File(tmpSave))
      {
        unmanageFileIO = False;
        DoErrorDialog( (GETSTR(16,4, "Unable to write data to file")) );
      }
      else{
          if (!SaveMeNot)
              SaveMeNot = True;
          Dirty = False;
          SavedOnce = True;
          if (unmanageFileIO)
             XtUnmanageChild(fileIODialog);
      }
    }
  }

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Do_FileIO\n");
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Read_File                                                    *
 *                                                                         *
 * Purpose:   To read the contents of the specified file and create either *
 *            a multi-color icon (if the file is an XPM file) or a bi-     *
 *            color icon (if the file is an XBM file).  Further, if the    *
 *            file is an XBM file, attempt to find a matching mask file    *
 *            and read it as well, using to result to determine which      *
 *            pixels should be set to transparent.                         *
 *                                                                         *
 ***************************************************************************/

Boolean
Read_File(
        char *fnameIn )
{
  char *base_name, *suffix, fname[MAXPATHLEN+1], *tmp, *tmp2;
  unsigned int mask_width_ret, mask_height_ret;
  int mask_x_hot, mask_y_hot, first;
  struct stat statBuf;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Read_File\n");
#endif

  if ( !fnameIn || !(*fnameIn) )
    return False;

  tmp  = strchr(fnameIn, ':');
  tmp2 = strchr(fnameIn, '/');

  /*** - convert from "<host>:/..." to a path on the locale host. ***/
  if (tmp && tmp2 && (tmp2 == tmp+1))
  {
    char	*netfile, *localfile;
    
    tmp[0] = '\0';
    netfile = tt_host_file_netfile(fnameIn, tmp+1);
    localfile = tt_netfile_file(netfile);

    strncpy(fname, localfile, MAXPATHLEN);
    tmp[0] = ':';

    tt_free(netfile);
    tt_free(localfile);
  }
  else     /* wasn't in form "<host>:/<path>" so use name as is... */
  {
    strcpy(fname, fnameIn);
  }

  /*** - if we got a NULL base_name, return FALSE             ***/
  base_name = strrchr(fname, '/');
  if (fname)
    base_name = (base_name ? base_name + 1 : fname);
  if (!base_name)
    return (False);

  /*** - if it's not a regular file, don't use it            ***/
  if (stat(fname, &statBuf) == 0)                   /* success */
  {
    if ( (statBuf.st_mode & S_IFMT) == S_IFDIR ||
         (statBuf.st_mode & S_IFMT) == S_IFCHR ||
         (statBuf.st_mode & S_IFMT) == S_IFBLK )
    {
      return False;
    }
    if (statBuf.st_size == 0)
    {
      Process_Clear();
      Process_Resize();
      strcpy(last_fname, fname);
      ChangeTitle();
      successFormat = FORMAT_NONE;
      return True;
    }
  }
  else
  {
    return False;            /* file doesn't exist, return failure */
  }


/*** before we do anything else, make sure ***/
/*** the editor interface refreshes itself ***/

  XmUpdateDisplay(mainWindow);

  pix_ret = 0;
  shape_ret = 0;
  mask_ret = 0;
  xpm_ReadAttribs.valuemask = READ_FLAGS;
  xpm_ReadAttribs.colorsymbols = colorSymbols;
  xpm_ReadAttribs.numsymbols = NUM_PENS;


/*** if the file ends in .pm or .xpm, try reading it as an ***/
/*** XPM file first.  Then try XBM format if XPM fails.    ***/
/***                                                       ***/
/*** if the file ends in .bm or .xbm, try reading it as an ***/
/*** XBM file first.  Then try XPM format if XBM fails.    ***/
/***                                                       ***/
/*** if the file doesn't match any of these suffixes, try  ***/
/*** reading it as an XPM file first.  Then try XBM format ***/
/*** if XPM fails.                                         ***/

/*** FIRST, does a suffix exist? ***/
  suffix = strrchr(base_name, '.');
  if ((suffix) && ((int)strlen(suffix) > 1))
    suffix++;
  if (suffix) {
    if (!strcmp(suffix, "bm") || !strcmp(suffix, "xbm"))
      first = FORMAT_XBM;
    else
      first = FORMAT_XPM;
   }
  else
    first = FORMAT_XPM;

/*** try to read the XPM/XBM file, in the order ***/
/*** specified by the 'first' format.           ***/
  if (first == FORMAT_XPM) {
    status = _DtXpmReadFileToPixmap(dpy, tablet_win,
                fname, &pix_ret, &shape_ret, &xpm_ReadAttribs);

#ifdef DEBUG
  if (debug) {
    if (debug_image)
      XDestroyImage(debug_image);
    if (debug_shape)
      XDestroyImage(debug_shape);
    debug_status = _DtXpmReadFileToImage(dpy, fname,
                &debug_image, &debug_shape, &xpm_ReadAttribs);
   }
#endif

    if (status != XpmSuccess) {
      status = XReadBitmapFile(dpy, tablet_win,
                fname, &width_ret, &height_ret, &pix_ret, &x_hot,
                &y_hot);
      if (status != BitmapSuccess) {
#ifdef DEBUG
  if (debug)
    stat_out("Leaving Read_File - XPM/XBM read attempt failed.\n");
#endif
        return (False);
       }
      else
        successFormat = FORMAT_XBM;
     }
    else
      successFormat = FORMAT_XPM;
   }
  else {
    status = XReadBitmapFile(dpy, tablet_win, fname,
                &width_ret, &height_ret, &pix_ret, &x_hot, &y_hot);
    if (status != BitmapSuccess) {
      status = _DtXpmReadFileToPixmap(dpy, tablet_win,
                fname, &pix_ret, &shape_ret, &xpm_ReadAttribs);
      if (status != XpmSuccess) {
#ifdef DEBUG
  if (debug)
    stat_out("Leaving Read_File - XBM/XPM read attempt failed.\n");
#endif
        return (False);
       }
      else
        successFormat = FORMAT_XPM;
     }
    else
      successFormat = FORMAT_XBM;
   }

/*** If we got this far, we successfully read in a file. ***/
/*** If the 'successFormat' is FORMAT_XBM, try to find   ***/
/*** and accompanying mask file and load it too.  The    ***/
/*** format for a mask file name is:                     ***/
/***     <pathname><base_name>_m<suffix> (optional suffix)***/
  if (successFormat == FORMAT_XBM) {
/*** does a suffix exist? ***/
    if (suffix) {
      strncpy(dummy, fname, ((suffix-fname)-1));
      dummy[(int) (suffix-fname)-1] = '\0';
      strcat(dummy, "_m.");
      strcat(dummy, suffix);
#ifdef DEBUG
  if (debug) {
    stat_out("  full-filename = '%s'\n", fname);
    stat_out("  suffix = '%s'\n", suffix);
   }
#endif
     }
    else {
         strcpy(dummy, fname);
         strcat(dummy, "_m");
    }
#ifdef DEBUG
  if (debug)
    stat_out("  mask-file = '%s'\n", dummy);
#endif
    status = XReadBitmapFile(dpy, tablet_win, dummy,
                &mask_width_ret, &mask_height_ret, &mask_ret,
                &mask_x_hot, &mask_y_hot);
    if (status == BitmapSuccess) {
      if ((width_ret != mask_width_ret) || (height_ret != mask_height_ret)) {
        XFreePixmap(dpy, mask_ret);
        mask_ret = 0;
       }
     }
    else
      mask_ret = 0;
   }

  strcpy(last_fname, fname);
  ChangeTitle();

#ifdef DEBUG
  if (debug) {
    stat_out("Finished Reading file '%s'\n", last_fname);
    stat_out("Leaving Read_File\n");
   }
#endif

  return (True);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Write_File                                                   *
 *                                                                         *
 * Purpose:   To write the current icon to the specified filename as       *
 *            either an XPM file or XBM file, depending on the current     *
 *            value of the fileFormat flag.  Further, if the current       *
 *            fileFormat is FORMAT_XBM, create a second XBM file which     *
 *            contains a mask for the XBM file just written.               *
 *                                                                         *
 ***************************************************************************/
Boolean
Write_File(
        char *fnameIn )
{
  extern int tt_tmpfile_fd;
  int i, j;
  int mask_needed;
  Boolean SUN;
  char *base_name, *suffix, fname[MAXPATHLEN], *tmp, *tmp2, *vend;
  Pixmap scratch_pix;
  XImage *scratch_shape, *scratch_mask;
  struct stat statbuf;        /* Information on a file. */


#ifdef DEBUG
  if (debug)
    stat_out("Entering Write_File\n");
#endif
  SUN = False;   /* Assume machine other than SUN */
  if ( !fnameIn || !(*fnameIn) )
    return False;

  tmp  = strchr(fnameIn, ':');
  tmp2 = strchr(fnameIn, '/');

  /*** - convert from "<host>:/..." to a path on the locale host. ***/
  if (tmp && tmp2 && (tmp2 == tmp+1))
  {
    char	*netfile, *localfile;
    
    tmp[0] = '\0';
    netfile = tt_host_file_netfile(fnameIn, tmp+1);
    localfile = tt_netfile_file(netfile);

    strncpy(fname, localfile, MAXPATHLEN - 1);
    fname[MAXPATHLEN - 1] = 0;
    tmp[0] = ':';

    tt_free(netfile);
    tt_free(localfile);
  }
  else     /* wasn't in form "<host>:/<path>" so use name as is... */
  {
    strcpy(fname, fnameIn);
  }


  base_name = strrchr(fname, '/');
  if (fname) {
    base_name = (base_name ? base_name + 1 : fname);
  }

  strcpy(last_fname, fname);
  ChangeTitle();

#ifdef DEBUG
  if (debug)
    stat_out("Writing file '%s'\n", last_fname);
#endif

  if (fileFormat == FORMAT_XPM) {
    xpm_WriteAttribs.x_hotspot = X_Hot;
    xpm_WriteAttribs.y_hotspot = Y_Hot;
    xpm_WriteAttribs.width  = icon_width;
    xpm_WriteAttribs.height = icon_height;
    xpm_WriteAttribs.cpp    = 1;
    xpm_WriteAttribs.colorsymbols = colorSymbols;
    xpm_WriteAttribs.numsymbols = NUM_PENS;
    xpm_WriteAttribs.ncolors = NUM_PENS;
    xpm_WriteAttribs.valuemask = WRITE_FLAGS;
#ifdef DEBUG
  if (debug)
    Dump_AttribStruct(&xpm_WriteAttribs);
#endif
    status = _DtXpmWriteFileFromPixmap(dpy, fname, color_icon, 0,
                                &xpm_WriteAttribs);

/*******
    status = _DtXpmWriteFileFromPixmap(dpy, fname, color_icon, NULL, NULL);
********/
    if (status != XpmSuccess) {
#ifdef DEBUG
  if (debug) {
    stat_out("Leaving Write_File - XPM write failed. ");
    switch (status) {
      case XpmOpenFailed : stat_out("(XpmOpenFailed)\n"); break;
      case XpmNoMemory   : stat_out("(XpmNoMemory)\n"); break;
      default            : stat_out("(UNKNOWN cause)\n"); break;
    }
  }
#endif
      return (False);
    }
  }
  else {
/*** FIRST, does a suffix exist? ***/
  suffix = strrchr(base_name, '.');
  if ((suffix) && ((int)strlen(suffix) > 1))
    suffix++;
/*** SECOND, is it a valid suffix? ***/
  if (suffix) {
    if (strcmp(suffix, "bm") && strcmp(suffix, "xbm"))
      suffix = NULL;
  }
/*** THIRD, construct the mask filename ***/
    if (suffix) {
      strncpy(dummy, fname, ((suffix-fname)-1));
      dummy[(int) (suffix-fname)-1] = '\0';
      strcat(dummy, "_m.");
      strcat(dummy, suffix);
    }
    else {
      strcpy(dummy, fname);
      strcat(dummy, "_m");
    }
/*** FOURTH, construct XImages for the shape and mask bitmaps ***/
    mask_needed = False;
    scratch_shape = XGetImage(dpy, mono_icon, 0, 0, icon_width, icon_height,
                        AllPlanes, format);
    scratch_mask = XGetImage(dpy, mono_icon, 0, 0, icon_width, icon_height,
                        AllPlanes, format);
    if (!scratch_shape || !scratch_mask) {
      if (scratch_shape)
        XDestroyImage(scratch_shape);
      if (scratch_mask)
        XDestroyImage(scratch_mask);
      return (False);
    }

    /* is this a SUN machine?                   */
    /* if so use special code else use old code */
    vend = ServerVendor(dpy);
    if ( strncmp( vend, "Sun", 3) == 0) {
       SUN = TRUE;
       for (i=0; i<icon_width; i++)
         for (j=0; j<icon_height; j++)
           if (XGetPixel(scratch_shape, i, j) == Transparent) {
            XPutPixel(scratch_shape, i, j,  white_pixel);
            mask_needed = True;
        }
       for (i=0; i<icon_width; i++)
         for (j=0; j<icon_height; j++)
           if (XGetPixel(scratch_mask, i, j) != Transparent)
             XPutPixel(scratch_mask, i, j,  black_pixel);
           else
             XPutPixel(scratch_mask, i, j,  white_pixel);
    }else {
       for (i=0; i<icon_width; i++)
         for (j=0; j<icon_height; j++)
          if (XGetPixel(scratch_shape, i, j) == Transparent) {
            XPutPixel(scratch_shape, i, j,  black_pixel);
            mask_needed = True;
          }
       for (i=0; i<icon_width; i++)
         for (j=0; j<icon_height; j++)
           if (XGetPixel(scratch_mask, i, j) != Transparent)
             XPutPixel(scratch_mask, i, j,  white_pixel);
           else
             XPutPixel(scratch_mask, i, j,  black_pixel);
    }


/*** FIFTH, write out the shape and mask bitmaps ***/
    scratch_pix = XCreatePixmap(dpy, root, icon_width, icon_height,
                                DefaultDepth(dpy, screen));
    if (!scratch_pix) {
      if (scratch_shape)
        XDestroyImage(scratch_shape);
      if (scratch_mask)
        XDestroyImage(scratch_mask);
      return (False);
    }
    /* don't set GXcopyInverte for SUN machines */
    if (!SUN)
    XSetFunction(dpy, Mono_gc, GXcopyInverted);
    XPutImage(dpy, scratch_pix, Mono_gc, scratch_shape, 0, 0, 0, 0,
                        icon_width, icon_height);
    XSetFunction(dpy, Mono_gc, GXcopy);
    status = XWriteBitmapFile(dpy, fname, scratch_pix, icon_width, icon_height,
                        X_Hot, Y_Hot);
    if (status != BitmapSuccess)
    {
      XDestroyImage(scratch_shape);
      XDestroyImage(scratch_mask);
      XFreePixmap(dpy, scratch_pix);
      return (False);
    }
    if (mask_needed) {
      XPutImage(dpy, scratch_pix, Mono_gc, scratch_mask, 0, 0, 0, 0,
                        icon_width, icon_height);
      status = XWriteBitmapFile(dpy, dummy, scratch_pix, icon_width,
                        icon_height, X_Hot, Y_Hot);
      if (status != BitmapSuccess)
      {
        XDestroyImage(scratch_shape);
        XDestroyImage(scratch_mask);
        XFreePixmap(dpy, scratch_pix);
        return (False);
      }
    } /***TAG***/
    XFreePixmap(dpy, scratch_pix);
    XDestroyImage(scratch_shape);
    XDestroyImage(scratch_mask);
  } /* else */

  /* Don't know if this is needed....
  if ( SUN )
     {
        SUN = False;
        black_pixel = 0;
        white_pixel = 1;
     }
  */

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Write_File\n");
#endif
    if( (fileFormat != FORMAT_XPM) && (!mask_needed) && (stat(tmpSave, &statbuf) == 0) )
      unlink(dummy);
#ifdef __TOOLTALK
    if (tt_tmpfile_fd != -1)
       send_tt_saved();
#endif

  return (True);
}

/***************************************************************************
 *                                                                         *
 * Routine:   Display_XPMFile                                              *
 *                                                                         *
 * Purpose:   Assuming that we have successfully read in an XPM file, use  *
 *            the data from the file to render the proper rasters to the   *
 *            color_icon and mono_icon (and their corresponding widgets).  *
 *                                                                         *
 *            Rendering the color icon is straight-forward, since the      *
 *            pixmap generated by Read_File() is already correct.  For     *
 *            the mono icon, we extract an XImage for the entire color     *
 *            pixmap, and then convert each pixel individually to one      *
 *            three values: black, white, or transparent.  For each pixel, *
 *            the following test is performed: 1st) compare it to each     *
 *            static color.  If a match is found, convert the pixel to     *
 *            the appropriate mono equivalent.  2nd) if the 1st step fails *
 *            to find a match, try the same comparisons against the        *
 *            dynamic colors.  3rd) if the 2nd step also fails, use the    *
 *            PixelTableLookup() function.                                 *
 *                                                                         *
 *            Once the coversion process is complete, both the color_icon  *
 *            and mono_icon (and their corresponding widgets) can be       *
 *            rendered.                                                    *
 *                                                                         *
 ***************************************************************************/

void
Display_XPMFile(
        int width,
        int height )
{
  int converted, i, j, k;
  int pixelTableIndex;
  XImage *scratch_img, *mono_img, *mask_img;
  Pixel cpixel, mpixel;
#ifdef DEBUG
  int reset_debug;
#endif

#ifdef DEBUG
  if (debug)
    stat_out("Entering Display_XPMFile\n");
  reset_debug=False;
#endif

/*** resize the color and mono icon windows, and the tablet ***/
/*** to their (new) correct height and width.               ***/
  Init_Icons(width, height, DO_NOT_SAVE);

/*** Grab an image for both (potentially modified) ***/
/*** color and mono conversions                    ***/
  scratch_img = XGetImage(dpy, pix_ret, 0, 0, width, height,
                                AllPlanes, format);
  mono_img = XGetImage(dpy, pix_ret, 0, 0, width, height,
                                AllPlanes, format);
  mask_img = NULL;
  if (shape_ret)
    mask_img = XGetImage(dpy, shape_ret, 0, 0, width, height,
                                AllPlanes, format);
#ifdef DEBUG
  if (debug) {
    debug = False;
    reset_debug = True;
   }
#endif
  PixelTableClear;  /* force new pixel table now */
  for (i=0; i<width; i++)
    for (j=0; j<height; j++) {
      converted = False;
      cpixel = XGetPixel(scratch_img, i, j);
      if (mask_img) {
        mpixel = XGetPixel(mask_img, i, j);
        if (!mpixel) {
          cpixel = Transparent;
          XPutPixel(scratch_img, i, j, cpixel);
         }
       } /* if(mask_img) */
      for (k = 0; k < NUM_STATICS; k++)
        if (cpixel == StaticPen[k]) {
          XPutPixel(mono_img, i, j, StaticMono[k]);
          converted = True;
         } /* if(cpixel...) */
      if (!converted) {
        for (k = 0; k < NUM_DYNAMICS; k++)
          if (cpixel == DynamicPen[k]) {
            XPutPixel(mono_img, i, j, DynamicMono[k]);
            converted = True;
           } /* if(cpixel...) */
       } /* if(!converted) */
      if (!converted) {
        pixelTableIndex = PixelTableLookup (cpixel, False);
        XPutPixel(mono_img, i, j, PIXEL_TABLE_MONO(pixelTableIndex));
       } /* if(!converted) */
     } /* for(j...) */
#ifdef DEBUG
  if (reset_debug) {
    debug = True;
    reset_debug = False;
   }
#endif

  XPutImage(dpy, color_icon, Color_gc, scratch_img,
                0, 0, 0, 0, width, height);
  XPutImage(dpy, mono_icon, Mono_gc, mono_img,
                0, 0, 0, 0, width, height);

/*** this following stuff all gets done regardless ***/

  XCopyArea(dpy, color_icon, XtWindow(iconImage),
                Color_gc, 0, 0, width, height, 0, 0);
  XCopyArea(dpy, mono_icon, XtWindow(monoImage),
                Mono_gc, 0, 0, width, height, 0, 0);
  icon_width = width;
  icon_height = height;
  fileFormat = FORMAT_XPM;
  XmToggleButtonGadgetSetState(formatMenu_xpm_tb, True, True);
/* This line is not realy needed since an Exposed event will be generated */
  Repaint_Exposed_Tablet();
  XDestroyImage(scratch_img);
  XFreePixmap(dpy, pix_ret);
  if (shape_ret)
    XFreePixmap(dpy, shape_ret);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Display_XPMFile\n");
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   Display_XBMFile                                              *
 *                                                                         *
 * Purpose:   Assuming that we have successfully read in an X bitmap file  *
 *            (and, potentially, a mask file as well), use the data from   *
 *            the file(s) to render the proper rasters to the color_icon   *
 *            and mono_icon (and their corresponding widgets).             *
 *                                                                         *
 *            Rendering the bitmap is straight-forward (create a scratch   *
 *            pixmap of the correct depth and copy-plane the just-read     *
 *            bitmap onto it).  If there is a mask bitmap as well, then    *
 *            the scratch pixmap must be modified such that for every      *
 *            0 bit in the mask bitmap, the corresponding pixel of the     *
 *            scratch pixmap should be set to the Transparent pixel.       *
 *            Once this is done, the scratch pixmap is copied to both      *
 *            the color_icon and mono_icon (since, for bitmaps, they're    *
 *            identical).  Then free the scratch pixmap.                   *
 *                                                                         *
 ***************************************************************************/

void
Display_XBMFile(
        int width,
        int height )
{
  int i, j;
  XImage *test_img, *scratch_img;
  Pixmap scratch_pix;

#ifdef DEBUG
  if (debug)
    stat_out("Entering Display_XBMFile\n");
#endif

  scratch_pix = XCreatePixmap(dpy, root, width, height,
                                DefaultDepth(dpy, screen));
  Init_Icons(width, height, DO_NOT_SAVE);
  XSetBackground(dpy, scratch_gc, white_pixel);
  XFillRectangle(dpy, scratch_pix, scratch_gc, 0, 0, width, height);
  XSetForeground(dpy, scratch_gc, black_pixel);
  XCopyPlane(dpy, pix_ret, scratch_pix, scratch_gc, 0, 0,
                width, height, 0, 0, 1);
  if (mask_ret) {
    test_img = XGetImage(dpy, mask_ret, 0, 0, width, height,
        AllPlanes, format);
    scratch_img = XGetImage(dpy, scratch_pix, 0, 0, width, height,
        AllPlanes, format);
    for (i=0; i<width; i++)
      for (j=0; j<height; j++)
        if (!XGetPixel(test_img, i, j))
          XPutPixel(scratch_img, i, j, Transparent);
    XPutImage(dpy, scratch_pix, scratch_gc, scratch_img,
                0, 0, 0, 0, width, height);
   } /* if(mask_ret) */
  XCopyArea(dpy, scratch_pix, color_icon, Color_gc, 0, 0,
                width, height, 0, 0);
  XCopyArea(dpy, scratch_pix, mono_icon, Mono_gc, 0, 0,
                width, height, 0, 0);
  XCopyArea(dpy, color_icon, XtWindow(iconImage),
                Color_gc, 0, 0, width, height, 0, 0);
  XCopyArea(dpy, mono_icon, XtWindow(monoImage),
                Mono_gc, 0, 0, width, height, 0, 0);
  icon_width = width;
  icon_height = height;
  fileFormat = FORMAT_XBM;
  XmToggleButtonGadgetSetState(formatMenu_xbm_tb, True, True);
/* This line is not realy needed since an Exposed event will be generated */
  Repaint_Exposed_Tablet();
  if (mask_ret) {
    XDestroyImage(test_img);
    XDestroyImage(scratch_img);
   }
  XFreePixmap(dpy, scratch_pix);
  XFreePixmap(dpy, pix_ret);
  if (mask_ret)
    XFreePixmap(dpy, mask_ret);

#ifdef DEBUG
  if (debug)
    stat_out("Leaving Display_XBMFile\n");
#endif
}


/***************************************************************************
 *                                                                         *
 * Routine:   SetFileIODialogInfo                                          *
 *                                                                         *
 * Purpose:   Set FileIODialog information... title, path, etc.            *
 *                                                                         *
 ***************************************************************************/
void
SetFileIODialogInfo( void )
{
  static int currentTitle = SAVE_AS;  /* initial title is for Save_As... */
  static XmString saveTitle = NULL;
  static XmString openTitle = NULL;
  static XmString saveLabel = NULL;
  static XmString openLabel = NULL;
  static XmString OpenOKLabel = NULL;
  static XmString SaveOKLabel = NULL;

  static char *untitledStr = NULL;
  static Widget textWidget = NULL;
  static char newName[MAX_FNAME];
  static char dirStr[MAX_FNAME];
  static char tmpStr[MAX_FNAME];

  Arg args[10];
  int  n,dirlen=0,filelen=0,tst, ln;
  char *strOrig = NULL;
  XmString  tmpXmStr;
  char tmp[MAX_FNAME];
  char *tmp1= NULL;
  char *tmp2= NULL;
  int c;
  int startSelect, endSelect;

  if (DialogFlag == OPEN) /* Dialog is for File-Open menu item */
  {
    /* set title to "Icon Editor - Open File" if needed */
      if (!openTitle)
        openTitle = GETXMSTR(2,8,  "Icon Editor - Open File");
      n = 0;
      XtSetArg (args[n], XmNdialogTitle, openTitle);         n++;
      XtSetValues (fileIODialog, args, n);
      currentTitle = OPEN;

      /* set selection label to "Open File" */
      if (!openLabel)
        openLabel = GETXMSTR(2,17,  "Enter file name:");
      n = 0;
      XtSetArg (args[n], XmNselectionLabelString, openLabel);         n++;
      OpenOKLabel = GETXMSTR(2,22,  "Open");
      XtSetArg (args[n], XmNokLabelString, OpenOKLabel);              n++;
      XtSetValues (fileIODialog, args, n);
  }
  else  /* Dialog is for File-Save or File-Save_As menu item */
  {
    /* set title to "Icon Editor - Save As" if needed */
      if (!saveTitle)
        saveTitle = GETXMSTR(2,6,  "Icon Editor - Save As");
      n = 0;
      XtSetArg (args[n], XmNdialogTitle, saveTitle);         n++;
      XtSetValues (fileIODialog, args, n);
      currentTitle = SAVE_AS;

      /* set selection label to "Enter file name:" */
      if (!saveLabel)
        saveLabel = GETXMSTR(2,16,  "Enter file name:");
      n = 0;
      XtSetArg (args[n], XmNselectionLabelString, saveLabel);         n++;
      SaveOKLabel = GETXMSTR(2,24,  "Save");
      XtSetArg (args[n], XmNokLabelString, SaveOKLabel);              n++;
      XtSetValues (fileIODialog, args, n);
  }
    /*                                                  */
    /* set string to "UNTITLED" with appropriate suffix */
    /* when apropriate                                  */

    if (!untitledStr)
      untitledStr = GETSTR(2,20, "UNTITLED");
    if (!textWidget)
      textWidget = XmFileSelectionBoxGetChild(fileIODialog, XmDIALOG_TEXT);
    strOrig = XmTextFieldGetString(textWidget);

    dirStr[0] = '\0';
    newName[0]='\0';
    tmpStr[0]='\0';
    startSelect = 0;

    /* prepare to test */
    tmp1 = strrchr(strOrig, '.');

    strcat(tmpStr, untitledStr);
    strcat(tmpStr, ".m.pm");
    tst=strcmp(last_fname, tmpStr);

    if ( tst==0 ) {/* untitled */
       if ( tmp1 ) { /* previous string exists */
          if (currentTitle != SAVE_AS) strcat(newName, strOrig);
          else {
               strcat(newName, untitledStr);
               strcat(newName, ".m.pm");}
	       /* Update the FSB */
	       XmFileSelectionDoSearch(fileIODialog,(XmString)NULL);

       }
       else { /* First time arownd */
          if (strOrig && strOrig[0]!='\0')
             strcat(newName, strOrig);
          else {
             strcat(newName, untitledStr);
             strcat(newName, ".m.pm");
          }

    XSync(dpy, 0);
          /* rebuild last file name */
          last_fname[0] = '\0';
          strcpy(last_fname, dirStr);
          strcat(last_fname, newName);
       }
    }
    else { /* not untitled */
         tmp1= strrchr(last_fname, '/');

	 /*
	  * Check if any '/' characters found
	  */
	 if (tmp1)
	 {
	     /*
	      * Strip path into directory name and file name
	      */
             c = tmp1[1];
             tmp2 = strchr(tmp1, c);
             strcpy(newName, tmp2);

             /* make and insert the directory name */
             ln = strlen(last_fname) - strlen(tmp1);
             strncpy(dirStr, last_fname, ln);
             dirStr[ln] = '\0';
	 }
	 else
	 {
	     /*
	      * Path is a simple filename
	      * Set filename to be path name
	      * Set directory name = "."
	      */
             strcpy(newName, last_fname);
             dirStr[0] = '.';
             dirStr[1] = '\0';
	 }

         tmpXmStr = XmStringCreateLocalized (dirStr);
         n = 0;
         XtSetArg (args[n], XmNdirectory, tmpXmStr);      n++;
         XtSetValues (fileIODialog, args, n);
         XmStringFree(tmpXmStr);
    }
     ln=0;
     /* set the Highlighted string */
     newName[strlen(newName)+1] ='\0';
     tmp1 = strchr(newName, '.');
     if (tmp1 && tmp1[0]!='\0') ln = (int)strlen(tmp1);
     endSelect = strlen(newName)- ln;

    /* now set the text field and set selection for highlighted portion */
    XmTextFieldSetString(textWidget, newName);
    XmTextFieldSetSelection(textWidget, startSelect, endSelect, CurrentTime);

    XSync(dpy, 0);
    XmProcessTraversal(textWidget, XmTRAVERSE_CURRENT);
    XSync(dpy, 0);

    XtFree(strOrig);

  /* if file lists are being used, re-scan the directory contents */
 /* if (xrdb.useFileLists)
    XmFileSelectionDoSearch(fileIODialog, NULL);*/
}



#ifdef DEBUG
/***************************************************************************
 *                                                                         *
 * Routine:   Dump_AttribStruct  #ifdef DEBUG only (for bba)               *
 *                                                                         *
 * Purpose:   Print out the entier contents of the XpmAttributes struct.   *
 *                                                                         *
 ***************************************************************************/
int
Dump_AttribStruct(
        XpmAttributes *xpma )
{
  int i, j;

  if (xpma->valuemask) {
    stat_out("  valuemask: (");
    if (xpma->valuemask & XpmVisual)
        stat_out("XpmVisual|");
    if (xpma->valuemask & XpmColormap)
        stat_out("XpmColormap|");
    if (xpma->valuemask & XpmDepth)
        stat_out("XpmDepth|");
    if (xpma->valuemask & XpmSize)
        stat_out("XpmSize|");
    if (xpma->valuemask & XpmHotspot)
        stat_out("XpmHotspot|");
    if (xpma->valuemask & XpmCharsPerPixel)
        stat_out("XpmCharsPerPixel|");
    if (xpma->valuemask & XpmColorSymbols)
        stat_out("XpmColorSymbols|");
    if (xpma->valuemask & XpmRgbFilename)
        stat_out("XpmRgbFilename|");
    if (xpma->valuemask & XpmInfos)
        stat_out("XpmInfos");
    stat_out(")\n");
   }
  else
    stat_out("  valuemask: NULL\n");

  stat_out("  Colormap: %d\n", xpma->colormap);
  stat_out("  depth: %d\n", xpma->depth);
  stat_out("  width: %d\n", xpma->width);
  stat_out("  height: %d\n", xpma->height);
  stat_out("  x_hotspot: %d\n", xpma->x_hotspot);
  stat_out("  y_hotspot: %d\n", xpma->y_hotspot);
  stat_out("  cpp: %d\n", xpma->cpp);
  stat_out("  npixels: %d\n", xpma->npixels);
  if (xpma->npixels) {
    stat_out("  Pixels: ");
    for (i=0; i<xpma->npixels; i++) {
      stat_out("%d, ", xpma->pixels[i]);
      if (i%10 == 0)
        stat_out("\n");
     }
    stat_out("\n");
   }
  stat_out("  numsymbols: %d\n", xpma->numsymbols);
  if (xpma->numsymbols) {
    for (i=0; i<xpma->numsymbols; i++)
      stat_out("    %18s | %18s | %d\n", xpma->colorsymbols[i].name,
                 xpma->colorsymbols[i].value, xpma->colorsymbols[i].pixel);
   }
  stat_out("  rgb_fname: \"%s\"\n",((xpma->rgb_fname)?(xpma->rgb_fname):"(nil)"));
  stat_out("  ---------------------------------\n");
  stat_out("  ncolors: %d\n", xpma->ncolors);
  if (xpma->ncolors) {
    for (i=0; i<xpma->ncolors; i++)
      if (xpma->colorTable[i][0] != NULL)
        stat_out("    %3d \"%1s\" \"%18s\" \"%5s\" \"%5s\" \"%5s\" \"%5s\"\n",
                i+1,
                ((xpma->colorTable[i][0])?(xpma->colorTable[i][0]):"(nil)"), 
		((xpma->colorTable[i][1])?(xpma->colorTable[i][1]):"(nil)"),
                ((xpma->colorTable[i][2])?(xpma->colorTable[i][2]):"(nil)"), 
		((xpma->colorTable[i][3])?(xpma->colorTable[i][3]):"(nil)"),
                ((xpma->colorTable[i][4])?(xpma->colorTable[i][4]):"(nil)"), 
		((xpma->colorTable[i][5])?(xpma->colorTable[i][5]):"(nil)")
	);
   }
  stat_out("  hints_cmt: \"%s\"\n",((xpma->hints_cmt)?(xpma->hints_cmt):"(nil)"));
  stat_out("  colors_cmt: \"%s\"\n",((xpma->colors_cmt)?(xpma->colors_cmt):"(nil)"));
  stat_out("  pixels_cmt: \"%s\"\n",((xpma->pixels_cmt)?(xpma->pixels_cmt):"(nil)"));
  stat_out("  mask_pixel: 0x%x\n",xpma->mask_pixel);
}
#endif
