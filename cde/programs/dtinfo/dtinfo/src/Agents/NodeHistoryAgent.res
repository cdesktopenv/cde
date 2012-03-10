! $XConsortium: NodeHistoryAgent.res /main/5 1996/10/11 21:00:32 cde-hal $

Dtinfo
  node_hist
    width: 400
    allowShellResize: false
    deleteResponse: unmap
    form
      book
        top_form; left_form
      section
        top_form
      listSW
        top_widget(book); left_form; right_form; bottom_widget(separator)
        leftOffset: 4
        rightOffset: 4
        topOffset: 4
        bottomOffset: 6
        list
          visibleItemCount: 10
          listSizePolicy: constant
      separator
        left_form; right_form; bottom_widget(display)
        bottomOffset: 4
        hightlightThickness: 0
      display
        INSENSITIVE; Btn_1of3
      close
        Btn_2of3
      help
        Btn_3of3
