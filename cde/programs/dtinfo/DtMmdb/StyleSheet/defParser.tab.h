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
/* $XConsortium: defParser.tab.h /main/3 1996/06/11 17:46:23 cde-hal $ */
#define STAR 257
#define COMMA 258
#define COLON 259
#define SEMI_COLON 260
#define FSOPEN 261
#define FSCLOSE 262
#define OPER_parenopen 263
#define OPER_parenclose 264
#define INTEGER 265
#define REAL 266
#define NORMAL_STRING 267
#define QUOTED_STRING 268
#define REF_NAME 269
#define TYPE 270
#define ValueListOpt 271
typedef union
{
 unsigned char 	charData;
 unsigned char* charPtrData;
 int   		intData;
 float		realData;
 FeatureValue*	valueData;

 defv_t*    	valueListPtrData;
 TypeValues*   	typeValuesPtrData;
 type_values_list_t*   	typeValuesListPtrData;

 def_list_t*	defListPtrData;
 FeatureDef*	defPtrData;

} YYSTYPE;
extern YYSTYPE defParserlval;
