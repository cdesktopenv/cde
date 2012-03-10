Dtinfo
  graphic
    deleteResponse: destroy
    minWidth: 220
    minHeight: 170
    form
      menu_bar
        *tearOffModel: tear_off_disabled
        top_form; left_form; right_form
        *XmToggleButton.indicatorType: one_of_many
        *file_menu
          print.sensitive: False
        *view_menu
          z_100.set: True
          radioBehavior: true
        *panner_menu
          radioBehavior: true
      message
        left_form ; right_form; bottom_form
        alignment: alignment_beginning
      sep2
        left_form; right_form; bottom_widget(message)
      frame
        top_widget(menu_bar); left_form; right_form; bottom_widget(sep2)
        topOffset: 2; bottomOffset: 2; leftOffset: 2; rightOffset: 2
        shadowType: shadow_etched_in
        subform
          porthole
            borderWidth: 0
            top_form; left_form; right_form; bottom_form
            background: white
          panner
            internalSpace: 1
