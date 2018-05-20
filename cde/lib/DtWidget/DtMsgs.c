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
/* $XConsortium: DtMsgs.c /main/7 1996/06/19 11:27:11 cde-dec $ */
/*
 * DtWidget/DtMsgs.c
 */
/*** const causes the HP compiler to complain.  Remove reference until   ***
 *** the build and integration people can figure out how to get this to  ***
 *** work.                                                               ***/
/* #define CONST 	const */
#define CONST 

#ifdef I18N_MSG

#include <nl_types.h>

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif 

#define _DTWIDGET_CAT_NAME	"DtWidget"

#endif /* I18N_MSG */

#include <DtWidgetI.h>

/*** Common messages across libDtWidget ***/
CONST char _DtMsgCommon_0000[] =
   "Close";
CONST char _DtMsgCommon_0001[] =
   "Help";

/*** ComboBox.c ***/
CONST char _DtMsgComboBox_0000[] =
   "DtComboBoxWidget: Invalid alignment resource (defaulting to DtALIGNMENT_CENTER).";

CONST char _DtMsgComboBox_0001[] =
   "DtComboBoxWidget: Invalid marginHeight resource (defaulting to 2).";

CONST char _DtMsgComboBox_0002[] =
   "DtComboBoxWidget: Invalid marginWidth resource (defaulting to 2).";

CONST char _DtMsgComboBox_0003[] =
   "DtComboBoxWidget: Invalid horizontalSpacing resource (defaulting to 0).";

CONST char _DtMsgComboBox_0004[] =
   "DtComboBoxWidget: Invalid verticalSpacing resource (defaulting to 0).";

CONST char _DtMsgComboBox_0005[] =
   "DtComboBoxWidget: Invalid orientation resource (defaulting to DtRIGHT).";

CONST char _DtMsgComboBox_0006[] =
   "DtComboBoxWidget: Invalid itemCount resource (defaulting to 0).";

CONST char _DtMsgComboBox_0007[] =
   "DtComboBoxWidget: Invalid selectedPosition resource (defaulting to 0).";

CONST char _DtMsgComboBox_0008[] =
   "DtComboBoxWidget: Unable to set textField resource.";

CONST char _DtMsgComboBox_0009[] =
   "DtComboBoxWidget: Unable to find item to set (DtComboBoxSetItem).";

CONST char _DtMsgComboBox_0010[] =
   "DtComboBoxWidget: Unable to find item to select (DtComboBoxSelectItem).";

CONST char _DtMsgComboBox_0011[] =
   "Resizing failed.  Maybe longest item is wider than space allowed.";

CONST char _DtMsgComboBox_0012[] =
   "ComboBox";

CONST char _DtMsgComboBox_0013[] =
   "cvtStringToType: wrongParameters, String to XmRType conversion needs no extra arguments";

CONST char _DtMsgComboBox_0014[] =
   "DtComboBoxDeletePos: wrongParameters, Invalid position value";

/*** DialogBox.c TitleBox.c ***/
CONST char _DtMsgDialogBox_0000[] =
   "Incorrect child type.";


/*** DialogBox.c ***/
CONST char _DtMsgDialogBox_0001[] =
   "Invalid button position.";


/*** Editor.c & SearchDlg.c ***/
/* Default value for DtNformatSettingsDialogTitle */
CONST char _DtMsgEditor_0000[] =
   "Format Settings";

/* Default values for DtNleftMarginFieldLabel and DtNrightMarginFieldLabel */
CONST char _DtMsgEditor_0001[] =
   "Right Margin:";

CONST char _DtMsgEditor_0002[] =
   "Left Margin:";

/* Default values for DtNleftAlignToggleLabel, DtNrightAlignToggleLabel, */
/* DtNjustifyToggleLabel, and DtNcenterToggleLabel */
CONST char _DtMsgEditor_0003[] =
   "Left Align";

CONST char _DtMsgEditor_0004[] =
   "Right Align";

CONST char _DtMsgEditor_0005[] =
   "Justify";

CONST char _DtMsgEditor_0006[] =
   "Center";

/* Default values for DtNformatParagraphButtonLabel and */
/* DtNformatAllButtonLabel */
CONST char _DtMsgEditor_0007[] =
   "Paragraph";

CONST char _DtMsgEditor_0008[] =
   "All";

/* Default value for DtNspellDialogTitle resource */
CONST char _DtMsgEditor_0009[] =
   "Spell";

/* Default value for DtNfindChangeDialogTitle resource */
CONST char _DtMsgEditor_0010[] =
   "Find/Change";

/* Default value for DtNmisspelledListLabel resource */
CONST char _DtMsgEditor_0011[] =
   "Misspelled Words:";

/* Default value for DtNfindFieldLabel resource */
CONST char _DtMsgEditor_0012[] =
   "Find:";

/* Default value for DtNchangeFieldLabel resource */
CONST char _DtMsgEditor_0013[] =
   "Change To:";

/* Default value for DtNfindButtonLabel, DtNchangeButtonLabel, and */
/*  DtNchangeAllButtonLabel resources */
CONST char _DtMsgEditor_0014[] =
   "Find";

CONST char _DtMsgEditor_0015[] =
   "Change";

CONST char _DtMsgEditor_0016[] =
   "Change All";

/* 'Unable to locate word' message */
CONST char _DtMsgEditor_0017[] =
   "Unable to find the string %s in the current document.";

/* Default value for DtNinformationDialogTitle resource */
CONST char _DtMsgEditor_0018[] =
   "Information";

/* Default values for DtNcurrentLineLabel, DtNtotalLineCountLabel, 
 * DtNoverstrikeLabel, and DtNinsertLabel resources */
CONST char _DtMsgEditor_0019[] =
   "Line:";

CONST char _DtMsgEditor_0020[] =
   "Total:";

CONST char _DtMsgEditor_0021[] =
   "Overstrike";

CONST char _DtMsgEditor_0022[] =
   " ";

/* Default values for messages and title when spell filter is
 * not executable or not found */
CONST char _DtMsgEditor_0023[] =
   "Error executing spell filter.";

CONST char _DtMsgEditor_0024[] =
   "Check to see if filter, %s, is installed and in the path.";

CONST char _DtMsgEditor_0025[] =
   "Editor Error";



/*** Icon.c ***/
CONST char _DtMsgIcon_0000[] =
   "Incorrect alignment.";

CONST char _DtMsgIcon_0001[] =
   "Incorrect behavior.";

CONST char _DtMsgIcon_0002[] =
   "Incorrect fill mode.";

CONST char _DtMsgIcon_0003[] =
   "Incorrect string or pixmap position.";

CONST char _DtMsgIcon_0004[] =
   "Incorrect margin width or height.";


/*** Icon.c TitleBox.c ***/
CONST char _DtMsgIcon_0005[] =
   "Incorrect shadow type.";


/*** Indicator.c ***/
CONST char _DtMsgIndicator_0000[] =
   "/var/mail/";

/*** MenuButton.c ***/
CONST char _DtMsgMenuButton_0000[] =
   "XmNmenuPost has illegal value.";

CONST char _DtMsgMenuButton_0001[] =
   "DtMenuButtonWidget: Invalid Parent.";

CONST char _DtMsgMenuButton_0002[] =
   "DtMenuButtonWidget: Invalid Submenu.";

/*** SpinBox.c ***/
CONST char _DtMsgSpinBox_0000[] =
   "DtSpinBoxWidget: Invalid arrowSensitivity resource (defaulting to DtARROWS_SENSITIVE).";

CONST char _DtMsgSpinBox_0001[] =
   "DtSpinBoxWidget: Invalid alignment resource (defaulting to DtALIGNMENT_CENTER).";

CONST char _DtMsgSpinBox_0002[] =
   "DtSpinBoxWidget: Invalid initialDelay resource (defaulting to 250).";

CONST char _DtMsgSpinBox_0003[] =
   "DtSpinBoxWidget: Invalid marginHeight resource (defaulting to 2).";

CONST char _DtMsgSpinBox_0004[] =
   "DtSpinBoxWidget:  Invalid marginWidth resource (defaulting to 2).";

CONST char _DtMsgSpinBox_0005[] =
   "DtSpinBoxWidget: Invalid arrowLayout resource (defaulting to DtARROWS_BEGINNING).";

CONST char _DtMsgSpinBox_0006[] =
   "DtSpinBoxWidget: Invalid repeatDelay resource (defaulting to 200).";

CONST char _DtMsgSpinBox_0007[] =
   "DtSpinBoxWidget: Invalid itemCount resource (defaulting to 0).";

CONST char _DtMsgSpinBox_0008[] =
   "DtSpinBoxWidget: Invalid position resource (defaulting to 0).";

CONST char _DtMsgSpinBox_0009[] =
   "DtSpinBoxWidget: Invalid position resource (defaulting to minimum).";

CONST char _DtMsgSpinBox_0010[] =
   "DtSpinBoxWidget:  Invalid decimalPoints resource (defaulting to 0).";

CONST char _DtMsgSpinBox_0011[] =
   "DtSpinBoxWidget: Invalid minimum resource (defaulting to maximum).";

CONST char _DtMsgSpinBox_0012[] =
   "DtSpinBoxWidget:  Unable to set textField resource.";

CONST char _DtMsgSpinBox_0013[] =
   "DtSpinBoxWidget: Unable to find item to set (DtSpinBoxSetItem).";

CONST char _DtMsgSpinBox_0014[] =
   "SpinBox";

/*** TitleBox.c ***/
CONST char _DtMsgTitleBox_0000[] =
   "Incorrect title position.";

CONST char _DtMsgTitleBox_0001[] =
   "Incorrect title alignment.";

CONST char _DtMsgTitleBox_0002[] =
   "Only one child should be inserted in a frame.";

CONST char _DtMsgTitleBox_0003[] =
   "Invalid margin width.";

CONST char _DtMsgTitleBox_0004[] =
   "Invalid margin height.";

#ifdef I18N_MSG

/*****************************************************************************
 *
 * Function: _DtWidgetGetMessage
 *
 * Parameters:
 *
 *   int	set -		The message catalog set number.
 *
 *   int	n - 		The message number.
 *
 *   char	*s -		The default message if the message is not
 *				retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 *
 *****************************************************************************/

char * 
_DtWidgetGetMessage(int set,
		    int n,
		    char *s)
{
  char *msg;
  nl_catd catopen();
  char *catgets();
  static int first = 1;
  static nl_catd nlmsg_fd;
  
  if ( first ) 
    {
      _DtProcessLock();
      if ( first)
	{
	  nlmsg_fd = catopen(_DTWIDGET_CAT_NAME, NL_CAT_LOCALE);
	  first = 0;
	}
      _DtProcessUnlock();
    }

  msg=catgets(nlmsg_fd,set,n,s);
  return (msg);
}
#endif /* I18N_MSG */
