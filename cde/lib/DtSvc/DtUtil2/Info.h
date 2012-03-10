/* $XConsortium: Info.h /main/2 1996/03/26 15:05:44 barstow $
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 *
 */

#ifndef _dt_info_h_
#define _dt_info_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	DtINFO_SHOW_OK,
	DtINFO_SHOW_BAD_LOCATOR,	/* the locator argument is NULL */
	DtINFO_SHOW_TT_OPEN_FAIL,	/* tt_open() failed */
	DtINFO_SHOW_MSG_CREATE_FAIL,	/* tt_message_create() failed */
	DtINFO_SHOW_MSG_SEND_FAIL	/* tt_message_send() failed */
} DtInfoShowStatus;

extern DtInfoShowStatus DtInfoShowTopic (
	const char	* info_lib,	/* The InfoLib to browse */
	const char	* locator);	/* The locator in Generalized Locator 
					   Format */

#ifdef __cplusplus
}
#endif

#endif /* _dt_info_h_ */
