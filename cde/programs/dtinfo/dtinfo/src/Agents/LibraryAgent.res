! $XConsortium: LibraryAgent.res /main/11 1996/11/20 18:28:21 cde-hal $

!====== Book list
!Dtinfo.doc_tree
Dtinfo
  deleteResponse: do_nothing
  allowShellResize: false
! geometry values are added to the min sizes!
  geometry: 120x400
  minWidth: 380
  minHeight: 200
  mainw
!------ Menus
    menu_bar
      *file_menu
        view
          sensitive: False
        detach
          sensitive: False
        close
          accelerator: Alt<Key>F4
          acceleratorText: Alt F4
      *edit_menu
        copy
          sensitive: False
        remove
          sensitive: False
      *options_menu
        auto_track
          set: on
        show_tool_bar
          set: on
        show_search_area
          set: on
        on_intro
          labelString: Introduction...
          mnemonic: I
        on_tasks
          labelString: Tasks...
          mnemonic: T
        on_reference
          labelString: Reference...
          mnemonic: R
        on_item
          labelString: On Item
        on_help
          labelString: On Help...
          mnemonic: H
        on_about
          labelString: About Information Manager...
          mnemonic: A
!------ Tools
    tool_bar
      view
        labelPixmap: Dtview.m.pm
        labelInsensitivePixmap: Dtviewi.m.pm
        labelType: pixmap
        sensitive: False
      detach
        labelPixmap: Dtdetach.m.pm
        labelInsensitivePixmap: Dtdetachi.m.pm
        labelType: pixmap
        sensitive: False
      print
        labelPixmap: Dtinfopr.m.pm
        labelInsensitivePixmap: Dtinfopri.m.pm
        labelType: pixmap
        sensitive: False
      add
        labelPixmap: Dtinfoadd.m.pm
        labelInsensitivePixmap: Dtinfoaddi.m.pm
        labelType: pixmap
      remove
        labelPixmap: Dtinfosub.m.pm
        labelInsensitivePixmap: Dtinfosubi.m.pm
        labelType: pixmap
        sensitive: False
      query
        labelPixmap: Dteditqry.m.pm
        labelInsensitivePixmap: Dteditqryi.m.pm
        labelType: pixmap
      marks_list
        labelPixmap: Dtbmark.m.pm
        labelInsensitivePixmap: Dtbmarki.m.pm
        labelType: pixmap
!------ Search area
    search_area
      leftOffset: 3; rightOffset: 3
      borderWidth: 1
      *topAttachment: attach_form
      *bottomAttachment: attach_form
      scope
        leftAttachment: attach_form
      text
! text's attachments are set in the code
        rightOffset: 3
        bottomOffset: 2; topOffset: 2
      scope_editor
        right_form; rightOffset: 2
        bottomOffset: 3; topOffset: 3
        sensitive: true
!------ Message area
    *frame
      top_form
      left_form
      right_form
      bottom_widget(message)
!------ List
      *doc_list
        listSizePolicy: constant
        selectionPolicy: extended_select
        visibleItemCount: 12
        width: 400
        translations: #override \
                          ~s ~c ~m ~a <Btn1Down>: OutlineListBeginSelect()\n\
                          ~s ~c ~m ~a <Btn1Up>: OutlineListEndSelect()
    *message
      shadowThickness: 0
      bottom_form; left_form; right_form
      topOffset: 2; bottomOffset: 2; leftOffset: 3; rightOffset: 2
      text
        marginWidth: 1; marginHeight: 1
        editable: false
        cursorPositionVisible: false
        traversalOn: false
        shadowThickness: 0
