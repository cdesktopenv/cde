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
/* ufontrsrc.h 1.18 - Fujitsu source for CDEnext    96/10/30 13:13:44      */
/* $XConsortium: ufontrsrc.h /main/9 1996/11/08 01:52:14 cde-fuj $ */
/*
 *  (c) Copyright 1995 FUJITSU LIMITED
 *  This is source code modified by FUJITSU LIMITED under the Joint
 *  Development Agreement for the CDEnext PST.
 *  This is unpublished proprietary source code of FUJITSU LIMITED
 */



typedef struct _Resource {
    char	*mg_write_snf;
    char	*me_write_snf;
    char	*me_illegal_code;
    char	*me_non_memory;
    char	*mg_non_code;
    char	*me_illegal_srccode;
    char	*me_illegal_destcode;
    char	*me_nodelete_code;
    char	*me_wait;
    char	*me_non_srcfile;

    char	*mn_no_font;
    char	*mn_plural_font;
    char	*mn_not_udc;
    char	*mn_no_mem;
    char	*mn_illegal_area;
    char	*mn_no_read;
    char	*mn_prop_font;
    char	*mn_saved_open;
    char	*mn_saved_exit;
    char	*mn_cpy_lost;
    char	*mn_no_perm;

    Boolean	codepoint;

    char	*l_font;
    char	*l_edit;
    char	*l_manage;
    char	*l_info;
    char	*l_open;
    char	*l_selectitem;
    char	*l_codeset;
    char	*l_style;
    char	*l_width;
    char	*l_height;
    char	*l_cancel;
    char	*l_copy;
    char	*l_overlay;
    char	*l_add;
    char	*l_delete;
    char	*l_open_w;
    char	*l_save;
    char	*l_exit;
    char	*l_manage_w;
    char	*l_xlfd;
    char	*l_codearea;
    char	*l_copy_w;
    char	*l_clear;
    char	*l_set;
    char	*l_reverse;
    char	*l_cut;
    char	*l_Copy;
    char	*l_paste;
    char	*l_roll;
    char	*l_updown_roll;
    char	*l_leftright_roll;
    char	*l_undo;

    char	*l_manage_code;
    char	*l_copy_src_code;
    char	*l_copy_dest_code;

    char	*l_ok;

    char	*l_question_title;
    char	*l_warning_title;
    char	*l_error_title;
    char	*l_copy_title;
    char	*l_open_title;
    char	*l_add_title;
    char	*l_xlfd_title;
    char	*l_codearea_title;

    char	*file_name;
    char	*xlfd_name;
    char	*codearea;

    char	*l_do_save;
    char	*l_dont_save;
    char	*l_do_save_exit;
    char	*l_dont_save_exit;

    char	*l_ptog;
    char	*l_ptog_cmd;

    char	*mg_start_del_s;
    char	*mg_start_del_cs;
    char	*mg_start_del_m;
    char	*mg_start_del_cm;

    char	*mg_register;

    char	*l_code;
    char	*l_code_range;


/* CreateCaptionFrame() */
    Boolean	    capt_resize	;	/* resizable:		*/
    int		    capt_lftoff	;	/* leftOffset:		*/
    int		    capt_topoff	;	/* topOffset:		*/
    int		    capt_rghoff ;	/* RightOffset:		*/

    Pixel	    pane_background;
    Pixel	    pane_foreground;
    Pixel	    background;
    Pixel	    foreground;

/* CreateMenuButtons() */
    Dimension	    menu_margin ;	/* margin:		*/

/* CreateDialogAndButtons() */
    Boolean	    dia_tm_automng ;	/* autoUnmanage:	*/
    Dimension	    dia_tm_width ;	/* marginWidth:		*/
    Dimension	    dia_tm_height ;	/* marginHeight:	*/
    Boolean	    dia_tm_resize ;	/* noResize:		*/
    Boolean	    dia_tm_minimize ;	/* minimizeButtons:	*/

    Dimension	    dia_rw_width ;	/* marginWidth:		*/
    Dimension	    dia_rw_height ;	/* marginHeight:	*/
    Dimension	    dia_rw_spacing ;	/* spacing:		*/

/* CreateDrawingArea() */
    Dimension	    draw_border ;	/* borderWidth:		*/
    int		    draw_topoff ;	/* topOffset:		*/
    int		    draw_lftoff ;	/* leftOffset:		*/

/* PopupNotice() */
    Boolean	    pop_resize 	;	/* noResize:		*/
    Boolean	    pop_minimize ;	/* minimizeButtons:	*/

/* CreateTemplateDialog() */
    Boolean	    temp_resize ;	/* noResize:		*/
    Boolean	    temp_minimize ;	/* minimizeButtons:	*/

    Dimension	    temp_width 	;	/* width:		*/
    Dimension	    temp_height ;	/* height:		*/

/* CreateMenuBarAndFooterMessageForm() */

    int		    ed_bar_topoff ;	/* topOffset:		*/
    int		    ed_bar_lftoff ;	/* leftOffset:		*/
    int		    ed_bar_rghoff ;	/* rightOffset:		*/

    int		    ed_frm_btmoff ;	/* bottomOffset:	*/
    int		    ed_frm_lftoff ;	/* leftOffset:		*/
    int		    ed_frm_rghoff ;	/* rightOffset:		*/

    Dimension	    ed_msg_margintop ;	/* marginTop:		*/
    Dimension	    ed_msg_marginbtm ;	/* marginBottom:	*/

    Dimension	    ed_mesg_margintop ;	/* marginTop:		*/
    Dimension	    ed_mesg_marginbtm ;	/* marginBottom:	*/

    int		    ed_sep_btmoff ;	/* bottomOffset:	*/
    int		    ed_sep_lftoff ;	/* leftOffset:		*/
    int		    ed_sep_rghoff ;	/* rightOffset:		*/
    Dimension	    ed_sep_margin ;	/* margin:		*/

    int		    ed_wge_topoff ;	/* topOffset:		*/
    int		    ed_wge_lftoff ;	/* leftOffset:		*/
    int		    ed_wge_rghoff ;	/* rightOffset:		*/
    int		    ed_wge_btmoff ;	/* bottomOffset:	*/

/* CreateFrame() */
    Boolean	    frame_resize ;	/* resizable:		*/

/* CreateScrollBase() */
    Dimension	    scll_fr_width ;	/* width:		*/

    Dimension	    scll_fr_thick ;	/* shadowThickness:	*/

    int		    scll_bar_lftoff ;	/* leftOffset:		*/

 /*	put data from resource database */
    char	* exec_label ;
    char	* quit_label ;

    char	* message ;

    char	* code ;
    char	* previous ;
    char	* next ;
    char	* apply ;

    char	* falerrmsg[26];
} Resource;


/**********/
