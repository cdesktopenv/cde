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
static char rcsid[] =
  "$XConsortium: SrvFile_io.c /main/8 1996/09/25 09:41:53 barstow $";
*/
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

/*****************************************************************************
 *****************************************************************************
 **
 **  File:        SrvFile_io.c
 **
 **  Project:     HP DT Style Manager (integrated into dtsession)
 **
 **  Description:
 **  -----------
 **       This file initializes the user specified ( or default) palette
 **       for this session.
 **
 *******************************************************************
 **  (c) Copyright Hewlett-Packard Company, 1990.  All rights are  
 **  reserved.  Copying or other reproduction of this program      
 **  except for archival purposes is prohibited without prior      
 **  written consent of Hewlett-Packard Company.		     
 ********************************************************************
 **
 **
 **
 *****************************************************************************
 *************************************<+>*************************************/
/* #includes */

#include <fcntl.h>
#include "Srv.h"
#include "SrvFile_io.h"

/* #defines */

#define BUFSIZE              1024

#define MSG2  ((char *)GETMESSAGE(28, 3, "Color Server Warning, the size of file is invalid: ")) 
#define MSG3  ((char *)GETMESSAGE(28, 4, "Color Server Warning, invalid information in '"))
#define MSG3a ((char *)GETMESSAGE(28, 5, "' removing file and starting again.\n"))

/* Static Function Declarations */


static Boolean FindPalette( 
                        char *palette,
                        char *directory) ;
static struct _palette* ReadPaletteFile( 
                        Display *dpy,
                        int     screen_number,
                        char *palettePath,
                        char *palette) ;
static int ParsePaletteInfo( 
                        Display *dpy,
                        int numDisplay,
                        char *buf,
                        int nbytes,
                        struct _palette *new_palette) ;
static void InitializeBW( 
                        unsigned long color,
                        int num_of_colors,
                        struct _palette *new_palette) ;

/* static variables */

static int error_value;

#define HOME               "HOME"
#define USER_PALETTE_DIR   "/.dt/palettes"
#define SYSTEM_PALETTE_DIR CDE_INSTALLATION_TOP "/palettes"
#define CONFIG_PALETTE_DIR CDE_CONFIGURATION_TOP "/palettes"
#define DEFAULT_PALETTE    "Default.dp"


/************************************************************************
**
** GetPaletteDefinition -
**  Query the database for the Dtstyle.paletteDirectories resource
**  Use Dtstyle default (Default) if not specified.
**  Search the directories for the palette in reverse order
**
************************************************************************/
struct _palette * 
GetPaletteDefinition( 
Display *dpy,
int     screen_number,
char    *palette)

{
    struct _palette *paletteDef;
    char *str_type_return;
    XrmValue value_return;
    XrmDatabase db;
    Boolean match = False;
    char *palettePath;
    char *path;
    char *p, *d;
    char *home;
    char dir[256];
                    

    if (FindPalette (palette, SYSTEM_PALETTE_DIR))
    {
       /* 
	* First look for palette in the system location
        */
        palettePath = (char *) SRV_MALLOC (strlen(SYSTEM_PALETTE_DIR) + 1 );
        strcpy(palettePath, SYSTEM_PALETTE_DIR);
        match = True;
    }
    else if (FindPalette (palette, CONFIG_PALETTE_DIR))
    {
       /* 
	* Next check the local config location
        */
        palettePath = (char *) SRV_MALLOC (strlen(CONFIG_PALETTE_DIR) + 1 );
        strcpy(palettePath, CONFIG_PALETTE_DIR);
        match = True;
    }
    else 
    {
	palettePath = NULL;
    }

    /*  Get Dtstyle.paletteDirectories value */
    db = XtDatabase(dpy);
    if (XrmGetResource (db, "dtstyle.paletteDirectories",
                            "Dtstyle.PaletteDirectories",
                            &str_type_return, &value_return))
    {
        /* Make Local Copy of string */
        path = (char *) SRV_MALLOC( value_return.size + 1 );
        strcpy (path, value_return.addr);
    }
    else 
    {
        path = NULL;
    }

    /* Look for palette in paletteDirectories */
    if (path != NULL)
    {
        /* Loop through paletteDirectories looking in each directory
         * till we find the palette file. Take first occurrance.
         * Copy directory name into dir.  Look for NULL or space 
         */

        p = path;
        while (*p != '\0')
        {
            d = dir;
            while (*p != ' ' && *p != '\0')
                *d++ = *p++;
            *d = '\0';
            if (FindPalette (palette, dir))
            {
                palettePath = (char *)SRV_REALLOC(palettePath, 
                                                strlen(SYSTEM_PALETTE_DIR) + 1);
                strcpy(palettePath, dir);
                match = True;
                break;
            }
        }
    }
        
    /* Look for palette in $HOME/.dt/palettes */
    /* If there is a duplicate, take it */

    if ((home=getenv(HOME)) == NULL)
      home="";
    path = (char *) SRV_REALLOC (path, 
        strlen(home) + strlen(USER_PALETTE_DIR) + 1);
    strcpy(path, home);
    strcat(path, USER_PALETTE_DIR);

    if (FindPalette (palette, path))
    {
        palettePath = (char *) SRV_REALLOC (palettePath, strlen(path) + 1 );
        strcpy(palettePath, path);
        match = True;
    }
    
    if (match)
    {
        /* Parse the data from the palette file */
        paletteDef = (struct _palette *) ReadPaletteFile(dpy, 
                                                         screen_number, 
                                                         palettePath,
                                                         palette);
    } 
    else /* default to system Default */
    {
        palettePath = (char *) SRV_REALLOC (palettePath,
                                            strlen(SYSTEM_PALETTE_DIR) + 1);
        strcpy(palettePath, SYSTEM_PALETTE_DIR);
        paletteDef = (struct _palette *)ReadPaletteFile(dpy, 
                                                        screen_number, 
                                                        palettePath,
                                                        DEFAULT_PALETTE);
    }

    SRV_FREE(path);
    SRV_FREE(palettePath);

    return (paletteDef);

}

/************************************************************************
**
**  FindPalette -
**  Open the directory and look for the palette file.
**  If found, read in data and return true.
**  If not found, return false
**
************************************************************************/
static Boolean
FindPalette( 
char *palette,
char *directory)

{
    DIR  *dirp;
    struct dirent *file_descpt;

    /* Open the directory */
    if( (dirp = opendir(directory)) == NULL)
    {
       return(False);
    }
    else
    {
        file_descpt = readdir(dirp);
    }

    /* cycle through the files in the directory until found a match */
    while( file_descpt != NULL)
    {
        /* check for a palette filename match */
        if (strcmp(palette, file_descpt->d_name) == 0)
        {
            closedir(dirp);
            return(True);
        }
        else 
        {
            /* read the next file */
            file_descpt = readdir(dirp);
        }
    } /* while( file_descpt != NULL) */

    closedir(dirp);
    return (False);
}

/***************************************************************************
 *
 * ReadPaletteFile - this routines reads and parses the palette file.
 *    It fills in the pCurrentPalette structure for the screen_number
 *    that was passed in. 
 *
 *************************************************************************/
static struct _palette * 
ReadPaletteFile(
        Display *dpy,
        int     screen_number,
        char *palettePath,
        char *palette )
{
   struct _palette *new_palette;
   int             fd, nbytes;
   char            buf[BUFSIZE];
   char            tmpPalette[BUFSIZE];
   char            *token1;
   char            *tmpStr2;
   char            *tmpStr;
   char            *fullPath;

   /* 
    * create full path, don't forget to add 1 for the NULL byte
    * and 1 for the slash in the format string for sprintf.
    */
   fullPath = (char *) SRV_MALLOC (strlen(palettePath) + strlen(palette) + 2);
   sprintf(fullPath,"%s/%s", palettePath, palette); 

   if( (fd = open( fullPath, O_RDONLY)) == -1)
   { /* open failed */
        printf("error opening %s\n",fullPath);
        SRV_FREE(fullPath);
        return((struct _palette *) NULL);
   }
      
   /*
   **  Read a buffer of data ... BUFSIZE == 1024, as long as 
   **  screen < MAX_NUM_SCREENS (5) then we should never need more. 
   */
   nbytes = read(fd, buf, BUFSIZE);
   if(nbytes == 0 || nbytes == BUFSIZE)  /* A bogus number of bytes */
   { /* read failed */
       /*
	* Don't forget to add 1 for the NULL byte and 2 for the 
	* period and the newline in the format string for sprintf
	*/
       tmpStr = (char *)SRV_MALLOC(strlen(MSG2) + strlen(fullPath) + 3);
       sprintf(tmpStr,"%s%s.\n", MSG2, palettePath); 
       _DtSimpleError(XmSCOLOR_SRV_NAME, DtWarning, NULL, tmpStr, NULL); 
       SRV_FREE(tmpStr);
       SRV_FREE(fullPath);
       close(fd);
       return((struct _palette *) NULL);
   }
   else 
   { /* read suceeded */
      /* Allocate space for this new palette. */
      new_palette = (palettes *)SRV_MALLOC( sizeof(struct _palette) + 1 );

      /*  allocate enough space for the name */
      strcpy(tmpPalette, palette); 
      for (token1=tmpPalette; *token1; token1++);
      while (token1!=tmpPalette && *token1!='.') token1--;
      if (!strcmp(token1,PALETTE_SUFFIX)) *token1 = '\0';
      new_palette->name = (char *)SRV_MALLOC(strlen(tmpPalette) + 1);
      strcpy(new_palette->name, (char *) tmpPalette);

      new_palette->converted = NULL;
      new_palette->converted_len = 0L;

      /* set the next pointer to NULL*/
      new_palette->next = NULL;

      if (ParsePaletteInfo(dpy, screen_number, buf, nbytes, new_palette) == -1)
      {
        /* palette file is bad */
         tmpStr = (char *)SRV_MALLOC(strlen(MSG3) + strlen(MSG3a) + strlen(palettePath) +1);
         tmpStr2 = (char *)SRV_MALLOC(strlen(MSG3)+1);
         sprintf(tmpStr2,"%s", MSG3); 
         sprintf(tmpStr,"%s%s%s", tmpStr2, palettePath, MSG3a); 
         _DtSimpleError(XmSCOLOR_SRV_NAME, DtWarning, NULL, tmpStr, NULL); 
         SRV_FREE(tmpStr);
         SRV_FREE(tmpStr2);
         error_value = 1;
         unlink(palettePath);
         SRV_FREE(fullPath);
         close(fd);
         return((struct _palette *) NULL);
      }
   }

   /* close the file */
   close(fd);
   SRV_FREE(fullPath);

   return(new_palette);
}

/***********************************************************************
 *
 * ParsePaletteInfo - This routine reads from the buffer(buf) the 
 *      actual data into the new_palette.  It reads in the bg colors
 *      then uses the XmCalculateColorRGB to generate the ts, bs, and sc 
 *      colors.  This routine doesn't allocate any pixel numbers but
 *      does generate the RGB values for each color in a palette.
 *
 ***********************************************************************/
static int 
ParsePaletteInfo(
        Display *dpy,
        int screen_num,
        char *buf,
        int nbytes,
        struct _palette *new_palette )
{
   char            tmpbuf[BUFSIZE];
   int             count;
   int             num_of_colors;
   int             result;
   XColor          tmp_color;
   int             buf_count;
   static XmColorProc   calcRGB = NULL;

   num_of_colors = new_palette->num_of_colors = 0;

   if(colorSrv.TypeOfMonitor[screen_num] != XmCO_BLACK_WHITE)
      if((strcmp(new_palette->name, W_O_B)) == 0 || 
                            (strcmp(new_palette->name, B_O_W)) == 0  ||
                            (strcmp(new_palette->name, W_ONLY)) == 0 ||
                            (strcmp(new_palette->name, B_ONLY)) == 0) 
         return(-1);

   buf_count = 0;
  /* read in background colors until end of file */
   while(buf_count < nbytes && buf[buf_count] != '!')
   {
      count = 0;

     /* read in a BG color */
      while(buf[buf_count] != '\012')
         tmpbuf[count++] = buf[buf_count++];
      tmpbuf[count++] = '\0';
      buf_count++;

      /* get the RGB value (XColor) of the background */
      result = XParseColor(dpy, DefaultColormap(dpy, screen_num),
                                                  tmpbuf, &tmp_color);
      /* there was an error */
      if(result == 0)
          return(-1);

      new_palette->color[num_of_colors].bg.red = tmp_color.red;
      new_palette->color[num_of_colors].bg.blue = tmp_color.blue;
      new_palette->color[num_of_colors].bg.green = tmp_color.green;
   
/*
** Now lets generate all the colors which go along with this bg i.e. ts,
** fg, bs, and sc.
*/
      if (calcRGB == NULL) calcRGB = XmGetColorCalculation();
      (*calcRGB)(&tmp_color,
                                &(new_palette->color[num_of_colors].fg),
                                &(new_palette->color[num_of_colors].sc),
                                &(new_palette->color[num_of_colors].ts),
                                &(new_palette->color[num_of_colors].bs));

      if(colorSrv.TypeOfMonitor[screen_num] == XmCO_BLACK_WHITE)
          InitializeBW(tmp_color.red, num_of_colors, new_palette);

      else /* Not Black and White */
      {
         if(colorSrv.UsePixmaps[screen_num] == TRUE)
         {
           /* the values generated by XmCalculateColorRBG are invalid */
            new_palette->color[num_of_colors].ts.red = 65535; 
            new_palette->color[num_of_colors].ts.blue = 65535;
            new_palette->color[num_of_colors].ts.green = 65535;

            new_palette->color[num_of_colors].bs.red = 0;
            new_palette->color[num_of_colors].bs.blue = 0;
            new_palette->color[num_of_colors].bs.green = 0;
         }

         if(colorSrv.FgColor[screen_num] != DYNAMIC)
	 {
             if(colorSrv.FgColor[screen_num] == BLACK)
             {
                new_palette->color[num_of_colors].fg.red = 0;
                new_palette->color[num_of_colors].fg.blue = 0;
                new_palette->color[num_of_colors].fg.green = 0;
             }
             else /* colorSrv.FgColor[screen_num] == WHITE */
             {
                new_palette->color[num_of_colors].fg.red = 65535;
                new_palette->color[num_of_colors].fg.blue = 65535;
                new_palette->color[num_of_colors].fg.green = 65535;
             }
	 }
      } /* else Not Black and White */

      num_of_colors++;
      new_palette->num_of_colors++;
    
      if(colorSrv.TypeOfMonitor[screen_num] == XmCO_BLACK_WHITE)
         if( new_palette->num_of_colors == 2)
           break;

      if(colorSrv.TypeOfMonitor[screen_num] == XmCO_LOW_COLOR || 
	 colorSrv.TypeOfMonitor[screen_num] == XmCO_MEDIUM_COLOR)
         if( new_palette->num_of_colors == 4)
           break;

   } /* while */
   if(colorSrv.TypeOfMonitor[screen_num] == XmCO_LOW_COLOR) 
   {
      new_palette->num_of_colors = 2;
      SwitchAItoPS(new_palette);
   }
   return(0);
}

/***********************************************************************
 *
 * InitializeBW - the type of monitor is a black and white, so
 *      initialize the colors to black and white.  If the color passed
 *      in is 0 (black) set everything to 65535 (white). Otherwise
 *      set everything to 0 (black).
 *
 ***********************************************************************/
static void 
InitializeBW(
        unsigned long color,
        int num_of_colors,
        struct _palette *new_palette )
{
   if(color == 0L)
   {
      new_palette->color[num_of_colors].fg.red = 65535;
      new_palette->color[num_of_colors].fg.blue = 65535;
      new_palette->color[num_of_colors].fg.green = 65535;

      new_palette->color[num_of_colors].ts.red = 65535;
      new_palette->color[num_of_colors].ts.blue = 65535;
      new_palette->color[num_of_colors].ts.green = 65535;

      new_palette->color[num_of_colors].bs.red = 65535;
      new_palette->color[num_of_colors].bs.blue = 65535;
      new_palette->color[num_of_colors].bs.green = 65535;

      new_palette->color[num_of_colors].sc.red = 0;
      new_palette->color[num_of_colors].sc.blue = 0;
      new_palette->color[num_of_colors].sc.green = 0;
   }
   else
   {
      new_palette->color[num_of_colors].fg.red = 0;
      new_palette->color[num_of_colors].fg.blue = 0;
      new_palette->color[num_of_colors].fg.green = 0;

      new_palette->color[num_of_colors].ts.red = 0;
      new_palette->color[num_of_colors].ts.blue = 0;
      new_palette->color[num_of_colors].ts.green = 0;

      new_palette->color[num_of_colors].bs.red = 0;
      new_palette->color[num_of_colors].bs.blue = 0;
      new_palette->color[num_of_colors].bs.green = 0;

      new_palette->color[num_of_colors].sc.red = 65535;
      new_palette->color[num_of_colors].sc.blue = 65535;
      new_palette->color[num_of_colors].sc.green = 65535;
   }
}

void 
SwitchAItoPS(
        struct _palette *new_palette )
{
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

/***********************************************************************
 *
 * SaveDefaultPalette - used to save the palette.dt file to 
 *         either $HOME/.dt/$DISPLAY/current or $HOME/.dt/$DISPLAY/home.
 *         The parameter mode determines whether it is home or
 *         current.  
 *
 ***********************************************************************/
void 
SaveDefaultPalette(
        Display *dpy,
        char *dtPath,
        int mode )
{
}
