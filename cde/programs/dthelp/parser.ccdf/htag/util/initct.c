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
/* $XConsortium: initct.c /main/3 1995/11/08 11:40:22 rswiston $ */
/* Copyright (c) 1987, 1988 Hewlett-Packard Co. */
/* M_initctype initializes the array used to indicate which ASCII
   characters can appear within SGML names */

#include "basic.h"
#include "common.h"
extern char m_chartype[M_CHARSETLEN] ;

void m_initctype(void);

void m_initctype(void)
  {
    int i ;

/* ****************************** NOTE: **********************************
   If the set of name characters is ever modified, make appropriate changes
   in ELTDEF's scanner for testing for valid C identifiers as parameter
   names */
    for (i = 0 ; i < M_CHARSETLEN ; i++) m_chartype[i] = M_NONNAME ;
    for (i = 'A' ; i <= 'Z' ; i++) m_chartype[i] = M_NMSTART ;
    for (i = 'a' ; i <= 'z' ; i++) m_chartype[i] = M_NMSTART ;
    for (i = '0' ; i <= '9' ; i++) m_chartype[i] = M_DIGIT ;
    m_chartype['.'] = m_chartype['-'] = M_NAMECHAR ;
    }
