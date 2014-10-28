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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * $TOG: WArgList.h /main/4 1998/04/17 11:45:37 mgreess $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#ifndef WArgList_h
#define WArgList_h

// Allow setting of resources by name in arg list
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
#define CASTRNAM (char*)
#define CASTVAL  (void*)(size_t)
#else
#define CASTRNAM
#define CASTVAL
#endif

#define RSRC_SET(RSC,TYP,RNAM) \
   inline WArgList& RSC(TYP val) { return Add(CASTRNAM RNAM, CASTVAL val); }

// Use this one to comment out call
#define DONT_SET(RSC,TYP,RNAM)

class WArgList {

protected :

   ArgList	args;
   Cardinal	num_args;
   Cardinal	alloc_args;

   Arg&		Grow(Cardinal n);

public :

   WArgList(String name, XtArgVal value, ...);
   inline	WArgList() {
      args       = NULL;
      num_args   =
      alloc_args = 0;
   }

   inline	WArgList(Cardinal len) {
      args       = new Arg[len];
      num_args   = 0;
      alloc_args = len;
   }

   inline	WArgList(ArgList a, Cardinal n) {
      args       = a;
      num_args   = n;
      alloc_args = 0;
   }

   inline	~WArgList() {
      if ( alloc_args ) {
		delete [] args;
      }
   }

   WArgList(const WArgList &r);

   inline ArgList	Args() const	{ return args; }
   inline Cardinal	NumArgs() const { return num_args; }
   inline void		Reset()		{ num_args = 0; }
   inline Arg&		operator[] (Cardinal n);
   WArgList&	Add(String name, XtArgVal value);
   inline WArgList&	Add(String name, void* value) {
      return Add(name, (XtArgVal)value);
   }

#ifdef XmString_h
   inline WArgList&	Add(String name, const WXmString& value) {
      return Add(name, (XmString)value);
   }
#endif

   friend class	WObject;
   friend class	CConstraint;

// The following methods allow setting of resources by name in arg list

   RSRC_SET(Accelerator,	char*,		"accelerator")
   RSRC_SET(AcceleratorText,	XmString,	"acceleratorText")
   RSRC_SET(Accelerators,	XtTranslations,	"accelerators")
   RSRC_SET(AdjustLast,		Boolean,	"adjustLast")
   RSRC_SET(AdjustMargin,	Boolean,	"adjustMargin")
   RSRC_SET(Alignment,		unsigned char,	"alignment")
   RSRC_SET(AllowOverlap,	Boolean,	"allowOverlap")
   RSRC_SET(AllowResize,	Boolean,	"allowResize")
   RSRC_SET(AllowShellResize,	Boolean,	"allowShellResize")
   RSRC_SET(AncestorSensitive,	Boolean,	"ancestorSensitive")
   RSRC_SET(ApplyLabelString,	XmString,	"applyLabelString")
   RSRC_SET(Argc,		int,		"argc")
   RSRC_SET(Argv,		void*,		"argv")
   RSRC_SET(ArmColor,		Pixel,		"armColor")
   RSRC_SET(ArmPixmap,		Pixmap,		"armPixmap")
   RSRC_SET(ArrowDirection,	unsigned char,	"arrowDirection")
   RSRC_SET(AutoShowCursorPosition,	Boolean,	"autoShowCursorPosition")
   RSRC_SET(AutoUnmanage,	Boolean,	"autoUnmanage")
   RSRC_SET(AutomaticSelection,	Boolean,	"automaticSelection")
   RSRC_SET(Background,		Pixel,		"background")
   RSRC_SET(BackgroundPixmap,	Pixmap,		"backgroundPixmap")
   RSRC_SET(BlinkRate,		int,		"blinkRate")
   RSRC_SET(BorderColor,	Pixel,		"borderColor")
   RSRC_SET(BorderPixmap,	Pixmap,		"borderPixmap")
   RSRC_SET(BorderWidth,	Dimension,	"borderWidth")
   RSRC_SET(BottomAttachment,	unsigned char,	"bottomAttachment")
   RSRC_SET(BottomOffset,	int,		"bottomOffset")
   RSRC_SET(BottomPosition,	int,		"bottomPosition")
   RSRC_SET(BottomShadowColor,	Pixel,		"bottomShadowColor")
   RSRC_SET(BottomShadowPixmap,	Pixmap,		"bottomShadowPixmap")
   RSRC_SET(BottomWidget,	Widget,		"bottomWidget")
   RSRC_SET(ButtonFontList,	XmFontList,	"buttonFontList")
   RSRC_SET(CancelButton,	Widget,		"cancelButton")
   RSRC_SET(CancelLabelString,	XmString,	"cancelLabelString")
   RSRC_SET(CascadePixmap,	Pixmap,		"cascadePixmap")
   RSRC_SET(ClipWindow,		Widget,		"clipWindow")
   RSRC_SET(Colormap,		void*,		"colormap")
   RSRC_SET(Columns,		short,		"columns")
   RSRC_SET(Command,		XmString,	"command")
   RSRC_SET(CommandWindow,	Widget,		"commandWindow")
   DONT_SET(CreatePopupChildProc,	XtProc,	"createPopupChildProc")
   RSRC_SET(CursorPosition,	XmTextPosition,	"cursorPosition")
   RSRC_SET(CursorPositionVisible,	Boolean,	"cursorPositionVisible")
   RSRC_SET(DecimalPoints,	short,		"decimalPoints")
   RSRC_SET(DefaultButton,	Widget,		"defaultButton")
   RSRC_SET(DefaultButtonType,	unsigned char,	"defaultButtonType")
   RSRC_SET(DefaultPosition,	Boolean,	"defaultPosition")
   RSRC_SET(DeleteResponse,	unsigned char,	"deleteResponse")
   RSRC_SET(Depth,		int,		"depth")
   RSRC_SET(DialogStyle,	unsigned char,	"dialogStyle")
   RSRC_SET(DialogTitle,	XmString,	"dialogTitle")
   RSRC_SET(DialogType,		unsigned char,	"dialogType")
   RSRC_SET(DirMask,		XmString,	"dirMask")
   RSRC_SET(DirSpec,		XmString,	"dirSpec")
   RSRC_SET(DoubleClickInterval,	int,	"doubleClickInterval")
   RSRC_SET(EditMode,		unsigned char,	"editMode")
   RSRC_SET(Editable,		Boolean,	"editable")
   RSRC_SET(EntryAlignment,	unsigned char,	"entryAlignment")
   RSRC_SET(EntryBorder,	Dimension,	"entryBorder")
   RSRC_SET(EntryClass,		int,		"entryClass")
   DONT_SET(FileSearchProc,	XtProc,		"fileSearchProc")
   RSRC_SET(FillOnArm,		Boolean,	"fillOnArm")
   RSRC_SET(FillOnSelect,	Boolean,	"fillOnSelect")
   RSRC_SET(FilterLabelString,	XmString,	"filterLabelString")
   RSRC_SET(FontList,		XmFontList,	"fontList")
   RSRC_SET(Foreground,		Pixel,		"foreground")
   RSRC_SET(FractionBase,	int,		"fractionBase")
   RSRC_SET(Geometry,		char*,		"geometry")
   RSRC_SET(Height,		Dimension,	"height")
   RSRC_SET(HeightInc,		int,		"heightInc")
   RSRC_SET(HelpLabelString,	XmString,	"helpLabelString")
   RSRC_SET(HighlightColor,	Pixel,		"highlightColor")
   RSRC_SET(HighlightOnEnter,	Boolean,	"highlightOnEnter")
   RSRC_SET(HighlightPixmap,	Pixmap,		"highlightPixmap")
   RSRC_SET(HighlightThickness,	short,		"highlightThickness")
   RSRC_SET(HistoryItemCount,	int,		"historyItemCount")
   RSRC_SET(HistoryItems,	XmStringTable,	"historyItems")
   RSRC_SET(HistoryMaxItems,	int,		"historyMaxItems")
   RSRC_SET(HistoryVisibleItemCount,	int,	"historyVisibleItemCount")
   RSRC_SET(HorizontalScrollBar,	Widget,	"horizontalScrollBar")
   RSRC_SET(HorizontalSpacing,	int,		"horizontalSpacing")
   RSRC_SET(IconMask,		Pixmap,		"iconMask")
   RSRC_SET(IconName,		char*,		"iconName")
   RSRC_SET(IconPixmap,		Pixmap,		"iconPixmap")
   RSRC_SET(IconWindow,		Widget,		"iconWindow")
   RSRC_SET(IconX,		int,		"iconX")
   RSRC_SET(IconY,		int,		"iconY")
   RSRC_SET(Iconic,		Boolean,	"iconic")
   RSRC_SET(Increment,		int,		"increment")
   RSRC_SET(IndicatorOn,	Boolean,	"indicatorOn")
   RSRC_SET(IndicatorSize,	Dimension,	"indicatorSize")
   RSRC_SET(IndicatorType,	unsigned char,	"indicatorType")
   RSRC_SET(InitialDelay,	int,		"initialDelay")
   RSRC_SET(InitialState,	int,		"initialState")
   RSRC_SET(Input,		Boolean,	"input")
   DONT_SET(InputCreate,	XtProc,		"inputCreate")
   RSRC_SET(InsertPosition,	XtOrderProc,	"insertPosition")
   RSRC_SET(IsAligned,		Boolean,	"isAligned")
   RSRC_SET(IsHomogeneous,	Boolean,	"isHomogeneous")
   RSRC_SET(ItemCount,		int,		"itemCount")
   RSRC_SET(Items,		XmStringTable,	"items")
   RSRC_SET(KeyboardFocusPolicy,	unsigned char,	"keyboardFocusPolicy")
   RSRC_SET(LabelFontList,	XmFontList,	"labelFontList")
   RSRC_SET(LabelInsensitivePixmap,	Pixmap,	"labelInsensitivePixmap")
   RSRC_SET(LabelPixmap,	Pixmap,		"labelPixmap")
   RSRC_SET(LabelString,	XmString,	"labelString")
   RSRC_SET(LabelType,		unsigned char,	"labelType")
   RSRC_SET(LeftAttachment,	unsigned char,	"leftAttachment")
   RSRC_SET(LeftOffset,		int,		"leftOffset")
   RSRC_SET(LeftPosition,	int,		"leftPosition")
   RSRC_SET(LeftWidget,		Widget,		"leftWidget")
   RSRC_SET(ListItemCount,	int,		"listItemCount")
   RSRC_SET(ListItems,		XmStringTable,	"listItems")
   RSRC_SET(ListLabelString,	XmString,	"listLabelString")
   RSRC_SET(ListMarginHeight,	short,		"listMarginHeight")
   RSRC_SET(ListMarginWidth,	short,		"listMarginWidth")
   RSRC_SET(ListSizePolicy,	unsigned char,	"listSizePolicy")
   RSRC_SET(ListSpacing,	short,		"listSpacing")
   RSRC_SET(ListUpdated,	Boolean,	"listUpdated")
   RSRC_SET(ListVisibleItemCount,	int,	"listVisibleItemCount")
   RSRC_SET(MainWindowMarginHeight,	short,	"mainWindowMarginHeight")
   RSRC_SET(MainWindowMarginWidth,	short,	"mainWindowMarginWidth")
   RSRC_SET(MappedWhenManaged,	Boolean,	"mappedWhenManaged")
   RSRC_SET(MappingDelay,	int,		"mappingDelay")
   RSRC_SET(Margin,		short,		"margin")
   RSRC_SET(MarginBottom,	short,		"marginBottom")
   RSRC_SET(MarginHeight,	Dimension,	"marginHeight")
   RSRC_SET(MarginLeft,		short,		"marginLeft")
   RSRC_SET(MarginRight,	short,		"marginRight")
   RSRC_SET(MarginTop,		short,		"marginTop")
   RSRC_SET(MarginWidth,	Dimension,	"marginWidth")
   RSRC_SET(MaxAspectX,		int,		"maxAspectX")
   RSRC_SET(MaxAspectY,		int,		"maxAspectY")
   RSRC_SET(MaxHeight,		int,		"maxHeight")
   RSRC_SET(MaxLength,		int,		"maxLength")
   RSRC_SET(MaxWidth,		int,		"maxWidth")
   RSRC_SET(Maximum,		int,		"maximum")
   RSRC_SET(MenuAccelerator,	char*,		"menuAccelerator")
   RSRC_SET(MenuBar,		Widget,		"menuBar")
   RSRC_SET(MenuHelpWidget,	Widget,		"menuHelpWidget")
   RSRC_SET(MenuHistory,	Widget,		"menuHistory")
   RSRC_SET(MessageAlignment,	unsigned char,	"messageAlignment")
   RSRC_SET(MessageString,	XmString,	"messageString")
   RSRC_SET(MessageWindow,	Widget,		"messageWindow")
   RSRC_SET(MinAspectX,		int,		"minAspectX")
   RSRC_SET(MinAspectY,		int,		"minAspectY")
   RSRC_SET(MinHeight,		int,		"minHeight")
   RSRC_SET(MinWidth,		int,		"minWidth")
   RSRC_SET(MinimizeButtons,	Boolean,	"minimizeButtons")
   RSRC_SET(Minimum,		int,		"minimum")
   RSRC_SET(Mnemonic,		char,		"mnemonic")
   RSRC_SET(MultiClick,		unsigned char,	"multiClick")
   RSRC_SET(MustMatch,		Boolean,	"mustMatch")
   RSRC_SET(MwmDecorations,	int,		"mwmDecorations")
   RSRC_SET(MwmFunctions,	int,		"mwmFunctions")
   RSRC_SET(MwmInputMode,	int,		"mwmInputMode")
   RSRC_SET(MwmMenu,		char*,		"mwmMenu")
   RSRC_SET(NavigationType,	XmNavigationType,	"navigationType")
   RSRC_SET(NoResize,		Boolean,	"noResize")
   RSRC_SET(NumColumns,		short,		"numColumns")
   RSRC_SET(OkLabelString,	XmString,	"okLabelString")
   RSRC_SET(Orientation,	unsigned char,	"orientation")
   DONT_SET(OutputCreate,	XtProc,		"outputCreate")
   RSRC_SET(OverrideRedirect,	Boolean,	"overrideRedirect")
   RSRC_SET(Packing,		unsigned char,	"packing")
   RSRC_SET(PageIncrement,	int,		"pageIncrement")
   RSRC_SET(PaneMaximum,	Dimension,	"paneMaximum")
   RSRC_SET(PaneMinimum,	Dimension,	"paneMinimum")
   RSRC_SET(Pattern,		XmString,	"pattern")
   RSRC_SET(PendingDelete,	Boolean,	"pendingDelete")
   RSRC_SET(PopupEnabled,	Boolean,	"popupEnabled")
   RSRC_SET(PositionIndex,	short,		"positionIndex")
   RSRC_SET(ProcessingDirection,	unsigned char,	"processingDirection")
   RSRC_SET(PromptString,	XmString,	"promptString")
   RSRC_SET(PushButtonEnabled,	Boolean,	"pushButtonEnabled")
   RSRC_SET(RadioAlwaysOne,	Boolean,	"radioAlwaysOne")
   RSRC_SET(RadioBehavior,	Boolean,	"radioBehavior")
   RSRC_SET(RecomputeSize,	Boolean,	"recomputeSize")
   RSRC_SET(RefigureMode,	Boolean,	"refigureMode")
   RSRC_SET(RepeatDelay,	int,		"repeatDelay")
   RSRC_SET(Resizable,		Boolean,	"resizable")
   RSRC_SET(ResizeHeight,	Boolean,	"resizeHeight")
   RSRC_SET(ResizePolicy,	unsigned char,	"resizePolicy")
   RSRC_SET(ResizeWidth,	Boolean,	"resizeWidth")
   RSRC_SET(RightAttachment,	unsigned char,	"rightAttachment")
   RSRC_SET(RightOffset,	int,		"rightOffset")
   RSRC_SET(RightPosition,	int,		"rightPosition")
   RSRC_SET(RightWidget,	Widget,		"rightWidget")
   RSRC_SET(RowColumnType,	unsigned char,	"rowColumnType")
   RSRC_SET(Rows,		short,		"rows")
   RSRC_SET(RubberPositioning,	Boolean,	"rubberPositioning")
   RSRC_SET(SashHeight,		Dimension,	"sashHeight")
   RSRC_SET(SashIndent,		Position,	"sashIndent")
   RSRC_SET(SashShadowThickness,	int,	"sashShadowThickness")
   RSRC_SET(SashWidth,		Dimension,	"sashWidth")
   RSRC_SET(SaveUnder,		Boolean,	"saveUnder")
   RSRC_SET(ScaleHeight,	Dimension,	"scaleHeight")
   RSRC_SET(ScaleWidth,		Dimension,	"scaleWidth")
   RSRC_SET(ScrollBarDisplayPolicy,	unsigned char,	"scrollBarDisplayPolicy")
   RSRC_SET(ScrollBarPlacement,	unsigned char,	"scrollBarPlacement")
   RSRC_SET(ScrollHorizontal,	Boolean,	"scrollHorizontal")
   RSRC_SET(ScrollLeftSide,	Boolean,	"scrollLeftSide")
   RSRC_SET(ScrollTopSide,	Boolean,	"scrollTopSide")
   RSRC_SET(ScrollVertical,	Boolean,	"scrollVertical")
   RSRC_SET(ScrolledWindowMarginHeight,	short,	"scrolledWindowMarginHeight")
   RSRC_SET(ScrolledWindowMarginWidth,	short,	"scrolledWindowMarginWidth")
   RSRC_SET(ScrollingPolicy,	unsigned char,	"scrollingPolicy")
   RSRC_SET(SelectColor,	Pixel,		"selectColor")
   RSRC_SET(SelectInsensitivePixmap,	Pixmap,	"selectInsensitivePixmap")
   RSRC_SET(SelectPixmap,	Pixmap,		"selectPixmap")
   RSRC_SET(SelectThreshold,	int,		"selectThreshold")
   RSRC_SET(SelectedItemCount,	int,		"selectedItemCount")
   RSRC_SET(SelectedItems,	XmStringTable,	"selectedItems")
   RSRC_SET(SelectionArray,	XtPointer,	"selectionArray")
   RSRC_SET(SelectionArrayCount,	int,	"selectionArrayCount")
   RSRC_SET(SelectionLabelString,	XmString,	"selectionLabelString")
   RSRC_SET(SelectionPolicy,	unsigned char,	"selectionPolicy")
   RSRC_SET(Sensitive,		Boolean,	"sensitive")
   RSRC_SET(SeparatorOn,	Boolean,	"separatorOn")
   RSRC_SET(SeparatorType,	unsigned char,	"separatorType")
   RSRC_SET(Set,		Boolean,	"set")
   RSRC_SET(ShadowThickness,	short,		"shadowThickness")
   RSRC_SET(ShadowType,		unsigned char,	"shadowType")
   RSRC_SET(ShellUnitType,	unsigned char,	"shellUnitType")
   RSRC_SET(ShowArrows,		Boolean,	"showArrows")
   RSRC_SET(ShowAsDefault,	short,		"showAsDefault")
   RSRC_SET(ShowSeparator,	Boolean,	"showSeparator")
   RSRC_SET(ShowValue,		Boolean,	"showValue")
   RSRC_SET(SkipAdjust,		Boolean,	"skipAdjust")
   RSRC_SET(SliderSize,		int,		"sliderSize")
   DONT_SET(Source,		XmTextSource,	"source")
   RSRC_SET(Spacing,		Dimension,	"spacing")
   RSRC_SET(StringDirection,	unsigned char,	"stringDirection")
   RSRC_SET(SubMenuId,		Widget,		"subMenuId")
   RSRC_SET(SymbolPixmap,	Pixmap,		"symbolPixmap")
   RSRC_SET(TextAccelerators,	XtTranslations,	"textAccelerators")
   RSRC_SET(TextColumns,	short,		"textColumns")
   RSRC_SET(TextFontList,	XmFontList,	"textFontList")
   RSRC_SET(TextString,		XmString,	"textString")
   RSRC_SET(TextTranslations,	XtTranslations,	"textTranslations")
   RSRC_SET(Title,		char*,		"title")
   RSRC_SET(TitleString,	XmString,	"titleString")
   RSRC_SET(TopAttachment,	unsigned char,	"topAttachment")
   RSRC_SET(TopCharacter,	XmTextPosition,	"topCharacter")
   RSRC_SET(TopItemPosition,	int,		"topItemPosition")
   RSRC_SET(TopOffset,		int,		"topOffset")
   RSRC_SET(TopPosition,	int,		"topPosition")
   RSRC_SET(TopShadowColor,	Pixel,		"topShadowColor")
   RSRC_SET(TopShadowPixmap,	Pixmap,		"topShadowPixmap")
   RSRC_SET(TopWidget,		Widget,		"topWidget")
   RSRC_SET(Transient,		Boolean,	"transient")
   RSRC_SET(Translations,	XtTranslations,	"translations")
   RSRC_SET(TraversalOn,	Boolean,	"traversalOn")
   RSRC_SET(UnitType,		unsigned char,	"unitType")
   RSRC_SET(UserData,		void*,		"userData")
   RSRC_SET(Value,		char*,		"value")
   RSRC_SET(Value,		int,		"value")
   RSRC_SET(VerifyBell,		Boolean,	"verifyBell")
   RSRC_SET(VerticalScrollBar,	Widget,		"verticalScrollBar")
   RSRC_SET(VerticalSpacing,	int,		"verticalSpacing")
   RSRC_SET(VisibleItemCount,	int,		"visibleItemCount")
   RSRC_SET(VisibleWhenOff,	Boolean,	"visibleWhenOff")
   RSRC_SET(VisualPolicy,	unsigned char,	"visualPolicy")
   RSRC_SET(Waitforwm,		Boolean,	"waitforwm")
   RSRC_SET(WhichButton,	unsigned int,	"whichButton")
   RSRC_SET(Width,		Dimension,	"width")
   RSRC_SET(WidthInc,		int,		"widthInc")
   RSRC_SET(WindowGroup,	Widget,		"windowGroup")
   RSRC_SET(WmTimeout,		int,		"wmTimeout")
   RSRC_SET(WordWrap,		Boolean,	"wordWrap")
   RSRC_SET(WorkWindow,		Widget,		"workWindow")
   RSRC_SET(X,			Position,	"x")
   RSRC_SET(Y,			Position,	"y")
   RSRC_SET(_Screen,		void*,		"screen")

#if XmVersion >= 1002
// Constraint Resources -- Valid only in a frame
   RSRC_SET(ChildType,		      unsigned char, "childType")
   RSRC_SET(ChildHorizontalAlignment, unsigned char, "childHorizontalAlignment")
   RSRC_SET(ChildHorizontalSpacing,   Dimension,     "childHorizontalSpacing")
   RSRC_SET(ChildVerticalAlignment,   unsigned char, "childVerticalAlignment")
#endif

   inline WArgList& LeftAttachment(unsigned char to_what, Widget w) {
      LeftAttachment(to_what);
      LeftWidget(w);
      return *this;
   }
   inline WArgList& LeftAttachment(unsigned char to_what, Widget w,int off) {
      LeftAttachment(to_what);
      LeftWidget(w);
      LeftOffset(off);
      return *this;
   }
   inline WArgList& LeftAttachment(unsigned char to_what, int posoff) {
      LeftAttachment(to_what);
      if( to_what == XmATTACH_POSITION ) LeftPosition(posoff);
      else				 LeftOffset(posoff);
      return *this;
   }
   inline WArgList& LeftAttachment(unsigned char to_what, int pos, int off) {
      LeftAttachment(to_what);
      LeftPosition(pos);
      LeftOffset(off);
      return *this;
   }

   inline WArgList& RightAttachment(unsigned char to_what, Widget w) {
      RightAttachment(to_what);
      RightWidget(w);
      return *this;
   }
   inline WArgList& RightAttachment(unsigned char to_what, Widget w,int off) {
      RightAttachment(to_what);
      RightWidget(w);
      RightOffset(off);
      return *this;
   }
   inline WArgList& RightAttachment(unsigned char to_what, int posoff) {
      RightAttachment(to_what);
      if( to_what == XmATTACH_POSITION ) RightPosition(posoff);
      else				 RightOffset(posoff);
      return *this;
   }
   inline WArgList& RightAttachment(unsigned char to_what, int pos, int off) {
      RightAttachment(to_what);
      RightPosition(pos);
      RightOffset(off);
      return *this;
   }

   inline WArgList& TopAttachment(unsigned char to_what, Widget w) {
      TopAttachment(to_what);
      TopWidget(w);
      return *this;
   }
   inline WArgList& TopAttachment(unsigned char to_what, Widget w,int off) {
      TopAttachment(to_what);
      TopWidget(w);
      TopOffset(off);
      return *this;
   }
   inline WArgList& TopAttachment(unsigned char to_what, int posoff) {
      TopAttachment(to_what);
      if( to_what == XmATTACH_POSITION ) TopPosition(posoff);
      else				 TopOffset(posoff);
      return *this;
   }
   inline WArgList& TopAttachment(unsigned char to_what, int pos, int off) {
      TopAttachment(to_what);
      TopPosition(pos);
      TopOffset(off);
      return *this;
   }

   inline WArgList& BottomAttachment(unsigned char to_what, Widget w) {
      BottomAttachment(to_what);
      BottomWidget(w);
      return *this;
   }
   inline WArgList& BottomAttachment(unsigned char to_what, Widget w,int off) {
      BottomAttachment(to_what);
      BottomWidget(w);
      BottomOffset(off);
      return *this;
   }
   inline WArgList& BottomAttachment(unsigned char to_what, int posoff) {
      BottomAttachment(to_what);
      if( to_what == XmATTACH_POSITION ) BottomPosition(posoff);
      else				 BottomOffset(posoff);
      return *this;
   }
   inline WArgList& BottomAttachment(unsigned char to_what, int pos, int off) {
      BottomAttachment(to_what);
      BottomPosition(pos);
      BottomOffset(off);
      return *this;
   }
};

inline Arg&
WArgList::operator [] (Cardinal n)
{
   if (n < num_args|| n < alloc_args) return args[n];
   else				      return Grow(n);
}


#endif // WArgList_h
