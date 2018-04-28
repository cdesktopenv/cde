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
/* $XConsortium: escbase.h /main/3 1995/10/31 12:00:08 rswiston $ */

/** ANSI Escape sequences */
#define  HOME_UP              "\033[H"

#define  ERASE_FROM_CUR       "\033[J"
#define  ERASE_TO_CUR         "\033[1J"
#define  ERASE_ALL            "\033[2J"
#define  CLEAR_DISP           ERASE_ALL
#define  ERASE_LN_FROM_CUR    "\033[K"
#define  ERASE_LN_TO_CUR      "\033[1K"
#define  ERASE_LN_ALL         "\033[2K"
#define  ERASE_N_CHAR         "\033[%dX"

#define  CURSOR_UP            "\033[A"
#define  CURSOR_N_UP          "\033[%dA"
#define  CURSOR_DOWN          "\033[B"
#define  CURSOR_N_DOWN        "\033[%dB"
#define  CURSOR_N_FORW        "\033[%dC"
#define  CURSOR_N_BACK        "\033[%dD"
#define  GOTO_X_Y             "\033[%d;%dH"
#define  GOTO_X_Y_HVP         "\033[%d;%df"
#define  INDEX                "\033D"
#define  REVERSEINDEX         "\033M"
#define  NEXTLINE             "\033E"
#define  CURSOR_N_PREV_LINE   "\033[%dF"
#define  CURSOR_N_COLUMN      "\033[%dG"


#define  INSERT_N_LINE        "\033[%dL"
#define  INSERT_N_BLANK       "\033[%d@"
#define  DELETE_N_LINE        "\033[%dM"
#define  DELETE_N_CHAR        "\033[%dP"

#define  INSERT_MODE          "\033[4h"
#define  REPLACE_MODE         "\033[4l"

#define  CUR_POSN_STAT        "\033[6n"
#define  CUR_POSN_RPT         "\033[%d;%dR"
#define  TERMINAL_STAT        "\033[5n"
#define  TERMINAL_GOOD        "\033[0n"
#define  TERMINAL_BAD         "\033[3n"

#define  CLEAR_CUR_TAB        "\033[g"
#define  CLEAR_ALL_TABS       "\033[3g"
#define  SET_TAB              "\033H"


#define  SAVE_CURSOR          "\0337"
#define  RESTORE_CURSOR       "\0338"
#define  APPLN_KEYPAD         "\033="
#define  NORMAL_KEYPAD        "\033>"


#define  NORMAL               "\033[0m"
#define  BOLD                 "\033[1m"
#define  UNDERSCORE           "\033[4m"
#define  BLINK                "\033[5m"
#define  INVERSE              "\033[7m"


#define  SET_SCROLL_REGN      "\033[%d;%dr"

/*** DEC Private Mode Set (DECSET) ***/

#define  APPL_CUR_KEYS        "\033[?1h"
#define  COLUMN_132           "\033[?3h"
#define  SMOOTH_SCROLL        "\033[?4h"
#define  REVERSE_VIDEO        "\033[?5h"
#define  ORIGIN_MODE          "\033[?6h"
#define  WRAP_MODE            "\033[?7h"
#define  MARGIN_BELL          "\033[?44h"
#define  REVERSE_WRAP         "\033[?45h"
#define  START_LOGGING        "\033[?46h"

#define  NORMAL_CUR_KEYS      "\033[?1l"
#define  COLUMN_80            "\033[?3l"
#define  JUMP_SCROLL          "\033[?4l"
#define  NORMAL_VIDEO         "\033[?5l"
#define  CURSOR_MODE          "\033[?6l"
#define  NO_WRAP_MODE         "\033[?7l"
#define  NO_MARGIN_BELL       "\033[?44l"
#define  NO_REVERSE_WRAP      "\033[?45l"
#define  STOP_LOGGING         "\033[?46l"

#define  SAVE_CUR_KEYS           "\033[?1s"
#define  SAVE_COLUMN_TYPE        "\033[?3s"
#define  SAVE_SCROLL_TYPE        "\033[?4s"
#define  SAVE_VIDEO_TYPE         "\033[?5s"
#define  SAVE_SCREEN_MODE        "\033[?6s"
#define  SAVE_WRAP_MODE          "\033[?7s"
#define  SAVE_MARGIN_BELL_TYPE   "\033[?44s"
#define  SAVE_REVERSE_WRAP_TYPE  "\033[?45s"
#define  SAVE_LOGGING_TYPE       "\033[?46s"

#define  REST_CUR_KEYS           "\033[?1r"
#define  REST_COLUMN_TYPE        "\033[?3r"
#define  REST_SCROLL_TYPE        "\033[?4r"
#define  REST_VIDEO_TYPE         "\033[?5r"
#define  REST_SCREEN_MODE        "\033[?6r"
#define  REST_WRAP_MODE          "\033[?7r"
#define  REST_MARGIN_BELL_TYPE   "\033[?44r"
#define  REST_REVERSE_WRAP_TYPE  "\033[?45r"
#define  REST_LOGGING_TYPE       "\033[?46r"
