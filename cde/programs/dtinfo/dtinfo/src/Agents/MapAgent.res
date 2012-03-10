Dtinfo
  map
    deleteResponse: unmap
    minWidth: 300
    minHeight: 300
    *pane
      spacing: 14
      top_form; left_form; right_form; bottom_widget(bottomform)
      allowResize: true
      topform
        *geoTattler: on
        paneMinimum: 75
        pframe
          topAttachment: attach_form
          bottomAttachment: attach_form
          leftAttachment: attach_form
          shadowType: shadow_in
          panner
!            cursorName: FONT decw$cursor 40 41
            pointerColorBackground: white
            shadowThickness: 0
            borderWidth: 0
            internalSpace: 1
!            foreground: #8db4e8
        lock
          topAttachment: attach_form
          bottomAttachment: attach_form
          rightAttachment: attach_form
      tframe
        marginWidth: 2
        marginHeight: 2
        porthole
!          *background: lightsteelblue4
          borderWidth: 0
          tree
            autoReconfigure: false
            XmForm
              XmPushButton
                shadowThickness: 0
                borderWidth: 0
                left_form
              XmArrowButton
                shadowThickness: 0
                arrowDirection: arrow_right
                width: 18; height: 18; topOffset: 2
                right_form; top_form
    *bottomform
      leftAttachment: attach_form
      rightAttachment: attach_form
      bottomAttachment: attach_form
      bottomOffset: 2
      close
        bottom_form; left_pos(33); right_pos(66); rightOffset: 3
      help
        left_pos(66)
        bottom_form; right_form; rightOffset: 3

