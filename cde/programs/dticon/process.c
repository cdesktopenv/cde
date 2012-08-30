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
/* $TOG: process.c /main/7 1999/08/16 11:03:31 mgreess $ */
/********************************************************************
*  (c) Copyright 1993, 1994 Hewlett-Packard Company
*  (c) Copyright 1993, 1994 International Business Machines Corp.
*  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
*  (c) Copyright 1993, 1994 Unix System Labs, Inc., a subsidiary of
*      Novell, Inc.
**********************************************************************/
/******************************************************************************
 **  Program:           dticon.c
 **
 **  Description:       X11-based multi-color icon editor
 **
 **  File:              process.c, which contains the following subroutines or
 **                     functions:
 **                       Process_New()
 **                       Process_Open()
 **                       Process_Save()
 **                       Process_SaveAs()
 **                       Process_Quit()
 **                       Process_Query_OK()
 **                       Process_Query_Cancel()
 **                       Process_Size_OK()
 **                       Eval_NewSize()
 **                       Process_Size_Cancel()
 **                       Process_StdErr_OK()
 **                       Process_Undo()
 **                       Process_Cut()
 **                       Process_Copy()
 **                       Process_Paste()
 **                       Process_Scale()
 **                       Process_Resize()
 **                       Process_AddHotspot()
 **                       Process_DeleteHotspot()
 **                       Process_Clear()
 **                       Process_RotateLeft()
 **                       Process_RotateRight()
 **                       Process_FlipV()
 **                       Process_FlipH()
 **                       Process_GridState()
 **                       Process_DropCheckOp()
 **                       Do_DropCheckOp()
 **                       Process_DropOp()
 **                       Do_Paste()
 **                       Do_DropOp()
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
 *****************************************************************************
*/
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/TextF.h>
#include "externals.h"
#include <string.h>

#include <Xm/DragC.h>
#include <Dt/Dnd.h>

#include <Dt/HelpDialog.h>
#include "main.h"

#ifdef __TOOLTALK
#include <Tt/tttk.h>
extern void ReplyToMessage( );
extern Tt_message replyMsg;
#endif

static void Do_DropCheckOp(DtDndTransferCallback);
static void Do_DropOp(void);

extern Widget optionsMenu_grid;
Widget editMenu_paste_pb;
Widget editMenu_cut_pb;
Widget editMenu_copy_pb;
Widget editMenu_rotate_pb;
Widget editMenu_flip_pb;
Widget editMenu_scale_pb;
Widget editMenu_undo_pb;
extern Widget newWidthText, newHeightText;

extern GC scratch_gc;
extern char dummy[];

Boolean Write_File( char * );
Boolean Read_File( char * );

extern void Process_SaveAs(void);
extern void Eval_NewSize(int, int);
extern void Process_Copy(XImage **, XImage **);
extern void Process_Resize(void);

/*-----------------------------------------------------------*/
/* Insert application global declarations here               */
/*-----------------------------------------------------------*/
static char     undo_file[MAX_FNAME];          /* save the file name after new */
char dropFileName[MAX_FNAME];
int SaveMeNot = TRUE;     /* used to flag a save as for existing file */
int SavedOnce = False;
int  NewFlag = False;             /* use for undo after new */

/***************************************************************************
 *                                                                         *
 * Routine:   Process_New                                                  *
 *                                                                         *
 * Purpose:   Process the selection of the NEW button in the 'File'        *
 *            pulldown menu.  If the 'Dirty' flag is set, changes have     *
 *            been made to the drawing tablet since the last save, which   *
 *            would be lost by quitting.  Notify the user of this and      *
 *            allow them the chance to change their minds.  If they do     *
 *            not, pop-up the NewIconDialog for the user to select new     *
 *            dimensions (if desired) for the new icon.                    *
 *                                                                         *
 ***************************************************************************/
void
Process_New( void )
{
  DialogFlag = NEW;
  if (Dirty)
    DoQueryDialog( GETSTR(16,16, "The current icon has not been saved.\n\nYour changes will be lost.") );
  else {
    Process_Resize();
    DialogFlag = NONE; }
}

/***************************************************************************
 *                                                                         *
 * Routine:   Process_Open                                                 *
 *                                                                         *
 * Purpose:   Process the selection of the OPEN button in the 'File'       *
 *            pulldown menu.  If the 'Dirty' flag is set, changes have     *
 *            been made to the drawing tablet since the last save, which   *
 *            would be lost by quitting.  Notify the user of this and      *
 *            allow them the chance to change their minds.  If they do     *
 *            not, pop-up the FileSelectionDialog for the user to select   *
 *            the new file to be loaded.                                   *
 *                                                                         *
 ***************************************************************************/

void
Process_Open( void )
{
  DialogFlag = OPEN;
  fileIOMode = FILE_READ;
  if (Dirty)
    DoQueryDialog( GETSTR(16,16, "The current icon has not been saved.\nYour changes will be lost.") );
  else
  {
    XtManageChild(fileIODialog);
    SetFileIODialogInfo();
  }
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_Save                                                 *
 *                                                                         *
 * Purpose:   Process the selection of the SAVE button in the 'File'       *
 *            pulldown menu.  This automatically saves the current icon    *
 *            to the same file it was loaded from, or saved to the last    *
 *            time.  If this file was created 'from scratch' and no        *
 *            previous save operation has taken place, selecting this      *
 *            operation behaves the same way that the SAVE_AS operation    *
 *            behaves.                                                     *
 *                                                                         *
 ***************************************************************************/

void
Process_Save( void )
{
  static char *untitledStr = NULL;
  static char newName[MAX_FNAME];
  char *tmp1 = NULL;
  char *tmp2 = NULL;
  int c;

  DialogFlag = SAVE;
  fileIOMode = FILE_WRITE;

  untitledStr = GETSTR(2,20, "UNTITLED");
  tmp1= strrchr(last_fname, '/');
  if (tmp1) {
     c = tmp1[1];
     tmp2 = strchr(tmp1, c);
     strcpy(newName, tmp2);
  }
  if (strncmp(newName, untitledStr, 8) == 0 || last_fname[0] == '\0') Process_SaveAs();
  else
 {
  if (SavedOnce == True)
  {
    if (!Write_File(last_fname))
      DoErrorDialog( GETSTR(16,4, "Unable to write data to file") );
    else
      Dirty = False;
  }
  else
    Process_SaveAs();
 }
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_SaveAs                                               *
 *                                                                         *
 * Purpose:   Process the selection of the SAVE_AS button in the 'File'    *
 *            pulldown menu.  This pops-up the FileSelectionDialog which   *
 *            prompts the user to select the file in which to save the     *
 *            current icon.                                                *
 *                                                                         *
 ***************************************************************************/

void
Process_SaveAs( void )
{
  DialogFlag = SAVE_AS;
  fileIOMode = FILE_WRITE;
  XtManageChild(fileIODialog);
  SetFileIODialogInfo();
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_Quit                                                 *
 *                                                                         *
 * Purpose:   Process the selection of the QUIT button in the 'File'       *
 *            pulldown menu.  If the 'Dirty' flag is set, changes have     *
 *            been made to the drawing tablet since the last save, which   *
 *            would be lost by quitting.  Notify the user of this and      *
 *            allow them the chance to change their minds.                 *
 *                                                                         *
 ***************************************************************************/

void
Process_Quit( void )
{
  extern int ttMark;
  extern int tt_tmpfile_fd;

  DialogFlag = QUIT;
  if (Dirty)
    DoQueryDialog( GETSTR(16,16, "The current icon has not been saved.\nYour changes will be lost.") );
  else {
#ifdef __TOOLTALK
  edit_notifier(NULL, 0, 1);
  ttdt_session_quit( 0, 0, 1 );
  ttdt_close( 0, 0, 1 );
  tt_release( ttMark );
  if (tt_tmpfile_fd != -1) {
     unlink(last_fname);
     if (fileFormat != FORMAT_XPM ) {
        unlink(dummy);
     } /* if */
  } /* if */
#endif
exit(0);
} /* else */
}

/*****************************************************************************/

void
Process_Query_OK( void )
{
  extern int ttMark;
  extern int tt_tmpfile_fd;

  switch (DialogFlag) {
    case QUIT :
#ifdef __TOOLTALK
  edit_notifier(NULL, 0, 1);
  ttdt_session_quit( 0, 0, 1 );
  ttdt_close( 0, 0, 1 );
  tt_release( ttMark );
  if (tt_tmpfile_fd != -1) {
     unlink(last_fname);
     if (fileFormat != FORMAT_XPM ) {
        unlink(dummy);}
  }
#endif
                  exit(0);
    break;
    case NEW  : Process_Resize();
                DialogFlag= NONE;
    break;
    case OPEN :   XtManageChild(fileIODialog);
                  SetFileIODialogInfo();
    break;
    case SAVE_AS : SaveMeNot = False;
                   Do_FileIO(NULL, NULL, NULL);
    break;
    case GRAB :   Do_GrabOp();
    break;
    case DROP :   Do_DropOp();
    break;
   } /* switch */
   XSync(dpy, 0);
}

void
Process_Query_Cancel( void )
{
  DialogFlag = NONE;
}

void
Process_Size_OK( void )
{
  char *widthStr, *heightStr;
  int new_width, new_height;

  XtUnmanageChild(newIconDialog);
  widthStr = XmTextFieldGetString(newWidthText);
  heightStr = XmTextFieldGetString(newHeightText);
  new_width = atoi(widthStr);
  new_height = atoi(heightStr);
  Eval_NewSize(new_width, new_height);
  DialogFlag = NONE;
}

void
Eval_NewSize(
        int width,
        int height )
{
  char old_width[10], old_height[10];
  int flag;

  if ((width < 1) || (width > xrdb.maxIconWidth) ||
      (height < 1) || (height > xrdb.maxIconHeight)) {
    sprintf(old_width, "%d", icon_width);
    XmTextFieldSetString(newWidthText, old_width);
    sprintf(old_height, "%d", icon_height);
    XmTextFieldSetString(newHeightText, old_height);
    DoErrorDialog(GETSTR(16,10,"Invalid width and/or\nheight specified"));
   }
  else {

    if (DialogFlag == NEW)
      flag = DO_NOT_SAVE;
    else
      flag = DO_SAVE;

    Backup_Icons();    /* for undo */
    Init_Icons(width, height, flag);

   }
}

void
Process_Size_Cancel( void )
{
  DialogFlag = NONE;
  XtUnmanageChild(newIconDialog);
}

void
Process_StdErr_OK( void )
{
  DialogFlag = NONE;
  XtUnmanageChild(stdErrDialog);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_Undo                                                 *
 *                                                                         *
 * Purpose:   Process the selection of the 'Undo' button in the 'Edit'     *
 *            pulldown menu.  If no previous icon state is available,      *
 *            notify the user via a pop-up error dialog.  Otherwise,       *
 *            reverse the Backup_Icon() function behavior, update the      *
 *            tablet to reflect the change, and set the UndoFlag to        *
 *            False (indicating that no further undo ops. are possible).   *
 *                                                                         *
 ***************************************************************************/

void
Process_Undo( void )
{
  if (UndoFlag) {
    if ((icon_width != backup_width) || (icon_height != backup_height))
      Init_Icons(backup_width, backup_height, DO_NOT_SAVE);
    XCopyArea(dpy, prev_color_icon, color_icon,
 Color_gc, 0, 0, icon_width, icon_height, 0, 0);
    XCopyArea(dpy, prev_mono_icon, mono_icon,
 Mono_gc, 0, 0, icon_width, icon_height, 0, 0);
    if (XtWindow(iconImage))
      XCopyArea(dpy, color_icon, XtWindow(iconImage), Color_gc,
 0, 0, icon_width, icon_height, 0, 0);
    if (XtWindow(monoImage))
      XCopyArea(dpy, mono_icon, XtWindow(monoImage), Mono_gc,
 0, 0, icon_width, icon_height, 0, 0);
    Repaint_Exposed_Tablet();
    UndoFlag = False;
    Dirty = True;
    XtSetSensitive( editMenu_undo_pb, False);

  if ( NewFlag == TRUE ){
    NewFlag = False;
    strcpy(last_fname, undo_file);
    last_fname[strlen(last_fname)] = '\0';
    ChangeTitle();}
   }
  else
    DoErrorDialog( GETSTR(16,8,"There is no previous\nimage available") );
}

void
Process_Cut( void )
{
  Process_Copy(&CutCopy, &CutCopy_mono);
  if (Selected) {
    Backup_Icons();
    XSetForeground(dpy, scratch_gc, Transparent);
    XFillRectangle(dpy, color_icon, scratch_gc, select_box.x, select_box.y,
 select_box.width, select_box.height);
    XFillRectangle(dpy, mono_icon, scratch_gc, select_box.x, select_box.y,
 select_box.width, select_box.height);
    XFillRectangle(dpy, XtWindow(iconImage), scratch_gc,
 select_box.x, select_box.y, select_box.width, select_box.height);
    XFillRectangle(dpy, XtWindow(monoImage), scratch_gc,
 select_box.x, select_box.y, select_box.width, select_box.height);
    Transfer_Back_Image(select_box.x, select_box.y,
   (select_box.x+select_box.width),
   (select_box.y+select_box.height), FILL);
 XtSetSensitive( editMenu_paste_pb, True);
   }
}

void
Process_Copy(
 XImage **img,
 XImage **img_mono )
{
  if (Selected) {
    *img = XGetImage(dpy, color_icon, select_box.x, select_box.y,
 select_box.width, select_box.height, AllPlanes, format);
    *img_mono = XGetImage(dpy, mono_icon, select_box.x, select_box.y,
 select_box.width, select_box.height, AllPlanes, format);
 XtSetSensitive( editMenu_paste_pb, True);
   }
  else
    DoErrorDialog( GETSTR(16,12, "No area has been selected") );
}

void
Process_Paste( void )
{
  if (CutCopy) {
    Backup_Icons();
    Backup_G_Op = GraphicsOp;
    GraphicsOp  = S_PASTE;
    FirstRigid = True;
    /* grayout unusable stuff */
    XtSetSensitive( editMenu_cut_pb, False);
    XtSetSensitive( editMenu_copy_pb, False);
    XtSetSensitive(editMenu_rotate_pb, False);
    XtSetSensitive(editMenu_flip_pb,  False);
    XtSetSensitive(editMenu_scale_pb,  False);
    XSync(dpy, 0);
   }
  else
    DoErrorDialog(GETSTR(16,14, "No area was previously\ncut or copied"));
}

void
Process_Scale( void )
{
  if (Selected) {
    Backup_Icons();
    Backup_G_Op = GraphicsOp;
    GraphicsOp  = S_SCALE_1;
    FirstRigid = True;
   }
  else
    DoErrorDialog( GETSTR(16,12, "No area has been selected") );
}

void
Process_Resize( void )
{
  int flag;
  char old_width[10], old_height[10];
  static char *untitledStr = NULL;

  if ( DialogFlag == NEW ) {
    strcpy(undo_file, last_fname);
    last_fname[0] = '\0';
    SavedOnce = False;
    Backup_Icons();    /* for undo */
    flag = DO_NOT_SAVE;
    Init_Icons(icon_width, icon_height, flag);
    if (!untitledStr)
      untitledStr = GETSTR(2,20, "UNTITLED");
    strcpy(last_fname, untitledStr);
    strcat(last_fname, ".m.pm");
    last_fname[strlen(last_fname)] = '\0';
    ChangeTitle();
    Repaint_Exposed_Tablet();
    Dirty = False;
    NewFlag = TRUE;
    }
  else{
    sprintf(old_width, "%d", icon_width);
    XmTextFieldSetString(newWidthText, old_width);
    sprintf(old_height, "%d", icon_height);
    XmTextFieldSetString(newHeightText, old_height);
    XtManageChild(newIconDialog);
    XmProcessTraversal(newWidthText, XmTRAVERSE_CURRENT);
    XmTextFieldSetSelection(newWidthText, 0, 3, CurrentTime); }
}

/***************************************************************************
 *                                                                         *
 * Routine:   Process_Clear                                                *
 *                                                                         *
 * Purpose:   Process the selection of the 'Clear' button in the 'Edit'    *
 *            pulldown menu.  This should be undo'able, just like a normal *
 *            graphics ops., so back up the icons before wiping out their  *
 *            contents.  After painting them to transparent, copy the      *
 *            contents of the color icon back onto the tablet.             *
 *                                                                         *
 ***************************************************************************/

void
Process_Clear( void )
{
  Backup_Icons();
  XSetForeground(dpy, scratch_gc, Transparent);
  XFillRectangle(dpy, color_icon, scratch_gc, 0, 0, icon_width, icon_height);
  XFillRectangle(dpy, mono_icon, scratch_gc, 0, 0, icon_width, icon_height);
  if (XtWindow(iconImage))
    XCopyArea(dpy, color_icon, XtWindow(iconImage), scratch_gc,
 0, 0, icon_width, icon_height, 0, 0);
  if (XtWindow(monoImage))
    XCopyArea(dpy, mono_icon, XtWindow(monoImage), scratch_gc,
 0, 0, icon_width, icon_height, 0, 0);
  Repaint_Exposed_Tablet();
  Dirty = False;
}
/***************************************************************************
 *                                                                         *
 * Routine:   Process_GrabImage                                            *
 *                                                                         *
 * Purpose:   You're a computer programmer.  *YOU* figure out what it does.*
 *                                                                         *
 ***************************************************************************/

void
Process_GrabImage( void )
{
  DialogFlag = GRAB;
  if (Dirty)
    DoQueryDialog( GETSTR(16,16, "The current icon has not been saved.\nYour changes will be lost.") );
  else
    Do_GrabOp();
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_AddHotspot                                           *
 *                                                                         *
 * Purpose:   Process the selection of the 'Add Hotspot' button in the     *
 *            'Edit' pulldown menu.  Save the current graphics ops. flag   *
 *            and set the flag to S_HOTSPOT.  The actual [x,y] location    *
 *            selection for the hotspot will occur in the event processing *
 *            loop for the tablet.                                         *
 *                                                                         *
 ***************************************************************************/

void
Process_AddHotspot( void )
{
  Backup_G_Op = GraphicsOp;
  GraphicsOp  = S_HOTSPOT;
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_DeleteHotspot                                        *
 *                                                                         *
 * Purpose:   Process the selection of the 'Delete Hotspot' button in the  *
 *            'Edit' pulldown menu.  Set the X and Y hot values to -1, set *
 *            the hotSpot flag to FALSE, and repaint the exposed portion   *
 *            of the tablet (to remove the visible hotspot indicator).     *
 *                                                                         *
 ***************************************************************************/

void
Process_DeleteHotspot( void )
{
  X_Hot = -1;
  Y_Hot = -1;
  hotSpot = False;
  Repaint_Exposed_Tablet();
}

void
Process_RotateLeft( void )
{
  XImage *color_img, *mono_img;

  Process_Copy(&color_img, &mono_img);
  /* Turn off Paste since no area is available to Paste */
  XtSetSensitive( editMenu_paste_pb, False);
  if (Selected) {
    Backup_Icons();
    Rotate = XGetImage(dpy, root, 0, 0, color_img->height, color_img->width,
    AllPlanes, format);
    Rotate_mono = XGetImage(dpy, root, 0, 0, mono_img->height, mono_img->width,
    AllPlanes, format);
    Block_Rotate(color_img, Rotate, ROTATE_L);
    Block_Rotate(mono_img, Rotate_mono, ROTATE_L);
    XDestroyImage(color_img);
    XDestroyImage(mono_img);
    Backup_G_Op = GraphicsOp;
    GraphicsOp  = S_ROTATE;
    FirstRigid = True;
   }
}

void
Process_RotateRight( void )
{
  XImage *color_img, *mono_img;

  Process_Copy(&color_img, &mono_img);
  /* Turn off Paste since no area is available to Paste */
  XtSetSensitive( editMenu_paste_pb, False);
  if (Selected) {
    Backup_Icons();
    Rotate = XGetImage(dpy, root, 0, 0, color_img->height, color_img->width,
    AllPlanes, format);
    Rotate_mono = XGetImage(dpy, root, 0, 0, mono_img->height, mono_img->width,
    AllPlanes, format);
    Block_Rotate(color_img, Rotate, ROTATE_R);
    Block_Rotate(mono_img, Rotate_mono, ROTATE_R);
    XDestroyImage(color_img);
    XDestroyImage(mono_img);
    Backup_G_Op = GraphicsOp;
    GraphicsOp  = S_ROTATE;
    FirstRigid = True;
   }
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_FlipV                                                *
 *                                                                         *
 * Purpose:   Process the selection of the 'Vertical' button in the 'Edit' *
 *            ->'Flip Area' pulldown menu.  This should be undo'able,      *
 *            just like a normal graphics ops., so back up the icons       *
 *            first, then call Mirror_Image(), with the flag VERTICAL.     *
 *            If no area is SELECTED, map the error dialog with the        *
 *            appropriate message.                                         *
 *                                                                         *
 ***************************************************************************/

void
Process_FlipV( void )
{
  if (Selected) {
    Backup_Icons();
    Mirror_Image(VERTICAL);
   }
  else
    DoErrorDialog( GETSTR(16,12, "No area has been selected") );
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_FlipH                                                *
 *                                                                         *
 * Purpose:   Process the selection of the 'Horizontal' button in the      *
 *            'Edit'->'Flip Area' pulldown menu.  This should be undo'able,*
 *            just like a normal graphics ops., so back up the icons       *
 *            first, then call Mirror_Image(), with the flag HORIZONTAL.   *
 *            If no area is SELECTED, map the error dialog with the        *
 *            appropriate message.                                         *
 *                                                                         *
 ***************************************************************************/

void
Process_FlipH( void )
{
  if (Selected) {
    Backup_Icons();
    Mirror_Image(HORIZONTAL);
   }
  else
    DoErrorDialog( GETSTR(16,12, "No area has been selected") );
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_GridState                                            *
 *                                                                         *
 * Purpose:   Process the selection of the 'Visible Grid' button in the    *
 *            'Options' pulldown menu.  Set the GridEnabled internal flag  *
 *            to reflect the current state of the toggle.  If the value is *
 *            different from the previous value, repaint the tablet.       *
 *                                                                         *
 ***************************************************************************/

void
Process_GridState( void )
{
  Arg args[10];
  int i;
  Boolean new_val;

  i = 0;
  XtSetArg(args[i], XmNset, &new_val); i++;
  XtGetValues(optionsMenu_grid, args, i);
#ifdef DEBUG
  if (debug) {
    stat_out("Toggling tablet grid ");
    switch (new_val) {
      case True   : stat_out("ON\n");
      break;
      case False  : stat_out("OFF\n");
      break;
     }
   }
#endif
  if (new_val != GridEnabled) {
    GridEnabled = new_val;
    Repaint_Exposed_Tablet();
   }
}
/***************************************************************************
 *                                                                         *
 * Routine:   ConvertDropName                                              *
 *                                                                         *
 * Purpose:  Convert the "object" received from bms to a full path name    *
 *           note:  I am making BIG assumptions about the format of the    *
 *                  file I am getting from dtfile. "<host> - <path>"      *
 * WARNING:  I have used an Xe function directly (XeIsLocalHostP), rather  *
 *           than include Dt/Connect.h, which was causing bad things to    *
 *           happen at build time, probably because dticon is not ansi-   *
 *           clean (it tried to get c++ version of /usr/include/stdlib.h?) *
 *           It's simply too late to clean up the ansi... (the bell tolls) *
 *                                                                         *
 ***************************************************************************/
static char *
ConvertDropName( char *objects)
{
    char *host;
    char *path;
    char *fullName;
    char *tmp;
    char *netfile;

    host = objects;
    tmp = strchr(objects,' ');
    if (tmp==NULL)      /* shouldn't happen */
      return (strdup(strchr(objects, '/')));

    /* check if same host */
    tmp[0] = '\0';
    if ((Boolean)XeIsLocalHostP(host))
    {
        tmp[0] = ' ';
        return (strdup(strchr(objects, '/')));
    }

    /* different host... get full path name */
    path = tmp+3;      /* skip past the " - " */

    /* Convert to a valid name on the local host. */
    netfile = tt_host_file_netfile(host, path);
    fullName = tt_netfile_file(netfile);
    tt_free(netfile);

    tmp[0] = ' ';      /* put back the " " after host name */
    return (fullName);
}


/***************************************************************************
 *                                                                         *
 * Routine:   Process_DropCheckOp                                          *
 *                                                                         *
 * Purpose:   Validate the drag-n-drop operation that just occured on the  *
 *            tablet window.                                               *
 *                                                                         *
 ***************************************************************************/

void
Process_DropCheckOp(
 Widget w,
 XtPointer client_data,
 XtPointer call_data)
{
  DtDndTransferCallback transferInfo = (DtDndTransferCallback) call_data;

  /* save name in global array for later (Do_DropOp function) */
  /*
   * REMIND:  Need to address case of multiple file names - here and
   *   elsewhere in the code. This continues with the assumption
   *  that there is only one file name transfered.
   */
  if (transferInfo->dropData->numItems > 0)
  {
    strncpy (dropFileName, transferInfo->dropData->data.files[0],
      MAX_FNAME);
  }
  else
    dropFileName[0] = '\0';

  Do_DropCheckOp(transferInfo);

#ifdef DEBUG
  if (debug) {
    stat_out("      file-name = %s\n", dropFileName);
   }
#endif

}

extern Widget  formatMenu_xpm_tb, formatMenu_xbm_tb;
extern int successFormat, x_hot, y_hot;
extern unsigned int width_ret, height_ret;

/***************************************************************************
 *                                                                         *
 * Routine:   Do_DropCheckOp                                               *
 *                                                                         *
 * Purpose:   Verify the drag-n-drop operation that just occured on the    *
 *            tablet window.                                               *
 *                                                                         *
 ***************************************************************************/

static void
Do_DropCheckOp(
 DtDndTransferCallback transferInfo)
{

#ifdef DEBUG
  if (debug) {
    stat_out("    Doing DROP OPERATION :\n");
    stat_out("    name is = %s\n", dropFileName);
   }
#endif

  if (dropFileName[0] != '\0')
  {
    if (!Read_File(dropFileName))
    {
       DoErrorDialog( GETSTR(16,2,
      "The file cannot be accessed\nor contains invalid data") );
      transferInfo->status = DtDND_FAILURE;
    }
    else
    {
      transferInfo->status = DtDND_SUCCESS;
    } /* else */
  } /* if */
}

void
Process_DropOp(
 Widget w,
        XtPointer client_data,
        XtPointer call_data)
{
      DialogFlag = DROP;

      if (Dirty)
        DoQueryDialog( GETSTR(16,16, "The current icon has not been saved.\n\nYour changes will be lost.") );
      else  {
            Do_DropOp();}
}

static void
Do_DropOp(void)
{
      if (successFormat == FORMAT_XPM)
      {
        X_Hot = xpm_ReadAttribs.x_hotspot;
        Y_Hot = xpm_ReadAttribs.y_hotspot;
        Display_XPMFile(xpm_ReadAttribs.width, xpm_ReadAttribs.height);
      }
      else if (successFormat == FORMAT_XBM)
      {
        X_Hot = x_hot;
        Y_Hot = y_hot;
        Display_XBMFile(width_ret, height_ret);
      }

      Dirty = False;
}

/***************************************************************************
 *                                                                         *
 * Routine:   Do_Paste                                                     *
 *                                                                         *
 * Purpose:   Paste the CutCopy image at the tablet location specified by  *
 *            the [x,y] parameters in the call.                            *
 *                                                                         *
 ***************************************************************************/

void
Do_Paste(
        int x,
        int y )
{
  XImage *color_img, *mono_img;

  if (GraphicsOp == S_PASTE) {
    color_img = CutCopy;
    mono_img  = CutCopy_mono;
   }
  else if (GraphicsOp == S_ROTATE) {
    color_img = Rotate;
    mono_img  = Rotate_mono;
   }
  else {
    color_img = Scale;
    mono_img  = Scale_mono;
   }

  if (GraphicsOp == S_PASTE)
  {
    GraphicsOp = S_WAIT_RELEASE;
  }
  else
  {
    GraphicsOp = Backup_G_Op;
    if (Backup_G_Op == SELECT)
      Start_HotBox(CONTINUE);
    Backup_G_Op = 0;
  }

  XPutImage(dpy, color_icon, Color_gc, color_img, 0, 0, x, y,
  color_img->width, color_img->height);
  XPutImage(dpy, mono_icon, Mono_gc, mono_img, 0, 0, x, y,
  mono_img->width, mono_img->height);
  XCopyArea(dpy, color_icon, XtWindow(iconImage), Color_gc,
  x, y, color_img->width, color_img->height, x, y);
  XCopyArea(dpy, mono_icon, XtWindow(monoImage), Mono_gc,
  x, y, mono_img->width, mono_img->height, x, y);
  Transfer_Back_Image(x, y, x+color_img->width, y+color_img->height, FILL);
}

