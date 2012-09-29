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
/* $XConsortium: icon_selection_dialog.c /main/10 1996/10/15 13:39:13 rswiston $ */
/*******************************************************************************
        icon_selection_dialog.c

       Associated Header file: icon_selection_dialog.h
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(USL) || defined(__uxp__)
#define S_ISLNK(mode) ((mode & S_IFMT) == S_IFLNK)
#endif

#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/XmPrivate.h> /* XmeFlushIconFileCache */
#include <X11/cursorfont.h>

#include <Dt/Icon.h>

#include "dtcreate.h"
#include "UxXt.h"
#include "cmncbs.h"
#include "cmnrtns.h"
#include "cmnutils.h"
#include "fileio.h"
#include "ErrorDialog.h"

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

#include <sys/types.h> /* for directory routines */
#ifdef _KERNEL
#undef _KERNEL
#endif
#include <dirent.h>    /* includes sys/dir.h */

#include <Xm/ScrolledW.h>
#include <Dt/Icon.h>
/*#include "Container.h"*/

static  char *selected_icon_name = (char *)NULL;
static  Widget selected_icon = (Widget)NULL;
static  int icon_count = 0;
char    *main_filter;
char    *file_filter_global;
DtIconGadget **icons_in_container = NULL;


int myscandir(char *, struct dirent ***, int (), int ());
int get_top_b1_position (int buttoncount);
int get_top_b2_position (int buttoncount);
void load_directories_list (char **dirlist);
char *get_initial_filter (void);
void load_filter_text_field (char *filter);
char *update_filter (char *filter);
void apply_filter (char *filter);
char *initialize_filter (char *filter_end);
struct dirent **build_dirent_list (char *filter, int *filecount);
void update_container_contents (char *filter);
void free_container_contents (void);
int new_name_select(struct dirent *, char *);
/*int name_select(struct dirent *);*/
static  void    selectionCB_icon_scrolled_container ( Widget UxWidget,
						  XtPointer UxClientData,
						  XtPointer UxCallbackArg);
static  void    calc_bottom_attachment( Widget UxWidget,
					XtPointer UxClientData,
					XtPointer UxCallbackArg);

/*******************************************************************************
       The following header file defines the context structure.
*******************************************************************************/

#define CONTEXT_MACRO_ACCESS 1
#include "icon_selection_dialog.h"
#undef CONTEXT_MACRO_ACCESS


/*******************************************************************************
Auxiliary code from the Declarations Editor:
*******************************************************************************/

/*********************************************************************/
/* auxfuncs - icon selection dialog auxiliary functions              */
/*********************************************************************/

/***************************************************************************/
/*                                                                         */
/*  myscandir                                                              */
/*                                                                         */
/***************************************************************************/
int myscandir(char *pszDirName, struct dirent ***NameList, int Select(), int Compare())

{
  DIR            *dirptr;
  struct dirent  *dp;
  struct dirent  *dptmp;
  int            count = 0;
  /* int            size = 0; */
  struct dirent  **adirent;
#define          AINCREMENT  ((int)512)
  int            sizeArray = AINCREMENT;
  char           *msgPtr, *fmtPtr, *errPtr;

  if (!(dirptr = opendir(pszDirName))) {
     /* INTERNATIONALIZE */
     msgPtr = "Could not open the following directory:";
     fmtPtr = "%s\n   %s";
     errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			strlen(pszDirName) + 1) * sizeof(char));
     sprintf(errPtr, fmtPtr, msgPtr, pszDirName);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
     return(0);
  }

#if 0
  size = sizeof(void *) * 1024;
  *NameList = (struct dirent **)malloc(sizeof(void *) * 1024);
#endif

  adirent = (struct dirent **)malloc(sizeof(void *) * sizeArray);
  for (dp = readdir(dirptr); dp != NULL; dp = readdir(dirptr)) {

#if 0
     if ((*Select)(dp, pszDirName)) {
        (*NameList)[count] = dp;
     if ((*Select)(dp)) {
#endif

     if ((*Select)(dp, pszDirName)) {
        dptmp = (struct dirent *)malloc(sizeof(struct dirent) + strlen(dp->d_name) + 1);
        memcpy(dptmp, dp, sizeof(struct dirent));
        strcpy(dptmp->d_name, dp->d_name);
        adirent[count] = dptmp;
        count++;
        if (count == (sizeArray - 1)) {
           adirent = (struct dirent **)realloc(adirent, (sizeof(void *) * (sizeArray + AINCREMENT)));
           if (adirent) {
              sizeArray += AINCREMENT;
           }
        }
     }
  }

  *NameList = adirent;
  closedir(dirptr);
  return(count);
}

/***************************************************************************/
/*                                                                         */
/* build_dirent_list - builds a list of icon files to be displayed         */
/*                   in a container                                        */
/* Input: char *filter - directory in which to search for files            */
/* Output: struct dirent **namelist - list of files                        */
/*                                                                         */
/***************************************************************************/
struct dirent **build_dirent_list (char *filter, int *filecount)

{
  char           *dirname, *dirname_end;
  DIR            *dirptr;
  struct dirent  **namelist;
  char           *msgPtr, *fmtPtr, *errPtr;

  /********************************************************************/
  /* initialize filecount and namelist                                */
  /********************************************************************/
  *filecount = 0;
  namelist = NULL;

  /********************************************************************/
  /* get length of file filter and create a copy                      */
  /********************************************************************/
  dirname = (char *)calloc (strlen(filter)+1, sizeof(char));

#ifdef DEBUG
  if (!dirname) {
    printf("error callocing memory for dirname\n");
    printf("dirname of size: %i\n", strlen(dirname));
  }
#endif

  dirname = strcpy (dirname, filter);

  /********************************************************************/
  /* strip wildcard from end of directory name                        */
  /********************************************************************/
  dirname_end = strrchr (dirname, '/');
  dirname_end[1] = '\0';

  /********************************************************************/
  /* build list of file name that match the wildcard string           */
  /********************************************************************/
  dirptr = (DIR *)opendir (dirname);

  if (dirptr) {

     *filecount = myscandir (dirname, &namelist, new_name_select, NULL);

#ifdef DEBUG
     printf("# of icons = %d\n", *filecount);
#endif
     closedir (dirptr);
  } else {
     /* INTERNATIONALIZE */
     msgPtr = "Could not open the following directory:";
     fmtPtr = "%s\n   %s";
     errPtr = XtMalloc((strlen(msgPtr) + strlen(fmtPtr) +
			strlen(dirname) + 1) * sizeof(char));
     sprintf(errPtr, fmtPtr, msgPtr, dirname);
     display_error_message(CreateActionAppShell, errPtr);
     XtFree(errPtr);
     namelist = NULL;
  }

#ifdef DEBUG
  printf ("Freeing dirname, sizeof: %ld\n", (long)sizeof(dirname));
#endif

  free (dirname);
  return (namelist);
}

/***************************************************************************/
/*                                                                         */
/*  new_name_select - determines if file is included in list of icon       */
/*                files to be displayed in a container                     */
/*  Input: struct dirent *dir_entry - directory entry structure            */
/*  Output: (int)found - >0 if file name accepted, 0 otherwise             */
/*                                                                         */
/***************************************************************************/
int new_name_select (struct dirent *dir_entry, char *pszdir_name)
{
  struct stat stat_buffer;
  char        filename[256];

  /***********************************************************************/
  /* This will only filter the icons of the filter size which by default */
  /* is medium icons.                                                    */
  /***********************************************************************/
  if (strstr(dir_entry->d_name, file_filter_global)) {
     sprintf(filename, "%s%s", pszdir_name, dir_entry->d_name);
     if (!stat(filename, &stat_buffer)) {
        if ( (S_ISREG(stat_buffer.st_mode)) ||
             (S_ISLNK(stat_buffer.st_mode)) ) {
           return(1);
        }
     }
  }
  return(0);

#if 0
  /***********************************************************************/
  /* This is the new filter to get all three sizes of icons.             */
  /***********************************************************************/
  if (bShowPixmaps) {
    if ( (strstr(dir_entry->d_name, ".l.pm")) ||
         (strstr(dir_entry->d_name, ".m.pm")) ||
         (strstr(dir_entry->d_name, ".t.pm")) ) {
       sprintf(filename, "%s%s", pszdir_name, dir_entry->d_name);
       if (!stat(filename, &stat_buffer)) {
          if ( (S_ISREG(stat_buffer.st_mode)) ||
               (S_ISLNK(stat_buffer.st_mode)) ) {
             return(1);
          }
       }
    }
  } else {
    if ( (strstr(dir_entry->d_name, ".l.bm")) ||
         (strstr(dir_entry->d_name, ".m.bm")) ||
         (strstr(dir_entry->d_name, ".t.bm")) ) {
       sprintf(filename, "%s%s", pszdir_name, dir_entry->d_name);
       if (!stat(filename, &stat_buffer)) {
          if ( (S_ISREG(stat_buffer.st_mode)) ||
               (S_ISLNK(stat_buffer.st_mode)) ) {
             return(1);
          }
       }
    }
  }
  return(0);
#endif

}

#if 0
/***************************************************************************/
/*                                                                         */
/*  new_name_select - determines if file is included in list of icon       */
/*                files to be displayed in a container                     */
/*  Input: struct dirent *dir_entry - directory entry structure            */
/*  Output: (int)found - >0 if file name accepted, 0 otherwise             */
/*                                                                         */
/***************************************************************************/
int new_name_select (struct dirent *dir_entry, char *pszdir_name)
{
  struct stat stat_buffer;
  char        filename[256];

  if (strstr(dir_entry->d_name, file_filter_global)) {
     sprintf(filename, "%s%s", pszdir_name, dir_entry->d_name);
     if (!stat(filename, &stat_buffer)) {
        if ( (S_ISREG(stat_buffer.st_mode)) ||
             (S_ISLNK(stat_buffer.st_mode)) ) {
           return(1);
        }
     }
  }
  return(0);
}
#endif

/***************************************************************************/
/*                                                                         */
/* int get_top_b1_position - determine position of button                  */
/* INPUT: int buttoncount - number of buttons in row                       */
/* OUTPUT: int position - position of left edge of button                  */
/*                                                                         */
/***************************************************************************/
int get_top_b1_position (int buttoncount)
{
  int position;

  switch (buttoncount) {
          case 1: position = 38; break;
          case 2: position = 14; break;
          case 3: position =  8; break;
          default : position = 0;
          };
  return (position);
}


/***************************************************************************/
/*                                                                         */
/* int get_top_b2_position - determine position of button                  */
/* INPUT: int buttoncount - number of buttons in row                       */
/* OUTPUT: int position - position of left edge of button                  */
/*                                                                         */
/***************************************************************************/
int get_top_b2_position (int buttoncount)
{
  int position;

  switch (buttoncount) {
          case 2: position = 60; break;
          case 3: position = 40; break;
          default : position = 0;
          };
  return (position);
}


/***************************************************************************/
/*                                                                         */
/*  void load_directories_list - load scrolled list with directory         */
/*                               names                                     */
/*  Input: char **dirlist - list of text strings                           */
/*  Output: none                                                           */
/*                                                                         */
/***************************************************************************/
void load_directories_list (char **dirlist)
{
  XmStringTable xmstringlist;
  int stringcount = 0;
  int i;

  /******************************************************************/
  /* get number of directories in list                              */
  /******************************************************************/
  stringcount = countItems (dirlist);

  /******************************************************************/
  /* convert strings to xmstrings                                   */
  /******************************************************************/
  xmstringlist = (XmStringTable) TextStringsToXmStrings (dirlist);

  /******************************************************************/
  /* put xmstrings in list                                          */
  /******************************************************************/
  XtVaSetValues (dir_scrolled_list, XmNitems, xmstringlist,
                 XmNitemCount, stringcount, NULL);

  XmListSelectPos(dir_scrolled_list, 1, False);
  /******************************************************************/
  /* free xmstrings array                                           */
  /******************************************************************/
  for (i=0; i < stringcount; i++) {
     XmStringFree(xmstringlist[i]);
  }
  free(xmstringlist);

  return;
}

/***************************************************************************/
/*                                                                         */
/*  void initialize_filter - build path/wildcard filter                    */
/*  Input: none                                                            */
/*  Output: char *initial_filter                                           */
/*                                                                         */
/***************************************************************************/
char *initialize_filter (char *filter_end)
{
  char *filter, *initial_filter;
  int filter_length, filter_end_length;
  int add_slash = 0;

  /******************************************************************/
  /* initial filter string is first directory in list               */
  /******************************************************************/
  filter = get_initial_filter ();

  /******************************************************************/
  /* determine space needed                                         */
  /******************************************************************/
  filter_length = strlen(filter);
  if ((filter[filter_length-1] == '/') && (filter_end[0] == '/')) {
     filter_end++;
  } else {
     if (!(filter[filter_length-1] == '/') && !(filter_end[0] == '/')) {
         add_slash = 1;
         filter_length++;
     }
  }

  filter_end_length = strlen(filter_end);

  /******************************************************************/
  /* allocate space and put string in initial filter                */
  /******************************************************************/
  initial_filter = (char *)calloc (filter_length + filter_end_length + 1,
                                 sizeof(char));
#ifdef DEBUG
  if (!initial_filter) printf("Error callocing memory in initialize_filter.\n");
#endif
  initial_filter = strcpy (initial_filter, filter);
  if (add_slash) initial_filter[filter_length-1] = '/';
  initial_filter = strcat (initial_filter, filter_end);

  return(initial_filter);
}

/***************************************************************************/
/*                                                                         */
/*  char *get_initial_filter - get value of first file filter              */
/*  Input: none                                                            */
/*  Output: char *new_filter - string containing filter                    */
/*                                                                         */
/***************************************************************************/
char *get_initial_filter (void)
{
  char *new_filter;

  /******************************************************************/
  /* initial filter is first one in directory list                  */
  /******************************************************************/
  new_filter = (char *)directories_list[0];
  return (new_filter);
}

/***************************************************************************/
/*                                                                         */
/*  char *update_filter - put main filter end on new main filter           */
/*  Input: char *filter - new filter from which gets main filter end       */
/*  Output: char *new_filter - new main filter                             */
/*                                                                         */
/***************************************************************************/
char *update_filter (char *filter)
{
  char *filter_end, *new_filter, *main_end;
  char *fe_ptr, *ffg_ptr;

  /******************************************************************/
  /* get new end                                                    */
  /******************************************************************/
  main_end = strrchr (main_filter, '/');
  filter_end = (char *)calloc(strlen(main_end)+1, sizeof(char));

#ifdef DEBUG
  if (!filter_end)
     printf("Error callocing memory for filter_end in update_filter.\n");
#endif

  filter_end = strcpy (filter_end, main_end);

  /******************************************************************/
  /* determine what filter should be depending on icon type (pm/bm) */
  /******************************************************************/
  if (fe_ptr = strrchr(filter_end, EXT_DELIMITER)) {
     ffg_ptr = strrchr(file_filter_global, EXT_DELIMITER);
     if (bShowPixmaps) {
        if ((int)strlen(fe_ptr) >= (int)strlen(PIXMAP_EXT)) {
           strcpy(fe_ptr, PIXMAP_EXT);
        }
        if ( (ffg_ptr) && ((int)strlen(ffg_ptr) >= (int)strlen(PIXMAP_EXT)) ) {
           strcpy(ffg_ptr, PIXMAP_EXT);
        }
     } else {
        if ((int)strlen(fe_ptr) >= (int)strlen(BITMAP_EXT)) {
           strcpy(fe_ptr, BITMAP_EXT);
        }
        if ( (ffg_ptr) && ((int)strlen(ffg_ptr) >= (int)strlen(BITMAP_EXT)) ) {
           strcpy(ffg_ptr, BITMAP_EXT);
        }
     }
  }

  /******************************************************************/
  /* find old end on main filter and delete it                      */
  /******************************************************************/
  main_end = memset (main_end, '\0', strlen(main_end));

  /******************************************************************/
  /* allocate space for new filter                                  */
  /******************************************************************/
  new_filter = (char *)calloc (strlen(filter) +
                             strlen(filter_end) + 1, sizeof(char));
#ifdef DEBUG
  if (!new_filter)
     printf("Error callocing memory for new_filter in update_filter.\n");
#endif

  new_filter = strcpy (new_filter, filter);

  /******************************************************************/
  /* put new end on filter                                          */
  /******************************************************************/
  new_filter = strcat (new_filter, filter_end);

  /******************************************************************/
  /* display new file filter in text field                          */
  /******************************************************************/
  load_filter_text_field (new_filter);

  /******************************************************************/
  /* free resources                                                 */
  /******************************************************************/
  free(filter_end);

  return (new_filter);
}

/***************************************************************************/
/*                                                                         */
/*  void apply_filter - apply filter to create a list of files and         */
/*                      display files in scrolled list                     */
/*  Input: char *filter - file filter to use                               */
/*  Output: none                                                           */
/*                                                                         */
/***************************************************************************/
void apply_filter (char *filter)
{
  char *old_main;

  /******************************************************************/
  /* Get length of main file filter and create a copy               */
  /******************************************************************/
  old_main = (char *)calloc (strlen(main_filter)+1, sizeof(char));

#ifdef DEBUG
  if (!old_main) printf("Calloc error for old_main, in apply filter.\n");
#endif

  old_main = strcpy (old_main, main_filter);

  /******************************************************************/
  /* Update main filter to reflect changes made by user             */
  /******************************************************************/
  main_filter = update_filter (filter);

  /******************************************************************/
  /* if the filter has changed, update container with new icons     */
  /******************************************************************/
  if (strcmp (old_main, main_filter)) {

     /******************************************************************/
     /* Turn on the hour glass                                         */
     /******************************************************************/
     TurnOnHourGlassAllWindows();

     /******************************************************************/
     /* Since we are changing to a new directory, clear out the        */
     /* globals and clear the selected icon text string.               */
     /******************************************************************/
     if (selected_icon) {
        _DtIconSetState(selected_icon, FALSE, FALSE);
        selected_icon = (Widget)NULL;
     }
     if (selected_icon_name) {
        XtFree(selected_icon_name);
        selected_icon_name = (char *)NULL;
     }
     XmTextFieldSetString (icon_name_text_field, "");

     /******************************************************************/
     /* Update the container icon gadgets                              */
     /******************************************************************/
     update_container_contents (main_filter);

     /******************************************************************/
     /* Turn off the hour glass                                        */
     /******************************************************************/
     TurnOffHourGlassAllWindows();
  }

#ifdef DEBUG
  printf ("Freeing old_main, sizeof: %ld\n", (long)sizeof(old_main));
#endif

  free (old_main);
  return;
}


/***************************************************************************/
/*                                                                         */
/*  void load_filter_text_field - display file filter text                 */
/*  Input: char *filter - file filter text string                          */
/*  Output: none                                                           */
/*                                                                         */
/***************************************************************************/
void load_filter_text_field (char *filter)
{
  if (use_filter_field) {
     /* display file filter in text field */
     XmTextFieldSetString (filter_text_field, filter);
     /* set cursor to end of string */
     XmTextSetInsertionPosition (filter_text_field, strlen(filter));
  }
  return;
}


/***************************************************************************/
/*                                                                         */
/*  void update_container_contents - create new icon gadgets which         */
/*                             correspont to the new filter                */
/*  Input: char *filter - file filter text string                          */
/*  Output: none                                                           */
/*                                                                         */
/***************************************************************************/
void  update_container_contents (char *filter)
{
  char          *path, *pathend;
  char          iconfile[MAXFILENAME];
  int           namecount, lcv;
  struct dirent **filelist;
  int           count, old_count;
  int           n;
  Arg           args[10];
  XmString      xmstring;
  char          *ptr;

  XtUnmanageChild (icon_scrolled_container);
  /* free_container_contents (); */

  path = (char *)calloc (strlen(filter)+1, sizeof(char));

#ifdef DEBUG
  if (!path) printf("Calloc error for path, in update_container_contents.\n");
#endif

  path = strcpy (path, filter);
  pathend = strrchr (path, '/');
  pathend[1] = '\0';

  /******************************************************************/
  /* Get number of icon files and list of file names                */
  /******************************************************************/
  filelist = build_dirent_list (filter, &namecount);

  /******************************************************************/
  /* Set up argument list                                           */
  /******************************************************************/
  n = 0;
  /*
  XtSetArg(args[n], XmNimageName, "/usr/dt/appconfig/icons/C/Dtactn.m.pm"); n++;
  */
  XtSetArg(args[n], XmNimageName, NULL); n++;
  XtSetArg(args[n], XmNfillMode, XmFILL_SELF); n++;
  XtSetArg(args[n], XmNbehavior, XmICON_TOGGLE); n++;
  XtSetArg(args[n], XmNrecomputeSize, TRUE); n++;
  XtSetArg(args[n], XmNfillOnArm, TRUE); n++;
  XtSetArg(args[n], XmNtraversalOn, TRUE); n++;

  /******************************************************************/
  /* Need at least one icon for the empty directory icon.           */
  /******************************************************************/
  if (namecount) {
     count = namecount;
  } else {
     count = 1;
  }

  /******************************************************************/
  /* create any additional icon gadgets                             */
  /******************************************************************/
  if (count > icon_count) {
     old_count = icon_count;
     icon_count = count;
     icons_in_container = (DtIconGadget **)realloc
                                      (icons_in_container,
                                      (icon_count+1) * sizeof(DtIconGadget *));
     /*memset(&(icons_in_container[icon_count]), 0, sizeof(DtIconGadget *));*/
     for (lcv = old_count; lcv < icon_count; lcv++) {
        icons_in_container[lcv] =
        /*(DtIconGadget *)XtCreateManagedWidget (filelist[lcv]->d_name,*/
          (DtIconGadget *)XtCreateManagedWidget ("IconGadget",
                            dtIconGadgetClass, (Widget)icon_scrolled_container,
                            args, n);
          XtAddCallback((Widget) icons_in_container[lcv], XmNcallback,
                       (XtCallbackProc) selectionCB_icon_scrolled_container,
                       (XtPointer) NULL);

#ifdef DEBUG
       if (!icons_in_container[lcv])
          printf("Error creating icon gadget, in update_container_contents\n");
#endif

#if 0
       if (!icons_in_container[lcv]) {
          printf ("Could not allocate memory to create new icon\n", lcv);
       }
#endif

#ifdef DEBUG
       } else {
          printf ("Created icon #%i\n", lcv);
       }
#endif

     }  /* end for */
  }     /* end if (count > icon_count) */

  /******************************************************************/
  /* load icon gadgets with information                             */
  /******************************************************************/
  if (namecount) {
    /****************************************************************/
    /* The following flush call is a work-around for a known motif  */
    /* bug.  This is here in the application because motif will not */
    /* correct this bug because doing so would impair performance.  */
    /* Therefore, I am forced to add this call to the code.         */
    /* The problem this fixes is that when new icons are added to   */
    /* the filesystem after the application is already up and       */
    /* running, the icon gadgets to not see them.                   */
    /****************************************************************/
    XmeFlushIconFileCache(NULL);

    for (lcv = 0; lcv < count; lcv++) {
       sprintf (iconfile, "%s%s", path, filelist[lcv]->d_name);
       ptr = strstr(filelist[lcv]->d_name, ".m.");
       if (ptr) {
          *ptr = '\0';
       }
       xmstring = XmStringCreateLocalized(filelist[lcv]->d_name);
       XtVaSetValues ((Widget) icons_in_container[lcv],
                             XmNstring,        xmstring,
                             XmNimageName,     iconfile,
                             NULL);
       XtManageChild((Widget) icons_in_container [lcv]);
       XmStringFree(xmstring);

#ifdef DEBUG
      printf ("Loading icon gadget #%i\n", lcv);
#endif

    }  /* end for */

  /******************************************************************/
  /* This is the empty directory case                               */
  /******************************************************************/
  } else {
       strcpy(iconfile, GETMESSAGE(11, 60, "[Empty]"));
       xmstring = XmStringCreateLocalized(iconfile);
       XtVaSetValues ((Widget) icons_in_container[0],
                             XmNstring,        xmstring,
                             XmNimageName,     (char *)NULL,
                             NULL);
       XtManageChild((Widget) icons_in_container[0]);
       XmStringFree(xmstring);
  }

  /******************************************************************/
  /* unmanage any spare icon gadgets                                */
  /******************************************************************/
  for (lcv = count; lcv < icon_count; lcv++) {
     XtUnmanageChild ((Widget) icons_in_container[lcv]);
  }

  /******************************************************************/
  /* free dirent list                                               */
  /******************************************************************/
  for (lcv = 0; lcv < namecount; lcv++) {
     free (filelist[lcv]);
  }
  if (namecount) {
     free (filelist);
  }

  XtManageChild (icon_scrolled_container);
  return;

}

#ifdef _ICONSELECTOR_DESTROY_ENABLED
/***************************************************************************/
/*                                                                         */
/*  void free_container_contents - free icon gadgets in container          */
/*  Input: none                                                            */
/*  Output: none                                                           */
/*                                                                         */
/***************************************************************************/
void  free_container_contents (void)
{
  int    lcv = 0;
  char   *filename;
  Pixmap icon_pixmap;
  Screen *screen_id;

  if (icons_in_container) {
     screen_id = XtScreen (icons_in_container[0]);
     lcv = 0;
     while (icons_in_container[lcv]) {
#if 0
       XtVaGetValues (icons_in_container[lcv], XmNimageName, &filename,
                      XmNpixmap, &icon_pixmap, NULL);
       XmDestroyPixmap (screen_id, icon_pixmap);

       XtVaSetValues (icons_in_container[lcv], XmNpixmap, NULL, NULL);
#endif
       XtDestroyWidget (icons_in_container[lcv]);
       /*
       icons_in_container[lcv]=NULL;
       */
       lcv++;
     }  /* end while icons_in_container */
     free (icons_in_container);
     icons_in_container = NULL;
  }

#ifdef DEBUG
  printf ("In free_container_contents...\n");
  printf ("Just destroyed all %i icons.\n", lcv);
#endif

  icon_count = 0;

  return;
}
#endif

#if 0
/***************************************************************************/
/*                                                                         */
/*  void load_new_filter - displays selected directory string as           */
/*                         new filter base                                 */
/*  Input: char *new_filter - new directory string to use as filter        */
/*  Output: none                                                           */
/*                                                                         */
/***************************************************************************/
void load_new_filter (char *new_filter)
{
/* no longer need old filter */
free (main_filter);

/* create space for new filter */
main_filter = (char *)calloc (strlen(new_filter)+3, sizeof (char));
#ifdef DEBUG
if (!main_filter) printf("Calloc error for main_filter, in load_new_filter.\n");
#endif
/* copy new filter and add filter end */
main_filter = strcpy (main_filter, new_filter);
main_filter = strcat (main_filter, "/*");
/* display new filter */
load_filter_text_field (main_filter);

return;
}
#endif

/*****************************************************************************/
/*       The following are callback functions.                               */
/*****************************************************************************/

/***************************************************************************/
/*                                                                         */
/*   calc_bottom_attachment                                                */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
static  void    calc_bottom_attachment( Widget UxWidget,
                                             XtPointer UxClientData,
                                             XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;

  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {
    if (numberOfTopButtons > 0) {
       XtVaSetValues (UxWidget, XmNbottomWidget, top_button_form, NULL);
    } else if (use_icon_name_field) {
       XtVaSetValues (UxWidget, XmNbottomWidget, icon_name_textfield_label, NULL);
    } else XtVaSetValues (UxWidget, XmNbottomWidget, separatorGadget1, NULL);
  }
  return;
}

/***************************************************************************/
/*                                                                         */
/*  activateCB_bottom_button                                               */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/* default cb for bottom buttons */
static  void    activateCB_bottom_button( Widget UxWidget,
                                             XtPointer UxClientData,
                                             XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;

  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {

  }
  return;
}
/******************************************************************************/
/* activateCB_NoFindIconDialog_ChangeNameButton                               */
/******************************************************************************/
void    activateCB_NoFindIconDialog_ChangeNameButton( Widget  UxWidget,
                                        XtPointer UxClientData,
                                        XtPointer UxCallbackArg)

{
  XtDestroyWidget(UxWidget);
}

/******************************************************************************/
/* activateCB_NoFindIconDialog_NameOKButton                                   */
/******************************************************************************/
void    activateCB_NoFindIconDialog_NameOKButton( Widget  UxWidget,
                                        XtPointer UxClientData,
                                        XtPointer UxCallbackArg)

{
  char *basename = (char *)NULL;

  TurnOnHourGlassAllWindows();
  if (bottom_button_one_cb) {
     GetWidgetTextString(icon_name_text_field, &basename);
     bottom_button_one_cb(UxWidget,
                         (XtPointer)basename, UxCallbackArg);
     if (basename) XtFree(basename);
  }
  TurnOffHourGlassAllWindows();
  UxPopdownInterface (IconSelector);
  XtDestroyWidget(UxWidget);
}

/******************************************************************************/
/* display_nofindicon_message - display cant find icons error dialog          */
/* INPUT:  Widget parent - parent to position error dialog new                */
/* OUTPIT: none                                                               */
/******************************************************************************/
void display_nofindicon_message (Widget parent)
{
  Widget dialog;
  Widget widChild;
  char   buffer[512];

  dialog = XmCreateErrorDialog(parent, "nofindiconDialog", NULL, 0);

  widChild = XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON);
  XtVaSetValues(widChild,
                RES_CONVERT( XmNlabelString, "Change Name"),
                NULL);

  widChild = XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
  XtVaSetValues(widChild,
                RES_CONVERT( XmNlabelString, "Name OK"),
                NULL);

  XtUnmanageChild (XmMessageBoxGetChild (dialog, XmDIALOG_HELP_BUTTON));

  XtAddCallback( dialog, XmNokCallback,
          (XtCallbackProc) activateCB_NoFindIconDialog_ChangeNameButton,
          (XtPointer) NULL );
  XtAddCallback( dialog, XmNcancelCallback,
          (XtCallbackProc) activateCB_NoFindIconDialog_NameOKButton,
          (XtPointer) NULL );

  /*********  WARNING - this needs to be translated !!! ***********/
  strcpy(buffer, "There is no set of icons with that filename in the icon folders.\n");
  strcat(buffer, "Move the icons into one of the icon folers, then select \"Name OK\".\n");
  strcat(buffer, "Or, to select a different set of icons, select \"Change Name\".\n");
  strcat(buffer, "\nIf you are a software developer creating a registration package,\n");
  strcat(buffer, "ignore this message and select \"Name OK\".");
  XtVaSetValues (dialog,
                 RES_CONVERT(XmNdialogTitle, GETMESSAGE(6, 31, "Create Action - Error")),
                 RES_CONVERT(XmNmessageString, buffer),
                 NULL);

  XtManageChild (dialog);
  return;
}

/***************************************************************************/
/*                                                                         */
/*  activateCB_bottom_button1                                              */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/*  cb for bottom button 1 = OK */
static  void    activateCB_bottom_button1( Widget UxWidget,
                                             XtPointer UxClientData,
                                             XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;

  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {
    char *basename = (char *)NULL;
    char *fullname = (char *)NULL;
    char *msgPtr, *errPtr;

    GetWidgetTextString(icon_name_text_field, &basename);
    if (basename) {
       if (strchr(basename, '/')) {
          /**** WARNING - this needs to be in a message catalog in the future ****/
	  /* INTERNATIONALIZE */
	  msgPtr = "Invalid Icon filename.\n\
Pathnames are not valid in the \"Enter Icon Filename\" field.\n\
Enter only the name of the icon without the path or extensions.";
          errPtr = XtNewString(msgPtr);
          display_error_message(IconSelector, errPtr);
	  XtFree(errPtr);
       } else {
          FIND_ICONGADGET_ICON(basename, fullname, DtMEDIUM);
          if ( (fullname) && (check_file_exists(fullname)) ) {
             TurnOnHourGlassAllWindows();
             if (bottom_button_one_cb) {
                bottom_button_one_cb(UxWidget,
                                    (XtPointer)fullname, UxCallbackArg);
             }
             TurnOffHourGlassAllWindows();
             UxPopdownInterface (IconSelector);
             if (basename) XtFree(basename);
          } else {
             display_nofindicon_message(IconSelector);
          }
       }
    } else {
       msgPtr = GETMESSAGE(11, 50, "The Icon Filename is invalid.\n\
Please enter a valid icon file name in the\n'Enter Icon Filename' field.");
       errPtr = XtNewString(msgPtr);
       display_error_message(IconSelector, errPtr);
       XtFree(errPtr);
    }
  }
  return;

#if 0   /*************  old code *********************/
    char *pszIconFileName = (char *)NULL;
    char    buffer[MAXBUFSIZE];

    GetWidgetTextString(icon_name_text_field, &pszIconFileName);
    if ( (pszIconFileName) && (check_file_exists(pszIconFileName)) ) {
       TurnOnHourGlassAllWindows();
       if (bottom_button_one_cb) {
          bottom_button_one_cb(UxWidget,
                              (XtPointer)pszIconFileName, UxCallbackArg);
       }
#ifdef _ICONSELECTOR_DESTROY_ENABLED
       free_container_contents ();
       if (selected_icon_name) {
          XtFree(selected_icon_name);
       }
       selected_icon_name = (char *)NULL;
       XtDestroyWidget (XtParent(icon_selection_dialog));
       IconSelector = (Widget)NULL;
       TurnOffHourGlassAllWindows();
#else
       TurnOffHourGlassAllWindows();
       UxPopdownInterface (IconSelector);
#endif  /* _ICONSELECTOR_DESTROY_ENABLED */
       XtFree(pszIconFileName);
    } else {
       strcpy(buffer, GETMESSAGE(11, 50, "The Icon Filename is invalid.\nPlease enter a valid icon file name in the\n'Enter Icon Filename' field."));
       display_error_message(IconSelector, buffer);
    }
  }

#if 0
    if (selected_icon_name) {
      TurnOnHourGlassAllWindows();
      if (bottom_button_one_cb) {
         bottom_button_one_cb(UxWidget,
                             (XtPointer)selected_icon_name, UxCallbackArg);
      }
#ifdef _ICONSELECTOR_DESTROY_ENABLED
      free_container_contents ();
      XtFree(selected_icon_name);
      selected_icon_name = (char *)NULL;
      XtDestroyWidget (XtParent(icon_selection_dialog));
      IconSelector = (Widget)NULL;
      TurnOffHourGlassAllWindows();
#else
      TurnOffHourGlassAllWindows();
      UxPopdownInterface (IconSelector);
#endif  /* _ICONSELECTOR_DESTROY_ENABLED */
    }
  }
#endif
  return;
#endif  /*************  old code *********************/

}

/***************************************************************************/
/*                                                                         */
/*   activateCB_bottom_button2                                             */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/*  cb for bottom button 2 = CANCEL */
static  void    activateCB_bottom_button2( Widget UxWidget,
                                             XtPointer UxClientData,
                                             XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;

  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {
#ifdef _ICONSELECTOR_DESTROY_ENABLED
    free_container_contents ();
    XtDestroyWidget (XtParent(icon_selection_dialog));
    IconSelector = (Widget)NULL;
#else
    UxPopdownInterface (IconSelector);
#endif  /* _ICONSELECTOR_DESTROY_ENABLED */
  }
  return;
}

/***************************************************************************/
/*                                                                         */
/*  activateCB_bottom_button3                                              */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/* default cb for bottom button3 = HELP */
static  void    activateCB_bottom_button3( Widget UxWidget,
                                             XtPointer UxClientData,
                                             XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;

  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {
    DisplayHelpDialog(UxWidget, (XtPointer)HELP_ICONSELECTOR, UxCallbackArg);
  }
  return;
}

/***************************************************************************/
/*                                                                         */
/*   activateCB_filter_text_field                                          */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
static  void    activateCB_filter_text_field( Widget UxWidget,
                                             XtPointer UxClientData,
                                             XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;
  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {
    char *filter;

    filter = XmTextFieldGetString (UxWidget);
    apply_filter (filter);
  }
  UxIcon_selection_dialogContext = UxSaveCtx;
}

/***************************************************************************/
/*                                                                         */
/*  defaultActionCB_dir_scrolled_list                                      */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
static  void    defaultActionCB_dir_scrolled_list( Widget UxWidget,
                                                  XtPointer UxClientData,
                                                  XtPointer UxCallbackArg )
{
  _UxCicon_selection_dialog *UxSaveCtx, *UxContext;

  UxSaveCtx = UxIcon_selection_dialogContext;
  UxIcon_selection_dialogContext = UxContext =
                  (_UxCicon_selection_dialog *) UxGetContext( UxWidget );
  {
    XmListCallbackStruct *listcb;
    char *filter;

    listcb = (XmListCallbackStruct *) UxCallbackArg;
    filter = (char *)XmStringToText (listcb->item);
    apply_filter (filter);
 /* XtManageChild (icon_scrolled_win);  */
  }
  UxIcon_selection_dialogContext = UxSaveCtx;
}

/***************************************************************************/
/*                                                                         */
/*  selectionCB_icon_scrolled_container                                    */
/*                                                                         */
/***************************************************************************/
static  void    selectionCB_icon_scrolled_container ( Widget UxWidget,
                                                  XtPointer UxClientData,
                                                  XtPointer UxCallbackArg )
{
  char     *filename;
  XmString  xmbasename;
  char     *basename;

  XtVaGetValues (UxWidget, XmNimageName, &filename, NULL);
#ifdef DEBUG
  printf("In icon callback for '%s'\n", filename);
#endif
  if (use_icon_name_field) {
     XtVaGetValues (UxWidget, XmNstring, &xmbasename, NULL);
     basename = XmStringToText(xmbasename);
     XmTextFieldSetString (icon_name_text_field, basename);
     if (basename) free(basename);
  }
  if (selected_icon_name) {
     if (selected_icon != UxWidget) {
        _DtIconSetState(selected_icon, FALSE, FALSE);
     }
     XtFree(selected_icon_name);
     selected_icon_name = (char *)NULL;
  }
  if (filename) {
     selected_icon_name = XtMalloc(strlen(filename) + 1);
     selected_icon_name = strcpy(selected_icon_name, filename);
  } else {
     selected_icon_name = (char *)NULL;
  }
  selected_icon = UxWidget;

  return;
}

/***************************************************************************/
/*                                                                         */
/*  resizeCB_clipWindow                                                    */
/*                                                                         */
/***************************************************************************/
void    resizeCB_clipWindow( Widget UxWidget,
                             XtPointer UxClientData,
                             XtPointer UxCallbackArg )
{
  Widget     widvScrollbar;
  Widget     widRowColumn;
  int        increment;
  XtArgVal /* Dimension */  height;

  XtVaGetValues(XtParent(UxWidget), XmNverticalScrollBar, &widvScrollbar, NULL);
  widRowColumn = (Widget)UxClientData;
  XtVaGetValues(widRowColumn, XmNheight, &height, NULL);

#ifdef DEBUG
  printf("height = %d\n", height);
  printf("icon_count = %d\n", icon_count);
#endif

  increment = (int)height / icon_count;
  XtVaSetValues(widvScrollbar, XmNincrement, increment, NULL);

  return;

}

/***************************************************************************/
/*                                                                         */
/*  _Uxbuild_icon_selection_dialog                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
static Widget   _Uxbuild_icon_selection_dialog(void)
{
        Widget          _UxParent;
        char            *UxTmp0;
        int             nbutton = 0;
        int             ntotalbuttons = numberOfBottomButtons;
#define TIGHTNESS       20
        Arg             args[20];
        int             i=0;
        Widget          widclipWindow;

        /* Creation of icon_selection_dialog */
        _UxParent = UxParent;
        if ( _UxParent == NULL )
        {
                _UxParent = UxTopLevel;
        }

        _UxParent = XtVaCreatePopupShell( "icon_selection_dialog_shell",
                        xmDialogShellWidgetClass, _UxParent,
                        XmNx, 18,
                        XmNy, 116,
                        XmNwidth, 610,
                        XmNheight, 534,
                        XmNshellUnitType, XmPIXELS,
                        XmNkeyboardFocusPolicy, XmEXPLICIT,
                        XmNtitle, "icon_selection_dialog",
                        NULL );

        UxTmp0 = dialog_title ? (char *)dialog_title : "Icon Selection Window";

        icon_selection_dialog = XtVaCreateWidget( "icon_selection_dialog",
                        xmFormWidgetClass,
                        _UxParent,
                        XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                        XmNwidth, 550,
                        XmNheight, 534,
                        XmNunitType, XmPIXELS,
                        RES_CONVERT( XmNdialogTitle, UxTmp0 ),
                        XmNautoUnmanage, FALSE,
                        XmNdefaultPosition, FALSE,
                        NULL );
        UxPutContext( icon_selection_dialog, (char *) UxIcon_selection_dialogContext );

        /*******************************************************************/
        /* Set up help callback                                            */
        /*******************************************************************/
        XtAddCallback( icon_selection_dialog, XmNhelpCallback,
                (XtCallbackProc) helpCB_general,
                (XtPointer) HELP_ICONSELECTOR );


        /* Creation of bottom_button_form */
        bottom_button_form = XtVaCreateManagedWidget( "bottom_button_form",
                        xmFormWidgetClass,
                        icon_selection_dialog,
                        XmNskipAdjust, TRUE,
                        XmNfractionBase, ((TIGHTNESS * ntotalbuttons) - 1),

                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 20,
                        XmNbottomAttachment, XmATTACH_FORM,

                        NULL );
        UxPutContext( bottom_button_form, (char *) UxIcon_selection_dialogContext );


        UxTmp0 = bottom_button_one_label ? (char *)bottom_button_one_label : GETMESSAGE(6, 10, "OK");

        /* Creation of bottom_button1 = OK */
        bottom_button1 = XtVaCreateManagedWidget( "bottom_button1",
                        xmPushButtonWidgetClass,
                        bottom_button_form,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),

                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),

                        NULL );
        nbutton++;
        XtAddCallback( bottom_button1, XmNactivateCallback,
                (XtCallbackProc) activateCB_bottom_button1,
                (XtPointer) NULL );

        UxPutContext( bottom_button1, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = bottom_button_two_label ? (char *)bottom_button_two_label : GETMESSAGE(6, 12, "Cancel");

        /* Creation of bottom_button2 = CANCEL */
        bottom_button2 = XtVaCreateManagedWidget( "bottom_button2",
                        xmPushButtonWidgetClass,
                        bottom_button_form,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),

                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),

                        NULL );
        nbutton++;
        XtAddCallback( bottom_button2, XmNactivateCallback,
                (XtCallbackProc) bottom_button_two_cb ?
                      bottom_button_two_cb : activateCB_bottom_button2,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( bottom_button2, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = bottom_button_three_label ? (char *)bottom_button_three_label : GETMESSAGE(6, 13, "Help");

        /* Creation of bottom_button3 = HELP */
        bottom_button3 = XtVaCreateManagedWidget( "bottom_button3",
                        xmPushButtonWidgetClass,
                        bottom_button_form,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        XmNuserData, icon_selection_dialog,

                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),

                        NULL );
        nbutton++;
        XtAddCallback( bottom_button3, XmNactivateCallback,
                (XtCallbackProc) bottom_button_three_cb ?
                      bottom_button_three_cb : activateCB_bottom_button3,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( bottom_button3, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = bottom_button_four_label ? (char *)bottom_button_four_label : GETMESSAGE(12, 10, "Filter");

        /* Creation of bottom_button4 */
        bottom_button4 = XtVaCreateManagedWidget( "bottom_button4",
                        xmPushButtonWidgetClass,
                        bottom_button_form,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),

                        XmNleftAttachment, nbutton ?
                                            XmATTACH_POSITION : XmATTACH_FORM,
                        XmNleftPosition, TIGHTNESS * nbutton,
                        XmNrightAttachment, nbutton != (ntotalbuttons - 1) ?
                                             XmATTACH_POSITION : XmATTACH_FORM,
                        XmNrightPosition, (TIGHTNESS * nbutton)+(TIGHTNESS - 1),

                        NULL );
        nbutton++;
        XtAddCallback( bottom_button4, XmNactivateCallback,
                (XtCallbackProc) bottom_button_four_cb ?
                      bottom_button_four_cb : activateCB_bottom_button,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( bottom_button4, (char *) UxIcon_selection_dialogContext );


        /* Creation of separatorGadget1 */
        separatorGadget1 = XtVaCreateManagedWidget( "separatorGadget1",
                        xmSeparatorGadgetClass,
                        icon_selection_dialog,
                        XmNx, 0,
                        XmNy, 490,
                        XmNrightOffset, 1,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_FORM,
                        XmNbottomOffset, 10,
                        XmNbottomWidget, bottom_button_form,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        NULL );
        UxPutContext( separatorGadget1, (char *) UxIcon_selection_dialogContext );


        /* Creation of icon_name_text_field */
        icon_name_text_field = XtVaCreateManagedWidget( "icon_name_text_field",
                        xmTextFieldWidgetClass,
                        icon_selection_dialog,
                        XmNx, 9,
                        XmNy, 440,
                        XmNbottomOffset, 10,
                        XmNbottomWidget, separatorGadget1,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL );
        UxPutContext( icon_name_text_field, (char *) UxIcon_selection_dialogContext );
        ISD_SelectedIconTextField = icon_name_text_field;

        UxTmp0 = name_field_title ? (char *)name_field_title : GETMESSAGE(11, 12, "Enter Icon Filename");

        /* Creation of icon_name_textfield_label */
        icon_name_textfield_label = XtVaCreateManagedWidget( "icon_name_textfield_label",
                        xmLabelWidgetClass,
                        icon_selection_dialog,
                        XmNx, 9,
                        XmNy, 410,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        XmNbottomOffset, 0,
                        XmNbottomWidget, icon_name_text_field,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 0,
                        XmNleftWidget, icon_name_text_field,
                        XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        NULL );
        UxPutContext( icon_name_textfield_label, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = filter_field_title ? (char *)filter_field_title : GETMESSAGE(12, 10, "Filter");

        /* Creation of filter_textfield_label */
        filter_textfield_label = XtVaCreateManagedWidget( "filter_textfield_label",
                        xmLabelWidgetClass,
                        icon_selection_dialog,
                        XmNx, 10,
                        XmNy, 10,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNtopOffset, 10,
                        XmNtopAttachment, XmATTACH_FORM,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL );
        UxPutContext( filter_textfield_label, (char *) UxIcon_selection_dialogContext );


        /* Creation of filter_text_field */
        filter_text_field = XtVaCreateManagedWidget( "filter_text_field",
                        xmTextFieldWidgetClass,
                        icon_selection_dialog,
                        XmNx, 10,
                        XmNy, 40,
                        XmNtopOffset, 0,
                        XmNtopWidget, filter_textfield_label,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL );
        XtAddCallback( filter_text_field, XmNactivateCallback,
                (XtCallbackProc) activateCB_filter_text_field,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( filter_text_field, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = directory_title ? (char *)directory_title : GETMESSAGE(11, 30, "Icon Folders");

        /* Creation of directory_list_label */
        directory_list_label = XtVaCreateManagedWidget( "directory_list_label",
                        xmLabelWidgetClass,
                        icon_selection_dialog,
                        XmNx, 0,
                        XmNy, 90,
                        XmNlabelType, XmSTRING,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNtopOffset, 10,
                        XmNtopWidget, filter_text_field,
                        XmNtopAttachment, use_filter_field ? XmATTACH_WIDGET : XmATTACH_FORM,
                        XmNleftOffset, 10,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL );
        UxPutContext( directory_list_label, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = container_title ? (char *)container_title : GETMESSAGE(11, 11, "Icon Files");

        /* Creation of icon_container_label */
        icon_container_label = XtVaCreateManagedWidget( "icon_container_label",
                        xmLabelWidgetClass,
                        icon_selection_dialog,
                        XmNx, 281,
                        XmNy, 90,
                        XmNlabelType, XmSTRING,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        XmNalignment, XmALIGNMENT_BEGINNING,
                        XmNtopOffset, 10,
                        XmNtopWidget, filter_text_field,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNleftPosition, 55,
                        NULL );
        UxPutContext( icon_container_label, (char *) UxIcon_selection_dialogContext );


        /* Creation of top_button_form */
        top_button_form = XtVaCreateManagedWidget( "top_button_form",
                        xmFormWidgetClass,
                        icon_selection_dialog,
                        XmNresizePolicy, XmRESIZE_NONE,
                        XmNx, 10,
                        XmNy, 290,
                        XmNbottomOffset, 10,
                        XmNbottomWidget, use_icon_name_field ? icon_name_textfield_label : separatorGadget1,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        XmNrightOffset, 1,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_FORM,
                        NULL );
        UxPutContext( top_button_form, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = top_button_one_label ? (char *)top_button_one_label : "No_Label";

        /* Creation of top_button1 */
        top_button1 = XtVaCreateManagedWidget( "top_button1",
                        xmPushButtonWidgetClass,
                        top_button_form,
                        XmNx, 40,
                        XmNy, 0,
                        XmNleftOffset, 0,
                        XmNbottomAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNtopOffset, 0,
                        XmNleftPosition, get_top_b1_position (numberOfTopButtons),
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        NULL );
        XtAddCallback( top_button1, XmNactivateCallback,
                (XtCallbackProc) top_button_one_cb,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( top_button1, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = top_button_two_label ? (char *)top_button_two_label : "No_Label";

        /* Creation of top_button2 */
        top_button2 = XtVaCreateManagedWidget( "top_button2",
                        xmPushButtonWidgetClass,
                        top_button_form,
                        XmNx, 220,
                        XmNy, 0,
                        XmNleftOffset, 0,
                        XmNtopOffset, 0,
                        XmNleftPosition, get_top_b2_position (numberOfTopButtons),
                        XmNbottomAttachment, XmATTACH_FORM,
                        XmNleftAttachment, XmATTACH_POSITION,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        NULL );
        XtAddCallback( top_button2, XmNactivateCallback,
                (XtCallbackProc) top_button_two_cb,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( top_button2, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = top_button_three_label ? (char *)top_button_three_label : "No_Label";

        /* Creation of top_button3 */
        top_button3 = XtVaCreateManagedWidget( "top_button3",
                        xmPushButtonWidgetClass,
                        top_button_form,
                        XmNx, 310,
                        XmNy, 0,
                        RES_CONVERT( XmNlabelString, UxTmp0 ),
                        XmNleftPosition, 72,
                        XmNleftOffset, 1,
                        XmNleftAttachment, XmATTACH_POSITION,
                        XmNbottomOffset, 0,
                        XmNbottomAttachment, XmATTACH_FORM,
                        NULL );
        XtAddCallback( top_button3, XmNactivateCallback,
                (XtCallbackProc) top_button_three_cb,
                (XtPointer) UxIcon_selection_dialogContext );

        UxPutContext( top_button3, (char *) UxIcon_selection_dialogContext );

        UxTmp0 = (char *)((numberOfTopButtons > 0) ? top_button_form : icon_name_textfield_label);

        /* Creation of dir_scrolled_list */
        XtSetArg(args[i], XmNshadowThickness, 2); i++;
        XtSetArg(args[i], XmNlistSizePolicy, XmCONSTANT); i++;

        XtSetArg(args[i], XmNleftOffset, 10); i++;
        XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM); i++;
        XtSetArg(args[i], XmNtopOffset, 0); i++;
        XtSetArg(args[i], XmNtopWidget, directory_list_label); i++;
        XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
        XtSetArg(args[i], XmNrightOffset, 20); i++;
        XtSetArg(args[i], XmNrightWidget, icon_container_label); i++;
        XtSetArg(args[i], XmNrightAttachment, XmATTACH_WIDGET); i++;

        XtSetArg(args[i], XmNbottomOffset, 10); i++;
        if (numberOfTopButtons > 0) {
           XtSetArg(args[i], XmNbottomWidget, top_button_form); i++;
        } else if (use_icon_name_field) {
           XtSetArg(args[i], XmNbottomWidget, icon_name_textfield_label); i++;
        } else {
           XtSetArg(args[i], XmNbottomWidget, separatorGadget1); i++;
        }
        XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET); i++;

        dir_scrolled_list = XmCreateScrolledList(icon_selection_dialog,
                                                "dir_scrolled_list",
                                                args,
                                                i);
        XtVaSetValues(dir_scrolled_list,
                        XmNselectionPolicy, XmBROWSE_SELECT,
                        XmNvisibleItemCount, 15,
                        NULL );
        XtManageChild(dir_scrolled_list);

        UxPutContext( dir_scrolled_list, (char *) UxIcon_selection_dialogContext );
        calc_bottom_attachment (dir_scrolled_list,
                               (XtPointer) UxIcon_selection_dialogContext,
                               (XtPointer) NULL);
        XtAddCallback( dir_scrolled_list, XmNdefaultActionCallback,
                (XtCallbackProc) defaultActionCB_dir_scrolled_list,
                (XtPointer) UxIcon_selection_dialogContext );

        UxTmp0 = (char *)((numberOfTopButtons > 0) ? top_button_form : icon_name_textfield_label);

        /* Creation of icon_scrolled_win */
        icon_scrolled_win = XtVaCreateManagedWidget( "icon_scrolled_win",
                        xmScrolledWindowWidgetClass,
                        icon_selection_dialog,
                        XmNscrollingPolicy, XmAUTOMATIC,
/*                      XmNnavigationType, XmTAB_GROUP, */
                        XmNx, 282,
                        XmNy, 84,
                        XmNscrollBarDisplayPolicy, XmAS_NEEDED,
                        XmNrightOffset, 10,
                        XmNrightAttachment, XmATTACH_FORM,
                        XmNtopOffset, 0,
                        XmNtopWidget, icon_container_label,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNleftOffset, 0,
                        XmNleftWidget, icon_container_label,
                        XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
                        XmNbottomOffset, 10,
                        XmNbottomWidget, None,
                        XmNbottomAttachment, XmATTACH_WIDGET,
                        NULL );




        UxPutContext( icon_scrolled_win, (char *) UxIcon_selection_dialogContext );
        calc_bottom_attachment (icon_scrolled_win,
                               (XtPointer) UxIcon_selection_dialogContext,
                               (XtPointer) NULL);

        /* Creation of icon_scrolled_container */
        icon_scrolled_container = XtVaCreateManagedWidget( "icon_scrolled_container",
                        xmRowColumnWidgetClass,
                        icon_scrolled_win,
                        XmNnavigationType, XmTAB_GROUP,
                      /*XmNborderWidth, 1,*/
                      /*XmNnumColumns, 2,*/
                        XmNnumColumns, 1,
                        XmNorientation, XmVERTICAL,
                        XmNpacking, XmPACK_COLUMN,
                        XmNshadowThickness, 1,
                        NULL );

        XtVaSetValues(icon_selection_dialog,
                        XmNcancelButton, bottom_button2,
                        NULL );

        XtAddCallback( icon_selection_dialog, XmNdestroyCallback,
                      (XtCallbackProc) UxDestroyContextCB,
                      (XtPointer) UxIcon_selection_dialogContext);

        XtVaGetValues (icon_scrolled_win, XmNclipWindow, &widclipWindow, NULL);
        XtAddCallback( widclipWindow, XmNresizeCallback,
                (XtCallbackProc) resizeCB_clipWindow,
                (XtPointer) icon_scrolled_container );

        return ( icon_selection_dialog );
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

Widget  create_icon_selection_dialog(swidget    _UxUxParent,
        unsigned char   *_Uxdialog_title,
        unsigned char   *_Uxfilter_field_title,
        int     _Uxuse_filter_field,
        unsigned char   *_Uxfile_filter,
        unsigned char   *_Uxdirectory_title,
        unsigned char   **_Uxdirectories_list,
        unsigned char   *_Uxcontainer_title,
        int     _UxnumberOfTopButtons,
        unsigned char   *_Uxtop_button_one_label,
        void    (*_Uxtop_button_one_cb)(),
        unsigned char   *_Uxtop_button_two_label,
        void    (*_Uxtop_button_two_cb)(),
        unsigned char   *_Uxtop_button_three_label,
        void    (*_Uxtop_button_three_cb)(),
        int     _Uxuse_icon_name_field,
        unsigned char   *_Uxname_field_title,
        int     _UxnumberOfBottomButtons,
        unsigned char   *_Uxbottom_button_one_label,
        void    (*_Uxbottom_button_one_cb)(),
        unsigned char   *_Uxbottom_button_two_label,
        void    (*_Uxbottom_button_two_cb)(),
        unsigned char   *_Uxbottom_button_three_label,
        void    (*_Uxbottom_button_three_cb)(),
        unsigned char   *_Uxbottom_button_four_label,
        void    (*_Uxbottom_button_four_cb)() )


{
        char                   *initial_filter;
        char                   *icon_file_name;
        Widget                  rtrn;
        int                     lcv;
        _UxCicon_selection_dialog *UxContext;

        UxIcon_selection_dialogContext = UxContext =
                (_UxCicon_selection_dialog *) UxNewContext( sizeof(_UxCicon_selection_dialog), False );

        UxParent = _UxUxParent;
        dialog_title = _Uxdialog_title;
        filter_field_title = _Uxfilter_field_title;
        use_filter_field = _Uxuse_filter_field;
        file_filter = _Uxfile_filter;
        directory_title = _Uxdirectory_title;
        directories_list = _Uxdirectories_list;
        container_title = _Uxcontainer_title;
        numberOfTopButtons = _UxnumberOfTopButtons;
        top_button_one_label = _Uxtop_button_one_label;
        top_button_one_cb = _Uxtop_button_one_cb;
        top_button_two_label = _Uxtop_button_two_label;
        top_button_two_cb = _Uxtop_button_two_cb;
        top_button_three_label = _Uxtop_button_three_label;
        top_button_three_cb = _Uxtop_button_three_cb;
        use_icon_name_field = _Uxuse_icon_name_field;
        name_field_title = _Uxname_field_title;
        numberOfBottomButtons = _UxnumberOfBottomButtons;
        bottom_button_one_label = _Uxbottom_button_one_label;
        bottom_button_one_cb = _Uxbottom_button_one_cb;
        bottom_button_two_label = _Uxbottom_button_two_label;
        bottom_button_two_cb = _Uxbottom_button_two_cb;
        bottom_button_three_label = _Uxbottom_button_three_label;
        bottom_button_three_cb = _Uxbottom_button_three_cb;
        bottom_button_four_label = _Uxbottom_button_four_label;
        bottom_button_four_cb = _Uxbottom_button_four_cb;

        rtrn = _Uxbuild_icon_selection_dialog();

        switch (numberOfTopButtons){
                case 0 : XtUnmanageChild (top_button_form); break;
                case 1 : XtUnmanageChild (top_button2);
                case 2 : XtUnmanageChild (top_button3);
        };

        if (numberOfBottomButtons == 3)
           XtUnmanageChild (bottom_button4);

        if (!use_filter_field) {
           XtUnmanageChild (filter_textfield_label);
           XtUnmanageChild (filter_text_field);
        }

        if (!use_icon_name_field) {
           XtUnmanageChild (icon_name_textfield_label);
           XtUnmanageChild (icon_name_text_field);
        }

        TurnOnHourGlassAllWindows();

        /******************************************************************/
        /* save file_filter */
        /******************************************************************/
        file_filter_global = (char *)calloc(strlen((char *)file_filter)+1, sizeof(char));

#ifdef DEBUG
        if (!file_filter_global) printf("Calloc error for file_filter_global, in create_icon_selection_dialog.\n");
#endif

        if (file_filter[0] == '*')
           file_filter_global = strcpy(file_filter_global, (char *)&file_filter[1]);
        else
           file_filter_global = strcpy(file_filter_global, (char *)file_filter);

        load_directories_list ((char **)directories_list);
        initial_filter = initialize_filter ((char *)file_filter);

        /******************************************************************/
        /* save main_filter */
        /******************************************************************/
        main_filter = (char *)calloc(strlen(initial_filter)+1, sizeof(char));

#ifdef DEBUG
        if (!main_filter) printf("Calloc error for main_filter, in create_icon_selection_dialog.\n");
#endif

        main_filter = strcpy(main_filter, initial_filter);;

        icons_in_container = NULL;
        icon_count = 0;

        update_container_contents (initial_filter);

        /******************************************************************/
        /* set selected_icon_name to first name in list                   */
        /******************************************************************/
#if 0
        if (icons_in_container) {
           XtVaGetValues (icons_in_container[0], XmNimageName, &icon_file_name, NULL);
           XmTextFieldSetString (icon_name_text_field, icon_file_name);
           selected_icon_name = XtMalloc(strlen(icon_file_name) + 1);
           if (selected_icon_name)
              selected_icon_name = strcpy(selected_icon_name, icon_file_name);
        }
#endif

        load_filter_text_field (initial_filter);

        TurnOffHourGlassAllWindows();

        return(rtrn);
}
