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
/* $XConsortium: syncx.c /main/3 1996/07/04 02:32:52 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */
#include <stdio.h>

/*		"fal" -> "X"					*/
/* GetFProp.c	falGetFontProperty()		:	Xlib.h	*/
/* omDefault.c _falwcDefaultTextPerCharExtents():	omGeneric.c */
/* IntAtom.c	falInternAtom()			:	Xlib.h 	*/
/* Font.c	falFreeFont()			:	Xlib.h 	*/
/* FontInfo.c	falFreeFontInfo()		:	Xlib.h */
/* TextToStr.c	falFreeStringList()		:	Xlib.h */
/* FontNames.c	falFreeFontNames()		:	Xlib.h */

/* Bool */ falGetFontProperty()		{ return 0 ; }
/* Status */ _falwcDefaultTextPerCharExtents(){ return 0 ; }
void	_falmbDefaultDrawImageString()	{ return ; }
/* Atom */ falInternAtom()		{ return 0 ; }
int	_falwcGenericDrawString()	{ return 0 ; }
char	**_falParseBaseFontNameList()	{ return NULL ; }
/* XFontStruct * */ falLoadQueryFont()	{ return 0 ; }
falFreeFont()				{ return 0 ; }
falFreeFontInfo()			{ return 0 ; }
/* char	** */ falListFonts()		{ return 0 ; }
void	_falmbGenericDrawImageString()	{ return ; }
void	_falwcDefaultDrawImageString()	{ return ; }
int	_falmbDefaultDrawString()	{ return 0 ; }
int	_falmbGenericTextEscapement()	{ return 0 ; }
void	falFreeStringList()		{ return ; }
/* falrmStringToQuark()			{ return 0 ; } */
/* falrmPermStringToQuark()		{ return 0 ; } */
int	_falwcDefaultDrawString()	{ return 0 ; }
/* char	* */ falGetAtomName()		{ return 0 ; }
int	_falwcGenericTextExtents()	{ return 0 ; }
int	_falmbGenericTextExtents()	{ return 0 ; }
void	_falwcGenericDrawImageString()	{ return ; }
falFreeFontNames()			{ return 0 ; }
/* falrmQuarkToString()			{ return 0 ; } */
int	_falwcDefaultTextExtents()	{ return 0 ; }
/* Status */	_falmbGenericTextPerCharExtents(){ return 0 ; }
int	_falmbDefaultTextEscapement()	{ return 0 ; }
int	_falwcGenericTextEscapement()	{ return 0 ; }
int	_falmbDefaultTextExtents()	{ return 0 ; }
/* char	** */ falListFontsWithInfo()	{ return 0 ; }
int	_falwcDefaultTextEscapement()	{ return 0 ; }
/* Status */	_falwcGenericTextPerCharExtents(){ return 0 ; }
/* _fallcMapOSLocaleName()		{ return 0 ; } */
int	_falmbGenericDrawString()	{ return 0 ; }
/* Status */	_falmbDefaultTextPerCharExtents(){ return 0 ; }

