Dtinfo
  *add_library_agent
    deleteResponse: destroy
    form
      dialogStyle: dialog_full_application_modal
      autoUnmanage: False
      rightOffset: 10
      leftOffset: 10
      topOffset: 10
      bottomOffset: 10
      sub_form
        top_form; left_form; right_form
        theLabel
          top_form; bottom_form; left_form
          leftOffset: 5
          topOffset: 5
          bottomOffset: 5
          rightOffset: 5
        pick
          sensitive: True
          highlightThickness: 2
          marginLeft: 2
          marginRight: 2
          top_form; bottom_form; right_form
          leftAttachment: attach_none
          rightOffset: 7
          topOffset: 5
          bottomOffset: 5
          fontList: -dt-application-bold-r-normal-serif-12-*-iso8859-1
        text
          width: 250
          focus: True
          top_form; bottom_form; left_widget(theLabel); right_widget(pick)
          leftOffset: 5
          rightOffset: 5
          topOffset: 5
          bottomOffset: 5
      separator
        top_widget(sub_form); left_form; right_form
      ok
        sensitive: False
        left_form
        leftOffset: 5
        rightAttachment: attach_position
        rightPosition: 25
        rightOffset: 5
        top_widget(separator)
        topOffset: 5
        bottom_form
        bottomOffset: 5
      clear
        sensitive: False
        left_widget(ok)
        leftOffset: 5
        rightAttachment: attach_position
        rightPosition: 50
        rightOffset: 5
        top_widget(separator)
        topOffset: 5
        bottom_form
        bottomOffset: 5
      cancel
        sensitive: True
        left_widget(clear)
        leftOffset: 5
        rightAttachment: attach_position
        rightPosition: 75
        rightOffset: 5
        top_widget(separator)
        topOffset: 5
        bottom_form
        bottomOffset: 5
      help
        sensitive: True
        left_widget(cancel)
        leftOffset: 5
        rightOffset: 5
        right_form
        top_widget(separator)
        topOffset: 5
        bottom_form
        bottomOffset: 5
