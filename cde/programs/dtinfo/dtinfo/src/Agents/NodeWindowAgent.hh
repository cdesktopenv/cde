// $XConsortium: NodeWindowAgent.hh /main/3 1996/06/11 16:15:13 cde-hal $
#ifndef _NodeWindowAgent_hh
#define _NodeWindowAgent_hh

#ifdef MOTIF
#include "NodeWindowAgentMotif.hh"
#else
	#error Unable to determine which window system to use.
#endif

#endif /* _NodeWindowAgent_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
