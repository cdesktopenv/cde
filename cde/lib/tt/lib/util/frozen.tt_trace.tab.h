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
/* $XConsortium: frozen.tt_trace.tab.h /main/3 1995/10/23 10:35:31 rswiston $ */

typedef union
#ifdef __cplusplus
	TT_TRACE_STYPE
#endif
 {
	char	special;
	char	string[MAXPATHLEN];
	int		tokenval;
	struct {
		int state_type;
		int state_val;
	} stateval;
} TT_TRACE_STYPE;
extern TT_TRACE_STYPE tt_trace_lval;
# define _TT_TRACE_FOLLOW 257
# define _TT_TRACE_SINK 258
# define _TT_TRACE_SINK_APPEND 259
# define _TT_TRACE_FUNCTIONS 260
# define _TT_TRACE_ATTRIBUTES 261
# define _TT_TRACE_STATES 262
# define _TT_TRACE_OPS 263
# define _TT_TRACE_SENDER_PTYPES 264
# define _TT_TRACE_HANDLER_PTYPES 265
# define _TT_TRACE_ON 266
# define _TT_TRACE_OFF 267
# define _TT_TRACE_ALL 268
# define _TT_TRACE_NONE 269
# define _TT_TRACE_STATES_NEDD 270
# define _TT_TRACE_STATES_TT 271
# define _TT_TRACE_EDGE 272
# define _TT_TRACE_DELIVER 273
# define _TT_TRACE_DISPATCH 274
# define _TT_TRACE_VERSION 275
# define _TT_TRACE_IVAL 276
# define _TT_TRACE_STATES_VAL 277
# define _TT_TRACE_ALPHANUM 278
# define _TT_TRACE_FILENAME 279
# define _TT_TRACE_LINETERM 280
# define _TT_TRACE_PARSE_EOF 281
