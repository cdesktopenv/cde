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
/* $XConsortium: events.h /main/1 1996/07/29 17:08:51 cde-hp $ */
// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

EVENT(MessageEvent, message)
EVENT(DataEvent, data)
EVENT(StartElementEvent, startElement)
EVENT(EndElementEvent, endElement)
EVENT(PiEvent, pi)
EVENT(SdataEntityEvent, sdataEntity)
EVENT(ExternalDataEntityEvent, externalDataEntity)
EVENT(SubdocEntityEvent, subdocEntity)
EVENT(AppinfoEvent, appinfo)
EVENT(UselinkEvent, uselink)
EVENT(UsemapEvent, usemap)
EVENT(StartDtdEvent, startDtd)
EVENT(EndDtdEvent, endDtd)
EVENT(StartLpdEvent, startLpd)
EVENT(EndLpdEvent, endLpd)
EVENT(EndPrologEvent, endProlog)
EVENT(SgmlDeclEvent, sgmlDecl)
EVENT(CommentDeclEvent, commentDecl)
EVENT(SSepEvent, sSep)
EVENT(IgnoredReEvent, ignoredRe)
EVENT(ReOriginEvent, reOrigin)
EVENT(IgnoredRsEvent, ignoredRs)
EVENT(IgnoredCharsEvent, ignoredChars)
EVENT(MarkedSectionStartEvent, markedSectionStart)
EVENT(MarkedSectionEndEvent, markedSectionEnd)
EVENT(EntityStartEvent, entityStart)
EVENT(EntityEndEvent, entityEnd)
EVENT(EntityDeclEvent, entityDecl)
EVENT(NotationDeclEvent, notationDecl)
EVENT(ElementDeclEvent, elementDecl)
EVENT(AttlistDeclEvent, attlistDecl)
EVENT(LinkAttlistDeclEvent, linkAttlistDecl)
EVENT(AttlistNotationDeclEvent, attlistNotationDecl)
EVENT(LinkDeclEvent, linkDecl)
EVENT(IdLinkDeclEvent, idLinkDecl)
EVENT(ShortrefDeclEvent, shortrefDecl)
EVENT(IgnoredMarkupEvent, ignoredMarkup)
EVENT(EntityDefaultedEvent, entityDefaulted)
EVENT(SgmlDeclEntityEvent, sgmlDeclEntity)
