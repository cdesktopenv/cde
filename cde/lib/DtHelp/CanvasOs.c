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
/* $XConsortium: CanvasOs.c /main/5 1996/05/09 03:40:38 drk $ */
/************************************<+>*************************************
 ****************************************************************************
 **
 **   File:	   UtilSDL.c
 **
 **   Project:     Cde Help System
 **
 **   Description: Utility functions for parsing an SDL volume.
 **
 **  (c) Copyright 1987, 1988, 1989, 1990, 1991, 1992 Hewlett-Packard Company
 **
 **  (c) Copyright 1993, 1994 Hewlett-Packard Company
 **  (c) Copyright 1993, 1994 International Business Machines Corp.
 **  (c) Copyright 1993, 1994 Sun Microsystems, Inc.
 **  (c) Copyright 1993, 1994 Novell, Inc.
 **
 ****************************************************************************
 ************************************<+>*************************************/

/*
 * system includes
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/*
 * Canvas Engine includes
 */
#include "CanvasP.h"

/*
 * private includes
 */
#include "bufioI.h"
#include "CanvasOsI.h"
#include "FormatUtilI.h"

#ifdef NLS16
#endif

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/

/******************************************************************************
 *
 * Private defines.
 *
 *****************************************************************************/
/******************************************************************************
 *
 * Private macros.
 *
 *****************************************************************************/
/******************************************************************************
 *
 * Private data.
 *
 *****************************************************************************/
/******************************************************************************
 *
 * Private Functions
 *
 *****************************************************************************/
/******************************************************************************
 *
 * Semi Public Functions
 *
 *****************************************************************************/
/*********************************************************************
 * Function: _DtCvRunInterp
 *
 *    _DtCvRunInterp calls a script and maybe gets data.
 *
 *********************************************************************/
int
_DtCvRunInterp(
    int			(*filter_exec)(),
    _DtCvPointer	  client_data,
    char		 *interp,
    char                 *data,
    char                **ret_data)
{
    int          result;
    int          myFd;
    FILE        *myFile;
    int          size;
    int          writeBufSize = 0;
    char        *writeBuf     = NULL;
    char        *ptr;
    char        *fileName;
    char        *newData;
    char         readBuf[BUFSIZ];
    BufFilePtr   myBufFile;

    /*
     * ask for permission to run the interperator command.
     */
    newData = data;
    if (filter_exec != NULL && (*filter_exec)(client_data,data,&newData) != 0)
	return -1;

    /*
     * open a temporary file to write the data to.
     */
    fileName = tempnam(NULL, NULL);
    if (fileName == NULL)
      {
	if (newData != data)
	    free(newData);
	return -1;
      }

    /*
     * write the data to file.
     */
    result = -1;
#if defined(linux)
    myFd   = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
#else
    myFd   = open(fileName, O_WRONLY | O_CREAT | O_TRUNC);
#endif
    if (myFd != -1)
      {
	/*
	 * write the data to the file
	 */
        result = write(myFd, newData, strlen(newData));

	if (result != -1)
	  {
	    /*
	     * change the access permissions so the interpreter can read
	     * the file.
	     */
	    result = fchmod(myFd, S_IRUSR | S_IRGRP | S_IROTH);
	  }

        /*
         * close the file.
         */
        close(myFd);
      }

    if (newData != data)
	free(newData);

    if (result == -1)
      {
	unlink(fileName);
	free(fileName);
	return -1;
      }

    /*
     * create the system command string with its parameters
     */
    ptr = (char *) malloc(sizeof(interp) + strlen(fileName) + 1);
    if (!ptr)
      {
	unlink(fileName);
	free(fileName);
        return -1;
      }

    strcpy (ptr, interp);
    strcat (ptr, " ");
    strcat (ptr, fileName);

    myFile = popen(ptr, "r");

    /*
     * free the command
     */
    free (ptr);

    /*
     * check for problems
     */
    if (!myFile) /* couldn't create execString process */
      {
	unlink(fileName);
	free(fileName);
        return -1;
      }

    /*
     * create a file handler for the pipe.
     */
    myBufFile = _DtHelpCeCreatePipeBufFile(myFile);
    if (myBufFile == NULL)
      {
        (void) pclose(myFile); /* don't check for error, it was popen'd */
	unlink(fileName);
	free(fileName);
        return -1;
      }

    /*
     * read the file the pipe writes to until the pipe finishes.
     * Create a string from the results
     */
    do {
        readBuf[0] = '\0';
        ptr        = readBuf;

        result = _DtHelpCeGetNxtBuf(myBufFile, readBuf, &ptr, BUFSIZ);

        if (result > 0)
          {
            size = strlen(readBuf);
            if (writeBuf == NULL)
                writeBuf = (char *) malloc (size + 1);
            else
                writeBuf = (char *) realloc (writeBuf, writeBufSize + size + 1);

            if (writeBuf != NULL)
              {
                writeBuf[writeBufSize] = '\0';
                strcat(writeBuf, readBuf);
                writeBufSize += size;
              }
            else
                result = -1;
          }
    } while (result != -1 && !feof(FileStream(myBufFile)));

    /*
     * close the pipe
     */
    _DtHelpCeBufFileClose (myBufFile, True);

    if (result == -1)
      {
	if (writeBuf != NULL)
	    free(writeBuf);
	
	writeBuf = NULL;
      }
    else
	result = 0;

    /*
     * unlink the temporary file and free the memory.
     */
    unlink(fileName);
    free(fileName);

    /*
     * return the data
     */
    *ret_data = writeBuf;

    return result;

}  /* End _DtCvRunInterp */
