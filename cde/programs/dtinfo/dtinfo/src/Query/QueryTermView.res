! $XConsortium: QueryTermView.res /main/7 1996/10/11 21:10:29 cde-hal $

Dtinfo.query_editor.form
  pane
    spacing: 14
    query_area
      *qterm_view
        left_form; right_form
        select
          left_form; top_form
          topOffset: 4
        prefix
!x          left_widget(select); top_form
          left_form; top_form
        *prefix_menu
          tearOffModel: tear_off_disabled
        term_text
          resizable: true
          left_widget(prefix); top_form; right_widget(pw_button)
          rightOffset: 4
        pw_button
          height: 30
          right_widget(connective); top_form
          topOffset: 3; marginWidth: 3; marginHeight: 3
          leftOffset: 4
          arrowDirection: arrow_right
          shadowThickness: 0
        connective
          right_form; top_form
!          labelString:
          *entryAlignment: alignment_beginning
          *alignment: alignment_beginning
        *connect_menu
          tearOffModel: tear_off_disabled
        pws_form
!!         left_widget(pw_button); right_widget(connective)
          left_form; right_widget(connective)
          top_widget(prefix)
          topOffset: 2
          weight_label
            left_form; top_form; bottom_form
          weight_field
            columns: 1
!x           left_widget(weight_label); right_pos(33)
            left_widget(weight_label); right_pos(50)
            top_form; bottom_form
            maxLength: 3
          proximity_label
!x            left_pos(33); top_form; bottom_form
            left_pos(50); top_form; bottom_form
            sensitive: False
          proximity_field
            columns: 1
!x           left_widget(proximity_label); right_pos(66)
            left_widget(proximity_label); right_form
            top_form; bottom_form
            maxLength: 5
            sensitive: False
          scope_label
            left_pos(66); top_form; bottom_form
            sensitive: False
          scope_field
            columns: 1
            left_widget(scope_label); right_form; top_form; bottom_form
            sensitive: False
