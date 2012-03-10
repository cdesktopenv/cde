/* $XConsortium: MessageAgent.hh /main/3 1996/06/11 16:14:20 cde-hal $ */

#ifndef _MessageAgent_hh
#define _MessageAgent_hh
     
#ifdef MOTIF
#include "MessageAgentMotif.hh"
#else
	#error Unable to determine which window system to use.
#endif

#endif /* _MessageAgent_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
