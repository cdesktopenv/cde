! Search History List Resources
Dtinfo
  search_history
    width: 450
    minWidth: 360
    minHeight: 225
    deleteResponse: do_nothing
    allowShellResize: false
    form
      header
        leftOffset: 4
        rightOffset: 0
        top_form; left_form; right_form
        alignment: alignment_beginning
      listSW
        top_widget(header)
        bottom_widget(separator); left_form; right_form
        bottomOffset: 6
        leftOffset: 4
        rightOffset: 4
        topOffset: 4
        list
          listSizePolicy: constant
!          scrollBarDisplayPolicy: static
          visibleItemCount: 10
      separator
        left_form; right_form
        highlightThickness: 0
        bottom_widget(activate)
        bottomOffset: 4
      activate
        INSENSITIVE; Btn_1of3
      close
        Btn_2of3
      help
        Btn_3of3

