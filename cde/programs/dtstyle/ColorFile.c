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
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:        ColorFile.c
 **
 **   Project:     DT 3.0
 **
 **   Description:
 **
 **
 **  (c) Copyright Hewlett-Packard Company, 1990.  
 **
 **
 **
 ****************************************************************************
 ************************************<+>*************************************/
/* $XConsortium: ColorFile.c /main/5 1995/10/30 13:08:20 rswiston $ */
/*+++++++++++++++++++++++++++++++++++++++*/
/* include files                         */
/*+++++++++++++++++++++++++++++++++++++++*/

#include <stdio.h>
#include <fcntl.h>

#include <X11/Xlib.h>

#ifdef __hpux
#include <ndir.h>               /*   opendir(), directory(3C) */
#else
#if SVR4 || sco
#include <dirent.h>             /* opendir(), directory(3C) */
#else
#include <sys/dir.h>
#ifdef __apollo
#include <X11/apollosys.h>      /* needed for S_ISDIR macro */
#endif
#endif
#endif


#include <Xm/Xm.h>

#include <Dt/UserMsg.h>
#include <Dt/DtNlUtils.h>

#include "Main.h"
#include "ColorMain.h"
#include "ColorEdit.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* include extern functions              */
/*+++++++++++++++++++++++++++++++++++++++*/
#include "ColorFile.h"

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local #defines                        */
/*+++++++++++++++++++++++++++++++++++++++*/
#define MAX_NUM_DIRECTORIES  12
#define BUFSIZE              1024
#define TMP_DEFAULT_NAME     "tmppalette"

/* these defines used to create error dialog */
#define  ERR1  ((char *)GETMESSAGE(16, 11, "Unable to delete '%s'.\n"))
#define  ERR2  ((char *)GETMESSAGE(16, 12, "Check $HOME/.dt/errorlog for hints."))
#define  ERR3  ((char *)GETMESSAGE(16, 19, "Could not open directory %s."))
#define  ERR4  ((char *)GETMESSAGE(16, 20, "Could not open %s."))
#define  ERR5  ((char *)GETMESSAGE(16, 21, "%s is an invalid palette file.\n"))
#define  ERR9  ((char *)GETMESSAGE(16, 18, "Warning, Too many directories listed in the resource paletteDirectories,\n Maximum number is %d."))

#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP "/opt/dt"
#endif

/*+++++++++++++++++++++++++++++++++++++++*/
/* Local typedefs                        */
/*+++++++++++++++++++++++++++++++++++++++*/
/* Palette Directories structure */

typedef struct {
   int NumOfDirectories;
   char *directories[MAX_NUM_DIRECTORIES];
} Dir_data;

Dir_data palette_dirs;


/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Functions                    */
/*+++++++++++++++++++++++++++++++++++++++*/


static void SwitchAItoPS( palette *new_palette );
static palette *RemovePaletteFromList( palette *tmp_palette );
static void SetColors(
                        int num_of_colors,
                        palette *new_palette) ;
static void ReadInInfo(
                        char *buf,
                        int nbytes,
                        palette *new_palette );
static void InitializeBW(
                        unsigned long color,
                        int num_of_colors,
                        palette *new_palette ) ;



/*+++++++++++++++++++++++++++++++++++++++*/
/* Internal Variables                    */
/*+++++++++++++++++++++++++++++++++++++++*/
int NumOfPalettes = 0;

static char *PALETTE_DIR = CDE_INSTALLATION_TOP "/palettes/";
static char *PALETTE_DIR_ADM = "/etc/dt/palettes/";
static char *DEFAULT_FILENAME = "palette.dt";
static char *B_O_W = "BlackWhite.dp";
static char *W_O_B = "WhiteBlack.dp";
static char *W_ONLY = "White.dp";
static char *B_ONLY = "Black.dp";
static char *home_palette_dir;

static int loopcount = 0;
static DIR  *dirp;
#ifdef __hpux
static struct direct *file_descpt;
#else
static struct dirent *file_descpt;
#endif /* __hpux */
static Boolean open_dir = False;
extern XrmDatabase hm_pl_DB;

XColor saved_color[2];


/*
** readinpalettes reads in the different palettes for the customizer.
**
** The directories read from include:
**     1. the system palettes (PALETTE_DIR)
**     2. the user defined directories specified by the resource PaletteDir. 
**     3. the users $HOME/.dt/palettes
*/

void 
ReadInPalettes(
        char *text )
{
/*
**  if text is NULL then open only the global dir and user dir, 
**  otherwise open all the directories listed *text. 
*/
   AddSystemPath();

   if (text != NULL)
      AddDirectories(text);

   AddHomePath();
   
   ReadPaletteLoop(False);

}

/*
**  ReadPalette reads the contents of the file in_filename, allocates space
**  for a new palette, and puts it in there.
**  The varible in_filename passed in should be just the filename 
**  of the file.  The absolute file name is constructed in this routine.
**  i.e.  directory/file.
**  Finally this routine ups the count for number of palettes in the
**  customizer.
**  The parameter length, which is passed in, is the length of the in_filename
**  string.
**  After the system palettes are read in, if there are duplicate copies 
**  of palette names, the first one encountered is the one that is used.  
**  Subsequent palettes with the same name are discarded.
**  Then when the users home palette directory is read, if there are
**  duplicates, the users home palettes is kept.
*/
void 
ReadPalette(
        char *directory,
        char *in_filename,
#if NeedWidePrototypes
        int length )
#else
        short length )
#endif
{
   char *filename;
   int  i, fd, nbytes;
   char buf[BUFSIZE];
   palette *new_palette, *tmp_palette;
   Boolean  modify, add, found;
   char *tmpStr;
   char *p;

   modify = FALSE;
   if (strcmp(directory, PALETTE_DIR) == 0)
       add = TRUE;
   else
       add = FALSE;
       
/* Allocate space for this new palette.*/
   new_palette = (palette *)XtMalloc( sizeof(palette) + 1);

/* create the filename to be read by adding directory in front of it */
   filename = (char *)XtMalloc( strlen(directory) + length + 2);
   strcpy(filename, directory); 

   if(DtCharCount(DtStrrchr(directory, '/')) != 1)
        strcat(filename, "/"); 
   strcat(filename, in_filename); 

/* open the file for reading */
   if( (fd = open( filename, O_RDONLY)) == -1)
   {
      tmpStr = (char *)XtMalloc(strlen(ERR4) + strlen(filename) + 1);
      sprintf(tmpStr, ERR4, filename);
      _DtSimpleErrnoError (progName, DtWarning, NULL, tmpStr, NULL);
      XtFree(tmpStr);
      XtFree(filename);
      return;
   }
   else 
   {
    /* determine number of bytes in the name of the palette */
       i = strlen(in_filename) - strlen(PALETTE_SUFFIX);

    /* allocate enough space for the name */
      new_palette->name = (char *)XtMalloc(i + 1);

    /* load the filename into the name element */
      p = strstr (in_filename, PALETTE_SUFFIX);
      *p = '\0';
      strcpy(new_palette->name, in_filename);

/* 
** Now we have a name lets check to see if it is already in the linked
** list.  If it is and it is from the system palettes, replace it.
** If it is already in the list but is not from the system palettes,
** don't overwrite it - discard the one just read and go on to next palette.
*/
      if(pHeadPalette != NULL)
      {
         found = FALSE;
         tmp_palette = pHeadPalette;
         while(tmp_palette->next != NULL )
         {
            if(strcmp(tmp_palette->name, new_palette->name)) 
               tmp_palette = tmp_palette->next;
            else
            {
               found = TRUE;
               if ((strcmp(tmp_palette->directory, PALETTE_DIR) == 0) ||
                   (strcmp(directory, home_palette_dir) == 0))
               {
		   XtFree(new_palette->name);   
		   XtFree((char *)new_palette);   
		   XtFree(tmp_palette->directory);   
		   new_palette = tmp_palette;
		   modify = TRUE;
               }
               else
	       {
		   XtFree(new_palette->name);   
		   XtFree((char *)new_palette);   
               }
               break;
            }
         }
        /* check the last palette */
         if (!found)
         {
	     if (strcmp(tmp_palette->name, new_palette->name) == 0) 

                 if ((strcmp(tmp_palette->directory, PALETTE_DIR) == 0) ||
                     (strcmp(directory, home_palette_dir) == 0))
		 {
		    XtFree(new_palette->name);
		    XtFree((char *)new_palette);
		    XtFree(tmp_palette->directory);
		    new_palette = tmp_palette;
		    modify = TRUE;
		 }
		 else 
		 {
		    if (!add) /* not system palettes */
		    {
		       XtFree(new_palette->name);   
		       XtFree((char *)new_palette);   
		    }
		 }

             else
	        add = TRUE;
	 }
      }

      if ((modify == TRUE)||(add == TRUE))
      {
	/* allocate enough space for the directory */
	  new_palette->directory = (char *)XtMalloc(strlen(directory) + 1);

	/* load the directory name into the directory element */
	  strcpy(new_palette->directory, directory);

	/* set the num_of_colors  to 0 */
	  new_palette->num_of_colors = 0;

	  nbytes = read(fd, buf, BUFSIZE);
	  if( nbytes == BUFSIZE || nbytes == 0)
	  {
	     tmpStr = (char *)XtMalloc(strlen(ERR5) + strlen(filename) + 1);
	     sprintf(tmpStr, ERR5, filename);
	     _DtSimpleError (progName, DtWarning, NULL, tmpStr, NULL);
	     XtFree(filename);
	     XtFree(tmpStr);
	     close(fd);
	     return;
	  }
	  else
	  {
	   /* go parse the data from the palette file */
	     ReadInInfo(buf, nbytes, new_palette);
	  } 

      } /* modify == TRUE */

      /* close the file */
      close(fd); 

    } /* else */

/*
**  Find the next available location within the linked list of palettes to 
**  store the palette just created.  If it is the first one store it in
**  pHeadPalettes, else store it in the first place a NULL is encountered
**  for the next member of the palette structure.
*/
    if (add == TRUE)
    {
      /* set the next pointer to NULL*/
       new_palette->next = NULL;

      /* increment the total number of palettes in the customizer */
       NumOfPalettes++;

       if( pHeadPalette == NULL /* First entry */
          || (pHeadPalette != NULL && strcmp(pHeadPalette->name, new_palette->name) > 0)) /* Earlier entry than current list head */
       {
           new_palette->item_position = 1;
           if(pHeadPalette)
           {
               new_palette->next = pHeadPalette;
               /* Increment position poineter of other items in the list */
	       tmp_palette = pHeadPalette;
               while( tmp_palette != NULL)
               {
                   tmp_palette->item_position += 1;
                   tmp_palette = tmp_palette->next;
               }
           }

           pHeadPalette = new_palette;
       }
       else
       {
           tmp_palette = pHeadPalette;

           /* Search through the linked list to find the first entry with a 
              name > new entries name, new item will be inserted after it */
           while(tmp_palette->next && strcmp(tmp_palette->next->name, new_palette->name) < 0) 
           {
               tmp_palette = tmp_palette->next;
           }

           /* Insert the new palette */
           new_palette->next = tmp_palette->next;
           tmp_palette->next = new_palette;
           new_palette->item_position = tmp_palette->item_position + 1;

           /* Now continue incrementing through the list increasing the position
              count of all items following the new entry */
           tmp_palette = new_palette->next;
           while( tmp_palette != NULL)
           {
              tmp_palette->item_position += 1;
              tmp_palette = tmp_palette->next;
           }
       }
    }

  /* done with filename so XtFree it */
   XtFree(filename);

}
/***************************************************************************
 *
 * WriteOutPalette -
 * This routine writes out a file (palette) to the users .dt/palettes
 * directory. The palette name is passed into the routine. 
 *
 ***************************************************************************/
int 
WriteOutPalette(
        char *paletteName )
{
   char *temp, *tmpStr;
   char *filename;
   int  i, fd;

  /* use the $HOME environment varible then constuct the full file name */
   filename = (char *)XtMalloc(strlen(style.home) + 
                               strlen(DT_PAL_DIR) +
                               strlen(paletteName) + 
                               strlen(PALETTE_SUFFIX) + 2);

  /* create the full path name plus file name */
   strcpy(filename, style.home);
   strcat(filename, DT_PAL_DIR);
   strcat(filename, paletteName);
   strcat(filename, PALETTE_SUFFIX);

   /* open the file for writing */
#if defined(linux)
   if( (fd = open( filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
#else
   if( (fd = open( filename, O_RDWR | O_CREAT)) == -1)
#endif
   {
       tmpStr = (char *)XtMalloc(strlen(ERR4) + strlen(filename) + 1);
       sprintf (tmpStr, ERR4, filename);
       _DtSimpleErrnoError (progName, DtWarning, NULL, tmpStr, NULL);
       XtFree(tmpStr);
       XtFree(filename);
       return(-1);
   }
   else
   {
       temp = (char *)XtMalloc(60);
       for(i = 0; i < XmCO_MAX_NUM_COLORS; i++)
       {
        /* put the colors of the palette in the form #RRRRGGGGBBBB */   
           if(TypeOfMonitor == XmCO_LOW_COLOR && (i == 0 || i == 1))
               sprintf(temp,"#%04x%04x%04x\n", saved_color[i].red, 
                                               saved_color[i].green, 
                                               saved_color[i].blue);
           else if(TypeOfMonitor == XmCO_LOW_COLOR && i == 2)
               sprintf(temp,"#%04x%04x%04x\n",
                                   pCurrentPalette->color[1].bg.red,
                                   pCurrentPalette->color[1].bg.green,
                                   pCurrentPalette->color[1].bg.blue);
           else if(TypeOfMonitor == XmCO_LOW_COLOR && i == 3)
               sprintf(temp,"#%04x%04x%04x\n",
                                   pCurrentPalette->color[0].bg.red,
                                   pCurrentPalette->color[0].bg.green,
                                   pCurrentPalette->color[0].bg.blue);
           else
               sprintf(temp,"#%04x%04x%04x\n",
                                   pCurrentPalette->color[i].bg.red,
                                   pCurrentPalette->color[i].bg.green,
                                   pCurrentPalette->color[i].bg.blue);
           write(fd, temp, strlen(temp));
       }
       XtFree(temp);
   }
   close(fd);
   /* make sure the file is read/writable */
   chmod(filename,438);
   XtFree(filename);
   return(0);
}

/***************************************************************************
 *
 * WriteOutDesc -
 * This routine writes out the description file for the added palette in the 
 * user's .dt/palettes directory.
 ***************************************************************************/
int 
WriteOutDesc(
        palette *tmp_palette )
{
   char *desc_file;
   char *desc_name;

   /* write the description resource to the user's home data base and write out */
   /* the database to the user's palatte directory */
 
   desc_name = (char *) XtMalloc(strlen("Palettes*") + 
				 strlen(tmp_palette->name) + 
				 strlen(".desc") + 1);
   
   sprintf(desc_name, "Palettes*%s.desc", tmp_palette->name);
   
   XrmPutStringResource(&hm_pl_DB, desc_name, tmp_palette->desc); 
    
   /* write out the description file */
   desc_file = (char *) XtMalloc(strlen(style.home) +(strlen("/.dt/palettes/desc.palettes") + 1));
   strcpy (desc_file, style.home);
   strcat (desc_file, "/.dt/palettes/desc.palettes");
   if (hm_pl_DB )
     XrmPutFileDatabase(hm_pl_DB, desc_file);
   /* make the file read, write */
   chmod(desc_file,438);
   XtFree(desc_file);
   XtFree(desc_name);

   return(0);
}

/*
** This routine removes a palette.  
** It actually creates a file in the users home palette directory with 
** the current palette name preceeded by a '~'.
** If the current palette is from the users home palette directory, that
** file is removed.
*/
Boolean 
RemovePalette( void )
{
    int  result;
    char *filename1, *filename2;
    char *tmpStr;

    /* prepend the palette name with '~' */

    filename1 = (char *)XtMalloc(strlen(pCurrentPalette->name) +2); 
    strcpy(filename1, "~");
    strcat(filename1, pCurrentPalette->name);

    if (WriteOutPalette(filename1) != 0)
    {
       tmpStr = (char *)XtMalloc(strlen(ERR1) + 
		       strlen(pCurrentPalette->name) + strlen(ERR2) + 1);
       sprintf(tmpStr, ERR1, pCurrentPalette->name);
       strcat(tmpStr,ERR2);
       ErrDialog (tmpStr, style.colorDialog);
       XtFree (tmpStr);
       XtFree (filename1);
       return(False);
    }

    if (strcmp(pCurrentPalette->directory, home_palette_dir) == 0)
    {
        /* get the $HOME environment varible and constuct the full file name */
        filename2 = (char *)XtMalloc(strlen(style.home) + strlen(DT_PAL_DIR) +
                   strlen(pCurrentPalette->name) + strlen(PALETTE_SUFFIX) + 1);

        /* create the full path name plus file name */
        strcpy(filename2, style.home);
        strcat(filename2, DT_PAL_DIR);
        strcat(filename2, pCurrentPalette->name);
        strcat(filename2, PALETTE_SUFFIX);

        result = unlink(filename2);

	if(result != 0)
	{
	    tmpStr = (char *)XtMalloc(strlen(ERR1) + strlen(filename2) + 1); 
	    sprintf (tmpStr, ERR1, filename2);
	    _DtSimpleErrnoError (progName, DtWarning, NULL, tmpStr, NULL);
	    XtFree(tmpStr);

	    tmpStr = (char *)XtMalloc(strlen(ERR1) +
                        strlen(pCurrentPalette->name) + strlen(ERR2) + 1);
	    sprintf(tmpStr, ERR1, pCurrentPalette->name);
            strcat(tmpStr, ERR2);
	    ErrDialog (tmpStr, style.colorDialog);
	    XtFree (tmpStr);
            XtFree(filename1);
            XtFree(filename2);
            return(False);
	}

        XtFree(filename2);
    }

    XtFree(filename1);
    return(True);

}

/************************************************************************
 *
 * AddSystemPath - adds the system palettes to the directory structure.
 *
 ************************************************************************/
void 
AddSystemPath( void )
{
  if(style.count > 4)
    return;
  
  /*
   ** Null out the head of the palette linked list
   */
  pHeadPalette = NULL;
  
  /* set the number of directories to search to 0, this is the first path
     added  */
  palette_dirs.NumOfDirectories = 0;
  
  /* store the system PALETTE_DIR in the palette_dirs structure */
  palette_dirs.directories[palette_dirs.NumOfDirectories] = 
    (char *) XtMalloc(strlen(PALETTE_DIR) + 1);
  strcpy(palette_dirs.directories[palette_dirs.NumOfDirectories], 
	 PALETTE_DIR);
  
  /* increment the Number of directories to search */
  palette_dirs.NumOfDirectories++;

  style.count++;
  
  /* store the admin PALETTE_DIR_ADM in the palette_dirs structure */
  palette_dirs.directories[palette_dirs.NumOfDirectories] = 
    (char *) XtMalloc(strlen(PALETTE_DIR_ADM) + 1);
  strcpy(palette_dirs.directories[palette_dirs.NumOfDirectories], 
	 PALETTE_DIR_ADM);
  
  /* increment the Number of directories to search */
  palette_dirs.NumOfDirectories++;
  
}

/************************************************************************
 *
 * AddHomePath - adds the users home directory to the directory structure.
 *
 ************************************************************************/
void 
AddHomePath( void )
{
   int  result;
   char *tmpStr;
   
   if(style.count > 6)
      return;

  /* get the $HOME environment varible and constuct the full path */
   home_palette_dir = (char *)XtMalloc(strlen(style.home) 
        + strlen(DT_PAL_DIR) + 1);

  /* create the full path name plus file name */
   strcpy(home_palette_dir, style.home);
   strcat(home_palette_dir, DT_PAL_DIR);

  /* open $HOME/.dt/palettes */
   if( (dirp = opendir(home_palette_dir)) == NULL)
   {
       /*  create .dt/palettes if it does not exit */
       result = mkdir(home_palette_dir, 511);
       if(result == -1)
       {
           tmpStr = (char *)XtMalloc(strlen(ERR3) + 
                strlen(home_palette_dir) +1);
           sprintf(tmpStr, ERR4, home_palette_dir);
           _DtSimpleError (progName, DtWarning, NULL, tmpStr, NULL);
           XtFree(tmpStr);
       }
   }
                                         
  /* store the directory in the palette_dirs structure */
   palette_dirs.directories[palette_dirs.NumOfDirectories] = 
                  (char *) XtMalloc(strlen(home_palette_dir) + 1);
   strcpy(palette_dirs.directories[palette_dirs.NumOfDirectories], 
                  home_palette_dir);
  /* increment the Number of directories to search */
   palette_dirs.NumOfDirectories++;

   style.count++;
}

/************************************************************************
 *
 * AddDirectories - adds the directories listed in the string text
 *     to the list of directories which must be checked for valid
 *     palette files.
 *
 ************************************************************************/
void 
AddDirectories( char *text )
{
   char *filename, *tmpStr;
   int i, count;

   if(style.count > 5)
      return;

  /* alloc enough space for temp length */
   filename = (char *)XtMalloc(strlen(text) + 1);

   count = 0;
   while(1)
   {
     /* copy directory name into the  name .. look for NULL or space */
      for(i=count; text[i] != ':' && text[i] != '\0'; i++)
          filename[i - count] = text[i];
      filename[i - count] = '\0';

     /* store the directory in the palette_dirs structure */
      palette_dirs.directories[palette_dirs.NumOfDirectories] = 
                   (char *)XtMalloc(strlen(filename) + 1);
      strcpy(palette_dirs.directories[palette_dirs.NumOfDirectories], 
                   filename);

     /* increment number of directories in the structure */
      palette_dirs.NumOfDirectories++;

     /* if the number of directories read is the maximum number of 
         directories then break */
      if (palette_dirs.NumOfDirectories == MAX_NUM_DIRECTORIES-1)
      {
          tmpStr = (char *)XtMalloc(strlen(ERR9) + 5);
          sprintf(tmpStr, ERR9, MAX_NUM_DIRECTORIES-2);
          _DtSimpleError (progName, DtWarning, NULL, tmpStr, NULL);
          XtFree(tmpStr);
          break;
      }

     /* if the last value was a NULL then all done,  Break out */
      if (text[i] == '\0')
         break;

      count = i + 1;

   } /* while(1) */

   XtFree(filename);
   style.count++;
}

/********************************************************************
 * 
 * CheckForDeletedFile - This routine looks through the linked list
 *     of palettes for the file name passed in.  If it finds it, that
 *     member of the linked list must be deleted. 
 *
 ********************************************************************/
void
CheckForDeletedFile(
#ifdef __hpux
        struct direct *file_descpt )
#else
        struct dirent *file_descpt )
#endif /* __hpux */
{
   char *filename;
   int i;
   palette *tmp_palette;

  /* alloc enough space for the name of the ~ file  */
   filename = (char *)XtMalloc(strlen (file_descpt->d_name) + 1);

  /* copy the name into filename taking out the ~ */
   for(i=0; i < strlen (file_descpt->d_name) - (strlen(PALETTE_SUFFIX) + 1); i++)
      filename[i] = file_descpt->d_name[i+1];
   filename[i] = '\0';

  /* now go see if the filename is in the palette link list */
   tmp_palette = pHeadPalette;
   while(tmp_palette->next != NULL )
     if(strcmp(tmp_palette->name, filename)) 
        tmp_palette = tmp_palette->next;
     else
        tmp_palette = RemovePaletteFromList(tmp_palette);

   /* check the last palette */
   if(tmp_palette->next == NULL && (strcmp(tmp_palette->name, filename) == 0))
      (void) RemovePaletteFromList(tmp_palette);

   XtFree(filename);
}

/****************************************************************************
 *
 * ReadInInfo - the routine used to actual parse the data from a palette
 *       file.  It parses 8 different background colors.  
 *
 ****************************************************************************/
static void
ReadInInfo(
        char *buf,
        int nbytes,
        palette *new_palette )
{
   int buf_count, count, result;
   char tmpbuf[50];
   XColor tmp_color;
   int  hue, sat, val;
   int num_of_colors = 0;

   for( buf_count = 0; buf_count < nbytes;)
   {
       count = 0;
   
     /* read in a background color */
       while( buf[buf_count] != '\012' && buf[buf_count] != ':' )
          tmpbuf[count++] = buf[buf_count++];
       tmpbuf[count] = '\0';

     /* temporary for now until all palettes are updated not to use : */
       if(buf[buf_count] == ':')
          while(buf[buf_count] != '\012')
             buf_count++;
       buf_count++;
   
      /* Parse the background color */
       result = XParseColor(style.display, style.colormap, tmpbuf, &tmp_color);

      /* if the result == 0 then the parse came back bad, uses the motif
         default background (#729FFF) */
       if(result == 0)
       {
          new_palette->color[num_of_colors].bg.red = 29184;
          new_palette->color[num_of_colors].bg.green = 40704;
          new_palette->color[num_of_colors].bg.blue = 65280;
       }
       else
       {
          new_palette->color[num_of_colors].bg.red = tmp_color.red;
          new_palette->color[num_of_colors].bg.green = tmp_color.green;
          new_palette->color[num_of_colors].bg.blue = tmp_color.blue;
       }
   
      /* use the motif routine to generate fg, sc, ts, and bs */
       if(TypeOfMonitor != XmCO_BLACK_WHITE) {
          if (edit.calcRGB == NULL) edit.calcRGB = XmGetColorCalculation();
          (*edit.calcRGB)(&tmp_color, 
                                  &(new_palette->color[num_of_colors].fg),
                                  &(new_palette->color[num_of_colors].sc),
                                  &(new_palette->color[num_of_colors].ts),
                                  &(new_palette->color[num_of_colors].bs));

           RGBtoHSV(new_palette->color[num_of_colors].bg.red,
                      new_palette->color[num_of_colors].bg.green,
                      new_palette->color[num_of_colors].bg.blue,
                      &hue, &sat, &val);
   

          SetColors(num_of_colors, new_palette);
       }
       else /* XmCO_BLACK_WHITE */
          InitializeBW(tmp_color.red, num_of_colors, new_palette);

       new_palette->num_of_colors++;
       num_of_colors++;
       if (num_of_colors == 8)
            break;
   
   } /* for */

  /* have now read in complete palette file .. set number of colors 
     appropriatly because every palette has 8 entries */
   if(TypeOfMonitor == XmCO_LOW_COLOR || TypeOfMonitor == XmCO_BLACK_WHITE)
       new_palette->num_of_colors = 2;
   
   if(TypeOfMonitor == XmCO_MEDIUM_COLOR)
       new_palette->num_of_colors = 4;

   if(TypeOfMonitor == XmCO_LOW_COLOR)
   {
      SwitchAItoPS(new_palette);
   }
} 

/***************************************************************************
 *
 * SetColors - 
 *       It sets bs and ts if Pixmaps are to be used for topshadow 
 *       and bottomshadow.
 *
 ***************************************************************************/
static void 
SetColors(
        int num_of_colors,
        palette *new_palette )
{

/*
**  Set the foreground pixel to either black or white depending on brightness
*/
   if (FgColor != DYNAMIC) 
   {
      if (FgColor == BLACK)
      {
         new_palette->color[num_of_colors].fg.red = 0;
         new_palette->color[num_of_colors].fg.blue = 0;
         new_palette->color[num_of_colors].fg.green = 0;
      }
      else /* WHITE */
      {
         new_palette->color[num_of_colors].fg.red = 65535;
         new_palette->color[num_of_colors].fg.blue = 65535;
         new_palette->color[num_of_colors].fg.green = 65535;
      }
   }


/*
** Now set ts and bs if the user is using pixmaps
*/
   if(UsePixmaps != FALSE)
   {
       new_palette->color[num_of_colors].ts.red = 65535; 
       new_palette->color[num_of_colors].ts.blue = 65535;
       new_palette->color[num_of_colors].ts.green = 65535;
   
       new_palette->color[num_of_colors].bs.red = 0;
       new_palette->color[num_of_colors].bs.blue = 0;
       new_palette->color[num_of_colors].bs.green = 0;
   }

}

/***************************************************************************
 *
 * InitializeBW - initializes the RGB values for the WhiteOnBlack and
 *        BlackOnWhite palettes.  The color passed in is used to determine
 *        if its White on Black or Black on White.
 *
 ***************************************************************************/
static void
InitializeBW(
        unsigned long color,
        int num_of_colors,
        palette *new_palette )
{
  /* if color passed in is black(background) its white on black so
     set everything to white */
   if(color == 0)
   {
       new_palette->color[num_of_colors].fg.red = 65535; 
       new_palette->color[num_of_colors].fg.blue = 65535;
       new_palette->color[num_of_colors].fg.green = 65535;
   
       new_palette->color[num_of_colors].sc.red = 65535;
       new_palette->color[num_of_colors].sc.blue = 65535;
       new_palette->color[num_of_colors].sc.green = 65535;
   
       new_palette->color[num_of_colors].ts.red = 0; 
       new_palette->color[num_of_colors].ts.blue = 0;
       new_palette->color[num_of_colors].ts.green = 0;
      
       new_palette->color[num_of_colors].bs.red = 65535;
       new_palette->color[num_of_colors].bs.blue = 65535;
       new_palette->color[num_of_colors].bs.green = 65535;
   }
   else
   {
       new_palette->color[num_of_colors].fg.red = 0; 
       new_palette->color[num_of_colors].fg.blue = 0;
       new_palette->color[num_of_colors].fg.green = 0;
   
       new_palette->color[num_of_colors].sc.red = 65535;
       new_palette->color[num_of_colors].sc.blue = 65535;
       new_palette->color[num_of_colors].sc.green = 65535;
   
       new_palette->color[num_of_colors].ts.red = 0; 
       new_palette->color[num_of_colors].ts.blue = 0;
       new_palette->color[num_of_colors].ts.green = 0;
   
       new_palette->color[num_of_colors].bs.red = 0;
       new_palette->color[num_of_colors].bs.blue = 0;
       new_palette->color[num_of_colors].bs.green = 0;
   }
}

/****************************************************************************
 *
 * SwitchAItoPS - used to in a XmCO_LOW_COLOR system to switch the Acitive and
 *    Inactive colorsets to use the Primary and Secondary colorsets.  It
 *    was determined that this would look much better to the user.
 *
 **************************************************************************/
static void 
SwitchAItoPS( palette *new_palette )
{
   saved_color[0].red = new_palette->color[0].bg.red;
   saved_color[0].green = new_palette->color[0].bg.green;
   saved_color[0].blue = new_palette->color[0].bg.blue;

   saved_color[1].red = new_palette->color[1].bg.red;
   saved_color[1].green = new_palette->color[1].bg.green;
   saved_color[1].blue = new_palette->color[1].bg.blue;

   new_palette->color[0].bg.red = new_palette->color[3].bg.red;
   new_palette->color[0].bg.green = new_palette->color[3].bg.green;
   new_palette->color[0].bg.blue = new_palette->color[3].bg.blue;

   new_palette->color[0].fg.red = new_palette->color[3].fg.red;
   new_palette->color[0].fg.green = new_palette->color[3].fg.green;
   new_palette->color[0].fg.blue = new_palette->color[3].fg.blue;

   new_palette->color[0].ts.red = new_palette->color[3].ts.red;
   new_palette->color[0].ts.green = new_palette->color[3].ts.green;
   new_palette->color[0].ts.blue = new_palette->color[3].ts.blue;

   new_palette->color[0].bs.red = new_palette->color[3].bs.red;
   new_palette->color[0].bs.green = new_palette->color[3].bs.green;
   new_palette->color[0].bs.blue = new_palette->color[3].bs.blue;

   new_palette->color[0].sc.red = new_palette->color[3].sc.red;
   new_palette->color[0].sc.green = new_palette->color[3].sc.green;
   new_palette->color[0].sc.blue = new_palette->color[3].sc.blue;

   new_palette->color[1].bg.red = new_palette->color[2].bg.red;
   new_palette->color[1].bg.green = new_palette->color[2].bg.green;
   new_palette->color[1].bg.blue = new_palette->color[2].bg.blue;

   new_palette->color[1].fg.red = new_palette->color[2].fg.red;
   new_palette->color[1].fg.green = new_palette->color[2].fg.green;
   new_palette->color[1].fg.blue = new_palette->color[2].fg.blue;

   new_palette->color[1].ts.red = new_palette->color[2].ts.red;
   new_palette->color[1].ts.green = new_palette->color[2].ts.green;
   new_palette->color[1].ts.blue = new_palette->color[2].ts.blue;

   new_palette->color[1].bs.red = new_palette->color[2].bs.red;
   new_palette->color[1].bs.green = new_palette->color[2].bs.green;
   new_palette->color[1].bs.blue = new_palette->color[2].bs.blue;

   new_palette->color[1].sc.red = new_palette->color[2].sc.red;
   new_palette->color[1].sc.green = new_palette->color[2].sc.green;
   new_palette->color[1].sc.blue = new_palette->color[2].sc.blue;

}

/****************************************************************************
 *
 * RemovePaletteFromList - used to remove the palette pointed to by 
 *    tmp_palette from the linked list of palettes.  The returned palette
 *    is the palette immediately before the deleted palette 
 *
 **************************************************************************/
static palette * 
RemovePaletteFromList( palette *tmp_palette )
{ 
   palette *tmp2_palette; 
   int i, count;

 /* it is in here ... get rid of this entry in the link list */
   if (tmp_palette->item_position == 1)
   {
       pHeadPalette = tmp_palette->next;
       tmp2_palette = pHeadPalette;
   }
   else
   {
     /* find palette just above the one being deleted */
       tmp2_palette = pHeadPalette;
       for (i=1; i < tmp_palette->item_position-1; i++)
           tmp2_palette = tmp2_palette->next;

      /* set the palette before's next to the one being deleted next */
       tmp2_palette->next = tmp_palette->next;
   }

    /* deallocate the palette structure */
   XtFree(tmp_palette->name);
   XtFree(tmp_palette->directory);
   XtFree((char *)tmp_palette);

     /* make sure the item_positions are correct*/
   count = 1;
   tmp_palette = pHeadPalette;
   while(tmp_palette->next != NULL )
   {
      tmp_palette->item_position = count++;
      tmp_palette = tmp_palette->next;
   }
   tmp_palette->item_position = count;

  /* decrease the number of palettes */
   NumOfPalettes--;

  return(tmp2_palette);
}

/*************************************************************************
 *
 * CheckFileType() - Determine if the system supports long file names
 *   or not .. set a flag accoringly.
 *
 ***********************************************************************/
void
CheckFileType( void )
{
   char *filename1 = "abcdefghijklmno";
   char *filename2 = "abcdefghijklmn";
   int  fd, fd1;
   char *tmpfile1, *tmpfile2;
 
  /* get the $HOME environment varible and constuct the full file name */
   tmpfile1 = (char *)XtMalloc(strlen(style.home) + strlen(DT_PAL_DIR) +
                                                     strlen(filename1) + 2);

  /* create the full path name plus file name */
   strcpy(tmpfile1, style.home);
   strcat(tmpfile1, DT_PAL_DIR);
   strcat(tmpfile1, filename1);

#if defined(linux)
   if( (fd = open( tmpfile1, O_CREAT, S_IRUSR | S_IWUSR)) == -1)
#else
   if( (fd = open( tmpfile1, O_CREAT)) == -1)
#endif
   {
        style. longfilename = False;
        XtFree(tmpfile1);
        style.count++;
        return;
   }
  /* get the $HOME environment varible and constuct the full file name */
   tmpfile2 = (char *)XtMalloc(strlen(style.home) + strlen(DT_PAL_DIR) +
                                                     strlen(filename2) + 2);

  /* create the full path name plus file name */
   strcpy(tmpfile2, style.home);
   strcat(tmpfile2, DT_PAL_DIR);
   strcat(tmpfile2, filename2);

#if defined(linux)
   if( (fd1 = open(tmpfile2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1)
#else
   if( (fd1 = open(tmpfile2, O_CREAT | O_EXCL)) == -1)
#endif
   {
       style.longfilename = False; 
   }
   else
   {
       style.longfilename = True;
       unlink(tmpfile2);
   }

   unlink(tmpfile1);
   XtFree(tmpfile1);
   XtFree(tmpfile2);
   close(fd);
   close(fd1);
   
}

Boolean 
ReadPaletteLoop( 
#if NeedWidePrototypes
	int startup )
#else
	Boolean startup )
#endif
{
   char *tmpStr;

   if(style.count > 7)
      return(True);

   /* read all files from all directories */
   while(loopcount<palette_dirs.NumOfDirectories && loopcount<MAX_NUM_DIRECTORIES)
   {
         
/*
** Open the directory .
**   dirp is a pointer to the directory stream
*/
      if( open_dir == False )
      {
         if( (dirp = opendir(palette_dirs.directories[loopcount])) == NULL)
         {
            /* note: if there is an error in opening a directory,
	     * the directory is not deleted from the palette_dirs
	     * structure.
	     */
            loopcount++; 
            if(loopcount < palette_dirs.NumOfDirectories &&
                                          loopcount < MAX_NUM_DIRECTORIES)
               return(False);
            style.count++;
            return(True);
         }
         else {
            open_dir = True;
/*
** get a pointer to the next file entry .. file_descpt
**    the structure for direct looks like:
**       struct direct {
**              long    d_fileno;     #file number of entry
**              short   d_reclen;     #length of this record
**              short   d_namlen;     #length of string in d_name
**              char    d_name[256];  #name 
**            };
*/
             file_descpt = readdir(dirp);
          }
      }

      while( file_descpt != NULL)
      {
       /* check for a valid palette file */
         if(!(strncmp(PALETTE_SUFFIX, DtStrrchr(file_descpt->d_name, '.'),
	                                  strlen(PALETTE_SUFFIX)))) 
         {
/*
** A file in the users home palette directory which begins with a ~
** is a palette that the user doesn't want.  Remove it from
** the palette link list.
*/
             if((strcmp(palette_dirs.directories[loopcount],
		                              home_palette_dir) == 0) && 
	                  (strncmp(file_descpt->d_name, "~", 1) == 0) &&
		           pHeadPalette != NULL)
             {
                CheckForDeletedFile(file_descpt);
             }
             else if (strncmp(file_descpt->d_name, "~", 1) != 0) 
	     {
	       /* make sure if Black and white monitor the file is one that
	          can be read for a XmCO_BLACK_WHITE monitor */
             if(TypeOfMonitor != XmCO_BLACK_WHITE ) 
	     { 
	        if((fnmatch(file_descpt->d_name, B_O_W, 0) != 0) &&
			(fnmatch(file_descpt->d_name, W_O_B, 0)  !=0 ) &&
			(fnmatch(file_descpt->d_name, W_ONLY, 0) !=0 ) &&
			(fnmatch(file_descpt->d_name, B_ONLY, 0) !=0 ))
	           ReadPalette(palette_dirs.directories[loopcount], 
					 file_descpt->d_name,
					 strlen (file_descpt->d_name));
	      }
	      else
	      {
	         if((fnmatch(file_descpt->d_name, B_O_W, 0) == 0) ||
			(fnmatch(file_descpt->d_name, W_O_B, 0)  == 0) ||
			(fnmatch(file_descpt->d_name, W_ONLY, 0) == 0) ||
			(fnmatch(file_descpt->d_name, B_ONLY, 0) == 0))
	            ReadPalette(palette_dirs.directories[loopcount], 
					 file_descpt->d_name,
					 strlen (file_descpt->d_name));
	       }
	   } 

      }  /* if valid palette file */

     /* read the next file */
      file_descpt = readdir(dirp);

      if(startup == True)
         return(False);

      } /* while( file_descpt != NULL) */

      closedir(dirp);
      open_dir = False;

      loopcount++;

   } /* while */

   style.count++;
   return(True);

}
