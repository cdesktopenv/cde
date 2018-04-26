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
/* $XConsortium: context.h /main/3 1996/06/19 17:14:30 drk $ */
/* context.h */

#define M      pos[0].g       /* Index of current token in model. */
#ifdef P
#undef P
#endif
#define P      pos[0].t       /* Index of current group in pos. */
#define G      pos[P].g       /* Index of current group in model. */
#define T      pos[P].t       /* Index of current token in its group. */
#define H      pos[P].h       /* Pointer to hit bits for current group. */
#define GHDR   mod[G]         /* Current group header. */
#define TOKEN  mod[M]         /* Current token. */
#define TTYPE (GET(TOKEN.ttype, TTMASK))  /* Token type of current token. */
#define TOCC  (GET(TOKEN.ttype, TOREP))   /* Occurrence for current token. */
#define GTYPE (GET(GHDR.ttype, TTMASK))   /* Token type of current group. */
#define GOCC  (GET(GHDR.ttype, TOREP))    /* Occurrence for current group. */
#define GNUM  GHDR.tu.tnum                /* Number of tokens in current grp. */
