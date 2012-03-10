Dtinfo.version:  $XConsortium: Dtinfo.res /main/25 1996/11/26 15:47:44 cde-hal $

!x Comments starting with "!x" will not appear in the output file.
!x Comments starting with "! " will appear in the output file.

Dtinfo*FontSans:         -dt-application-medium-r-normal-sans-*-*-*-*-p-*-iso8859-1
Dtinfo*FontSerif:        -dt-application-medium-r-normal-serif-*-*-*-*-p-*-iso8859-1
Dtinfo*FontMono:         -dt-application-medium-r-normal-serif-*-*-*-*-m-*-iso8859-1
Dtinfo*FontSymbol:       -dt-application-medium-r-normal-*-*-*-*-*-p-*-dtsymbol-1

Dtinfo.BorderWidth:	2
Dtinfo*highlightThickness:	1

!x #include "HelpDialog.res"
#include "Help.res"
#include "Macros.res"

#include "Query/QueryEditor.res"
#include "Query/QueryTermView.res"
#include "Agents/BookmarkEdit.res"
#include "Agents/MarkList.res"
#include "Agents/NodeHistoryAgent.res"
#include "Agents/SearchResultsAgent.res"
#include "Agents/LibraryAgent.res"
#include "Agents/NodeWindowAgent.res"
#include "Agents/PrintPanelAgent.res"
#include "Agents/FileSelect.res"
#include "Agents/Preferences.res"
#include "Agents/GraphicAgent.res"
#include "Agents/ScopeEditor.res"
#include "Agents/AddLibraryAgent.res"
#include "Agents/UrlAgent.res"

!	DtHelp dialog resources
! DtHelp does not allow external resource control of initial dialog height
!Dtinfo*helpdialog.height:		360
!Dtinfo*helpdialog_popup.height:	360
Dtinfo.helpdialog_popup.minHeight:	175
Dtinfo.helpdialog_popup.minWidth:	300
Dtinfo.helpdialog_popup*panedWindow.DisplayAreaForm.paneMinimum:	180
! DtHelp does not allow pathForm initial height control, so default
! paneMinimum for DisplayAreaForm is 180 instead of smaller...
Dtinfo.helpdialog_popup*panedWindow.pathForm.height:			160
Dtinfo.helpdialog_popup*panedWindow.pathForm.paneMinimum:		18

Dtinfo*ExpandableMap:	False

Dtinfo*messageDialogMinWidth: 350

Dtinfo*MapHighlightBackground:        black
Dtinfo*MapHighlightForeground:        white

Dtinfo*display_area.searchColor:       blue

Dtinfo*XmSeparatorGadget.highlightThickness: 0

! Icons and icon names for iconified primary windows

Dtinfo.node_hist.IconName: Section
Dtinfo*node_hist.iconPixmap: Dthist.l.pm

Dtinfo.doc_tree.IconName: Books
Dtinfo*doc_tree.iconPixmap: Dtinfo.l.pm

Dtinfo.mark_list.IconName: Marks
Dtinfo*mark_list.iconPixmap: Dtbmark.l.pm

Dtinfo.search_history.IconName: Search
Dtinfo*search_history.iconPixmap: Dtsrch.l.pm

Dtinfo.query_editor.IconName: Query
Dtinfo*query_editor.iconPixmap: Dteditqry.l.pm

Dtinfo.scope_editor.IconName: Scope
Dtinfo*scope_editor.iconPixmap: Dtscope_editor.l.pm

Dtinfo.node_view.IconName: Browser
Dtinfo*node_view.iconPixmap: Dtview.l.pm

Dtinfo.map.IconName: Map
Dtinfo*map.iconPixmap: Dtgraphmap.l.pm

! set printing background to white to get rid of 
! unsightly grey square

Dtinfo*page.background: white
Dtinfo*page.DisplayArea.background: white
Dtinfo*PrintShell*background: white

Dtinfo*footer.bookname.fontList: -dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*
Dtinfo*footer.page_no.fontList: -dt-interface user-medium-r-normal-xs*-*-*-*-*-*-*-*-*

! Dtinfo*KeyboardFocusPolicy:    Pointer

!Dtinfo.title:                            Dtinfo
!Dtinfo.deleteResponse:                   DO_NOTHING

!Dtinfo.geometry:                         72x186
! NOTE: For some reason Motif adds minHeight to geometry hight above...
!Dtinfo.minHeight:                        102
!Dtinfo.minWidth:                         0

Dtinfo
  MessageDialog
    MessageDialog
      defaultPosition:  False
      dialogStyle:      dialog_full_application_modal
      messageAlignment: alignment_center
      Help
        labelString:    Help
        sensitive:      False
      text
        verifyBell: on
        maxLength: 40

Dtinfo
  core_dump
    *background: red
    core_dump
      symbolPixmap: bomb32
      messageAlignment: alignment_center
  
Dtinfo*PinForeground: white

Dtinfo*RasterForeground: black
Dtinfo*RasterBackground: white

#include "Agents/MapAgent.res"

!! Then: need to make sure that Frame recalcs title position on resize

! *mark_icon.armColor: black
Dtinfo*mark_icon.labelType: pixmap
Dtinfo*mark_icon.shadowThickness: 1
Dtinfo*mark_icon.marginHeight: 0
Dtinfo*mark_icon.marginWidth: 0
Dtinfo*mark_icon.highlightThickness: 1
! *icon_popup.radioBehavior: true
! *popup_icon_popup*tearOffModel: tear_off_disabled

#include "Agents/SearchHistoryList.res"

