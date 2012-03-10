/* $XConsortium: common.h /main/3 1995/10/31 11:59:55 rswiston $ */
#define  NULLCHAR         '\0'
#define  BLANK            ' '

typedef enum {
   DECModeSet=1, DECModeReset=2, DECModeSave=3, DECModeRestore=4
} DECModeType;
