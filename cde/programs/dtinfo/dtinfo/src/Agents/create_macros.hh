// $XConsortium: create_macros.hh /main/3 1996/06/11 16:17:20 cde-hal $
#ifndef _create_macros_hh
#define _create_macros_hh

#define DECL(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, NAME)
#define DECLC(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, (char*)NAME)
#define DECLM(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, NAME, WAutoManage)
#define DECLMC(TYPE,VAR,PARENT,NAME) TYPE VAR (PARENT, (char*)NAME, WAutoManage)
#define ASSN(TYPE,VAR,PARENT,NAME) VAR = TYPE (PARENT, NAME)
#define ASSNM(TYPE,VAR,PARENT,NAME) VAR = TYPE (PARENT, NAME, WAutoManage)

#define SET_CALLBACK(WOBJ,CBACK,FUNC) \
  (WOBJ).CONCAT3(Set,CBACK,Callback) (this, (WWL_FUN) &CLASS::FUNC)
#define SET_CALLBACK_D(WOBJ,CBACK,FUNC,DATA) \
  (WOBJ).CONCAT3(Set,CBACK,Callback) (this, (WWL_FUN) &CLASS::FUNC, \
				      (void *) DATA)

#define ON_ACTIVATE(WOBJ,FUNC) SET_CALLBACK(WOBJ,Activate,FUNC)
#define ON_ACTIVATE_D(WOBJ,FUNC,DATA) SET_CALLBACK_D(WOBJ,Activate,FUNC,DATA)

#define CALL_DATA(TYPE,NAME) \
  TYPE *NAME = (TYPE *) wcb->CallData()

#ifndef CLASS
  #error "CLASS macro must be defined!"
#endif

#endif /* _create_macros_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
