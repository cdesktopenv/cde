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
/* $XConsortium: sheet.tab.h /main/3 1996/06/11 17:46:46 cde-hal $ */
#define TOKEN 257
#define CONTAINER 258
#define SET 259
#define LIST 260
#define INDEX_NAME 261
#define INV 262
#define COMPRESS 263
#define INV_NAME 264
#define AGENT_NAME 265
#define STORE_NAME 266
#define POSITION 267
#define INDEX 268
#define MPHF_INDEX 269
#define SMPHF_INDEX 270
#define BTREE_INDEX 271
#define INDEX_AGENT 272
#define MPHF 273
#define SMPHF 274
#define BTREE 275
#define HUFFMAN 276
#define DICT 277
#define EQUAL 278
#define NUMBER 279
#define STORE 280
#define PAGE_STORE 281
#define NM 282
#define V_OID 283
#define MODE 284
#define PAGE_SZ 285
#define CACHED_PAGES 286
#undef  BYTE_ORDER
#define BYTE_ORDER 287
#define SEPARATOR 288
typedef union
{
 char   *string;
 int    integer;
 desc*  trans;
 page_store_desc*  ps_trans;
} YYSTYPE;
extern YYSTYPE  schemalval;
