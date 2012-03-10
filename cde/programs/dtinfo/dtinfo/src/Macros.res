! $XConsortium: Macros.res /main/4 1996/07/18 17:27:10 rcs $

!x Macros to assist in defining resource files.

!x Form attachment macros

#define left_widget(W) leftAttachment: attach_widget; leftWidget: W
#define right_widget(W) rightAttachment: attach_widget; rightWidget: W
#define top_widget(W) topAttachment: attach_widget; topWidget: W
#define bottom_widget(W) bottomAttachment: attach_widget; bottomWidget: W
#define right_pos(P) rightAttachment: attach_position; rightPosition: P
#define left_pos(P) leftAttachment: attach_position; leftPosition: P
#define top_pos(P) topAttachment: attach_position; topPosition: P
#define bottom_pos(P) bottomAttachment: attach_position; bottomPosition: P
#define left_form leftAttachment: attach_form
#define right_form rightAttachment: attach_form
#define top_form topAttachment: attach_form
#define bottom_form bottomAttachment: attach_form

#define top_opposite(W) \
        topAttachment: attach_opposite_widget; topWidget: W
#define bottom_opposite(W) \
        bottomAttachment: attach_opposite_widget; bottomWidget: W
#define left_opposite(W) \
        leftAttachment: attach_opposite_widget; leftWidget: W
#define right_opposite(W) \
        rightAttachment: attach_opposite_widget; rightWidget: W

#define Btn_1of7 left_form; leftOffset: 3; right_pos(15); bottom_form
#define Btn_2of7 left_pos(15); right_pos(29); bottom_form
#define Btn_3of7 left_pos(29); right_pos(43); bottom_form
#define Btn_4of7 left_pos(43); right_pos(57); bottom_form
#define Btn_5of7 left_pos(57); right_pos(71); bottom_form
#define Btn_6of7 left_pos(71); right_pos(85); bottom_form
#define Btn_7of7 left_pos(85); right_form; right_offset: 3; bottom_form

#define Btn_1of6 left_form; leftOffset: 3; right_pos(16); bottom_form
#define Btn_2of6 left_pos(16); right_pos(33); bottom_form
#define Btn_3of6 left_pos(33); right_pos(50); bottom_form
#define Btn_4of6 left_pos(50); right_pos(67); bottom_form
#define Btn_5of6 left_pos(67); right_pos(84); bottom_form
#define Btn_6of6 left_pos(84); right_form; right_offset: 3; bottom_form

#define Btn_1of5 left_form; leftOffset: 3; right_pos(20); bottom_form
#define Btn_2of5 left_pos(20); right_pos(40); bottom_form
#define Btn_3of5 left_pos(40); right_pos(60); bottom_form
#define Btn_4of5 left_pos(60); right_pos(80); bottom_form
#define Btn_5of5 left_pos(80); right_form; right_offset: 3; bottom_form

#define Btn_1of4 left_form; leftOffset: 3; right_pos(25); bottom_form
#define Btn_2of4 left_pos(25); right_pos(50); bottom_form
#define Btn_3of4 left_pos(50); right_pos(75); bottom_form
#define Btn_4of4 left_pos(75); right_form; right_offset: 3; bottom_form

#define Btn_1of3 left_form; leftOffset: 3; right_pos(33); bottom_form
#define Btn_2of3 left_pos(33); right_pos(67); bottom_form
#define Btn_3of3 left_pos(67); right_form; right_offset: 3; bottom_form

#define label(W,S) W.labelString: S

#define BOLD  fontList: -dt-application-bold-r-normal-sans-*-140-*-*-p-*-iso8859-1
#define INSENSITIVE sensitive: false
