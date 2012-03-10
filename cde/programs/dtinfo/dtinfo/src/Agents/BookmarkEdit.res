! $XConsortium: BookmarkEdit.res /main/8 1996/11/22 20:55:30 cde-hal $

Dtinfo
  bookmark_edit
    deleteResponse: do_nothing
    minHeight: 220
    minWidth: 290
    form
      name
        topAttachment: attach_form
        leftAttachment: attach_form
        leftOffset: 2
      name_text
        columns: 40
        maxLength: 40
        translations: #override <Key>Return: next-tab-group()
        topAttachment: attach_widget
        topWidget: name
        leftAttachment: attach_form
        leftOffset: 5
      notes
        topAttachment: attach_widget
        topWidget: name_text
        leftAttachment: attach_form
        leftOffset: 2
      notes_textSW
        notes_text
          translations: #override <Key>Return: newline()
          rows: 5
          wordWrap: true
          scrollHorizontal: false
          editMode: multi_line_edit
        topAttachment: attach_widget
        topWidget: notes
        leftAttachment: attach_form
        leftOffset: 2
        rightAttachment: attach_form
        rightOffset: 3
        bottomAttachment: attach_widget
        bottomWidget: separator
        bottomOffset: 4
      separator
        leftAttachment: attach_form
        rightAttachment: attach_form
        bottomAttachment: attach_widget
        bottomWidget: ok
        bottomOffset: 2
        highlightThickness: 0
      ok
        leftAttachment: attach_form
        leftOffset: 3
        rightAttachment: attach_position
        rightPosition: 25
        bottomAttachment: attach_form
        sensitive: false
      view
        leftAttachment: attach_position
        leftPosition: 25
        rightAttachment: attach_position
        rightPosition: 50
        bottomAttachment: attach_form
      cancel
        leftAttachment: attach_position
        leftPosition: 50
        rightAttachment: attach_position
        rightPosition: 75
        bottomAttachment: attach_form
      help
        leftAttachment: attach_position
        leftPosition: 75
        rightAttachment: attach_form
        rightOffset: 3
        bottomAttachment: attach_form
