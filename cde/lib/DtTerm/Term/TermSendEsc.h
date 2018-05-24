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
/*
 * $XConsortium: TermSendEsc.h /main/1 1996/04/21 19:16:23 drk $";
 */

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef	_Dt_TermSendEsc_h
#define	_Dt_TermSendEsc_h

/*  These are the escape sequences that get sent from dtterm.  Some are 
    xterm, some vt220 and some are sun.
*/

/* vt220 cursor movement - normal mode */
#define ESC_CURSOR_UP          "\033[A"
#define ESC_CURSOR_DOWN        "\033[B"
#define ESC_CURSOR_RIGHT       "\033[C"
#define ESC_CURSOR_LEFT        "\033[D"
/* vt220 cursor movement - application mode */
#define ESC_CURSOR_UP_APP      "\033OA"
#define ESC_CURSOR_DOWN_APP    "\033OB"
#define ESC_CURSOR_RIGHT_APP   "\033OC"
#define ESC_CURSOR_LEFT_APP    "\033OD"
/* vt/xterm edit keys  */
#define ESC_FIND               "\033[1~"
#define ESC_INSERT_HERE        "\033[2~"
#define ESC_REMOVE             "\033[3~"
#define ESC_DELETE             "\033[3~"
#define ESC_SELECT             "\033[4~"
#define ESC_PREV_SCREEN        "\033[5~"
#define ESC_NEXT_SCREEN        "\033[6~"
#define ESC_HELP               "\033[28~"
#define ESC_MENU               "\033[29~"
#define ESC_DO                 "\033[29~"
/* vt function keys    */
#define F1_VT                  "\033[11~"
#define F2_VT                  "\033[12~"
#define F3_VT                  "\033[13~"
#define F4_VT                  "\033[14~"
#define F5_VT                  "\033[15~"
#define F6_VT                  "\033[17~"
#define F7_VT                  "\033[18~"
#define F8_VT                  "\033[19~"
#define F9_VT                  "\033[20~"
#define F10_VT                 "\033[21~"
#define F11_VT                 "\033[23~"
#define F12_VT                 "\033[24~"
#define F13_VT                 "\033[25~"
#define F14_VT                 "\033[26~"
#define F15_VT                 "\033[28~"
#define F16_VT                 "\033[29~"
#define F17_VT                 "\033[31~"
#define F18_VT                 "\033[32~"
#define F19_VT                 "\033[33~"
#define F20_VT                 "\033[34~"

/* SUN edit keys  */
#define ESC_FIND_SUN           "\033[1z"
#define ESC_INSERT_HERE_SUN    "\033[2z"
#define ESC_REMOVE_SUN         "\033[3z"
#define ESC_DELETE_SUN         "\033[3z"
#define ESC_SELECT_SUN         "\033[4z"
#define ESC_PREV_SCREEN_SUN    "\033[5z"
#define ESC_NEXT_SCREEN_SUN    "\033[6z"
#define ESC_HELP_SUN           "\033[196z"
#define ESC_MENU_SUN           "\033[197z"
#define ESC_DO_SUN             "\033[197z"
/* SUN function keys    */
#define F1_VT_SUN              "\033[224z"
#define F2_VT_SUN              "\033[225z"
#define F3_VT_SUN              "\033[226z"
#define F4_VT_SUN              "\033[227z"
#define F5_VT_SUN              "\033[228z"
#define F6_VT_SUN              "\033[229z"
#define F7_VT_SUN              "\033[230z"
#define F8_VT_SUN              "\033[231z"
#define F9_VT_SUN              "\033[232z"
#define F10_VT_SUN             "\033[233z"
#define F11_VT_SUN             "\033[192z"
#define F12_VT_SUN             "\033[193z"
#define F13_VT_SUN             "\033[194z"
#define F14_VT_SUN             "\033[195z"
#define F15_VT_SUN             "\033[196z"
#define F16_VT_SUN             "\033[197z"
#define F17_VT_SUN             "\033[198z"
#define F18_VT_SUN             "\033[199z"
#define F19_VT_SUN             "\033[200z"
#define F20_VT_SUN             "\033[201z"
#define F21_VT_SUN             "\033[208z"
#define F22_VT_SUN             "\033[209z"
#define F23_VT_SUN             "\033[210z"
#define F24_VT_SUN             "\033[211z"
#define F25_VT_SUN             "\033[212z"
#define F26_VT_SUN             "\033[213z"
#define F27_VT_SUN             "\033[214z"
#define F28_VT_SUN             "\033[215z"
#define F29_VT_SUN             "\033[216z"
#define F30_VT_SUN             "\033[217z"
#define F31_VT_SUN             "\033[218z"
#define F32_VT_SUN             "\033[219z"
#define F33_VT_SUN             "\033[220z"
#define F34_VT_SUN             "\033[221z"
#define F35_VT_SUN             "\033[222z"

#define DT_KP_Space     " " 
#define DT_KP_Tab       "\t"  
#define DT_KP_Enter     "\015"     /* carriage return */
#define DT_KP_F1        "\033OP"
#define DT_KP_F2        "\033OQ"
#define DT_KP_F3        "\033OR"
#define DT_KP_F4        "\033OS"
#define DT_KP_Equal     "=" 
#define DT_KP_Multiply  "*"  
#define DT_KP_Add       "+"    
#define DT_KP_Separator ","   
#define DT_KP_Subtract  "-"
#define DT_KP_Decimal   "."  
#define DT_KP_Divide    "/"
#define DT_KP_0         "0"
#define DT_KP_1         "1"
#define DT_KP_2         "2"    
#define DT_KP_3         "3"    
#define DT_KP_4         "4"    
#define DT_KP_5         "5"    
#define DT_KP_6         "6"    
#define DT_KP_7         "7"    
#define DT_KP_8         "8"    
#define DT_KP_9         "9"    

#define KP_APP_Space     "\033OA"  
#define KP_APP_Tab       "\033OI" 
#define KP_APP_Enter     "\033OM" 
#define KP_APP_F1        "\033OP"
#define KP_APP_F2        "\033OQ"
#define KP_APP_F3        "\033OR"
#define KP_APP_F4        "\033OS"
#define KP_APP_Equal     "\033OX"
#define KP_APP_Multiply  "\033Oj"  
#define KP_APP_Add       "\033Ok"    
#define KP_APP_Separator "\033Ol"   
#define KP_APP_Subtract  "\033Om"
#define KP_APP_Decimal   "\033On"  
#define KP_APP_Divide    "\033Oo"
#define KP_APP_0         "\033Op"
#define KP_APP_1         "\033Oq"
#define KP_APP_2         "\033Or"    
#define KP_APP_3         "\033Os"    
#define KP_APP_4         "\033Ot"    
#define KP_APP_5         "\033Ou"    
#define KP_APP_6         "\033Ov"    
#define KP_APP_7         "\033Ow"    
#define KP_APP_8         "\033Ox"    
#define KP_APP_9         "\033Oy"    

#endif	/* _Dt_TermSendEsc_h */
