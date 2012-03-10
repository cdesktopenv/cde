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
/* $XConsortium: sgmlincl.h /main/3 1996/06/19 17:17:42 drk $ */
/* SGMLINCL.H: Include file for parser core. */
#ifndef SGMLINCL              /* Don't include this file more than once. */
#define SGMLINCL 1
#include "config.h"
#include "std.h"
#include "entity.h"           /* Templates for entity control blocks. */
#include "action.h"           /* Action names for all parsing. */
#include "adl.h"              /* Definitions for attribute list processing. */
#include "error.h"            /* Symbols for error codes. */
#include "etype.h"            /* Definitions for element type processing. */
#include "keyword.h"          /* Definitions for keyword processing. */
#include "lextoke.h"          /* Symbols for tokenization lexical classes. */
#include "source.h"           /* Templates for source entity control blocks. */
#include "synxtrn.h"          /* Declarations for concrete syntax constants. */
#include "sgmlxtrn.h"         /* External variable declarations. */
#include "trace.h"            /* Declarations for internal trace functions. */
#include "sgmlmain.h"
#include "sgmlaux.h"
#include "sgmlfnsm.h"         /* ANSI C: Declarations for SGML functions. */
#endif /* ndef SGMLINCL */
