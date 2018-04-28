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
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: SmMigResources.c /main/4 1996/05/08 20:11:34 drk $ */
static char sccsid[] = "@(#)48  1.2  src/cde/cde1/dtsession/SmMigResources.c, desktop, cde41J, 9520A_all 5/16/95 08:31:12";
/*
 *   COMPONENT_NAME: desktop
 *
 *   FUNCTIONS: MigrateResources
 *
 *   ORIGINS: 27
 *
 *   IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 *   combined with the aggregated modules for this product)
 *   OBJECT CODE ONLY SOURCE MATERIALS
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#include <stdio.h>
#include <string.h>
#include <Dt/DtNlUtils.h>

/* defines for return codes */
#define RC_SUCCESS                         0
#define RC_OPEN_ERROR                      1
#define RC_END_OF_FILE                     2
#define RC_MEMORY_ALLOCATION_ERROR         3
#define RC_LINE_CONTINUED                  4
#define RC_PARTIAL_LINE                    5
#define RC_WRITE_ERROR_TEMP                6

/* type 1 resources are of the form:
     [*.]convert1[i]

   Example of a resource specification of this type:
     *foreground:                        Blue
*/
#define  NUMBER_OF_CONVERT1 15
static   char *convert1[NUMBER_OF_CONVERT1]
= {
       "displayResolution",
       "systemFont",
       "userFont",
       "FontList",
       "Font",
       "FontSet",
       "multiClickTime",
       "sessionVersion",
       "background",
       "foreground",
       "ColorUse",
       "HelpColorUse",
       "background",
       "foreground",
       "enableBtn1Transfer"
       }
       ;


/* type 2 resources are of the form:
     CONVERT2_0{* or .}CONVERT2_1{* or .}convert2[i]

   Example of a resource specification of this type:
     dtsession*extension*cycleTimeout:   10
*/
#define CONVERT2_0 "dtsession"
#define CONVERT2_1 "extension"
#define  NUMBER_OF_CONVERT2 5
static   char *convert2[NUMBER_OF_CONVERT2]
= {
       "cycleTimeout",
       "lockTimeout",
       "saverTimeout",
       "random",
       "saverList",
       }
       ;


/* type 3 resources are of the form:
     convert31[i]{* or .}convert32[i]

   Example of a resource specification of this type:
     Dtwm*useIconBox:                    True
*/
#define  NUMBER_OF_CONVERT3 26
static   char *convert31[NUMBER_OF_CONVERT3]
= {
       "dtsession",
       "dtsession",
       "dtsession",
       "dtsession",
       "dtsession",
       "dtsession",
       "Dtstyle",
       "Dtstyle",
       "Dtwm",
       "Dtwm",
       "Dtwm",
       "Dtwm",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "wsHelp",
       "cachedHelp"
       }
       ;
static   char *convert32[NUMBER_OF_CONVERT3]
= {
       "displayResolution"  ,
       "sessionLanguage"    ,
       "saverTimeout"       ,
       "cycleTimeout"       ,
       "lockTimeout"        ,
       "saverList"          ,
       "lockoutScale"       ,
       "timeoutScale"       ,
       "keyboardFocusPolicy",
       "focusAutoRaise"     ,
       "moveOpaque"         ,
       "useIconBox"         ,
       "onScreen"           ,
       "x"                  ,
       "y"                  ,
       "columns"            ,
       "rows"               ,
       "helpType"           ,
       "vPCount"            ,
       "tTitle"             ,
       "helpVolume"         ,
       "locationId"         ,
       "stringData"         ,
       "windowGroup"        ,
       "wsName"             ,
       "cachedCount"
       }
       ;

/* type 4 resources are of the form:
     {* or .}convert4[i]{* or .}CONVERT4_2

   Example of a resource specification of this type:
       *XmText*FontList:                   Big
*/
#define CONVERT4_2 "FontList"
#define  NUMBER_OF_CONVERT4 2
static   char *convert4[NUMBER_OF_CONVERT4]
= {
       "XmText",
       "XmTextField"
       }
       ;


/* type 5 resources are of the form:
     convert5[i]{* or .}<any value>{* or .}CONVERT5_2

   Example of a resource specification of this type:
       Dtwm*0*helpResources:             xxx
*/
#define CONVERT5_2 "helpResources"
#define  NUMBER_OF_CONVERT5 2
/* these are of the form p1*#*helpResources */
static   char *convert5[NUMBER_OF_CONVERT5]
= {
       "Dtwm",
       "Mwm"
       }
       ;


/* type 6 resources are of the form:
     CONVERT6_0<any value>{* or .}convert6[i]

   Example of a resource specification of this type:
       oWsHelp10*x:                       xxx
*/
#define CONVERT6_0 "oWsHelp"
#define  NUMBER_OF_CONVERT6 11
static   char *convert6[NUMBER_OF_CONVERT6]
= {
      "x",
      "y",
      "columns",
      "rows",
      "helpType",
      "vPCount",
      "tTitle",
      "helpVolume",
      "locationId",
      "stringData",
      "workspaces"
       }
       ;

/* type 7 resources are of the form:
     CONVERT7_01{* or .}<any value>{* or .}convert7[i]
                     - or -
     CONVERT7_02{* or .}<any value>{* or .}convert7[i]

   Examples of resource specifications of this type:
     Dtwm*0*initialWorkspace:           xxx
     Mwm*0*initialWorkspace:            xxx
*/
#define CONVERT7_01 "Mwm"
#define CONVERT7_02 "Dtwm"
#define  NUMBER_OF_CONVERT7   3
/* these are of the form Mwm|Dtwm*XXX*resource */
static   char *convert7[NUMBER_OF_CONVERT7]
= {
      "initialWorkspace",
      "workspaceList",
      "workspaceCount"
       }
       ;

/* type 8 resources are of the form:
     CONVERT8_01{* or .}<any value>{* or .}<any value>{* or .}convert8[i]
                     - or -
     CONVERT8_02{* or .}<any value>{* or .}<any value>{* or .}convert8[i]

   Examples of resource specifications of this type:
     Mwm*0*ws01*title:                  xxx
     Dtwm*1ws02*title:                  xxx
*/
#define CONVERT8_01 "Mwm"
#define CONVERT8_02 "Dtwm"
#define  NUMBER_OF_CONVERT8  3
static   char *convert8[NUMBER_OF_CONVERT8]
= {
      "title",
      "geometry",
      "iconBoxGeometry"
       }
       ;

/* type 9 resources are of the form:
     CONVERT9_01{* or .}<any value>{* or .}<any value>{* or .}convert91[i] \
             {* or .}convert92[i]
             - or -
     CONVERT9_02{* or .}<any value>{* or .}<any value>{* or .}convert91[i]  \
             {* or .}convert92[i]

   Example of resource specification of this type:
      Dtwm*0*ws01*backdrop*image:        Drops
*/
#define CONVERT9_01 "Mwm"
#define CONVERT9_02 "Dtwm"
#define  NUMBER_OF_CONVERT9  3
static   char *convert91[NUMBER_OF_CONVERT9]
= {
      "backdrop",
      "FrontPanel",
      "MyFrontPanel"
      }
       ;

static   char *convert92[NUMBER_OF_CONVERT9]
= {
      "image" ,
      "geometry",
      "geometry"
      }
       ;



/* type 10 resources are of the form:
     *<any value>{* or .}convert10[i]

   Example of a resource specification of this type:
       *0*ColorPalette:        Default.dp
*/
#define  NUMBER_OF_CONVERT10 3
static   char *convert10[NUMBER_OF_CONVERT10]
= {
       "MonochromePalette",
       "ColorUse",
       "ColorPalette"
       }
       ;


/* define the maximum fields in a resource specification
   (which does not include the value of the resource) that
   is required by this routine
*/
#define NUMBER_OF_FIELDS 6
static   char * field[NUMBER_OF_FIELDS];

/* this function determines whether a field extracted from the resource
   matches a string in the specified array.
   Returns:
      TRUE   = if strings match
      FALSE  = if string do not match.
*/
int check_match1(char * match[], int noelements, int match1)
{
   int i;
   int matched = FALSE;
   for (i=0;((matched == FALSE) &&
             (i<noelements));i++)
   {
      if (strcmp(field[match1],match[i]) == 0)
      {
         matched =TRUE;
      }
   }
   return(matched);
}
/* this function determines whether a fields extracted from the resource
   match strings in the specified arrays.
   Returns:
      TRUE   = if strings match
      FALSE  = if string do not match.
*/
int check_match2(char * match1_str[], char * match2_str[],
                 int noelements, int match1, int match2)
{
   int i;
   int matched = FALSE;
   for (i=0;((matched == FALSE) &&
             (i<noelements));i++)
   {

      if ((strcmp(field[match1],match1_str[i]) == 0) &&
          (strcmp(field[match2],match2_str[i]) == 0))
      {
         matched =TRUE;
      }
   }
   return(matched);
}

/* this function reads all resources from the input file.
   If the resource matches those written from within desktop on 4.1.1 and
   4.1.2, the resource is written to the output file.
   Otherwise, the resource is not written to the output file.

   Return codes:
     RC_SUCCESS
     RC_OPEN_ERROR
     RC_MEMORY_ALLOCATION_ERROR
     RC_WRITE_ERROR_TEMP

*/
MigrateResources(char * inputfile, char * outputfile)
{

   int                           size_of_buffer = 1024;
   int                           size_of_mbuffer = 1024;
   int                           rc = RC_SUCCESS;
   FILE *                        out_fh;
   FILE *                        in_fh;
   int                           write_line;
   int                           chars_written;
   int                           type_found = FALSE;
   char *                        string;
   char *                        realend;
   int                           done=0;
   int                           new_size = 0;
   int                           number_fields = 0;
   char *                        inputbuffer=NULL;
   char *                        inputbuffer1=NULL;
   char *                        unmodified_buffer=NULL;
   char *                        resource_end;
   char *                        first_space;
   char *                        tmp1;
   int                           i;
   int                           charlen;

   /* Initialize for multi-byte */
   DtNlInitialize();
   done = RC_SUCCESS;
   /* open input and output files and exit if not successful */
   in_fh = fopen(inputfile,"r");
   out_fh = fopen(outputfile,"w");
   if ((out_fh != NULL) && (in_fh != NULL))
   {

      /* allocate buffers for reading lines from the input file */
      inputbuffer = malloc ((size_of_buffer + 1) * sizeof(char *));
      inputbuffer1 = malloc ((size_of_buffer + 1) * sizeof(char *));
      unmodified_buffer = malloc ((size_of_mbuffer + 1)
                                    * sizeof(char *));
      if ((inputbuffer != NULL) &&
          (inputbuffer1 != NULL) &&
          (unmodified_buffer != NULL))
      {
          do
          {

             *inputbuffer = '\0';
             *inputbuffer1 = '\0';
             *unmodified_buffer = '\0';
             /* read and process each line from the input file */
             do

             {
                 /* read until an line has been read */
                 rc = RC_SUCCESS;
                 string = fgets((char *)inputbuffer1, size_of_buffer, in_fh);

                 /* if read was successful, then ...*/
                 if (string != NULL)
                 {
                      /* determine if a larger unmodified buffer
                         needs to be allocated
                      */
                      new_size = strlen(unmodified_buffer) +
                                 strlen(inputbuffer1) + 2;

                      if (new_size > size_of_mbuffer)
                      {
                          unmodified_buffer = realloc(unmodified_buffer,
                                                      new_size);
                          size_of_mbuffer = new_size;
                      }

                      /* save unmodified data read */
                      strcat(unmodified_buffer,inputbuffer1);

                      /* set indicator if entire line was not read */
                      DtLastChar(inputbuffer1,&realend,&charlen);
                      if ((charlen == 1) && (*realend != '\n'))
                      {
                          rc = RC_PARTIAL_LINE;
                      }
                      else
                      {
                          /* if entire line was read but it ends
                             with a continuation character
                             then, remove the ending continuation character or
                             spaces preceded by an ending
                             continuation character.
                          */
                          realend=DtPrevChar(inputbuffer1,realend);
                          for (;((DtIsspace(realend) != 0) &&
                                 (realend > inputbuffer1));)
                          {
                              realend=DtPrevChar(inputbuffer1,realend);
                          }
                          if ((mblen(realend,MB_CUR_MAX) == 1) &&
                              (*realend == '\\'))
                          {
                              *realend = '\0';
                              rc = RC_LINE_CONTINUED;
                          }
                      }

                      /* allocate larger input buffer if necessary */
                      new_size = strlen(inputbuffer) +
                                 strlen(inputbuffer1) + 2;

                      if (new_size > size_of_buffer)
                      {
                          inputbuffer = realloc(inputbuffer,new_size);
                          size_of_buffer = new_size;
                      }

                      /* concatenate modified buffer to previously
                         read buffer
                      */
                      strcat(inputbuffer,inputbuffer1);
                 }
                 else
                 {
                      rc = RC_END_OF_FILE;
                 }
             } while ((rc == RC_LINE_CONTINUED) || (rc == RC_PARTIAL_LINE));

             /* if read was successful, then determine if the resources
                read are part of the set that need to be retained.
             */
             type_found = FALSE;
             number_fields = 0;
             type_found=FALSE;
             if (rc == RC_SUCCESS)
             {
                 /* process non-comment lines */
                 if (strncmp(inputbuffer,"!",1) != 0)
                 {
                     /* determine the non-value portion of the
                        resource specification (i.e. the
                        part to the left of the ":" and to the
                        left of the first space).
                     */
                     resource_end = DtStrchr(inputbuffer,':');
                     first_space = DtStrchr(inputbuffer,' ');
                     if (resource_end != NULL)
                     {
                         if ((first_space != NULL) &&
                             (first_space < resource_end))

                         {
                             resource_end = NULL;
                         }
                     }
                     if (resource_end != NULL)
                     {
                         *resource_end = '\0';
                         /* determine the start of each field
                            in the non-value part of the resource.
                            It is assumed that the individual fields
                            are delimited by a "*" or ".".
                         */
                         field[0]=inputbuffer;
                         for (i=1;((i<NUMBER_OF_FIELDS) &&
                                   (field[i-1] != NULL));
                              i++)
                         {
                            /* determine the location of the next
                               delimiter - "* or "."
                               Set field pointer to first one found.
                            */
                            field[i]=DtStrchr(field[i-1],'*');
                            tmp1=DtStrchr(field[i-1],'.');
                            if (((tmp1 != NULL) && (tmp1 < resource_end) &&
                                 (tmp1 < field[i])) ||
                                (field[i] == NULL))
                            {
                                field[i]=tmp1;
                            }
                            if (field[i] != NULL)
                            {
                                /* terminate the previous field */
                                *field[i] = '\0';
                                /* move pointer to start of field */
                                field[i]++;
                            }
                            if (field[i] >= resource_end)
                            {
                                field[i]=NULL;
                            }
                         }
                         number_fields = i-1;
                         /* check for each type of resource
                            that should be retained in the output file.
                            See comments preceding the definition
                            of each array near the start of this file
                            for a description and example of each type.
                         */
                         if ((strlen(field[0]) == 0) &&
                             (number_fields == 2) &&
                             (check_match1(convert1,
                                           NUMBER_OF_CONVERT1,1) == TRUE))

                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 3) &&
                             (strcmp(field[0],CONVERT2_0) == 0) &&
                             (strcmp(field[1],CONVERT2_1) == 0) &&
                             (check_match1(convert2, NUMBER_OF_CONVERT2,2)
                                           == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 2) &&
                             (check_match2(convert31,convert32,
                                 NUMBER_OF_CONVERT3,0,1) == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 3) &&
                             (strcmp(field[2],CONVERT4_2) == 0) &&
                             (check_match1(convert4,
                                           NUMBER_OF_CONVERT4,1) == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 3) &&
                             (strcmp(field[2],CONVERT5_2) == 0) &&
                             (check_match1(convert5,
                                           NUMBER_OF_CONVERT5,0) == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 2) &&
                             (strlen(field[0]) > strlen(CONVERT6_0)) &&
                             (strncmp(field[0], CONVERT6_0,
                                      strlen(CONVERT6_0)) == 0) &&
                             (check_match1(convert6,
                                           NUMBER_OF_CONVERT6,1) == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 3) &&
                             ((strcmp(field[0],CONVERT7_01) == 0) ||
                              (strcmp(field[0],CONVERT7_02) == 0)) &&
                              (check_match1(convert7,
                                 NUMBER_OF_CONVERT7,2) == TRUE))
                         {

                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 4) &&
                             ((strcmp(field[0],CONVERT8_01) == 0) ||
                              (strcmp(field[0],CONVERT8_02) == 0)) &&
                             (check_match1(convert8,
                                           NUMBER_OF_CONVERT8,3) == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 5) &&
                             ((strcmp(field[0],CONVERT9_01) == 0) ||
                              (strcmp(field[0],CONVERT9_02) == 0)) &&
                             (check_match2(convert91,convert92,
                                           NUMBER_OF_CONVERT9,3,4) == TRUE))
                         {
                             type_found=TRUE;
                         }
                         if ((type_found == FALSE) &&
                             (number_fields == 3) &&
                             (check_match1(convert10,
                                           NUMBER_OF_CONVERT10,2) == TRUE))
                         {
                             type_found=TRUE;
                         }
                     }
                 }

                 /* if resource should be retained then
                    write resource to output file.
                 */
                 if (type_found == TRUE)
                 {
                     chars_written = fputs(unmodified_buffer,out_fh);
                     if (chars_written != strlen(unmodified_buffer))
                     {
                         if (rc == RC_SUCCESS)
                         {
                             rc = RC_WRITE_ERROR_TEMP;
                         }
                     }
                 }
             }

          } while (rc == RC_SUCCESS);
      }
      else
      {
          rc = RC_MEMORY_ALLOCATION_ERROR;
      }
      if (rc == RC_END_OF_FILE)
      {
          rc = RC_SUCCESS;
      }

      /* free all allocated buffers */
      if (inputbuffer != NULL)
      {
          free(inputbuffer);
      }
      if (inputbuffer1 != NULL)
      {
          free(inputbuffer1);
      }
      if (unmodified_buffer != NULL)
      {
          free(unmodified_buffer);
      }
   }
   else
   {
       rc = RC_OPEN_ERROR;
   }
   if (in_fh != NULL)
   {
       fclose(in_fh);
   }
   if (out_fh != NULL)
   {
       fclose(out_fh);
   }
   return(rc);
}

