/* $XConsortium: dtprintinfomsg.h /main/3 1995/11/06 09:33:49 rswiston $ */
#ifndef DTPRINTINFOMSG_H
#define DTPRINTINFOMSG_H

#if !defined(USL) && !defined(__uxp__)
/*
 * if __cplusplus is defined, the system header files take care
 * of themselves and putting this "extern C" here causes inconsistent linkage
 * specifications.
 */
#ifdef __cplusplus
extern "C" {
#endif
#endif /* ! USL */

#include <nl_types.h> 
#include <locale.h> 

#if defined(USL) || defined(__uxp__)
#ifdef __cplusplus
extern "C" {
#endif
#endif /* USL */

#include "dtprintinfo_msg.h"
#include "dtprintinfo_cat.h"

extern nl_catd dtprintinfo_cat;

#ifdef hpux
extern char *Catgets(nl_catd catd, int set_num, int msg_num, char *s);
#define MESSAGE(msg) \
   Catgets(dtprintinfo_cat,DTPRINTER_SET,msg,TXT_DTPRINTER_SET_ ## msg)
#else
#define MESSAGE(msg) \
   catgets(dtprintinfo_cat,DTPRINTER_SET,msg,TXT_DTPRINTER_SET_ ## msg)
#endif

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* DTPRINTINFOMSG_H */
