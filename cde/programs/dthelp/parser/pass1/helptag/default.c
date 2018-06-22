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
/* $XConsortium: default.c /main/3 1995/11/08 10:08:01 rswiston $ */
/*
Copyright 1988, 1989, 1992 Hewlett-Packard Co.
*/

/* HP Tag to TeX default processing */

#include "userinc.h"
#include "globdec.h"

int vextextsize(const M_WCHAR *keyword)
{
if (!keyword) return NORMAL;

if (! m_wcmbupstrcmp(keyword, QNORMAL)) return NORMAL;
else if (! m_wcmbupstrcmp(keyword, QSMALLER)) return SMALLER;
else if (! m_wcmbupstrcmp(keyword, QSMALLEST)) return SMALLEST;

m_err1("Unexpected value for ex/vex textsize parameter: %s", keyword);
return NORMAL;
}

int vgloss(const M_WCHAR *keyword)
{
if (!keyword) return GLOSS;

if (! m_wcmbupstrcmp(keyword, QGLOSS)) return GLOSS;
else if (! m_wcmbupstrcmp(keyword, QNOGLOSS)) return NOGLOSS;

m_err1("Unexpected value for gloss parameter: %s", keyword);
return GLOSS;
}

int vcenter(const M_WCHAR *keyword)
{
if (!keyword) return CENTER;

if (! m_wcmbupstrcmp(keyword, QLEFT)) return LEFT;
else if (! m_wcmbupstrcmp(keyword, QCENTER)) return CENTER;
else if (! m_wcmbupstrcmp(keyword, QRIGHT)) return RIGHT;

m_err1("Unexpected value for center parameter: %s", keyword);
return CENTER;
}

int vnumber(const M_WCHAR *keyword)
{
if (!keyword) return NONUMBER;

if (! m_wcmbupstrcmp(keyword, QNUMBER)) return NUMBER;
else if (! m_wcmbupstrcmp(keyword, QNONUMBER)) return NONUMBER;

m_err1("Unexpected value for number parameter: %s", keyword);
return NONUMBER;
}

LOGICAL vstack(const M_WCHAR *keyword)
{
if (!keyword) return FALSE;

if (! m_wcmbupstrcmp(keyword, QSIDE)) return FALSE;
else if (! m_wcmbupstrcmp(keyword, QSTACK)) return TRUE;

m_err1("Unexpected value for number parameter: %s", keyword);
return FALSE;
}

int vordertype(const M_WCHAR *keyword)
{
if (!keyword) return ARABIC;

if (! m_wcmbupstrcmp(keyword, QUALPHA)) return UALPHA;
else if (! m_wcmbupstrcmp(keyword, QLALPHA)) return LALPHA;
else if (! m_wcmbupstrcmp(keyword, QARABIC)) return ARABIC;
else if (! m_wcmbupstrcmp(keyword, QUROMAN)) return UROMAN;
else if (! m_wcmbupstrcmp(keyword, QLROMAN)) return LROMAN;

m_err1("Unexpected list order type: %s", keyword);
return ARABIC;
}

int vrsectpage(const M_WCHAR *keyword)
{
if (!keyword) return NEWPAGE;

if (! m_wcmbupstrcmp(keyword, QNEWPAGE)) return NEWPAGE;
else if (! m_wcmbupstrcmp(keyword, QSAMEPAGE)) return SAMEPAGE;

m_err1("Unexpected value for rsectpage parameter: %s", keyword);
return NEWPAGE;
}

int vlonglabel(const M_WCHAR *keyword)
{
if (!keyword) return WRAP;

if (! m_wcmbupstrcmp(keyword, QWRAP)) return WRAP;
else if (! m_wcmbupstrcmp(keyword, QNOWRAP)) return NOWRAP;

m_err1("Unexpected value for longlabel parameter: %s", keyword);
return WRAP;
}

int vspacing(const M_WCHAR *keyword)
{
if (!keyword) return LOOSE;

if (! m_wcmbupstrcmp(keyword, QTIGHT)) return TIGHT;
else if (! m_wcmbupstrcmp(keyword, QLOOSE)) return LOOSE;

m_err1("Unexpected value for spacing parameter: %s", keyword);
return LOOSE;
}

int vtype(const M_WCHAR *keyword)
{
if (!keyword) return BULLET;

if (! m_wcmbupstrcmp(keyword, QORDER)) return ORDER;
else if (! m_wcmbupstrcmp(keyword, QBULLET)) return BULLET;
else if (! m_wcmbupstrcmp(keyword, QPLAIN)) return PLAIN;
else if (! m_wcmbupstrcmp(keyword, QMILSPEC)) return ORDER;
else if (! m_wcmbupstrcmp(keyword, QCHECK)) return CHECK;

m_err1("Unexpected list type: %s", keyword);
return BULLET;
}
