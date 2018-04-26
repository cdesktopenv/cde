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
** $XConsortium: TermParseTable.h /main/1 1996/04/21 19:16:17 drk $
*/

/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef   _Dt_TermParseTable_h
#define   _Dt_TermParseTable_h

extern StateTable _DtTermStateStart;

extern void _DtTermDeviceStatus(Widget w);
extern void _DtTermPrimParserEnterNumAndPush(Widget w) ;
extern void _DtTermPrintState(Widget w) ;
extern void _DtTermCursorPos(Widget w) ;
extern void _DtTermEraseDisplay(Widget w) ;
extern void _DtTermInsertChars(Widget w) ;
extern void _DtTermCursorUp(Widget w) ;
extern void _DtTermCursorDown(Widget w) ;
extern void _DtTermCursorForward(Widget w) ;
extern void _DtTermCursorBack(Widget w) ;
extern void _DtTermCursorToLineUp(Widget w) ;
extern void _DtTermCursorToCol(Widget w) ;
extern void _DtTermEraseInLine(Widget w) ;
extern void _DtTermInsertLines(Widget w) ;
extern void _DtTermDeleteLines(Widget w) ;
extern void _DtTermDeleteChars(Widget w) ;
extern void _DtTermEnterNum(Widget w) ;
extern void _DtTermParsePushNum(Widget w) ;
extern void _DtTermInvokeG3(Widget w) ;
extern void _DtTermInvokeG2(Widget w) ;
extern void _DtTermScrollUp(Widget w) ;
extern void _DtTermScrollDown(Widget w) ;
extern void _DtTermEraseChars(Widget w) ;
extern void _DtTermSoftReset(Widget w) ;
extern void _DtTermClearParam(Widget w) ;
extern void _DtTermDeviceAttributes(Widget w) ;             
extern void _DtTermTabClear(Widget w) ;             
extern void _DtTermSetMode(Widget w) ;             
extern void _DtTermResetMode(Widget w) ;             
extern void _DtTermCharAttributes(Widget w) ;             
extern void _DtTermScrollingRegion(Widget w) ;             
extern void _DtTermRequestParam(Widget w) ;             
extern void _DtTermPModeSet(Widget w) ;             
extern void _DtTermPModeReset(Widget w) ;             
extern void _DtTermRestoreModeValues(Widget w) ;             
extern void _DtTermSaveModeValues(Widget w) ;             
extern void _DtTermAlignTest(Widget w);
extern void _DtTermInvokeG1(Widget w);
extern void _DtTermSelEraseInLine(Widget w);   /* vt220 */
extern void _DtTermSelEraseInDisplay(Widget w);   /* vt220 */
extern void _DtTermSaveCursor(Widget w);
extern void _DtTermRestoreCursor(Widget w);
extern void _DtTermAppKeypad(Widget w);
extern void _DtTermNormalKeypad(Widget w);
extern void _DtTermIndex(Widget w);
extern void _DtTermNextLine(Widget w);
extern void _DtTermTabSet(Widget w);
extern void _DtTermReverseIndex(Widget w);
extern void _DtTermSingleShiftG2(Widget w);
extern void _DtTermSingleShiftG3(Widget w);
extern void _DtTermDeviceControl(Widget w);
extern void _DtTermReset(Widget w);
extern void _DtTermSelectG2(Widget w);
extern void _DtTermSelectG3(Widget w);
extern void _DtTermParseTab(Widget w);
extern void _DtTermParseFontShift(Widget w);
extern void _DtTermParseFontG0(Widget w);
extern void _DtTermParseFontG1(Widget w);
extern void _DtTermParseFontG2(Widget w);
extern void _DtTermParseFontG3(Widget w);
extern void _DtTermParseStatus(Widget w);
extern void _DtTermChangeTextParam(Widget w);
extern void _DtTermSaveChar(Widget w);
extern void _DtTermS7C1T(Widget w);
extern void _DtTermS8C1T(Widget w) ;
extern void _DtTermSetCharEraseMode(Widget w);
extern void _DtTermSingleWide(Widget w);
extern void _DtTermDoubleHigh(Widget w);
extern void _DtTermDoubleWide(Widget w);
extern void _DtTermParseLF(Widget w);
extern void _DtTermLoadCharSet(Widget w);
extern void _DtTermParseBackspace(Widget w);
extern void _DtTermParseUserKeyLoad(Widget w);
extern void _DtTermParseUserKeyClear(Widget w);
extern void _DtTermParseHexDigit(Widget w);
extern void _DtTermSetCompatLevel(Widget w);
extern void _DtTermParseUserKeyLoadLast(Widget w);
extern void _DtTermParseSunMisc(Widget w);
extern void _DtTermParserSunWindowIcon(Widget w);
extern void _DtTermParserSunWindowTitle(Widget w);
extern void _DtTermParserSunIconFile(Widget w);
extern void _DtTermDeviceStatusAnsi(Widget w);

#endif /* _Dt_TermParseTable_h */
/* DON'T ADD ANYTHING AFTER THIS #endif... */
