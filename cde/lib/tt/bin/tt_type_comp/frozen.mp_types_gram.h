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
/* $XConsortium: frozen.mp_types_gram.h /main/3 1995/10/20 16:38:17 rswiston $ */

typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
	{
	int		num;
	char		*str;
	} YYSTYPE;
extern YYSTYPE yylval;
# define PER_FILE 257
# define PER_SESSION 258
# define START 259
# define FILE_SCOPE 260
# define SESSION_SCOPE 261
# define FILE_IN_SESSION 262
# define REQUEST 263
# define NOTIFICATION 264
# define VOID_ARGS 265
# define QUEUE 266
# define OPNUM 267
# define IN 268
# define OUT 269
# define INOUT 270
# define OTYPE 271
# define INHERIT 272
# define FROM 273
# define PTYPE 274
# define OBSERVE 275
# define HANDLE 276
# define HANDLE_PUSH 277
# define HANDLE_ROTATE 278
# define COLON 279
# define SEMICOLON 280
# define LCURL 281
# define RCURL 282
# define INFER 283
# define LPAREN 284
# define RPAREN 285
# define COMMA 286
# define EQUAL 287
# define CONTEXT 288
# define TT_IDENTIFIER 289
# define TT_STRING 290
# define TT_NUMBER 291
