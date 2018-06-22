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
/* $XConsortium: out.c /main/3 1995/11/08 10:46:48 rswiston $ */
/*   Copyright (c) 1994 Hewlett-Packard Co. */
/* Functions relevant to output of data characters */

#include "userinc.h"
#include "globdec.h"

void PutString(char *string)
{
fputs(string, outFile);
}

void PutWString(M_WCHAR *string)
{
char *mb_string;

mb_string = MakeMByteString(string);
fputs(mb_string, outFile);
m_free(mb_string, "Multi-byte string");
}

void PutWChar(M_WCHAR wchar)
{
M_WCHAR wc_string[2];
char *mb_string;

wc_string[0] = wchar;
wc_string[1] = 0;
mb_string = MakeMByteString(wc_string);
fputs(mb_string, outFile);
m_free(mb_string, "Multi-byte string");
}

void SaveWChar(M_WCHAR wchar)
{
CDATA_el *pCdata;

if (pCurrentElement->type != e_cdata)
    return; /* we're in an error condition */

pCdata = &(pCurrentElement->u.u_cdata);

pCdata->buffPos++;
if (pCdata->buffPos == pCdata->buffSize)
    {
    pCdata->buffSize += 256;
    pCdata->buffer = m_realloc(pCdata->buffer,
			       pCdata->buffSize,
			       "u_cdata.buffer");
    }
pCdata->buffer[pCdata->buffPos] = wchar;
}
