! $XConsortium: NodeWindowAgent.res /main/10 1996/11/22 20:53:53 cde-hal $

Dtinfo.nodeview.deleteResponse: do_nothing
Dtinfo.nodeview.mainw.form.shadowThickness:                    0

Dtinfo.nodeview.mainw.form.title.topAttachment:                ATTACH_FORM
Dtinfo.nodeview.mainw.form.title.leftAttachment:               ATTACH_FORM
Dtinfo.nodeview.mainw.form.title.rightAttachment:              ATTACH_FORM
Dtinfo.nodeview.mainw.form.title*resizable: false

Dtinfo
  nodeview.mainw
    form
      tab_area
        marginHeight: 0
        spacing: 5
        orientation: horizontal
        leftAttachment: attach_form
        rightAttachment: attach_form
        topAttachment: attach_widget
        topWidget: title
        XyzTabButton
          marginWidth: 8
          radius: 12
      message
        shadowThickness: 0
        bottom_form; left_form; right_form
        topOffset: 2; bottomOffset: 2; leftOffset: 3; rightOffset: 2
        text
!          background: lightslategrey
          marginWidth: 1; marginHeight: 1
          editable: false
          cursorPositionVisible: false
          traversalOn: false
          shadowThickness: 0
!          sensitive: false

Dtinfo.nodeview.mainw.form.scrolledw.XmScrollBar.highlightThickness: 2
Dtinfo.nodeview.mainw.form.scrolledw.XmScrollBar.traversalOn: true

Dtinfo.nodeview.mainw.form.frame.inform.DisplayAreaForm.topAttachment: ATTACH_WIDGET
Dtinfo.nodeview.mainw.form.frame.inform.DisplayAreaForm.leftAttachment: ATTACH_WIDGET
Dtinfo.nodeview.mainw.form.frame.inform.DisplayAreaForm.rightAttachment: ATTACH_WIDGET
Dtinfo.nodeview.mainw.form.frame.inform.DisplayAreaForm.bottomAttachment: ATTACH_WIDGET
Dtinfo.nodeview.mainw.form.frame.inform.DisplayAreaForm.display_area.shadowThickness: 0
Dtinfo.nodeview.mainw.form.frame.inform.DisplayAreaForm.leftOffset: 20
Dtinfo.nodeview.mainw.form.frame.inform.shadowType: XmSHADOW_IN

Dtinfo.nodeview.mainw.form.frame.topAttachment:            ATTACH_WIDGET
Dtinfo.nodeview.mainw.form.frame.topWidget:                tab_area
Dtinfo.nodeview.mainw.form.frame.leftAttachment:           ATTACH_FORM
Dtinfo.nodeview.mainw.form.frame.leftOffset:               3
Dtinfo.nodeview.mainw.form.frame.topOffset:                -4
Dtinfo.nodeview.mainw.form.frame.rightOffset:              3
Dtinfo.nodeview.mainw.form.frame.rightAttachment:          ATTACH_FORM
Dtinfo.nodeview.mainw.form.frame.bottomAttachment:         ATTACH_WIDGET
Dtinfo.nodeview.mainw.form.frame.bottomWidget:             message

Dtinfo
  nodeview.mainw
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

Dtinfo*title_menu.button.positionIndex: 0

Dtinfo.nodeview.mainw.form.resizePolicy:                       resize_grow

Dtinfo.nodeview.mainw.tool_bar.lock.indicatorSize: 15
! Pixmaps are specified in the code since they are dynamic.
! Place pixmaps named locked.xbm and unlocked.xbm in ~/bitmaps or
! ~/bitmaps/Dtinfo to change the pixmaps used.

Dtinfo
  nodeview
    mainw
      menu_bar
        *tearOffModel: tear_off_disabled
        *file_menu
          close
            accelerator: Alt<Key>F4
            acceleratorText: Alt F4
        *search_menu
          clear_search
            sensitive: false
        *marks_menu
          create_bmrk
            sensitive: false
          create_anno
            sensitive: false
          edit_mark
            sensitive: false
          move_mark
            sensitive: false
          delete_mark
            sensitive: false
        *options_menu
          detach_graphic
            sensitive: false
          show_tool_bar
            set: on
          show_search_area
            set: on
        debug
          background: hot pink
        *debug_menu
          *background: hot pink
          command_processor
            sensitive: False
          on_item
            labelString: On Item
      tool_bar
        space
          width: 5
          borderWidth: 0
          labelString: 
        history_prev
          labelPixmap: Dthistprev.m.pm
          labelInsensitivePixmap: Dthistprevi.m.pm
          labelType: pixmap
        history_next
          labelPixmap: Dthistnext.m.pm
          labelInsensitivePixmap: Dthistnexti.m.pm
          labelType: pixmap
        section_prev
          labelPixmap: Dtsectprev.m.pm
          labelType: pixmap
        section_next
          labelPixmap: Dtsectnext.m.pm
          labelType: pixmap
        search_prev
          labelPixmap: Dtsrchprev.m.pm
          labelInsensitivePixmap: Dtsrchprevi.m.pm
          labelType: pixmap
        search_next
          labelPixmap: Dtsrchnext.m.pm
          labelType: pixmap
          labelInsensitivePixmap: Dtsrchnexti.m.pm
        print
          labelPixmap: Dtinfopr.m.pm
          labelInsensitivePixmap: Dtinfopri.m.pm
          labelType: pixmap
          sensitive: False
        booklist
          labelPixmap: Dtbooklst.m.pm
          labelType: pixmap
        graphical_map
          labelPixmap: Dtgraphmap.m.pm
          labelType: pixmap
        lock
          set: off

Dtinfo.nodeview.mainw.form.scrolledw.scrollingPolicy: APPLICATION_DEFINED
Dtinfo.nodeview.mainw.form.scrolledw.hsb.orientation: HORIZONTAL
Dtinfo.nodeview.mainw.form.scrolledw.frame.shadowType: SHADOW_IN
Dtinfo.nodeview.mainw.form.scrolledw.spacing: 2
Dtinfo.nodeview.mainw.form.scrolledw.bottomOffset: 4

Dtinfo
  nodeview.mainw
    form
      scrolledw  
        vsb
          accelerators: <Key>d: PageDownOrRight(0)
          increment: 10
          pageIncrement: 100
          minimum: 0
          maximum: 100
          sliderSize: 100
          value: 0
                                            

Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.marginWidth: 0
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.marginHeight: 0
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.arrowDirection: ARROW_LEFT
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.right.arrowDirection: ARROW_RIGHT
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.fractionBase: 8
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.topAttachment: ATTACH_FORM
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.bottomAttachment: ATTACH_FORM
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.leftAttachment: ATTACH_POSITION
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.rightAttachment: ATTACH_POSITION
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.leftPosition: 1
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.left.rightPosition: 3
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.right.topAttachment: ATTACH_FORM
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.right.bottomAttachment: ATTACH_FORM
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.right.rightAttachment: ATTACH_POSITION
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.right.leftPosition: 5
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.XmForm.right.rightPosition: 7
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.sf.sleft.arrowDirection: ARROW_LEFT
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.sf.sright.arrowDirection: ARROW_RIGHT
Dtinfo.nodeview.mainw.form.XmForm.XmFrame.title.marginHeight: 0
