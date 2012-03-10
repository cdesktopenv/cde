/* $XConsortium: unique_id.h /main/3 1996/06/11 17:40:49 cde-hal $ */

#ifndef _unique_id_h
#define _unique_id_h

#ifdef __cplusplus
extern "C" {
#endif

#if defined( __STDC__ ) || defined( _HPUX_SOURCE )
const char *unique_id(void);
#else
char *unique_id();
#endif

#ifdef __cplusplus
}
#endif

#endif /* _unique_id_h */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
