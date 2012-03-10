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
/* $XConsortium: keyword.h /main/3 1996/06/19 17:15:23 drk $ */
/* KEYWORD.H: Definitions for markup declaration keyword processing.
*/
/* Default value types for attribute definition list declaration.
*/
#define DNULL    1            /* Default value: implied attribute. */
#define DREQ     2            /* Default value: required attribute. */
#define DCURR    3            /* Default value: current attribute. */
#define DCONR    4            /* Default value: content reference attribute. */
#define DFIXED   5            /* Default value: fixed attribute. */

/* External identifier types for entity and notation declarations.
*/
#define EDSYSTEM  1           /* SYSTEM (but not PUBLIC) identifier specified.*/
#define EDPUBLIC  2           /* PUBLIC (but not SYSTEM) identifier specified.*/
#define EDBOTH    3           /* PUBLIC and also SYSTEM identifiers specified.*/

/* Marked section keywords.
*/
#define MSTEMP   1
#define MSRCDATA 2
#define MSCDATA  3
#define MSIGNORE 4
