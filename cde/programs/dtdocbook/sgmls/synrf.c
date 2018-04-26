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
/* $XConsortium: synrf.c /main/3 1996/06/19 17:18:46 drk $ */
/* SYNRF: Reserved names and other constants for reference concrete syntax.
*/
#include "config.h"
#include "entity.h"           /* Templates for entity control blocks. */
#include "synxtrn.h"          /* Declarations for concrete syntax constants. */
#include "adl.h"              /* Definitions for attribute list processing. */
UNCH key[NKEYS][REFNAMELEN+1] = {
     "ANY",
     "ATTLIST",
     "CDATA",
     "CONREF",
     "CURRENT",
     "DEFAULT",
     "DOCTYPE",
     "ELEMENT",
     "EMPTY",
     "ENDTAG",
     "ENTITIES",
     "ENTITY",
     "FIXED",
     "ID",
     "IDLINK",
     "IDREF",
     "IDREFS",
     "IGNORE",
     "IMPLIED",
     "INCLUDE",
     "INITIAL",
     "LINK",
     "LINKTYPE",
     "MD",
     "MS",
     "NAME",
     "NAMES",
     "NDATA",
     "NMTOKEN",
     "NMTOKENS",
     "NOTATION",
     "NUMBER",
     "NUMBERS",
     "NUTOKEN",
     "NUTOKENS",
     "O",
     "PCDATA",
     "PI",
     "POSTLINK",
     "PUBLIC",
     "RCDATA",
     "RE",
     "REQUIRED",
     "RESTORE",
     "RS",
     "SDATA",
     "SHORTREF",
     "SIMPLE",
     "SPACE",
     "STARTTAG",
     "SUBDOC",
     "SYSTEM",
     "TEMP",
     "USELINK",
     "USEMAP"
};
/*
Local Variables:
c-indent-level: 5
c-continued-statement-offset: 5
c-brace-offset: -5
c-argdecl-indent: 0
c-label-offset: -5
End:
*/
