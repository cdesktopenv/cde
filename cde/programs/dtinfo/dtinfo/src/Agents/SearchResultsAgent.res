! $XConsortium: SearchResultsAgent.res /main/9 1996/11/22 20:54:38 cde-hal $

Dtinfo
  results
    deleteResponse: do_nothing
    allowShellResize: false
    width: 500
    minWidth: 325
    minHeight: 220
    form
      retain
        top_form; right_form
        topOffset: 2
        labelType: pixmap
        indicatorSize: 15
      retrieved
        top_form; left_form
        topOffset: 2; leftOffset: 3
      num_hits
        alignment: alignment_beginning
        top_form; left_widget(retrieved); right_widget(retain)
        topOffset: 2; leftOffset: 8
      scope
        left_form; top_widget(retrieved)
        leftOffset: 3
      scope_name
        leftAttachment: attach_opposite_widget
        leftWidget: num_hits
        top_widget(num_hits)
        right_form
        alignment: alignment_beginning
      query_label
        left_form; top_widget(scope)
        leftOffset: 3
      pane
        left_form; right_form; bottom_widget(separator)
        topOffset: 2
        bottomOffset: 4
        spacing: 20
        query_textSW
          query_text
            translations: #override <Key>Tab: next-tab-group()
            value: FILE A BUG REPORT
!            editable: false
            editMode: multi_line_edit
            wordWrap: true
            scrollHorizontal: false
! Have to turn the bell off or it beeps after dialog popped down.
            verifyBell: false
            traversalOn: false
!            cursorPositionVisible: false
        listform
          header
            top_form; left_form; right_form
            rightOffset: 0
            alignment: alignment_beginning
          listSW
            topAttachment: attach_form
            leftAttachment: attach_form
            rightAttachment: attach_form
            bottomAttachment: attach_form            
            list
              listSizePolicy: resize_if_possible
              visibleItemCount: 8
      separator
        leftAttachment: attach_form
        rightAttachment: attach_form
        bottomAttachment: attach_widget
        bottomWidget: display
        bottomOffset: 4
        hightlightThickness: 0
      display
        INSENSITIVE; Btn_1of3
      close
        Btn_2of3
      help
        Btn_3of3
        sensitive: true

