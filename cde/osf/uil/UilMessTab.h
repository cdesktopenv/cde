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
/* 
 *  @OSF_COPYRIGHT@
 *  COPYRIGHT NOTICE
 *  Copyright (c) 1990, 1991, 1992, 1993 Open Software Foundation, Inc.
 *  ALL RIGHTS RESERVED (MOTIF). See the file named COPYRIGHT.MOTIF for
 *  the full copyright text.
*/ 
/* 
 * HISTORY
*/ 
/*   $XConsortium: UilMessTab.h /main/11 1995/07/14 09:36:09 drk $ */

/*
*  (c) Copyright 1989, 1990, DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS. */

/*
**++
**  FACILITY:
**
**      DECwindows Toolkit User Interface Language Compiler (UIL)
**--
**/


char XmConst msg0[36] = "duplicate option \"%s\" was ignored";
char XmConst msg1[34] = "unknown option \"%s\" was ignored";
char XmConst msg2[43] = "additional UIL source file: %s was ignored";
char XmConst msg3[30] = "error opening source file: %s";
char XmConst msg4[43] = "error reading next line of source file: %s";
char XmConst msg5[19] = "Internal error: %s";
char XmConst msg6[32] = "line truncated at %d characters";
char XmConst msg7[31] = "value of %s is out of range %s";
char XmConst msg8[21] = "%s not terminated %s";
char XmConst msg9[37] = "unprintable character \\%d\\ ignored";
char XmConst msg10[32] = "unknown sequence \"%s\" ignored";
char XmConst msg11[46] = "unknown escape sequence \"\\%c\" - \\ ignored";
char XmConst msg12[46] = "name exceeds %d characters - truncated to: %s";
char XmConst msg13[35] = "compiler ran out of virtual memory";
char XmConst msg14[56] = "unexpected %s token seen - parsing resumes after \"%c\"";
char XmConst msg15[44] = "%s %s must be defined before this reference";
char XmConst msg16[41] = "context requires a %s - %s was specified";
char XmConst msg17[26] = "%s is not implemented yet";
char XmConst msg18[39] = "found %s value when expecting %s value";
char XmConst msg19[45] = "the %s %s is not supported for the %s object";
char XmConst msg20[62] = "this %s %s is superseded by the last definition in this %s %s";
char XmConst msg21[33] = "name %s previously defined as %s";
char XmConst msg22[43] = "value used in this context must be private";
char XmConst msg23[55] = "procedure %s was previously declared with %d arguments";
char XmConst msg24[56] = "found %s value - procedure %s argument must be %s value";
char XmConst msg25[33] = "found %s %s when expecting %s %s";
char XmConst msg26[24] = "%s %s was never defined";
char XmConst msg27[39] = "%s %s already specified for this %s %s";
char XmConst msg28[29] = "%s item not allowed in %s %s";
char XmConst msg29[45] = "compilation terminated - fix previous errors";
char XmConst msg30[38] = "internal error - submit defect report";
char XmConst msg31[2] = " ";
char XmConst msg32[35] = "%s missing following \"%s\" option";
char XmConst msg33[31] = "error opening listing file: %s";
char XmConst msg34[34] = "error writing to listing file: %s";
char XmConst msg35[51] = "invalid module structure - check UIL module syntax";
char XmConst msg36[31] = "too many source files open: %s";
char XmConst msg37[38] = "source line contains a null character";
char XmConst msg38[45] = "errors: %d  warnings: %d  informationals: %d";
char XmConst msg39[27] = "error opening UID file: %s";
char XmConst msg40[25] = "no UID file was produced";
char XmConst msg41[53] = "creation procedure is not supported by the %s widget";
char XmConst msg42[59] = "creation procedure is not allowed in a %s widget reference";
char XmConst msg43[58] = "creation procedure is required in a %s widget declaration";
char XmConst msg44[46] = "a NULL character in a string is not supported";
char XmConst msg45[43] = "widget %s is part of a circular definition";
char XmConst msg46[25] = "no source file specified";
char XmConst msg47[35] = "%s %s supports only a single %s %s";
char XmConst msg48[41] = "%s widget supports only a single control";
char XmConst msg49[22] = "unknown character set";
char XmConst msg50[47] = "place names clause before other module clauses";
char XmConst msg51[47] = "color letter string must be a single character";
char XmConst msg52[48] = "color letter used for prior color in this table";
char XmConst msg53[37] = "row %d must have same width as row 1";
char XmConst msg54[52] = "row %d, column %d: letter \"%c\" not in color table";
char XmConst msg55[32] = "too many %ss in %s, limit is %d";
char XmConst msg56[48] = "Subqualifier not allowed with negated qualifier";
char XmConst msg57[60] = "%s gadget is not supported - %s widget will be used instead";
char XmConst msg58[28] = "%s type is not valid for %s";
char XmConst msg59[66] = "support for this character set may be removed in a future release";
char XmConst msg60[49] = "the %s constraint is not supported for the %s %s";
char XmConst msg61[37] = "too many \"%s\" options, limit is %d";
char XmConst msg62[30] = "error closing source file: %s";
char XmConst msg63[35] = "the %s value is circularly defined";
char XmConst msg64[28] = "overriding built-in name %s";
char XmConst msg65[51] = "the %s argument does not support enumerated values";
char XmConst msg66[57] = "the %s argument does not support the %s enumerated value";
char XmConst msg67[40] = "$LANG contains an unknown character set";
char XmConst msg68[66] = "the %s object's controls hierarchy contains a reference to itself";
char XmConst msg69[41] = "value %s is too large for context buffer";
char XmConst msg70[42] = "forward referencing is not allowed for %s";
char XmConst msg71[34] = "cannot convert %s type to %s type";
char XmConst msg72[14] = "%s is invalid";
char XmConst msg73[30] = "error reading binary database";
char XmConst msg74[47] = "binary database compiled with a future version";
char XmConst msg75[32] = "error opening database file: %s";
char XmConst msg76[27] = "error writing UID file: %s";
char XmConst msg77[45] = "'%s' is an unknown Toolkit class record name";
char XmConst msg78[26] = "invalid include file name";
char XmConst msg79[49] = "incompatible unit types for arithmetic operation";
char XmConst msg80[54] = "%s used as charset name; %s used as charset component";

typedef struct
{
  XmConst int  l_severity;
  char XmConst *ac_text;
} diag_rz_msg_entry;

XmConst diag_rz_msg_entry diag_rz_msg_table[81] =
        {
	   { 2, msg0 },
	   { 2, msg1 },
	   { 3, msg2 },
	   { 4, msg3 },
	   { 4, msg4 },
	   { 4, msg5 },
	   { 3, msg6 },
	   { 3, msg7 },
	   { 3, msg8 },
	   { 3, msg9 },
	   { 3, msg10 },
	   { 3, msg11 },
	   { 3, msg12 },
	   { 4, msg13 },
	   { 3, msg14 },
	   { 3, msg15 },
	   { 3, msg16 },
	   { 3, msg17 },
	   { 3, msg18 },
	   { 2, msg19 },
	   { 1, msg20 },
	   { 3, msg21 },
	   { 3, msg22 },
	   { 3, msg23 },
	   { 3, msg24 },
	   { 3, msg25 },
	   { 3, msg26 },
	   { 3, msg27 },
	   { 3, msg28 },
	   { 4, msg29 },
	   { 4, msg30 },
	   { 1, msg31 },
	   { 3, msg32 },
	   { 4, msg33 },
	   { 4, msg34 },
	   { 3, msg35 },
	   { 4, msg36 },
	   { 3, msg37 },
	   { 1, msg38 },
	   { 4, msg39 },
	   { 1, msg40 },
	   { 3, msg41 },
	   { 3, msg42 },
	   { 3, msg43 },
	   { 2, msg44 },
	   { 3, msg45 },
	   { 4, msg46 },
	   { 2, msg47 },
	   { 2, msg48 },
	   { 3, msg49 },
	   { 3, msg50 },
	   { 3, msg51 },
	   { 3, msg52 },
	   { 3, msg53 },
	   { 3, msg54 },
	   { 3, msg55 },
	   { 4, msg56 },
	   { 2, msg57 },
	   { 3, msg58 },
	   { 2, msg59 },
	   { 2, msg60 },
	   { 2, msg61 },
	   { 2, msg62 },
	   { 3, msg63 },
	   { 2, msg64 },
	   { 2, msg65 },
	   { 2, msg66 },
	   { 3, msg67 },
	   { 3, msg68 },
	   { 4, msg69 },
	   { 4, msg70 },
	   { 3, msg71 },
	   { 3, msg72 },
	   { 4, msg73 },
	   { 4, msg74 },
	   { 4, msg75 },
	   { 4, msg76 },
	   { 2, msg77 },
	   { 4, msg78 },
	   { 4, msg79 },
	   { 1, msg80 },
        };

#ifndef NO_MESSAGE_CATALOG
#include <nl_types.h>
#include "UilMsgCatI.h"
externalref nl_catd uil_catd;

XmConst int msg_cat_table[81] = {
  UIL_0,  UIL_1,  UIL_2,  UIL_3,  UIL_4,
  UIL_5,  UIL_6,  UIL_7,  UIL_8,  UIL_9,
  UIL_10, UIL_11, UIL_12, UIL_13, UIL_14,
  UIL_15, UIL_16, UIL_17, UIL_18, UIL_19,
  UIL_20, UIL_21, UIL_22, UIL_23, UIL_24,
  UIL_25, UIL_26, UIL_27, UIL_28, UIL_29,
  UIL_30, UIL_31, UIL_32, UIL_33, UIL_34,
  UIL_35, UIL_36, UIL_37, UIL_38, UIL_39,
  UIL_40, UIL_41, UIL_42, UIL_43, UIL_44,
  UIL_45, UIL_46, UIL_47, UIL_48, UIL_49,
  UIL_50, UIL_51, UIL_52, UIL_53, UIL_54,
  UIL_55, UIL_56, UIL_57, UIL_58, UIL_59,
  UIL_60, UIL_61, UIL_62, UIL_63, UIL_64,
  UIL_65, UIL_66, UIL_67, UIL_68, UIL_69,
  UIL_70, UIL_71, UIL_72,
  UIL_V2_73, UIL_V2_74, UIL_V2_75, UIL_V2_76, 
  UIL_V2_77, UIL_78, UIL_V2_79, UIL_V2_80,
};

#endif
