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
/* $XConsortium: trieplk.c /main/3 1995/11/08 09:56:28 rswiston $ */
/*
                   Copyright 1986, 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Trie.c contains procedures for maintaining the tree structure
   used to store element names, delimiter strings, short reference strings,
   etc. */

#include <stdio.h>
#include "basic.h"
#include "common.h"
#include "trie.h"

extern M_CHARTYPE m_ctarray[M_CHARSETLEN] ;

int m_packedlook(M_PTRIE *xptrie, M_WCHAR *name);

/* Look for the string NAME in the packed trie PTRIE */
int m_packedlook(M_PTRIE *xptrie, M_WCHAR *name)
{
int current = 0 ;
int i ;
int c ;

for ( ; TRUE ; name++)
    {
    c = m_ctupper(*name) ;
    for (i = current ;
	 (int) ((xptrie + i)->symbol) < c && (xptrie + i)->more ;
	 i++) ;
    if ((int) ((xptrie + i)->symbol) == c)
	{
	if (! c) return((xptrie + i)->index) ;
	current = (xptrie + i)->index ;
	}
    else return(FALSE) ;
    }
}
