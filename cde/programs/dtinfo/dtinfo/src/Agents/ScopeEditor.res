! $XConsortium: ScopeEditor.res /main/9 1996/10/15 19:40:47 cde-hal $

Dtinfo
  scope_editor
    deleteResponse: unmap
    allowShellResize: false
    minWidth: 500
    minHeight: 245
    form
      fractionBase: 140
      resizable: false
      scope_option
        navigationType: tab_group
        left_form; top_form
!        OptionLabel
!          fontList: -dt-application-bold-r-normal-sans-*-140-*-*-p-*-iso8859-1
      books
!        fontList: -dt-application-bold-r-normal-sans-*-140-*-*-p-*-iso8859-1
        left_form; top_widget(scope_option)
        leftOffset: 5
      infobasesSW
        top_widget(books); left_form; bottom_widget(separator); right_pos(83)
        leftOffset: 5; bottomOffset: 6; topOffset: 2
        infobases
          listSizePolicy: constant
          visibleItemCount: 10
          translations: #override \
                        ~s ~c ~m ~a <Btn1Down>: OutlineListBeginSelect()\n\
                        ~s ~c ~m ~a <Btn1Up>: OutlineListEndSelect()
          selectionPolicy: multiple_select
      components
!        fontList: -dt-application-bold-r-normal-sans-*-140-*-*-p-*-iso8859-1
        topAttachment: attach_opposite_widget
        topWidget: books
        leftAttachment: attach_position
        leftPosition: 85
      componentsSW
        topAttachment: attach_widget
        topWidget: components
        rightAttachment: attach_form
        leftAttachment: attach_position
        leftPosition: 85;
        bottomAttachment: attach_widget
        bottomWidget: separator
        rightOffset: 5; bottomOffset: 6; topOffset: 2
        components
          listSizePolicy: constant
          translations: #override \
                        ~s ~c ~m ~a <Btn1Down>: OutlineListBeginSelect()\n\
                        ~s ~c ~m ~a <Btn1Up>: OutlineListEndSelect()
          selectionPolicy: multiple_select
      separator
        left_form; right_form; bottom_widget(new)
        bottomOffset: 5
        highlightThickness: 0
      XmPushButton
        bottom_form; bottomOffset: 4; rightOffset: 3
      new
        left_form; leftOffset: 4; right_pos(20)
      save
        left_pos(20); right_pos(40)
      rename
        left_pos(40); right_pos(60)
      delete
        left_pos(60); right_pos(80)
      reset
        left_pos(80); right_pos(100)
        sensitive: false
      close
        left_pos(100); right_pos(120)
      help
        left_pos(120); right_form; rightOffset: 4
