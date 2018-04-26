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
/* $XConsortium: parser.h /main/5 1996/03/25 09:20:09 rswiston $ */
/***************************************************************************/
/*                                                                         */
/*  parser.h                                                               */
/*                                                                         */
/***************************************************************************/

/* Field id Definitions for Actiondata Datastructure */
enum {
               DATA_ATTRIBUTES=1 ,
               DATA_CRITERIA     ,
               ACTION_OPEN       ,
               ACTION_PRINT_FTYPE,
               ACTION_PRINT
    };

/* Field id Definitions for Filetypedata Datastructure */
enum {
              ACTION_NAME=1,
              ACTION_CMD   ,
              ACTION_ICO   ,
              ACTION_HELP
    };
/* Tabletype definitions */
#define ACTION_TABLE       1
#define FILETYPE_TABLE     2
#define HELP_TEXT          3

/* Function prototype definitions */
int GetActionData(FILE *, ActionData *);


/* Error code Defines */

#define ACTION_FILEOPEN_ERROR          1
#define ACTION_FILE_MUCKED             2
#define ACTION_DATA_ERROR              3

/***************************************************************************/
/* Platform specifics                                                      */
/***************************************************************************/
#ifdef SVR4
#define bzero(pbuffer, size)  memset(pbuffer, 0, size)
#endif
