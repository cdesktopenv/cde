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
/* $XConsortium: token.h /main/1 1996/07/29 17:10:14 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef token_INCLUDED
#define token_INCLUDED 1

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

enum EnumToken {
  // tokenUnrecognized must be 0
  tokenUnrecognized,		// no token could be recognized
  tokenEe,			// end of entity
  tokenS,			// RS RE SPACE SEPCHAR
  tokenRe,			// RE
  tokenRs,			// RS
  tokenSpace,			// SPACE
  tokenSepchar,			// SEPCHAR
  tokenNameStart,		// X
  tokenDigit,			// 1
  tokenLcUcNmchar,		// LCNMCHAR or UCNMCHAR
  tokenChar,			// a legal data character
  tokenIgnoredChar,		// character in ignored marked section
  // delimiters and delimiters in context
  tokenAnd,
  tokenCom,
  tokenCroDigit,
  tokenCroNameStart,
  tokenDsc,
  tokenDso,
  tokenDtgc,
  tokenDtgo,
  tokenEroNameStart,
  tokenEroGrpo,
  tokenEtago,
  tokenEtagoNameStart,
  tokenEtagoTagc,
  tokenEtagoGrpo,
  tokenGrpc,
  tokenGrpo,
  tokenLit,
  tokenLita,
  tokenMdc,
  tokenMdoNameStart,
  tokenMdoMdc,
  tokenMdoCom,
  tokenMdoDso,
  tokenMinus,
  tokenMinusGrpo,
  tokenMscMdc,
  tokenNet,
  tokenOpt,
  tokenOr,
  tokenPero,
  tokenPeroNameStart,
  tokenPeroGrpo,
  tokenPic,
  tokenPio,
  tokenPlus,
  tokenPlusGrpo,
  tokenRefc,
  tokenRep,
  tokenRni,
  tokenSeq,
  tokenStago,
  tokenStagoNameStart,
  tokenStagoTagc,
  tokenStagoGrpo,
  tokenTagc,
  tokenVi,
  // short references start here
  tokenFirstShortref
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not token_INCLUDED */
