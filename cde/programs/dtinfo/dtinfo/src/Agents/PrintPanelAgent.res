! $XConsortium: PrintPanelAgent.res /main/6 1996/08/20 12:03:19 rcs $

Dtinfo
  *perfmeter
    *symbolPixmap:	Dtinfopr.m.pm
  print_panel
    allowShellResize: true
    form
      dialogStyle:      dialog_full_application_modal
      defaultPosition:  False
      autoUnmanage: false
      how_many_frame
        left_form; right_pos(49); top_form
        topOffset: 2; leftOffset: 5; marginHeight: 2
        how_many_title
          childType: frame_title_child
        how_many_form
          selected_label
            top_form; left_form; leftOffset: 4; topOffset: 2
          selected_field
            alignment: alignment_end
            right_form; rightOffset: 2
            top_form; topOffset: 2
          to_print_label
            top_widget(selected_label); left_form; bottom_form
            leftOffset: 4; bottomOffset: 5; topOffset: 8
          to_print_field
            alignment: alignment_end
            top_widget(selected_field); topOffset: 8
            right_form; rightOffset: 2
            bottom_form; bottomOffset: 5
      what_frame
        left_pos(51); right_form; top_form
        topOffset: 2; rightOffset: 5
!        bottom_opposite(how_many_frame)
        what_title
          childType: frame_title_child
        what_form
          print_nodes
            set: true
            alignment: alignment_beginning
            indicatorType: one_of_many
            left_form; right_form; top_form; leftOffset: 4; rightOffset: 4
          print_hierarchy
            alignment: alignment_beginning
            indicatorType: one_of_many
            top_widget(print_nodes); bottom_form; bottomOffset: 3
            left_form; right_form; leftOffset: 4; rightOffset: 4
      separator
        highlightThickness: 0
        left_form; right_form; top_widget(where_frame); bottom_widget(ok)
        topOffset: 7; bottomOffset: 5
      ok
        Btn_1of3
      cancel
        Btn_2of3
      help
        sensitive: True
        Btn_3of3
