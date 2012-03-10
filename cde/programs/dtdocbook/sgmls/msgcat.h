/* $XConsortium: msgcat.h /main/3 1996/06/19 17:16:27 drk $ */

#ifdef HAVE_CAT
#include <nl_types.h>
#else
typedef UNIV nl_catd;
#endif

/* Don't use prototypes here in case nl_types.h declares a conflicting
prototype. */

nl_catd catopen();
int catclose();
char *catgets();
