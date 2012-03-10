/* $XConsortium: WindowSystem.hh /main/3 1996/06/11 16:30:25 cde-hal $ */

#ifndef _WindowSystem_hh
#define _WindowSystem_hh

#ifdef MOTIF
#include "WindowSystemMotif.hh"
#else
	#error Unable to determine which window system to use.
#endif

#endif /* _WindowSystem_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
