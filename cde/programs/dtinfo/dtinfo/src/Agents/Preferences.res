! $XConsortium: Preferences.res /main/13 1996/11/22 20:54:23 cde-hal $

Dtinfo
  preferences
    allowShellResize: true
! really need to link to cancel cb for warning dialog
    deleteResponse: unmap
    form
      *XmTextField.editable: false
      options
        left_form; top_form
      frame
        left_form; right_form; top_widget(options); bottom_widget(ok)
        bottomOffset: 6; topOffset: 6
        leftOffset: 4; rightOffset: 4
        marginWidth: 4
        marginHeight: 4
        container
          left_form; right_form; top_widget(separator1)
          browse_prefs
            left_form; top_form; right_form; bottom_form
            browse_label
              top_form; left_form
            get_browse_geo
              right_form; rightOffset: 15
              bottomAttachment: attach_opposite_widget; 
              bottomWidget: browse_label
              marginHeight: 4
              marginWidth: 4
              fontList: -dt-application-bold-r-normal-serif-12-*-iso8859-1
            browse_geo
              left_pos(60); right_widget(get_browse_geo); top_form
              rightOffset: 2
              value: s_File_A_Bug
              columns: 10
              traversalOn: false
              cursorPositionVisible: false
            fs_label
              left_form; top_widget(browse_label); topOffset: 4
            fs_form
              left_pos(60)
              bottomAttachment: attach_none
              topAttachment: attach_opposite_widget
              topWidget: fs_label
              scale_field
                left_form; top_form
                value: s_File_Bug
                columns: 6
                traversalOn: false
                cursorPositionVisible: false
              scale_up
                top_form; bottom_pos(50); left_widget(scale_field)
                arrowDirection: arrow_up
                shadowThickness: 0
              scale_down
                bottom_form; top_pos(50); left_widget(scale_field)
                arrowDirection: arrow_down
                shadowThickness: 0
            lock_label
              left_form; top_widget(fs_label)
              topOffset: 4
            lock_toggle
              topAttachment: attach_opposite_widget
              topWidget: lock_label
              leftAttachment: attach_position
              leftPosition: 60
              right_form; rightOffset: 50
              bottomAttachment: attach_opposite_widget
              bottomWidget: lock_label;
              alignment: alignment_beginning
          map_prefs
            left_form; top_form; right_form; bottom_form
            map_label
              left_form; top_form
            map_geo
              topAttachment: attach_opposite_widget
              topWidget: map_label
              leftAttachment: attach_position
              leftPosition: 60
              rightAttachment: attach_widget
              rightWidget: get_map_geo
              rightOffset: 2
              value: s_File_A_Bug
              columns: 10
              traversalOn: false
              cursorPositionVisible: false
            get_map_geo
              right_form; rightOffset: 15
              bottomAttachment: attach_opposite_widget; 
              bottomWidget: map_label;
              marginHeight: 4
              marginWidth: 4
              fontList: -dt-application-bold-r-normal-serif-12-*-iso8859-1
            update_label
              left_form; top_widget(map_label); topOffset: 4
            update_toggle
              topAttachment: attach_opposite_widget
              topWidget: update_label
              leftAttachment: attach_position
              leftPosition: 60
              right_form; rightOffset: 50
              bottomAttachment: attach_opposite_widget 
              bottomWidget: update_label
              alignment: alignment_beginning
          history_prefs
            left_form; top_form; right_form; bottom_form
            nh_label
              left_form; top_form
            nh_form
              left_pos(70); top_form
              nh_field
                left_form; top_form; columns: 6
                value: s_File_Bug
                traversalOn: false
                cursorPositionVisible: false
              nh_up
                top_form; bottom_pos(50); left_widget(nh_field)
                arrowDirection: arrow_up
                shadowThickness: 0
              nh_down
                bottom_form; top_pos(50); left_widget(nh_field)
                arrowDirection: arrow_down
                shadowThickness: 0
            sh_label
              left_form; top_widget(nh_label); topOffset: 4
            sh_form
              left_pos(70); top_opposite(sh_label)
              sh_field
                left_form; top_form; columns: 6
                value: s_File_Bug
                traversalOn: false
                cursorPositionVisible: false
              sh_up
                top_form; bottom_pos(50); left_widget(sh_field)
                arrowDirection: arrow_up
                shadowThickness: 0
              sh_down
                bottom_form; top_pos(50); left_widget(sh_field)
                arrowDirection: arrow_down
                shadowThickness: 0
          search_prefs
            left_form; top_form; right_form; bottom_form
            max_hits
              left_form; top_form
            hits_form
              left_pos(70); top_form
              hits_field
                left_form; top_form; columns: 6
                value: s_File_Bug
                traversalOn: false
                cursorPositionVisible: false
              hits_up
                top_form; bottom_pos(50); left_widget(hits_field)
                arrowDirection: arrow_up
                shadowThickness: 0
              hits_down
                bottom_form; top_pos(50); left_widget(hits_field)
                arrowDirection: arrow_down
                shadowThickness: 0
            adisplay_label
              left_form; top_widget(hits_form); topOffset: 4
            adisplay_toggle
              topAttachment: attach_opposite_widget; topWidget: adisplay_label; leftAttachment: attach_position; leftPosition: 70
              bottomAttachment: attach_opposite_widget; bottomWidget: adisplay_label
              right_form; rightOffset: 30
              alignment: alignment_beginning
      ok
        Btn_1of5
        sensitive: false
      apply
        Btn_2of5
        sensitive: false
      reset
        Btn_3of5
        sensitive: false
      cancel
        Btn_4of5
      help
        Btn_5of5
        sensitive: true

! Default Preference values.  These are only the initial values!
! Once changed with the Preferences dialog they are no longer used.

Dtinfo*BrowseGeometry:   640x720
Dtinfo*FontScale:        1
Dtinfo*BrowseLock:       off
Dtinfo*MapGeometry:      520x350
Dtinfo*MapAutoUpdate:    true
Dtinfo*NodeHistSize:     100
Dtinfo*SearchHistSize:   50
Dtinfo*MaxSearchHits:    50
