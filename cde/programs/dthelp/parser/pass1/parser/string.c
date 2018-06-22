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
/* $XConsortium: string.c /main/3 1995/11/08 10:26:03 rswiston $ */
/* Copyright 1988, 1989 Hewlett-Packard Co. */

/* String.c executes string-code specified in the interface. */

#include "userinc.h"
#include "globdec.h"

/* Include generated code */
#include "stfile.c"

void m_stcaction(M_WCHAR *m_string, LOGICAL M_START, LOGICAL M_END)
{
    m_stackpar = m_stacktop->stparam ;
    (*m_sttable[m_stacktop->stccase])(m_string, M_START, M_END) ;
    }
