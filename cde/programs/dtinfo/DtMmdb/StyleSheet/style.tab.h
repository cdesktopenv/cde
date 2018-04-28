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
/* $XConsortium: style.tab.h /main/3 1996/06/11 17:46:36 cde-hal $ */
#define INTEGER 257
#define OPER_equality 258
#define OPER_relational 259
#define BOOLVAL 260
#define REAL 261
#define OPER_assign 262
#define ARRAYOPEN 263
#define ARRAYCLOSE 264
#define SEPARATOR 265
#define FSOPEN 266
#define FSCLOSE 267
#define OPER_modify 268
#define OPER_parent 269
#define OPER_attr 270
#define OPER_oneof 271
#define OPER_star 272
#define OPER_or 273
#define OPER_and 274
#define OPER_div 275
#define OPER_parenopen 276
#define OPER_parenclose 277
#define OPER_logicalnegate 278
#define PMEMOPEN 279
#define PMEMCLOSE 280
#define OPER_period 281
#define OPER_plus 282
#define OPER_minus 283
#define DIMENSION 284
#define NORMAL_STRING 285
#define UNIT_STRING 286
#define QUOTED_STRING 287
#define GI_CASE_SENSITIVE 288
#define SGMLGI_STRING 289
typedef union
{
 unsigned char  charData;
 unsigned char* charPtrData;
 unsigned int	boolData;
 int   		intData;
 float		realData;
 Expression*    expPtrData;
 TermNode*      termNodePtrData;
 FeatureValue*	FeatureValuePtrData;
 FeatureSet*	FeatureSetPtrData;
 Feature*	FeaturePtrData;
 SSPath*	PathPtrData;
 PathTerm*	PathTermPtrData;
 charPtrDlist* 	charPtrDlistData;
 PathFeatureList* PathFeatureListPtrData;
 CompositeVariableNode*      CompositeVariableNodePtrData;

 CC_TPtrSlist<FeatureValue>* FeatureValueSlistPtrData;

 PQExpr*	PQExprPtrData;
} YYSTYPE;
extern YYSTYPE stylelval;
