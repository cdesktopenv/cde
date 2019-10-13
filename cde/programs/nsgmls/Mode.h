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
/* $XConsortium: Mode.h /main/1 1996/07/29 16:57:54 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Mode_INCLUDED
#define Mode_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

enum Mode {
  grpMode,			// group
  alitMode,			// attribute value literals starting with LIT
  alitaMode,			// attribute value literals starting with LITA
  aliteMode,			// attribute value literals inside entity
  talitMode,			// tokenized attribute value literal
  talitaMode,
  taliteMode,
  mdMode,			// markup declaration
  mdMinusMode,			// markup declaration, also recognize minus
  mdPeroMode,			// markup declaration, also recognize pero
  comMode,			// comment
  sdcomMode,			// comment in an SGML declaration
  piMode,			// processing instruction
  refMode,			// reference
  imsMode,			// ignored marked section
  cmsMode,			// cdata marked section
  rcmsMode,			// rcdata marked section
  // These modes are needed only for the prologue.
  proMode,			// prologue
  dsMode,			// declaration subset not in marked section
				// nor in entity
  dsiMode,			// declaration subset in marked section or
				// in entity
  plitMode,			// parameter literal starting with LIT
  plitaMode,			// paramater literal starting with LITA
  pliteMode,			// parameter literal inside entity
  sdplitMode,			// parameter literal starting with LIT
                                // in an SGML declaration
  sdplitaMode,			// parameter literal starting with LIT
                                // in an SGML declaration
  grpsufMode,			// group suffix
  mlitMode,			// minimum literal starting with LIT
  mlitaMode,			// minimum literal starting with LITA
  asMode,			// data/link/result attribute specification
  slitMode,			// system id literal starting with LIT
  slitaMode,			// system id literal starting with LITA
  // These modes are needed only for the instance.
  cconMode,			// CDATA content
  rcconMode,			// RCDATA content
  cconnetMode,			// CDATA content, recognize NET
  rcconnetMode,			// RCDATA content, recognize NET
  rcconeMode,			// RCDATA content inside entity
  tagMode,			// start- or end-tag
  econMode,			// element content
  mconMode,			// mixed content
  econnetMode,			// element content, recognize NET
  mconnetMode			// mixed content, recognize NET
  };

const int nModes = mconnetMode + 1;

const int minShortrefMode = econMode;

#ifdef SP_NAMESPACE
}
#endif

#endif /* not Mode_INCLUDED */
