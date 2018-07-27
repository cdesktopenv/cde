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
/* xoakufont.h 1.4 - Fujitsu source for CDEnext    96/05/30 11:20:56      */
/* $XConsortium: xoakufont.h /main/4 1996/06/25 20:17:03 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */


#include "fssxeg.h"

extern Widget   CreateLabel(Widget owner, String name, String str);

extern int      readSNF(FalFontData **fdata,
			int *width,
			int *height,
			char *err);
extern int      writeSNF(int restart, int *err);
extern int      ptnClose(void);
extern int      ptnAdd(int code, char *ptn);
extern int      ptnGet(int code, char *ptn);
extern int      ptnSense(int code);
extern int      ptnDel(int code);
extern int      ptnSqToNo(int num);
extern int      ptnNoToSq(int ncode);
extern int      noToCode(int sno);
extern int      codeToNo(int code);
extern int      codeCheck(int code);
extern void     bitSetInfo(int width, int height);
extern void     bitSet(char *ptn, int cx, int cy);
extern void     bitReset(char *ptn, int cx, int cy);
extern int      bitRead(char *ptn, int cx, int cy);
extern void     bitPtnClear(char *ptn);
extern void     bitPtnCopy(char *d_ptn, char *s_ptn);
extern int      bitDrawLine(char    *ptn,
			    int     x1,
			    int     y1,
			    int     x2,
			    int     y2,
			    int     mode);
extern int      bitDrawRect(char    *ptn,
			    int     x,
			    int     y,
			    int     width,
			    int     height,
			    int     mode);
extern int      bitDrawCls(char    *ptn,
			   int     x,
			   int     y,
			   int     width,
			   int     height);
extern int      bitDrawSet(char    *ptn,
			   int     x,
			   int     y,
			   int     width,
			   int     height);
extern int      bitDrawRev(char    *ptn,
			   int     x,
			   int     y,
			   int     width,
			   int     height);
extern int      bitDrawCpy(char    *ptn,
			   int     sx,
			   int     sy,
			   int     width,
			   int     height,
			   int     cut_flag);
extern int      bitDrawRoll(char *ptn, int x, int y, int width, int height);
extern int      bitDrawSymV(char *ptn, int x, int y, int width, int height);
extern int      bitDrawSymH(char *ptn, int x, int y, int width, int height);

#define D_MAX   10

#define	PROC_POINT	0
#define	PROC_LINE	1
#define	PROC_RECT	2
#define	PROC_CLS	3
#define	PROC_SET	4
#define	PROC_REV	5
#define	PROC_CUT	6
#define	PROC_CPY	7
#define	PROC_PASTE	8
#define	PROC_ROLL	9
#define	PROC_SYMV	10
#define	PROC_SYMH	11
#define PROC_CIRCLE	12
#define PROC_ERASE	13
#define PROC_SELECT	14

#define	RES_MSG		(1L<<0)
#define	RES_PROC	(1L<<1)
#define	RES_SLCT	(1L<<2)
#define	RES_RSV		(1L<<3)


#define CPY_COPY	0
#define CPY_OVERLAY	1


#define CODE_ADD	0
#define CODE_DEL	1

#define SET_CDSET1	0
#define SET_CDSET3	1


#define	SET_NORMAL_U90_G1	0
#define	SET_NORMAL_G1	1
#define	SET_SYSTEM_G1	2
#define	SET_NORMAL	3
#define	SET_JEF		4
#define	SET_SYSTEM	5


#define MG_MAXSIZE	100


/**********************************************************************
 structure
 **********************************************************************/

/* editting character */
typedef struct _EditChar {
    FalFontData *fdata;
    int		width;
    int		height;
    int		code;
    int		ncode;
    char	ptn[MAXPTNBYTE];
    char	flag;

} EditChar;

/* character list */
typedef struct _EditList {
    int		nlist;
    int		statloc;
    int		slctloc;
    int		nptn;
    int		sqstart;
    Dimension	list_h;
    Dimension	elem_w;
    Dimension	elem_h;
    Pixel	back;
    Pixel	border;
} EditList;

/* editting pane */
typedef struct _EditPane {
    int		width;
    int		height;
    int		pix_w;
    int		pix_h;
    int		color;
    int		obj;
    Pixel	back;
    Pixel	border;
} EditPane;

/* X library */
typedef struct _XlibInf {
    Display	*display;
    Window	root_winID;
    Window	edit_winID;
    Window	disp_winID;
    Window	list_winID;	/* 1995.10.02 */
    GC		borderGC;
    GC		backGC;
    GC		rubGC;
    GC		dashGC;
    XImage	*dispImage;
} XlibInf;


typedef struct _DelInf {
    Dimension	elem_w;
    Dimension	elem_h;
    Dimension	ptn_w;
    Dimension	ptn_h;
    Dimension	list_h;
    int		s_ncode;
    int		e_ncode;
    int		sq_top;
    int		sq_start;
    int		sq_end;
    int		disp_num;
} DelInf;

/* X library */
typedef struct _DelPtnInf {
    Window	disp_winID;
    XImage	*dispImage;
    char	ptn[MAXPTNBYTE];
} DelPtnInf;

/* editting pattern */
typedef struct _EditPtnInf {
    char	drag_f;
    int		adj_px;
    int		adj_py;
    int		proc;
    char	slct_f;
    int		src1_px;
    int		src1_py;
    int		src2_px;
    int		src2_py;
    int		dest_px;
    int		dest_py;
    char	rsv_f;
    char	rsv_ptn[MAXPTNBYTE];
} EditPtnInf;

/* copy patterns */
typedef struct _CopyPtnInf {
/*  int		size;*/
    FalFontData	*fdata;
    int		s1_code;
    int		s2_code;
    int		d1_code;
    int		proc;
} CopyPtnInf;

/* fonts */
typedef struct _FontInf {
    int			num;
    FalFontDataList	*lp;
    Widget		*RecB;
} FontInf;

/**********************************************************************


 **********************************************************************/

Widget	toplevel;

Widget	editPtnW;
Widget	cpyPtnW;
Widget	mngPtnW;

EditChar	edg;
EditList 	edlist;
EditPane 	edpane;
XlibInf 	xl;
DelInf		dn;
DelPtnInf 	dl[D_MAX] ;
DelPtnInf 	dl_glyph[EDLIST_MAX];		 /* add dl_disp 1995.09.20 */
int		sq_disp[EDLIST_MAX];		 /* add sq_disp 1995.09.20 */

EditPtnInf 	em;
CopyPtnInf 	cpm;


/*
 * resources for font editor
 */

#include"ufontrsrc.h"		/* 1995.06.28 H.Chiba */

/**********/
