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
/*
 *   COMPONENT_NAME: austext
 *
 *   FUNCTIONS: add_free_space
 *              find_free_space
 *              init_header
 *
 *   ORIGINS: 27
 *
 *   (C) COPYRIGHT International Business Machines Corp. 1993,1995
 *   All Rights Reserved
 *   US Government Users Restricted Rights - Use, duplication or
 *   disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/******************* FILEMAN.C **********************
 * $XConsortium: fileman.c /main/8 1996/11/25 18:47:12 drk $
 * September 1993.
 * Garbage collection algorithm:  Fileman maintains
 *  2 tables at the front of the d99.  They keep track of
 * small sized 'holes' (unique words that appear only
 * once in a record)and large holes of free space.
 * Only the N largest holes in each table are saved,
 * so some fragmentation still exists.  This happens
 * particularly when a huge database is built from scratch.
 * As the size grows, more and more holes are discarded
 * because they are too small to handle any words as
 * the database grows.  For this reason, mrclean should
 * be run at least once after the database grows to a
 * fairly good size, to recover the wasted space.  Once the
 * database reaches its nominal size, the 2 tables are very
 * effective at recycling holes, so mrclean need not be
 * run for compression purposes.
 *
 * $Log$
 * Revision 2.2  1995/10/24  22:09:25  miker
 * Add prolog.
 *
 * Revision 2.1  1995/09/22  20:05:50  miker
 * Freeze DtSearch 0.1, AusText 2.1.8
 *
 * Revision 1.3  1995/09/05  17:27:37  miker
 * Names changes for DtSearch.
 */
#include "SearchP.h"
#include <stdlib.h>
#include <errno.h>

#define	PROGNAME	"FILEMAN"
#define	HOLE_SIZE_LIMIT	500

DtSrINT32	batch_size = 0;

/************************/
/*			*/
/*	Init Header	*/
/*			*/
/************************/
/* Initialize file_header data, and write it at the beginning of a new file */
void	init_header (FILE * fp, FILE_HEADER * flh)
{
    memset (flh, 0, sizeof(FILE_HEADER));
    fseek (fp, 0L, SEEK_SET);
    fwrite (flh, sizeof(FILE_HEADER), (size_t)1, fp);
    return;
} /* init_header() */


/********************************/
/*				*/
/*	 fread_d99_header	*/
/*				*/
/********************************/
/* Reads d99 header structure from front of passed d99 file.
 * Performs byte swap as necessary IN PASSED BUFFER.
 * Returns TRUE if successful, else FALSE.
 */
int	fread_d99_header (FILE_HEADER *flh, FILE *fp)          
{
    int		i;

    errno = 0;
    fseek (fp, 0L, SEEK_SET);
    if (fread (flh, sizeof(FILE_HEADER), (size_t)1, fp) != 1L)
	return FALSE;
    NTOHL (flh->hole_count[0]);
    NTOHL (flh->hole_count[1]);
    for (i = 0; i < NUM_HOLES; i++) {
	NTOHL (flh->hole_array[0][i].hole_size);
	NTOHL (flh->hole_array[0][i].offset);
	NTOHL (flh->hole_array[1][i].hole_size);
	NTOHL (flh->hole_array[1][i].offset);
    }
    return TRUE;
}  /* fread_d99_header() */


/********************************/
/*				*/
/*	 fwrite_d99_header	*/
/*				*/
/********************************/
/* Writes d99 header structure to front of passed d99 file.
 * Performs byte swap as necessary IN PASSED BUFFER.
 * Returns TRUE if successful, else FALSE.
 */
int	fwrite_d99_header (FILE_HEADER *flh, FILE *fp)
{
    int		i;

    HTONL (flh->hole_count[0]);
    HTONL (flh->hole_count[1]);
    for (i = 0; i < NUM_HOLES; i++) {
	HTONL (flh->hole_array[0][i].hole_size);
	HTONL (flh->hole_array[0][i].offset);
	HTONL (flh->hole_array[1][i].hole_size);
	HTONL (flh->hole_array[1][i].offset);
    }
    errno = 0;
    fseek (fp, 0L, SEEK_SET);
    if (fwrite (flh, sizeof(FILE_HEADER), (size_t)1, fp) == 1L)
	return TRUE;
    else
	return FALSE;
}  /* fwrite_d99_header() */


/************************/
/*			*/
/*	Find Free Space	*/
/*			*/
/************************/
/* Find free space that is big enough to hold a new record.
   On success - return pointer to the FREE_SPACE_STR.
   On failure - return NULL.
*/
FREE_SPACE_STR *find_free_space (DtSrINT32 req_size, FILE_HEADER * flh)
{
    static FREE_SPACE_STR space_found;
    FREE_SPACE_STR  del_rec;
    int             i, j, k;
    DtSrINT32	hole_check_size;
    float           coeff;


    j = -1;
    if (req_size <= HOLE_SIZE_LIMIT) {
	k = 0;
	coeff = 1.1;
    }
    else {
	k = 1;
	coeff = 1.2;
    }

    for (i = 0; i < flh->hole_count[k]; i++) {
	if (flh->hole_array[k][i].hole_size >= req_size) {
	    if (j < 0) {
		j = i;
	    }
	    else {	/**  check if it's the smallest one of the
				free space available ***/
		if (flh->hole_array[k][i].hole_size <
		    flh->hole_array[k][j].hole_size) {
		    j = i;
		}
	    }
	}
    }

    /* if big enough free space not found, return NULL */
    if (j < 0) {
	return NULL;
    }

    if (flh->hole_array[k][j].hole_size == req_size) {
	space_found.hole_size = flh->hole_array[k][j].hole_size;
	space_found.offset = flh->hole_array[k][j].offset;
	/* compact the hole_array */
	if (j == NUM_HOLES - 1) {
	    (flh->hole_count[k])--;
	    flh->hole_array[k][j].hole_size = 0;
	    return &space_found;
	}
	for (i = j; i < (flh->hole_count[k] - 1); i++) {
	    flh->hole_array[k][i].hole_size =
		flh->hole_array[k][i + 1].hole_size;
	    flh->hole_array[k][i].offset =
		flh->hole_array[k][i + 1].offset;
	}
	(flh->hole_count[k])--;
    }
    else {
	/* Hole size CAN NOT exceed global batch_size in borodin */
	hole_check_size = (req_size * coeff < batch_size) ?
	    req_size * coeff : batch_size;
	if (hole_check_size >= flh->hole_array[k][j].hole_size) {
	    space_found.hole_size = flh->hole_array[k][j].hole_size;
	    space_found.offset = flh->hole_array[k][j].offset;
	    /* compact the hole_array */
	    if (j == NUM_HOLES - 1) {
		flh->hole_array[k][j].hole_size = 0;
	    }
	    else {
		for (i = j; i < (flh->hole_count[k] - 1); i++) {
		    flh->hole_array[k][i].hole_size =
			flh->hole_array[k][i + 1].hole_size;
		    flh->hole_array[k][i].offset =
			flh->hole_array[k][i + 1].offset;
		}
	    }
	    (flh->hole_count[k])--;
	}
	else {
	    space_found.hole_size = req_size;
	    space_found.offset = flh->hole_array[k][j].offset;
	    flh->hole_array[k][j].hole_size -= req_size;
	    flh->hole_array[k][j].offset += req_size * sizeof(DtSrINT32);
	    if ((k == 1) &&
		(flh->hole_array[k][j].hole_size <= HOLE_SIZE_LIMIT)) {
		del_rec.hole_size = flh->hole_array[k][j].hole_size;
		del_rec.offset = flh->hole_array[k][j].offset;
		add_free_space (&del_rec, flh);
		if (j == NUM_HOLES - 1) {
		    (flh->hole_count[k])--;
		    flh->hole_array[k][j].hole_size = 0;
		    return &space_found;
		}
		for (i = j; i < (flh->hole_count[k] - 1); i++) {
		    flh->hole_array[k][i].hole_size =
			flh->hole_array[k][i + 1].hole_size;
		    flh->hole_array[k][i].offset =
			flh->hole_array[k][i + 1].offset;
		}
		(flh->hole_count[k])--;
	    }
	}
    }

    return &space_found;
}

/************ end of function find_free_space ******************/



/************************/
/*			*/
/*	Add Free Space	*/
/*			*/
/************************/
/* Adds freed space to the hole_array.
   If there'e no space left in the hole_array, checks if the new
   space is greater, than the smallest space in the hole_array.
   If yes - substitute it.
*/
void            add_free_space (FREE_SPACE_STR * del_rec, FILE_HEADER * flh)
{
    int		i, j, k;
    DtSrINT32	min_size;

    if (del_rec->hole_size <= HOLE_SIZE_LIMIT) {
	k = 0;
    }
    else {
	k = 1;
    }
    j = flh->hole_count[k];

    if (j < NUM_HOLES) {
	flh->hole_array[k][j].hole_size = del_rec->hole_size;
	flh->hole_array[k][j].offset = del_rec->offset;
	(flh->hole_count[k])++;
    }
    else {
	min_size = flh->hole_array[k][0].hole_size;
	j = 0;
	for (i = 1; i < NUM_HOLES; i++) {
	    if (flh->hole_array[k][i].hole_size < min_size) {
		min_size = flh->hole_array[k][i].hole_size;
		j = i;
	    }
	}
	if (del_rec->hole_size > flh->hole_array[k][j].hole_size) {
	    flh->hole_array[k][j].hole_size = del_rec->hole_size;
	    flh->hole_array[k][j].offset = del_rec->offset;
	}
    }

    return;
}

/************ end of function add_free_space ******************/
