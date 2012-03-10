/* $XConsortium: synvar.h /main/3 1995/10/31 12:01:44 rswiston $ */

/******************************************************************
 *
 * (c) Copyright Hewlett-Packard Company, 1993.
 *
 ******************************************************************/

#include   <stdio.h>
#include   <X11/Xlib.h>
#include   <synlib/synlib.h>

typedef struct {
    Display       *display;
    int            waitTime;
    SynFocus      *focusMap;
} TestData, *TestDataPtr;


#define WIN_WAIT_TIME       120
#define MULTI_CLICK_DELAY   5

#define IMAGE_DIR           "../image/"
#define EXPECTED_SUFFIX     ".exp"
#define ACTUAL_SUFFIX       ".act"

#define NEED_LEN            256

#define IMAGE_FILE_LEN      100  /* should be strlen(IMAGE_DIR) + 20 (say) */


#define TERM_EMU            "dtterm"
