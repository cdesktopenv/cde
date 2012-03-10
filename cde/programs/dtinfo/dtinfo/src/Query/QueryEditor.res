! $XConsortium: QueryEditor.res /main/9 1996/11/22 20:54:54 cde-hal $

Dtinfo
  query_editor
    deleteResponse: unmap
!    geometry: 620x320
    height: 320
!    allowShellResize: True
    form
      scope_option
        navigationType: tab_group
        left_form; top_form
      menu_bar
        top_form; left_form; right_form
        edit
          sensitive: False
      pane
        top_widget(scope_option); left_form; right_form
        bottom_widget(separator)
        qform
          qlabel
            left_form; top_form
            topOffset: 6
!            BOLD
          qtextSW
            left_widget(qlabel); top_form; bottom_form; right_form
            qtext
              cursorPositionVisible: False
              editable: False
              editMode: multi_line_edit
              rows: 2
              wordWrap: True
              verifyBell: False
              scrollHorizontal: False
              traversalOn: false
!              translations: #override <Key>Tab: next-tab-group()
        query_area
          scrollingPolicy: automatic
! Needs to be static until we can control sizing better.
          scrollBarDisplayPolicy: static
      separator
        bottom_widget(search); left_form; right_form
        bottomOffset: 4
        highlightThickness: 0
      hform
        Btn_2of4
        bottomOffset: 8
        *sensitive: False
        hist_prev
          left_form; top_form; bottom_form
          arrowDirection: arrow_left
          shadowThickness: 0
        hist_next
          top_form; bottom_form; left_widget(hist_prev)
          arrowDirection: arrow_right
          shadowThickness: 0
          topOffset: -1
        history
          top_form; bottom_form; left_widget(hist_next); right_form
          rightOffset: 3
      search
        Btn_1of5
        sensitive: False
      clear
        Btn_2of5
      scope
        Btn_3of5
      cancel
        Btn_4of5
      help
        Btn_5of5
        sensitive: True
