! $XConsortium: MarkList.res /main/6 1996/10/30 18:23:20 cde-hal $

! Mark List Resources

Dtinfo
  mark_list
    minWidth: 383
    minHeight: 225
    deleteResponse: unmap
    form
      listSW
        top_form; left_form; right_form
        bottom_widget(separator)
        topOffset: 4; leftOffset: 4; rightOffset: 4; bottomOffset: 6
        list
          listSizePolicy: constant
          visibleItemCount: 10
      separator
        highlightThickness: 0
        bottomOffset: 4
        left_form; right_form; bottom_widget(view)
      XmPushButton
        bottom_form
        leftAttachment: attach_position
        rightAttachment: attach_position
      view
        sensitive: False
        Btn_1of5
      edit
        sensitive: False
        Btn_2of5
      delete
        sensitive: False
        Btn_3of5
      close
        Btn_4of5
      help
        Btn_5of5

Dtinfo
  *mark_chooser
    minWidth: 200
    minHeight: 160
    deleteResponse: destroy
    form
      dialogStyle: dialog_full_application_modal
      listSW
        list
          visibleItemCount: 5
          listSizePolicy: constant
          items: item1, item2, item3, item4
          itemCount: 4
          width: 300
        left_form; leftOffset: 3
        right_form; rightOffset: 3
        top_form; bottom_widget(separator)
        bottomOffset: 4
      separator
        bottom_widget(ok); left_form; right_form
        bottomOffset: 4
        highlightThickness: 0
      ok
        sensitive: False
        Btn_1of3
        left_offset: 15
        rightPosition: 50
      cancel
        Btn_2of3
        leftPosition: 50
        rightAttachment: attach_form
        right_offset: 15
!      help
!        Btn_3of3

