/* xoakufont.h 1.4 - Fujitsu source for CDEnext    96/05/30 11:20:56      */
/* $XConsortium: xoakufont.h /main/4 1996/06/25 20:17:03 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



#include "fssxeg.h"

extern Widget   CreateOblB();
extern Widget   CreateOblBG();
extern Widget   CreateToglBG();
extern Widget   CreateToglB();
extern Widget   CreateRecBG();
extern Widget   CreateRecB();
extern Widget   CreateCapt();
extern Widget   CreateCaptG();
extern Widget   CreateLabelG();
extern Widget   CreateLabel();
extern void     SetString();

extern int      readSNF();
extern int      writeSNF();
extern int      ptnClose();
extern int      ptnAdd();
extern int      ptnGet();
extern int      ptnSense();
extern int      ptnDel();
extern int      GetInfo();
extern int      ptnSqToNo();
extern int      ptnNoToSq();
extern int      noToCode();
extern int      noToCode2();
extern int      codeToNo();
extern int      codeToNo2();
extern int      codeCheck();
extern int      codeCheck2();
extern void     bitSetInfo();
extern void     bitSet();
extern void     bitReset();
extern int      bitRead();
extern void     bitPtnClear();
extern void     bitPtnCopy();
extern int      bitDrawLine();
extern int      bitDrawRect();
extern int      bitDrawCls();
extern int      bitDrawSet();
extern int      bitDrawRev();
extern int      bitDrawCpy();
extern int      bitDrawMov();
extern int      bitDrawRoll();
extern int      bitDrawSymV();
extern int      bitDrawSymH();

#define D_MAX   10

#define SetString(w,s) 	UpdateMessage((s))

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



#ifdef DEBUG
#define Dprintf2 printf
#define Dprintf  printf	
#else
#define Dprintf2
#define Dprintf
#endif


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

#ifdef	substance_source
#define	ex
#define	I(x)	=(x)
#else
#define	ex	extern
#define	I(x)
#endif	/* substance_source */

ex  Widget	toplevel	I(NULL);

ex  Widget	editPtnW	I(NULL);
ex  Widget	cpyPtnW		I(NULL);
ex  Widget	mngPtnW		I(NULL);

ex  EditChar	edg
#ifdef	substance_source
={ 0,0,0,0,0,0,0, OFF }
#endif	/* substance_source */
;
ex  EditList 	edlist
#ifdef	substance_source
	={ EDLIST_MAX, 0,0,0,0,0,0,0,0,0 }
#endif	/* substance_source */
;
EditPane 	edpane
#ifdef	substance_source
	={ EDPANE_SIZE, EDPANE_SIZE, 0,0,0,0,0,0 }
#endif	/* substance_source */
;
ex  XlibInf 	xl;
ex  DelInf 	dn
#ifdef	substance_source
	={ 0,0,0,0,0,0,0,0,0,0,0 }
#endif	/* substance_source */
;
ex  DelPtnInf 	dl[D_MAX] ;
ex  DelPtnInf 	dl_glyph[EDLIST_MAX] ;		 /* add dl_disp 1995.09.20 */
ex  int		sq_disp[EDLIST_MAX] ;		 /* add sq_disp 1995.09.20 */

ex  EditPtnInf 	em
#ifdef	substance_source
	={ OFF,0,0,0,0,0,0,0,0,0,0,0,0,0 }
#endif	/* substance_source */
;
ex  CopyPtnInf 	cpm
#ifdef	substance_source
	={ 0,0,0,0,0 }
#endif	/* substance_source */
;

#undef	ex
#undef	I
 

/*
 * resources for font editor
 */

#include"ufontrsrc.h"		/* 1995.06.28 H.Chiba */


/*Resource resource; del 1995.05.30 H.Chiba BSC */


#ifdef substance_source                 /* add 1995.06.29 H.Chiba -> */


/* CreateCaptionFrame() */
Boolean		capt_resize	= True ;		/* resizable:	*/
int		capt_lftoff	= 2 ;			/* leftOffset:	*/
int		capt_topoff	= 2 ;			/* topOffset:	*/
int		capt_rghoff	= 2 ;			/* rightOffset:	*/

Pixel		pane_background;
Pixel		pane_foreground;

/* CreateMenuButtons() */
Dimension	menu_margin 	= 0 ;			/* margin:	*/

/* CreateDialogAndButtons() */
Boolean		dia_tm_automng 	= False ;		/* autoUnmanage: */
Dimension	dia_tm_width 	= 4 ;			/* marginWidth:	 */
Dimension	dia_tm_height 	= 4 ;			/* marginHeight: */
Boolean		dia_tm_resize 	= True ;		/* noResize:	 */
Boolean		dia_tm_minimize = True ;		/* minimizeButtons: */

Dimension	dia_rw_width 	= 6 ;			/* marginWidth:	 */
Dimension	dia_rw_height 	= 6 ;			/* marginHeight: */
Dimension	dia_rw_spacing 	= 4 ;			/* spacing:	 */ 

/* CreateDrawingArea() */
Dimension	draw_border 	= 1 ;			/* borderWidth:	*/
int		draw_topoff 	= 2 ;			/* topOffset:	*/
int		draw_lftoff 	= 2 ;			/* leftOffset:	*/

/* PopupNotice() */
Boolean		pop_resize 	= True ;		/* noResize:	*/
Boolean		pop_minimize 	= True ;		/* minimizeButtons: */

/* CreateTemplateDialog() */
Boolean		temp_resize 	= True ;		/* noResize:	*/
Boolean		temp_minimize 	= True ;		/* minimizeButtons: */

Dimension	temp_width 	= 1 ;			/* width:	*/
Dimension	temp_height 	= 1 ;			/* height:	*/

/* CreateMenuBarAndFooterMessageForm() */
#if 0
ed_functions 	= ( MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_CLOSE ) ;/* mwmFunctions:	*/
#endif

int		ed_bar_topoff 	= 1 ;			/* topOffset:	*/
int		ed_bar_lftoff 	= 1 ;			/* leftOffset:	*/
int		ed_bar_rghoff 	= 1 ;			/* rightOffset:	*/

int		ed_frm_btmoff 	= 3 ;			/* bottomOffset: */
int		ed_frm_lftoff 	= 3 ;			/* leftOffset:	 */
int		ed_frm_rghoff 	= 3 ;			/* rightOffset:	 */

Dimension	ed_msg_margintop = 3 ;			/* marginTop:	 */
Dimension	ed_msg_marginbtm = 3 ;			/* marginBottom: */

Dimension	ed_mesg_margintop = 3 ;			/* marginTop:	 */
Dimension	ed_mesg_marginbtm = 3 ;			/* marginBottom: */

int		ed_sep_btmoff 	= 3 ;			/* bottomOffset: */
int		ed_sep_lftoff 	= 0 ;			/* leftOffset:	 */
int		ed_sep_rghoff 	= 0 ;			/* rightOffset:	 */
Dimension	ed_sep_margin 	= 0 ;			/* margin:	 */

int		ed_wge_topoff 	= 10 ;			/* topOffset:	 */
int		ed_wge_lftoff 	= 10 ;			/* leftOffset:	 */
int		ed_wge_rghoff 	= 10 ;			/* rightOffset:	 */
int		ed_wge_btmoff 	= 10 ;			/* bottomOffset: */

/* CreateFrame() */
Boolean		frame_resize 	= False ;		/* resizable:	 */

/* CreateScrollBase() */
Dimension	scll_fr_width 	= 500 ;			/* width:	 */

Dimension	scll_fr_thick 	= 2 ;			/* shadowThickness: */

int		scll_bar_lftoff = 4 ;			/* leftOffset:	 */

#endif	/* substance_source */	/* -> add 1995.06.29 H.Chiba */

/**********/
