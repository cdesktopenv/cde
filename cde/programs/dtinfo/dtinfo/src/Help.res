! $XConsortium: Help.res /main/15 1996/11/20 18:39:23 cde-hal $

! *helpcardDbFiles: Olias.hlp

Dtinfo*helpdialog*DisplayArea.foreground: black

! +++ Info Manager Browser +++ 

Dtinfo
  *help.helpcard:                   common.HelpWin.HelpButton.ctx
  *helpcard*.helpcard:              common.HelpWin.HelpButton.ctx
  *VertScrollBar.helpcard:          common.ScrollBar.ctx
  *HorScrollBar.helpcard:           common.ScrollBar.ctx
  *sash.helpcard:                   common.Sash.ctx
  *helpHelpDialog_popup.helpcard:   dtinfo.helpHelpDialog-popup.win
  *indexHelpDialog_popup.helpcard:  dtinfo.indexHelpDialog-popup.win

! +++ Info Manager Help Menu +++ 

on_overview:				dtinfo.booklist.on-overview
on_tasks:				dtinfo.booklist.on-tasks
on_reference:				dtinfo.booklist.on-reference
on_help:				common.hlp
on_about:				dtinfo.booklist.on-about

! +++++++++++++++++++++++
! +++ Booklist Window +++
! +++++++++++++++++++++++

doc_list_help:				dtinfo.booklist.win

Dtinfo.helpcard:			dtinfo.booklist.win

Dtinfo.mainw

! +++ Booklist Menu +++

  *menu_bar.file.helpcard:		dtinfo.booklist.file-menu.ctx
  *menu_bar.edit.helpcard:		dtinfo.booklist.edit-menu.ctx
  *menu_bar.options.helpcard:		dtinfo.booklist.options-menu.ctx
  *menu_bar.windows.helpcard:		dtinfo.booklist.windows-menu.ctx

! +++ Booklist Tool Bar +++

  *view.helpcard:			dtinfo.booklist.browse.ctx
  *print.helpcard:			dtinfo.booklist.print.ctx
  *add.helpcard:			dtinfo.booklist.add.ctx
  *remove.helpcard:			dtinfo.booklist.remove.ctx
  *query.helpcard:			dtinfo.booklist.edit-query.ctx
  *marks_list.helpcard:			dtinfo.booklist.marks.ctx

! +++ Booklist Search Area +++

  *scope_editor.helpcard:		dtinfo.booklist.scope-editor.ctx
  *search_area.text.helpcard:		dtinfo.booklist.search.ctx
  *search_area*OptionButton.helpcard:	dtinfo.booklist.scope.ctx

! ++++++++++++++++++++++++++
! +++ Add infolib dialog +++
! ++++++++++++++++++++++++++

add_infolib_help: 			dtinfo.add-infolib.win

! +++++++++++++++++++++++
! +++ Nodeview Window +++
! +++++++++++++++++++++++

Dtinfo.nodeview.mainw.form.helpcard:	dtinfo.nodeview.win

Dtinfo.nodeview.mainw
  *DisplayAreaForm.display_area: dtinfo.nodeview.win

! +++ Nodeview Menu +++

  *menu_bar.file.helpcard:	dtinfo.nodeview.menu-bar.file.ctx
  *menu_bar.search.helpcard:	dtinfo.nodeview.menu-bar.search.ctx
  *menu_bar.go.helpcard:	dtinfo.nodeview.menu-bar.go.ctx
  *menu_bar.marks.helpcard:	dtinfo.nodeview.menu-bar.marks.ctx
  *menu_bar.options.helpcard:	dtinfo.nodeview.menu-bar.options.ctx
  *menu_bar.windows.helpcard:	dtinfo.nodeview.menu-bar.windows.ctx
  *menu_bar.help.helpcard:	common.HelpMenu.ctx

! +++ Nodeview Toolbar +++

  .tool_bar.history_prev.helpcard:  dtinfo.nodeview.tool-bar.history-prev.ctx
  .tool_bar.history_next.helpcard:  dtinfo.nodeview.tool-bar.history-next.ctx
  .tool_bar.section_prev.helpcard:  dtinfo.nodeview.tool-bar.section-prev.ctx
  .tool_bar.section_next.helpcard:  dtinfo.nodeview.tool-bar.section-next.ctx
  .tool_bar.search_prev.helpcard:   dtinfo.nodeview.tool-bar.search-prev.ctx
  .tool_bar.search_next.helpcard:   dtinfo.nodeview.tool-bar.search-next.ctx
  .tool_bar.print.helpcard:         dtinfo.nodeview.tool-bar.print.ctx
  .tool_bar.booklist.helpcard:      dtinfo.nodeview.tool-bar.booklist.ctx
  .tool_bar.graphical_map.helpcard: dtinfo.nodeview.tool-bar.graphical-map.ctx
  .tool_bar.lock.helpcard:          dtinfo.nodeview.tool-bar.lock.ctx

! +++ Nodeview Option Menu +++

  *form.title.OptionButton.helpcard: dtinfo.nodeview.title.ctx

! +++ Nodeview Search Area +++

  *search_area.scope.OptionButton.helpcard: dtinfo.booklist.scope.ctx
  *search_area.text.helpcard:               dtinfo.booklist.search.ctx
  *search_area.scope_editor.helpcard:	    dtinfo.booklist.scope-editor.ctx

! +++ Nodeview Scroll Bars +++

  *DisplayDtHelpVertScrollBar.helpcard: common.ScrollBar.ctx
  *DisplayHorzScrollBar.helpcard:       common.ScrollBar.ctx

! +++++++++++++++++++++++
! +++ Search Results +++
! +++++++++++++++++++++++

results_help:				dtinfo.results.win

Dtinfo.results
  *helpcard:				dtinfo.results.win
  *retain.helpcard:			dtinfo.results.retain.ctx
  *query_text.helpcard:			dtinfo.results.query-text.ctx
  *listform.listSW.list.helpcard:	dtinfo.results.listSW.list.ctx
  *display.helpcard:			dtinfo.results.display.ctx
  *close.helpcard:			dtinfo.results.close.ctx
  *help.helpcard:			common.HelpWin.HelpButton.ctx
  *pane*sash.helpcard:                     common.Sash.ctx
  *pane*VertScrollBar.helpcard:            common.ScrollBar.ctx
  *pane*HorScrollBar.helpcard:             common.ScrollBar.ctx

! +++++++++++++++++++++++
! +++ Bookmark Editor +++
! +++++++++++++++++++++++

bookmark_edit_help:		dtinfo.bookmark-edit.win

Dtinfo.bookmark_edit
  *helpcard:			dtinfo.bookmark-edit.win
  *name_text.helpcard:		dtinfo.bookmark-edit.name.ctx
  *notes_text.helpcard:		dtinfo.bookmark-edit.notes.ctx
  *ok.helpcard:			dtinfo.bookmark-edit.ok.ctx
  *view.helpcard:		dtinfo.bookmark-edit.view.ctx
  *cancel.helpcard:		dtinfo.bookmark-edit.cancel.ctx
  *help.helpcard:		common.HelpWin.HelpButton.ctx
  *VertScrollBar.helpcard:	common.ScrollBar.ctx

! +++++++++++++++++++++++
! +++ Preferences +++
! +++++++++++++++++++++++

#undef BASE
#define BASE dtinfo.preferences

preferences_help:			BASE.win

Dtinfo.preferences
  *helpcard:				BASE.win
  *options.OptionButton.helpcard:	BASE.options.OptionButton.ctx
  *ok.helpcard:				BASE.ok.ctx
  *apply.helpcard:			BASE.Apply.ctx
  *reset.helpcard:			BASE.reset.ctx
  *cancel.helpcard:			BASE.cancel.ctx
  *help.helpcard:			common.HelpWin.HelpButton.ctx

! +++ Browser Preferences +++

  *browse_prefs
    *browse_geo.helpcard:     		BASE.browse-prefs.browse-geo.ctx
    *get_browse_geo.helpcard: 		BASE.browse-prefs.get-browse-geo.ctx
    *fs_label.helpcard:       		BASE.browse-prefs.fs-form.scale-field.ctx
    *lock_toggle.helpcard:		BASE.browse-prefs.lock-toggle.ctx
    *fs_form.scale_up.helpcard:		BASE.browse-prefs.fs-form.scale-field.ctx
    *fs_form.scale_down.helpcard:	BASE.browse-prefs.fs-form.scale-field.ctx

! +++ Map Preferences +++

  *map_prefs.map_geo.helpcard:		BASE.map-prefs.map-geo.ctx
  *map_prefs.get_map_geo.helpcard:	BASE.map-prefs.get-map-geo.ctx
  *map_prefs.update_toggle.helpcard:	BASE.prefs.update-toggle.ctx
 
! +++ History Preferences +++

  *history_prefs.nh_field.helpcard:  BASE.history-prefs.nh-field.ctx
  *history_prefs.nh_up.helpcard:     BASE.history-prefs.nh-up.ctx
  *history_prefs.nh_down.helpcard:   BASE.history-prefs.nh-down.ctx
  *history_prefs.sh_field.helpcard:  BASE.history-prefs.sh-field.ctx
  *history_prefs.sh_up.helpcard:     BASE.history-prefs.sh-up.ctx
  *history_prefs.sh_down.helpcard:   BASE.history-prefs.sh-down.ctx

! +++ Search Preferences +++

  *search_prefs*hits_field.helpcard: 		BASE.search-prefs.hits-field.ctx
  *search_prefs*adisplay_toggle.helpcard: 	BASE.search-prefs.adisplay-toggle.ctx
  *search_prefs.hits_form.hits_up.helpcard:	BASE.search-prefs.hits-form.hits-up.ctx
  *search_prefs.hits_form.hits_down.helpcard:	BASE.search-prefs.hits-form.hits-down.ctx

! +++++++++++++++++++++++
! +++ Print Panel +++
! +++++++++++++++++++++++

print_panel_help:			dtinfo.print.win

Dtinfo*nodeview
  *PrintSetup_popup.PrintSetup.helpcard:	dtinfo.print.win
  .mainw*PrintSetup.Info.helpcard:		dtinfo.PrintSetup.Info.ctx
  .mainw*PrintSetup.Name.Text.helpcard:	dtinfo.PrintSetup.Name.Text.ctx
  .mainw*PrintSetup.SelectPrinter.helpcard:	dtinfo.PrintSetup.SelectPrinter.ctx
  .mainw*PrintSetup.FileName.helpcard:	dtinfo.PrintSetup.FileName.ctx
  .mainw*PrintSetup.SelectFile.helpcard:	dtinfo.PrintSetup.SelectFile.ctx
  .mainw*DestRadioBox.button_0.helpcard:	dtinfo.PrintSetup.DestRadioBox.button_0.ctx
  .mainw*DestRadioBox.button_1.helpcard:	dtinfo.PrintSetup.DestRadioBox.button_1.ctx
  .mainw*Copies.Copies_TF.helpcard:		dtinfo.doc-tree.PrintSetup.Copies.Copies-TF.ctx

  .mainw*PrintSetup*print_nodes.helpcard: 	dtinfo.PrintSetup.what-form.print-nodes.ctx
  .mainw*PrintSetup*print_hierarchy.helpcard:	dtinfo.PrintSetup.what-form.print-hierarchy.ctx
  .mainw*PrintSetup.Print.helpcard:		dtinfo.PrintSetup.Print.ctx
  .mainw*PrintSetup.Setup.helpcard:		dtinfo.PrintSetup.Setup.ctx
  .mainw*PrintSetup.Cancel.helpcard:		dtinfo.PrintSetup.Cancel.ctx
  .mainw*PrintSetup.Help:			common.HelpWin.HelpButton.ctx


Dtinfo.doc_tree
  *PrintSetup_popup.PrintSetup.helpcard:	dtinfo.print.win
  *PrintSetup.Info.helpcard:		dtinfo.PrintSetup.Info.ctx
  *PrintSetup.Name.Text.helpcard:	dtinfo.PrintSetup.Name.Text.ctx
  *PrintSetup.SelectPrinter.helpcard:	dtinfo.PrintSetup.SelectPrinter.ctx
  *PrintSetup.FileName.helpcard:	dtinfo.PrintSetup.FileName.ctx
  *PrintSetup.SelectFile.helpcard:	dtinfo.PrintSetup.SelectFile.ctx
  *DestRadioBox.button_0.helpcard:	dtinfo.PrintSetup.DestRadioBox.button-0.ctx
  *DestRadioBox.button_1.helpcard:	dtinfo.PrintSetup.DestRadioBox.button-1.ctx
  *Copies.Copies_TF.helpcard:		dtinfo.doc-tree.PrintSetup.Copies.Copies-TF.ctx
  
  *PrintSetup*print_nodes.helpcard: 	dtinfo.PrintSetup.what-form.print-nodes.ctx
  *PrintSetup*print_hierarchy.helpcard:	dtinfo.PrintSetup.what-form.print-hierarchy.ctx
  *PrintSetup.Print.helpcard:		dtinfo.PrintSetup.Print.ctx
  *PrintSetup.Setup.helpcard:		dtinfo.PrintSetup.Setup.ctx
  *PrintSetup.Cancel.helpcard:		dtinfo.PrintSetup.Cancel.ctx
  *PrintSetup.Help:			common.HelpWin.HelpButton.ctx

!  *how_many_title.helpcard:         dtinfo.print.how-many-title.ctx
!  *how_many_form.helpcard:          dtinfo.print.how-many-form.ctx
!  *how_many_form.selected_label.helpcard:  dtinfo.print.how-many-form.selected-label.ctx
!  *how_many_form.selected_field.helpcard:  dtinfo.print.how-many-form.selected-field.ctx
!  *how_many_form.to_print_label.helpcard:  dtinfo.print.how-many-form.to-print-label.ctx
!  *how_many_form.to_print_field.helpcard:  dtinfo.print.how-many-form.to print-field.ctx
!  *what_title.helpcard:             dtinfo.print.what-title.ctx
!  *what_form.print_nodes.helpcard:  dtinfo.print.what-form.print-nodes.ctx
!  *what_form.print_hierarchy.helpcard:  dtinfo.print.what-form.print-hierarchy.ctx
!  *where_title.helpcard:            dtinfo.print.where-title.ctx
!  *where_form.to_printer.helpcard:  dtinfo.print.where-form.to-printer.ctx
!  *where_form.to_file.helpcard:     dtinfo.print.where-form.to-file.ctx
!  *where_form.file_select.helpcard: dtinfo.print.where-form.file-select.ctx
!  *where_form.printer_name.helpcard: dtinfo.print.where-form.printer-name.ctx
!  *where_form.file_name.helpcard:   dtinfo.print.where-form.file-name.ctx
!  *ok.helpcard:                     dtinfo.print.ok.ctx
!  *cancel.helpcard:                 dtinfo.print.cancel.ctx
!  *help.helpcard:                   common.HelpWin.HelpButton.ctx
!  *file_select_popup*.helpcard:     dtinfo.print.file-select-popup.win
!  *file_select_popup*Items.helpcard:  dtinfo.print.file-select-popup.Items.ctx
!  *file_select_popup*ItemsList.helpcard:  dtinfo.print.file-select-popup.ItemsList.ctx
!  *file_select_popup*VertScrollBar.helpcard: common.ScrollBar.ctx
!  *file_select_popup*HorScrollBar.helpcard:  common.ScrollBar.ctx
!  *file_select_popup*Selection.helpcard:  dtinfo.print.file-select-popup.Selection.ctx
!  *file_select_popup*Text.helpcard:  dtinfo.print.file-select-popup.Text.ctx
!  *file_select_popup*OK.helpcard:    dtinfo.print.file-select-popup.OK.ctx
!  *file_select_popup*Apply.helpcard: dtinfo.print.file-select-popup.Apply.ctx
!  *file_select_popup*Cancel.helpcard:dtinfo.print.file-select-popup.Cancel.ctx
!  *file_select_popup*Help.helpcard:  common.HelpWin.HelpButton.ctx
!  *file_select_popup*FilterLabel.helpcard:  dtinfo.print.file-select-popup.FilterLabel.ctx
!  *file_select_popup*Dir.helpcard:   dtinfo.print.file-select-popup.Dir.ctx
!  *file_select_popup*DirList.helpcard:  dtinfo.print.file-select-popup.DirList.ctx



! +++++++++++++++++++++++
! +++ Graphical Map +++
! +++++++++++++++++++++++

map_help:                           	dtinfo.map.win

Dtinfo.map.rootform
  *helpcard:                   		dtinfo.map.win
  .pane.topform*panner.helpcard: 	dtinfo.map.pane.topform.panner.ctx
  .pane.topform.lock.helpcard:		dtinfo.map.autoUpB.ctx
  .pane.tframe.porthole.tree.helpcard:	dtinfo.map.win
  .pane.Sash.helpcard:              	common.Sash.ctx
  .bottomform.close.helpcard:		common.Close.ctx
  .bottomform.help.helpcard:		common.HelpWin.HelpButton.ctx

! +++++++++++++++++++++++
! +++ User Marks List +++
! +++++++++++++++++++++++

mark_list_help:				dtinfo.mark-list.win

Dtinfo.mark_list
  *helpcard:				dtinfo.mark-list.win
  *listSW.list.helpcard:		dtinfo.mark-list.list.ctx
  *view.helpcard:			dtinfo.mark-list.view.ctx
  *edit.helpcard:			dtinfo.mark-list.edit.ctx
  *delete.helpcard:			dtinfo.mark-list.delete.ctx
  *close.helpcard:			common.Close.ctx
  *help.helpcard:			common.HelpWin.HelpButton.ctx
  *listSW.VertScrollBar.helpcard:	common.ScrollBar.ctx

! +++++++++++++++++++++++
! +++ Section History +++
! +++++++++++++++++++++++

node_hist_help:			dtinfo.history.win

Dtinfo.node_hist.form
  *helpcard:			dtinfo.history.win
  *list.helpcard:		dtinfo.history.pane.listform.listSW.list.ctx
  *display.helpcard:		dtinfo.history.display.ctx
  *close.helpcard:		common.Close.ctx
  *help.helpcard:		common.HelpWin.HelpButton.ctx
  *VertScrollBar.helpcard:	common.ScrollBar.ctx
  *HorScrollBar.helpcard:	common.ScrollBar.ctx
  *sash.helpcard:		common.Sash.ctx

! +++++++++++++++++++++++
! +++ Search History +++
! +++++++++++++++++++++++

search_history_help:			dtinfo.search-history.win
 
Dtinfo.search_history
  *helpcard:				dtinfo.search-history.win
  *listSW.list.helpcard:		dtinfo.search-history.listSW.list.ctx
  *activate.helpcard:			dtinfo.search-history.activate.ctx
  *close.helpcard:			common.Close.ctx
  *help.helpcard:			common.HelpWin.HelpButton.ctx
  *listSW.VertScrollBar.helpcard:	common.ScrollBar.ctx
  *listSW.HorScrollBar.helpcard:	common.ScrollBar.ctx

! +++++++++++++++++++++++
! +++ Query Editor +++
! +++++++++++++++++++++++

query_editor_help:			dtinfo.query-editor.win

Dtinfo.query_editor
  *helpcard:				dtinfo.query-editor.win
  *scope_option.OptionButton.helpcard:	dtinfo.query-editor.scope-option.OptionButton.ctx
  *prefix.OptionButton.helpcard:	dtinfo.query-editor.pane.popup-prefix-menu.ctx
  *term_text.helpcard:			dtinfo.query-editor.pane.term-text.ctx
  *connective.OptionButton.helpcard:	dtinfo.query-editor.pane.connective.ctx
  *qform.qtextSW.qtext.helpcard:	dtinfo.query-editor.qform.ctx
  *sash.helpcard:			common.Sash.ctx

  *search.helpcard:			dtinfo.query-editor.XmPushButton.search.ctx
  *clear.helpcard:			dtinfo.query-editor.XmPushButton.clear.ctx
  *scope.helpcard:			dtinfo.query-editor.XmPushButton.scope.ctx 
  *close.helpcard:			common.Close.ctx
  *help.helpcard:			common.HelpWin.HelpButton.ctx
  *pane*VertScrollBar.helpcard:		common.ScrollBar.ctx
  *pane*HorScrollBar.helpcard:		common.ScrollBar.ctx

! +++++++++++++++++++++++
! +++ Search Scope +++
! +++++++++++++++++++++++

scope_editor_help:			dtinfo.scope-editor.win

Dtinfo.scope_editor
  *helpcard:				dtinfo.scope-editor.win
  *scope_option.OptionButton.helpcard:	dtinfo.scope-editor.scope-option.ctx
  *infobases.helpcard:			dtinfo.scope-editor.books.ctx
  *components.helpcard:			dtinfo.scope-editor.components.ctx
  *new.helpcard:			dtinfo.scope-editor.new.ctx
  *save.helpcard:			dtinfo.scope-editor.save.ctx
  *rename.helpcard:			dtinfo.scope-editor.rename.ctx
  *delete.helpcard:			dtinfo.scope-editor.delete.ctx
  *reset.helpcard:			dtinfo.scope-editor.reset.ctx
  *close.helpcard:			common.Close.ctx
  *help.helpcard:                   	common.HelpWin.HelpButton.ctx
  *VertScrollBar.helpcard:		common.ScrollBar.ctx
  *HorScrollBar.helpcard:		common.ScrollBar.ctx

! ++++++++++++++++++++++++
! +++ Detached Graphic +++
! ++++++++++++++++++++++++

graphic_on_window:		dtinfo.graphic.win
graphic_on_help:		common.hlp

Dtinfo.graphic
  *helpcard:			dtinfo.graphic.win
  *menu_bar.file.helpcard:	dtinfo.graphic.menu-bar.file.ctx
  *menu_bar.panner.helpcard:	dtinfo.graphic.menu-bar.panner.ctx
  *menu_bar.view.helpcard:	dtinfo.graphic.menu-bar.view.ctx
  *menu_bar.help.helpcard:	common.HelpMenu.ctx
  *frame*panner.helpcard:	dtinfo.graphic.panner.ctx

! ++++++++++++++++++++++
! +++  Open Locator  +++
! ++++++++++++++++++++++

open_url_help:		dtinfo.open-url.win

Dtinfo.open_url
  *helpcard:		dtinfo.open-url.win
  *form.text_field:	dtinfo.open-url.text-field.ctx
  *ok.helpcard:		dtinfo.open-url.ok.ctx
  *apply.helpcard:	dtinfo.open-url.apply.ctx
  *clear.helpcard:	dtinfo.open-url.clear.ctx
  *close.helpcard:	common.Close.ctx
  *help.helpcard:	common.HelpWin.HelpButton.ctx

! ++++++++++++ end ++++++++++++++
