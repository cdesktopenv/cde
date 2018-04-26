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
/* $XConsortium: strstr.c /main/3 1995/11/08 09:54:55 rswiston $ */
char *strstr ( s1, s2 )
char *s1, *s2 ; {
	
	int x, y ;
	
	y = strlen ( s2 ) ;
	x = strlen ( s1 ) - y ;
	
	if ( x < 0 ) return ( ( char * ) 0L ) ;
	do {
		if ( strncmp ( s1 + x, s2, y ) == 0 ) return ( s1 + x ) ;
		}
	while ( --x >= 0 ) ;
	
	return ( ( char * ) 0L ) ;
	}

