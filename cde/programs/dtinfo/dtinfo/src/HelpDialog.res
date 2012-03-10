! $XConsortium: HelpDialog.res /main/3 1996/07/18 15:08:49 drk $

*helpcard*helpText.rows:        10
*helpcard*helpText.columns:     40
*helpcard*helpText.wordWrap:    True

*helpcard.dialogTitle:          Dtinfo Help
!!*helpcard.symbolPixmap:       help.xbm
*helpcard.okLabelString:        Ok
*helpcard.cancelLabelString:    More...
*helpcard.helpLabelString:      Help

*helpHelpDialog.dialogTitle:   Help for Dtinfo Help
*helpHelpDialog.messageString:\
The help window provides hints on using this application.\n\
\nClick [Ok] to return to the application.\n\
\nClick [More] to read more detailed information in the browser.\n\
\nClick [Help] to display this dialog.

*helpErrorDialog.dialogTitle: Dtinfo Help Error
*helpErrorDialog.messageString: 
Dtinfo cannot find more detailed information\n\
pertaining to the currently displayed help.


*helpIndex.dialogTitle:         Dtinfo Help Index
! *helpIndex.noResize:            True
!!*helpIndex.symbolPixmap:      hal.xbm
*helpIndex.okLabelString:       Display
*helpIndex.cancelLabelString:   Close
*helpIndex.helpLabelString:     Help

*helpIndex*indexScrollList.visibleItemCount:    7
*helpIndex*indexScrollListSW.scrollBarDisplayPolicy:   as_needed
*helpIndex*indexScrollListSW*listSizePolicy: constant

*helpIndex*indexView.labelString:       View
*helpIndex*indexView.topAttachment:     attach_position
*helpIndex*indexView.topPosition:       10
!!*helpIndex*indexView.leftAttachment:  attach_position
!!*helpIndex*indexView.leftPosition:    90
*helpIndex*indexView.leftAttachment:    attach_widget
*helpIndex*indexView.leftWidget:        indexScrollListSW
*helpIndex*indexView.leftOffset:        10
*helpIndex*indexView.rightAttachment:   attach_form

*helpIndex*indexNext.labelString:       Next
*helpIndex*indexNext.topAttachment:     attach_position
*helpIndex*indexNext.topPosition:       40
!!*helpIndex*indexNext.topAttachment:   attach_widget
!!*helpIndex*indexNext.topWidget:       indexView
!!*helpIndex*indexNext.topOffset:       10
!!*helpIndex*indexNext.leftAttachment:  attach_position
!!*helpIndex*indexNext.leftPosition:    90
*helpIndex*indexNext.leftAttachment:    attach_widget
*helpIndex*indexNext.leftWidget:        indexScrollListSW
*helpIndex*indexNext.leftOffset:        10
*helpIndex*indexNext.rightAttachment:   attach_form
*helpIndex*indexPrev.labelString:       Prev
*helpIndex*indexPrev.topAttachment:     attach_position
*helpIndex*indexPrev.topPosition:       70
!!*helpIndex*indexPrev.topAttachment:   attach_widget
!!*helpIndex*indexPrev.topWidget:       indexNext
!!*helpIndex*indexPrev.topOffset:       10
!!*helpIndex*indexPrev.leftAttachment:  attach_position
!!*helpIndex*indexPrev.leftPosition:    90
*helpIndex*indexPrev.leftAttachment:    attach_widget
*helpIndex*indexPrev.leftWidget:        indexScrollListSW
*helpIndex*indexPrev.leftOffset:        10
*helpIndex*indexPrev.rightAttachment:   attach_form

*indexHelpDialog.dialogTitle:   Help for Help Index
*indexHelpDialog.messageString: When using the Help Index Dialog:\n\
\n\Double-click an item to view its HelpCard.\n\
\n\Press the "Next" button if you want to view the next HelpCard.\n\
\n\Press the "Prev" button if you want to view the previous HelpCard.\n\
\n\Press the "OK" button if you want to close the Help Index Dialog.\n

