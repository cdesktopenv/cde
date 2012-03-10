/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ttdnd.h /main/3 1995/10/23 09:57:38 rswiston $ 			 				 */
/*
 *
 * ttdnd.h
 *
 * Copyright (c) 1990 by Sun Microsystems, Inc.
 */
#ifndef _TTDND_H
#define _TTDND_H

#if defined(__STDC__) 
/* ANSI C */
#	define _TT_CONST const
#	define _TT_EXTERN_FUNC(type,name,list) type name list
#elif defined(__cplusplus)
/* C++ 2.0 or later*/
#	define _TT_CONST const
#	define _TT_EXTERN_FUNC(type,name,list) extern "C" { type name list; }
#else
/* Sun C (K&R C, almost) */
#	define _TT_CONST
#	define _TT_EXTERN_FUNC(type,name,list) type name()
#endif

#define TTDND_ATM_TARGET "_SUN_TOOLTALK"

#if defined(__cplusplus) || defined(__STDC__)
typedef int (Ttdnd_send_data_func)(int chunknumber, int maxchunk,
				   const char *type,
				   void *buffer, int *chunksize,
				   int *is_last);
typedef int (Ttdnd_receive_data_func)(int chunknumber, const char *type,
				      void *buffer, int chunksize,
				      int is_last);
typedef void Ttdnd_input_func(void);
#else
typedef int (Ttdnd_send_data_func)();
typedef int (Ttdnd_receive_data_func)();
typedef void Ttdnd_input_func();
#endif

_TT_EXTERN_FUNC(int,
		ttdnd_init,(int Xid, _TT_CONST char **validtypes,
			    Ttdnd_send_data_func *sendfunc,
			    Ttdnd_input_func **input_func, int *fd));

_TT_EXTERN_FUNC(char *,ttdnd_negotiate_types,
		(_TT_CONST char **sourcetypes, _TT_CONST char **targettypes));

_TT_EXTERN_FUNC(void,ttdnd_start_transfer,
		(int sourceXid, const char *source_type,
		 Ttdnd_receive_data_func *receivefunc));
#undef _TT_CONST
#undef _TT_EXTERN_FUNC
#endif /* _TTDND_H */
				
				 
