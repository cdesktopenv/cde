/* $XConsortium: debug.h /main/3 1996/06/11 16:47:24 cde-hal $ */
#ifndef ON_DEBUG
#ifdef DEBUG
#define ON_DEBUG(stmt)	stmt
#else
#define ON_DEBUG(stmt)
#endif
#endif

// for turning off assertions 
#ifndef DEBUG
#define NDEBUG
#endif
