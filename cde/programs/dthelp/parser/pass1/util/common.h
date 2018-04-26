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
/* $XConsortium: common.h /main/3 1995/11/08 10:28:31 rswiston $ */
/*
                   Copyright (c) 1987, 1988, 1989 Hewlett-Packard Co.
*/

/* Common.h contains definitions common to programs BUILD, ELTDEF, and PARSER.
Its purpose is to insure that limits, such as the maximum length of an SGML
name, are the same for all programs.
*/

#define M_MARKUP

/* White-space characters */
#define M_RE '\n'
#define M_SPACE ' '
#define M_TAB '\t'

/* Maximum length of a default parameter value */
#define M_LITLEN 240

/* Maximum length of an element name */
#define M_NAMELEN 64

/* M_NONNAME, M_NMSTART, M_NAMECHAR, and M_DIGIT are used to indicate 
   which characters can appear within element names*/
#define M_NONNAME 0
#define M_NMSTART 1
#define M_NAMECHAR 2
#define M_DIGIT 3

/* Number of characters in the character set */
#define M_CHARSETLEN 256

/* Structure for entities */
typedef struct m_entity M_ENTITY ;
struct m_entity {
  unsigned char type ;
  unsigned char wheredef ;
  M_WCHAR *content ;
  M_WCHAR *name ;
#if defined(BUILDEXTERN) || defined(ELTEXTERN)
  int index ;
  M_ENTITY *next ;
#endif
#if ! defined(BUILDEXTERN)
  int codeindex ;
#endif
  } ;

/* Declarations for character types */
typedef struct {
  LOGICAL type ;
  /* Must be same type as EOF */
  int upper ;
  int lower ;
  } M_CHARTYPE ;


#define m_ischar(x) ((int) (x) >= 0 && (int) (x) < M_CHARSETLEN)
#define m_cttype(x) (m_ischar(x) ? m_ctarray[x].type : M_NONNAME)
#define m_ctupper(x) (m_ischar(x) ? m_ctarray[x].upper : x)
#define m_ctlower(x) (m_ischar(x) ? m_ctarray[x].lower : x)
#define m_ctdigit(x) (m_ischar(x) ? m_ctarray[x].type == M_DIGIT : FALSE)
