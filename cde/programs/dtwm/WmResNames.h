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
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*
 * (c) Copyright 1987, 1988, 1989, 1990, 1993 HEWLETT-PACKARD COMPANY */

/*
 * Included Files:
 */

/*
 * Value definitions:
 */



/******************************<->*************************************
 *
 *  Window manager resource names ...
 *
 *
 *  Description:
 *  -----------
 * 
 ******************************<->***********************************/

/* mwm specific appearance and behavior resources: */

#define WmNall				"all"
#define WmNautoKeyFocus			"autoKeyFocus"
#define WmNautoRaiseDelay		"autoRaiseDelay"
#define WmNbackdropDirectories		"backdropDirectories"
#define WmNbitmapDirectory		"bitmapDirectory"
#define WmNblinkOnExec			"blinkOnExec"
#define WmNbuttonBindings		"buttonBindings"
#define WmNcleanText			"cleanText"
#define WmNclientAutoPlace		"clientAutoPlace"
#define WmNcolormapFocusPolicy		"colormapFocusPolicy"
#define WmNconfigFile			"configFile"
#define WmNcppCommand			"cppCommand"
#define WmNdeiconifyKeyFocus		"deiconifyKeyFocus"
#define WmNdoubleClickTime		"doubleClickTime"
#define WmNenableWarp			"enableWarp"
#define WmNenforceKeyFocus		"enforceKeyFocus"
#define WmNfadeNormalIcon		"fadeNormalIcon"
#define WmNfeedbackGeometry		"feedbackGeometry"
#define WmNframeBorderWidth		"frameBorderWidth"
#define WmNframeExternalShadowWidth	"frameExternalShadowWidth"
#define WmNfreezeOnConfig		"freezeOnConfig"
#define WmNgeometry			"geometry"
#define WmNiconAutoPlace		"iconAutoPlace"
#define WmNiconBoxGeometry		"iconBoxGeometry"
#define WmNiconBoxLayout		"iconBoxLayout"
#define WmNiconBoxName			"iconBoxName"
#define WmNiconBoxSBDisplayPolicy	"iconBoxSBDisplayPolicy"
#define WmNiconBoxScheme		"iconBoxScheme"
#define WmNiconBoxTitle			"iconBoxTitle"
#define WmNiconClick			"iconClick"
#define WmNiconDecoration		"iconDecoration"
#define WmNiconExternalShadowWidth	"iconExternalShadowWidth"
#define WmNiconImageMaximum		"iconImageMaximum"
#define WmNiconImageMinimum		"iconImageMinimum"
#define WmNiconPlacement		"iconPlacement"
#define WmNiconPlacementMargin		"iconPlacementMargin"
#define WmNimage			"image"
#define WmNimageBackground		"imageBackground"
#define WmNimageForeground		"imageForeground"
#define WmNinteractivePlacement		"interactivePlacement"
#define WmNkeyBindings			"keyBindings"
#define WmNkeyboardFocusPolicy		"keyboardFocusPolicy"
#define WmNlimitResize			"limitResize"
#define WmNlowerOnIconify		"lowerOnIconify"
#define WmNmarqueeSelectGranularity	"marqueeSelectGranularity"
#define WmNmaximumMaximumSize		"maximumMaximumSize"
#define WmNmoveThreshold		"moveThreshold"
#define WmNmultiScreen			"multiScreen"
#define WmNpassButtons			"passButtons"
#define WmNpassSelectButton		"passSelectButton"
#define WmNpositionIsFrame		"positionIsFrame"
#define WmNpositionOnScreen		"positionOnScreen"
#define WmNquitTimeout			"quitTimeout"
#define WmNraiseKeyFocus		"raiseKeyFocus"
#define WmNrefreshByClearing		"refreshByClearing"
#define WmNresizeBorderWidth		"resizeBorderWidth"
#define WmNresizeCursors		"resizeCursors"
#define WmNrootButtonClick		"rootButtonClick"
#define WmNsecondariesOnTop		"secondariesOnTop"
#define WmNsessionVersion               "sessionVersion"
#define WmNshowFeedback			"showFeedback"
#define WmNshowNames			"showNames"
#define WmNstartupKeyFocus		"startupKeyFocus"
#define WmNsubpanelDecoration		"subpanelDecoration"
#define WmNsubpanelResources            "subpanelResources"
#define WmNsystemButtonClick		"wMenuButtonClick"
#define WmNsystemButtonClick2		"wMenuButtonClick2"
#define WmNtransientDecoration		"transientDecoration"
#define WmNtransientFunctions		"transientFunctions"
#define WmNuseFrontPanel		"useFrontPanel"
#define WmNuseIconBox			"useIconBox"
#define WmNuseWindowOutline		"useWindowOutline"
#define WmNdtLite			"useDtLite"
#define WmNmoveOpaque                   "moveOpaque"
#define WmNframeStyle			"frameStyle"

/* conponent appearance resources: */

#define WmNactiveBackground		"activeBackground"
#define WmNactiveBackgroundPixmap	"activeBackgroundPixmap"
#define WmNactiveBottomShadowColor 	"activeBottomShadowColor"
#define WmNactiveBottomShadowPixmap	"activeBottomShadowPixmap"
#define WmNactiveForeground		"activeForeground"
#define WmNactiveTopShadowColor		"activeTopShadowColor"
#define WmNactiveTopShadowPixmap	"activeTopShadowPixmap"
#define WmNbackground			"background"
#define WmNbackgroundPixmap		"backgroundPixmap"
#define WmNbottomShadowColor 		"bottomShadowColor"
#define WmNbottomShadowPixmap		"bottomShadowPixmap"
#define WmNfont				"font"
#define WmNforeground			"foreground"
#define WmNsaveUnder			"saveUnder"
#define WmNtopShadowColor		"topShadowColor"
#define WmNtopShadowPixmap		"topShadowPixmap"

/* mwm - client specific resources: */

#define WmNabsentMapBehavior		"absentMapBehavior"
#define WmNclientDecoration		"clientDecoration"
#define WmNclientFunctions		"clientFunctions"
#define WmNfocusAutoRaise		"focusAutoRaise"
#define WmNhelpResources                "helpResources"
#define WmNiconImage			"iconImage"
#define WmNiconImageBackground		"iconImageBackground"
#define WmNiconImageBottomShadowColor	"iconImageBottomShadowColor"
#define WmNiconImageBottomShadowPixmap	"iconImageBottomShadowPixmap"
#define WmNiconImageForeground		"iconImageForeground"
#define WmNiconImageTopShadowColor	"iconImageTopShadowColor"
#define WmNiconImageTopShadowPixmap	"iconImageTopShadowPixmap"
#define WmNignoreWMSaveHints            "ignoreWMSaveHints"
#define WmNinitialWorkspace             "initialWorkspace"
#define WmNmatteBackground		"matteBackground"
#define WmNmatteBottomShadowColor	"matteBottomShadowColor"
#define WmNmatteBottomShadowPixmap	"matteBottomShadowPixmap"
#define WmNmatteForeground		"matteForeground"
#define WmNmatteTopShadowColor		"matteTopShadowColor"
#define WmNmatteTopShadowPixmap		"matteTopShadowPixmap"
#define WmNmatteWidth			"matteWidth"
#define WmNmaximumClientSize		"maximumClientSize"
#define WmNscreenList			"screenList"
#define WmNscreens			"screens"
#define WmNsystemMenu			"windowMenu"
#define WmNuseClientIcon		"useClientIcon"
#define WmNusePPosition			"usePPosition"
#define WmNworkspaceList	        "workspaceList"
#define WmNworkspaceCount	        "workspaceCount"

/* window manager part resource names: */

#define WmNclient			"client"
#define WmNfeedback			"feedback"
#define WmNicon				"icon"
#define WmNmenu				"menu"
#define WmNtitle			"title"
#define WmNdefaults			"defaults"
#define WmNbackdrop			"backdrop"
#define WmNcolorSetId			"colorSetId"
#define WmNfrontPanel			"frontPanel"
#define WmNworkspaceController		"workspaceController"
#define WmNworkspacePresence		"workspacePresence"
#define WmNworkspaceSwitch		"workspaceSwitch"

/* window manager client resource names: */

#define WmNiconBox			"iconbox"
#define WmNconfirmbox			"confirmbox"
#define WmNswitcher			"switcher"



/*************************************<->*************************************
 *
 *  Window manager resource classes ...
 *
 *
 *  Description:
 *  -----------
 * 
 *************************************<->***********************************/

/* mwm specific appearance and behavior resources: */

#define WmCAutoKeyFocus			"AutoKeyFocus"
#define WmCAutoRaiseDelay		"AutoRaiseDelay"
#define WmCBackdropDirectories		"BackdropDirectories"
#define WmCBitmapDirectory		"BitmapDirectory"
#define WmCBlinkOnExec			"BlinkOnExec"
#define WmCButtonBindings		"ButtonBindings"
#define WmCCleanText			"CleanText"
#define WmCClientAutoPlace		"ClientAutoPlace"
#define WmCColormapFocusPolicy		"ColormapFocusPolicy"
#define WmCConfigFile			"ConfigFile"
#define WmCCppCommand			"CppCommand"
#define WmCDeiconifyKeyFocus		"DeiconifyKeyFocus"
#define WmCDoubleClickTime		"DoubleClickTime"
#define WmCEnableWarp			"EnableWarp"
#define WmCEnforceKeyFocus		"EnforceKeyFocus"
#define WmCFadeNormalIcon		"FadeNormalIcon"
#define WmCFeedbackGeometry		"FeedbackGeometry"
#define WmCFrameBorderWidth		"FrameBorderWidth"
#define WmCFrameExternalShadowWidth	"FrameExternalShadowWidth"
#define WmCFreezeOnConfig		"FreezeOnConfig"
#define WmCGeometry			"Geometry"
#define WmCIconAutoPlace		"IconAutoPlace"
#define WmCIconBoxGeometry		"IconBoxGeometry"
#define WmCIconBoxLayout		"IconBoxLayout"
#define WmCIconBoxName			"IconBoxName"
#define WmCIconBoxSBDisplayPolicy	"IconBoxSBDisplayPolicy"
#define WmCIconBoxScheme		"IconBoxScheme"
#define WmCIconBoxTitle			"IconBoxTitle"
#define WmCIconClick			"IconClick"
#define WmCIconDecoration		"IconDecoration"
#define WmCIconExternalShadowWidth	"IconExternalShadowWidth"
#define WmCIconImageMaximum		"IconImageMaximum"
#define WmCIconImageMinimum		"IconImageMinimum"
#define WmCIconPlacement		"IconPlacement"
#define WmCIconPlacementMargin		"IconPlacementMargin"
#define WmCImage			"Image"
#define WmCImageBackground		"ImageBackground"
#define WmCImageForeground		"ImageForeground"
#define WmCInteractivePlacement		"InteractivePlacement"
#define WmCKeyBindings			"KeyBindings"
#define WmCKeyboardFocusPolicy		"KeyboardFocusPolicy"
#define WmCLimitResize			"LimitResize"
#define WmCLowerOnIconify		"LowerOnIconify"
#define WmCMarqueeSelectGranularity	"MarqueeSelectGranularity"
#define WmCMaximumMaximumSize		"MaximumMaximumSize"
#define WmCMoveThreshold		"MoveThreshold"
#define WmCMultiScreen			"MultiScreen"
#define WmCPassButtons			"PassButtons"
#define WmCPassSelectButton		"PassSelectButton"
#define WmCPositionIsFrame		"PositionIsFrame"
#define WmCPositionOnScreen		"PositionOnScreen"
#define WmCQuitTimeout			"QuitTimeout"
#define WmCRaiseKeyFocus		"RaiseKeyFocus"
#define WmCRefreshByClearing		"RefreshByClearing"
#define WmCResizeBorderWidth		"ResizeBorderWidth"
#define WmCResizeCursors		"ResizeCursors"
#define WmCRootButtonClick		"RootButtonClick"
#define WmCSecondariesOnTop		"SecondariesOnTop"
#define WmCSessionVersion               "SessionVersion"
#define WmCScreenList			"ScreenList"
#define WmCScreens			"Screens"
#define WmCShowFeedback			"ShowFeedback"
#define WmCShowNames			"ShowNames"
#define WmCStartupKeyFocus		"StartupKeyFocus"
#define WmCSubpanelDecoration		"SubpanelDecoration"
#define WmCSubpanelResources            "SubpanelResources"
#define WmCSystemButtonClick		"WMenuButtonClick"
#define WmCSystemButtonClick2		"WMenuButtonClick2"
#define WmCTransientDecoration		"TransientDecoration"
#define WmCTransientFunctions		"TransientFunctions"
#define WmCUseFrontPanel		"UseFrontPanel"
#define WmCUseIconBox			"UseIconBox"
#define WmCUseWindowOutline		"UseWindowOutline"
#define WmCDtLite			"UseDtLite"
#define WmCMoveOpaque                   "MoveOpaque"
#define WmCFrameStyle			"FrameStyle"
#define WmCWorkspaceList	        "WorkspaceList"
#define WmCWorkspaceCount	        "WorkspaceCount"

/* component appearance resources: */

#define WmCActiveBackground		"ActiveBackground"
#define WmCActiveBackgroundPixmap	"ActiveBackgroundPixmap"
#define WmCActiveBottomShadowColor 	"ActiveBottomShadowColor"
#define WmCActiveBottomShadowPixmap	"ActiveBottomShadowPixmap"
#define WmCActiveForeground		"ActiveForeground"
#define WmCActiveTopShadowColor		"ActiveTopShadowColor"
#define WmCActiveTopShadowPixmap	"ActiveTopShadowPixmap"
#define WmCBackground			"Background"
#define WmCBackgroundPixmap		"BackgroundPixmap"
#define WmCBottomShadowColor 		"BottomShadowColor"
#define WmCBottomShadowPixmap		"BottomShadowPixmap"
#define WmCFont				"Font"
#define WmCForeground			"Foreground"
#define WmCSaveUnder			"SaveUnder"
#define WmCTopShadowColor		"TopShadowColor"
#define WmCTopShadowPixmap		"TopShadowPixmap"

/* mwm - client specific resources: */

#define WmCAbsentMapBehavior		"AbsentMapBehavior"
#define WmCClientDecoration		"ClientDecoration"
#define WmCClientFunctions		"ClientFunctions"
#define WmCFocusAutoRaise		"FocusAutoRaise"
#define WmCHelpResources                "HelpResources"
#define WmCIconImage			"IconImage"
#define WmCIconImageBackground		"IconImageBackground"
#define WmCIconImageBottomShadowColor	"IconImageBottomShadowColor"
#define WmCIconImageBottomShadowPixmap	"IconImageBottomShadowPixmap"
#define WmCIconImageForeground		"IconImageForeground"
#define WmCIconImageTopShadowColor	"IconImageTopShadowColor"
#define WmCIconImageTopShadowPixmap	"IconImageTopShadowPixmap"
#define WmCIgnoreWMSaveHints            "IgnoreWMSaveHints"
#define WmCInitialWorkspace             "InitialWorkspace"
#define WmCMatteBackground		"MatteBackground"
#define WmCMatteBottomShadowColor	"MatteBottomShadowColor"
#define WmCMatteBottomShadowPixmap	"MatteBottomShadowPixmap"
#define WmCMatteForeground		"MatteForeground"
#define WmCMatteTopShadowColor		"MatteTopShadowColor"
#define WmCMatteTopShadowPixmap		"MatteTopShadowPixmap"
#define WmCMatteWidth			"MatteWidth"
#define WmCMaximumClientSize		"MaximumClientSize"
#define WmCSystemMenu			"WindowMenu"
#define WmCUseClientIcon		"UseClientIcon"
#define WmCUsePPosition			"UsePPosition"

/* window manager part resource names: */

#define WmCClient			"Client"
#define WmCFeedback			"Feedback"
#define WmCIcon				"Icon"
#define WmCMenu				"Menu"
#define WmCTitle			"Title"
#define WmCDefaults			"Defaults"
#define WmCBackdrop			"Backdrop"
#define WmCColorSetId			"ColorSetId"
#define WmCFrontPanel			"FrontPanel"
#define WmCWorkspaceController		"WorkspaceController"
#define WmCWorkspacePresence		"WorkspacePresence"
#define WmCWorkspaceSwitch		"WorkspaceSwitch"

/* window manager client resource names: */

#define WmCIconBox			"Iconbox"
#define WmCConfirmbox			"Confirmbox"
#define WmCSwitcher			"Switcher"



/*************************************<->*************************************
 *
 *  Window manager resource converter names ...
 *
 *
 *  Description:
 *  -----------
 * 
 *************************************<->***********************************/

#define WmRAbsentMapBehavior            "WmAMBehavior"
#define WmRCFocusPolicy			"WmCFocus"
#define WmRClientDecor			"WmCDecor"
#define WmRClientFunction		"WmCFunc"
#define WmRFrameStyle			"WmFrameStyle"
#define WmRIconBoxLayout		"WmIBLayout"
#define WmRIconDecor			"WmIDecor"
#define WmRIconPlacement		"WmIPlace"
#define WmRKFocusPolicy			"WmKFocus"
#define WmRSize				"WmSize"
#define WmRShowFeedback			"WmShowFeedback"
#define WmRUsePPosition			"WmUsePPosition"



/*************************************<->*************************************
 *
 *  Window manager resource set definitions and default resource values ...
 *
 *
 *  Description:
 *  -----------
 * 
 *************************************<->***********************************/




/*************************************<->*************************************
 *
 *  Mwm resource description file definitions ...
 *
 *
 *  Description:
 *  -----------
 * 
 *************************************<->***********************************/

/* Configuration resource types: */

#define CRS_BUTTON		(1L << 0)
#define CRS_KEY			(1L << 1)
#define CRS_MENU		(1L << 2)
#define CRS_ACCEL		(1L << 3)
#define CRS_ANY			(CRS_BUTTON | CRS_KEY | CRS_MENU | CRS_ACCEL)

#define WmNhelpDirectory		     "helpDirectory"

#define WmCHelpDirectory		     "HelpDirectory"

/**************************    eof  ************************/

