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
/******************************<+>*************************************
 **********************************************************************
 **
 **  File:        WmParseP.h
 **
 **  Project:     HP/Motif Workspace Manager (dtwm)
 **
 **  Description:
 **  -----------
 **  This file contains private front panel parsing definitions
 **  
 **
 **
 **********************************************************************
 **
 ** (c) Copyright 1991 HEWLETT-PACKARD COMPANY
 ** ALL RIGHTS RESERVED
 **
 **********************************************************************
 **********************************************************************
 **
 **
 **********************************************************************
 ******************************<+>*************************************/
#ifndef _Dt_WmParseP_h
#define _Dt_WmParseP_h
#include <stdio.h>

/*
 * Definitions of special characters
 */

#define DTWM_CHAR_START_BLOCK             '{'
#define DTWM_CHAR_END_BLOCK               '}'
#define DTWM_CHAR_COMMENT                 '#'
#define DTWM_CHAR_BUTTON_TYPE_START       '['
#define DTWM_CHAR_BUTTON_TYPE_END         ']'
#define DTWM_CHAR_IMAGE_FILE              '@'
#define DTWM_CHAR_EQUALS                  '='
#define DTWM_CHAR_PLUS                    '+'
#define DTWM_CHAR_MINUS                   '-'
#define DTWM_CHAR_F_EXEC                  '!'
#define DTWM_CHAR_SPACE		   ' '
#define DTWM_CHAR_DIRECTORY		   '/'
#define DTWM_CHAR_ENVIRONMENT		   '$'
#define DTWM_CHAR_L_PAREN		   '('
#define DTWM_CHAR_R_PAREN		   ')'
#define DTWM_CHAR_L_BRACE		   DTWM_CHAR_START_BLOCK
#define DTWM_CHAR_R_BRACE		   DTWM_CHAR_END_BLOCK
#define DTWM_CHAR_TAB		    	   '\t'
#define DTWM_CHAR_NEW_LINE	    	   '\n'
#define DTWM_CHAR_BACKSLASH	    	   '\\'

/*
 * Panel keywords (2.x compatibility)
 */
#define DTWM_FP_PANEL_OLD	_dtwmFpPanelOld
#define DTWM_FP_ROW_OLD	_dtwmFpRowOld

extern char _dtwmFpPanelOld[];
extern char _dtwmFpRowOld[];

#define DTWM_FP_DROP_EFFECTS	_dtwmFpDropEffects

extern char _dtwmFpDropEffects[];

/*
 * Top-level keywords (3.0 syntax)
 */
#define DTWM_FP_PANEL		_dtwmFpPanel
#define DTWM_FP_BOX		_dtwmFpBox
#define	DTWM_FP_COMMAND	_dtwmFpCommand
#define DTWM_FP_CONTROL	_dtwmFpControl
#define DTWM_FP_SWITCH		_dtwmFpSwitch

extern char _dtwmFpPanel[];
extern char _dtwmFpBox[];
extern char _dtwmFpCommand[];
extern char _dtwmFpControl[];
extern char _dtwmFpSwitch[];


#define DTWM_FP_ANIMATION	_dtwmFpAnimation
#define DTWM_FP_INCLUDE	_dtwmFpInclude

extern char _dtwmFpAnimation[];
extern char _dtwmFpInclude[];

/*
 * Panel, Box, Control, and Switch attribute keywords
 */
#define DTWM_FPA_COLOR_SET		_dtwmFpaColorSet
#define DTWM_FPA_BACKGROUND_TILE	_dtwmFpaBackgroundTile
#define DTWM_FPA_LAYOUT_POLICY		_dtwmFpaLayoutPolicy
#define DTWM_FPA_ORIENTATION		_dtwmFpaOrientation
#define DTWM_FPA_SUBPANEL_DIRECTION	_dtwmFpaSubpanelDirection
#define DTWM_FPA_NUMBER_OF_ROWS	_dtwmFpaNumberOfRows
#define DTWM_FPA_NUMBER_OF_COLUMNS	_dtwmFpaNumberOfColumns
#define DTWM_FPA_TITLE			_dtwmFpaTitle

extern char _dtwmFpaColorSet[];
extern char _dtwmFpaBackgroundTile[];
extern char _dtwmFpaLayoutPolicy[];
extern char _dtwmFpaOrientation[];
extern char _dtwmFpaSubpanelDirection[];
extern char _dtwmFpaNumberOfRows[];
extern char _dtwmFpaNumberOfColumns[];
extern char _dtwmFpaTitle[];

/*
 * Control old reserved names and control types
 */
#define	DTWM_FPN_DTWMBLANK	_dtwmFpnDtwmblank
#define	DTWM_FPN_DTWMBOX	_dtwmFpnDtwmbox
#define	DTWM_FPN_DTWMBUSY	_dtwmFpnDtwmbusy
#define	DTWM_FPN_DTWMCLOCK	_dtwmFpnDtwmclock
#define	DTWM_FPN_DTWMDATE	_dtwmFpnDtwmdate
#define	DTWM_FPN_DTWMLOAD	_dtwmFpnDtwmload
#define	DTWM_FPN_DTWMNAME	_dtwmFpnDtwmname
#define	DTWM_FPN_DTWMRIB	_dtwmFpnDtwmrib
#define	DTWM_FPN_DTWMSWITCH	_dtwmFpnDtwmswitch

#define	DTWM_FPT_CLOCK		_dtwmFptClock
#define	DTWM_FPT_DATE		_dtwmFptDate
#define	DTWM_FPT_BLANK		_dtwmFptBlank
#define	DTWM_FPT_BUSY		_dtwmFptBusy
#define	DTWM_FPT_BUTTON	_dtwmFptButton
#define	DTWM_FPT_CLIENT	_dtwmFptClient
#define	DTWM_FPT_ICONBOX	_dtwmFptIconbox
#define	DTWM_FPT_MAIL		_dtwmFptMail
#define DTWM_FPT_MONITOR_FILE	_dtwmFptMonitorFile

/*
 * Panel type keywords
 */
#define	DTWM_FPT_FRONT_PANEL	_dtwmFptFrontPanel

/*
 * Box type keywords
 */
#define	DTWM_FPT_PRIMARY	_dtwmFptPrimary
#define	DTWM_FPT_SECONDARY	_dtwmFptSecondary
#define	DTWM_FPT_SUBPANEL	_dtwmFptSubpanel
#define	DTWM_FPT_ROW_COLUMN	_dtwmFptRowColumn

/*
 * Layout types
 */
#define DTWM_FPT_STANDARD	_dtwmFptStandard
#define DTWM_FPT_AS_NEEDED	_dtwmFptAsNeeded

/*
 * Orientation keywords
 */
#define DTWM_FPT_HORIZONTAL	_dtwmFptHorizontal
#define DTWM_FPT_VERTICAL	_dtwmFptVertical

/*
 * Subpanel Direction keywords
 */
#define DTWM_FPT_NORTH		_dtwmFptNorth
#define DTWM_FPT_SOUTH		_dtwmFptSouth
#define DTWM_FPT_EAST		_dtwmFptEast
#define DTWM_FPT_WEST		_dtwmFptWest

extern char	_dtwmFpnDtwmblank[];
extern char	_dtwmFpnDtwmbox[];
extern char	_dtwmFpnDtwmbusy[];
extern char	_dtwmFpnDtwmclock[];
extern char	_dtwmFpnDtwmdate[];
extern char	_dtwmFpnDtwmload[];
extern char	_dtwmFpnDtwmname[];
extern char	_dtwmFpnDtwmrib[];
extern char	_dtwmFpnDtwmswitch[];

extern char	_dtwmFptClock[];
extern char	_dtwmFptDate[];
extern char	_dtwmFptBlank[];
extern char	_dtwmFptBusy[];
extern char	_dtwmFptButton[];
extern char	_dtwmFptClient[];
extern char	_dtwmFptIconbox[];
extern char	_dtwmFptMail[];
extern char	_dtwmFptMonitorFile[];

extern char	_dtwmFptFrontPanel[];

extern char	_dtwmFptPrimary[];
extern char	_dtwmFptSecondary[];
extern char	_dtwmFptSubpanel[];
extern char	_dtwmFptRowColumn[];

extern char	_dtwmFptStandard[];
extern char	_dtwmFptAsNeeded[];

extern char	_dtwmFptHorizontal[];
extern char	_dtwmFptVertical[];

extern char	_dtwmFptNorth[];
extern char	_dtwmFptSouth[];
extern char	_dtwmFptEast[];
extern char	_dtwmFptWest[];

/* 
 * Control attribute keywords (in addition to attributes above)
 */
#define DTWM_FPA_ALTERNATE_IMAGE	_dtwmFpaAlternateImage
#define DTWM_FPA_CLIENT_GEOMETRY	_dtwmFpaClientGeometry
#define DTWM_FPA_CLIENT_NAME	_dtwmFpaClientName
#define DTWM_FPA_CONTEXT_MENU	_dtwmFpaContextMenu
#define DTWM_FPA_DROP_ACTION	_dtwmFpaDropAction
#define DTWM_FPA_DROP_ANIMATION	_dtwmFpaDropAnimation
#define DTWM_FPA_PUSH_ANIMATION	_dtwmFpaPushAnimation
#define DTWM_FPA_HELP_STRING	_dtwmFpaHelpString
#define DTWM_FPA_HELP_TOPIC	_dtwmFpaHelpTopic
#define DTWM_FPA_IMAGE		_dtwmFpaImage
#define DTWM_FPA_LABEL		_dtwmFpaLabel
#define DTWM_FPA_PUSH_ACTION	_dtwmFpaPushAction
#define DTWM_FPA_PUSH_RECALL	_dtwmFpaPushRecall
#define DTWM_FPA_SUBPANEL	_dtwmFpaSubpanel
#define DTWM_FPA_TRUE		_dtwmFpaTrue
#define DTWM_FPA_TYPE		_dtwmFpaType

extern char	_dtwmFpaClientGeometry[];
extern char	_dtwmFpaClientName[];
extern char	_dtwmFpaContextMenu[];
extern char	_dtwmFpaDropAction[];
extern char	_dtwmFpaDropAnimation[];
extern char	_dtwmFpaPushAnimation[];
extern char	_dtwmFpaAlternateImage[];
extern char	_dtwmFpaHelpString[];
extern char	_dtwmFpaHelpTopic[];
extern char	_dtwmFpaImage[];
extern char	_dtwmFpaLabel[];
extern char	_dtwmFpaMonitorFile[];
extern char	_dtwmFpPanel[];
extern char	_dtwmFpaPushAction[];
extern char	_dtwmFpaPushRecall[];
extern char	_dtwmFpaSlidePanel[];
extern char	_dtwmFpaTrue[];
extern char	_dtwmFpaType[];

#endif  /* _Dt_WmParseP_h */
