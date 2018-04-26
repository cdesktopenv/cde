/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * File:         spc-obj.h $XConsortium: spc-obj.h /main/3 1995/10/26 15:43:20 rswiston $
 * Language:     C
 *
 * (c) Copyright 1989, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _spc_obj_h
#define _spc_obj_h

typedef SPC_Channel_Ptr (*SPC_F_open)(SPC_Channel_Ptr, int, XeString);
typedef int    (*SPC_F_close)(SPC_Channel_Ptr);
typedef int    (*SPC_F_read)(SPC_Channel_Ptr, int, XeString, int);
typedef int    (*SPC_F_write)(SPC_Channel_Ptr, XeString, int);
typedef int    (*SPC_F_reset)(SPC_Channel_Ptr);
typedef int    (*SPC_F_exec_proc)(SPC_Channel_Ptr);
typedef int    (*SPC_F_signal)(SPC_Channel_Ptr, int);
typedef int    (*SPC_F_attach)(SPC_Channel_Ptr, int);
typedef int    (*SPC_F_add_input)(SPC_Channel_Ptr, SbInputHandlerProc, void *); 
typedef int    (*SPC_F_wait_for_termination)(SPC_Channel_Ptr);
typedef int    (*SPC_F_pre_fork)(SPC_Channel_Ptr);
typedef int    (*SPC_F_post_fork)(SPC_Channel_Ptr, int);
typedef int    (*SPC_F_remove_logfile)(SPC_Channel_Ptr);

/*
 **
 ** New methods for B.00
 **
*/ 

typedef int    (*SPC_F_send_eof)(SPC_Channel_Ptr);
typedef int    (*SPC_F_set_termio)(SPC_Channel_Ptr, int, int, struct termios *);

#define channel_class_part  \
  root_class_part        \
  SPC_F_open open;       /* opening a channel */                  \
  SPC_F_close close;      /* close a channel */                        \
  SPC_F_read read;       /* read from a channel */                    \
  SPC_F_write write;      /* write to a channel */                     \
  SPC_F_reset reset;      /* reset io to channel */                    \
  SPC_F_exec_proc exec_proc;  /* execute a subprocess over a channel */    \
  SPC_F_signal signal;     /* signal a subprocess over a channel */     \
  SPC_F_attach attach;     /* attach a subprocess to this channel */    \
  SPC_F_add_input add_input;  /* how to add input callbacks to channel */  \
  SbInputCallbackProc input;        \
  SPC_F_wait_for_termination wait_for_termination; /* wait for subprocess to exit */  \
  SPC_F_pre_fork pre_fork;   /* setup before a fork */                    \
  SPC_F_post_fork post_fork;  /* setup after a fork */                     \
  SPC_F_remove_logfile remove_logfile; /* remove log file associated with channel */ \
  SPC_F_send_eof send_eof;    /* close stdin of subprocess */ \
  SPC_F_set_termio set_termio; /* set termio struct for PTY channels */

typedef struct channel_class {
  root_clasp base;
  channel_class_part
  } *channel_clasp;

typedef struct pty_channel_class {
  channel_clasp base;
  channel_class_part
  } *pty_channel_clasp;
  
typedef struct pipe_channel_class {
  channel_clasp base;
  channel_class_part
  } *pipe_channel_clasp;
  
typedef struct noio_channel_class {
  channel_clasp base;
  channel_class_part
  } *noio_channel_clasp;
  
typedef struct remote_channel_class {
  channel_clasp base;
  channel_class_part
  } *remote_channel_clasp;

#define call_parent_method(channel, func, args, result) { \
  pipe_channel_clasp parent_class=(pipe_channel_clasp) (channel)->class_ptr; \
  result=base_memf(parent_class, func, args); }

/* class definitions */

extern channel_clasp        channel_class;
extern pty_channel_clasp    pty_channel_class;
extern pipe_channel_clasp   pipe_channel_class;
extern noio_channel_clasp   noio_channel_class;
extern remote_channel_clasp remote_channel_class;

#endif /* _spc_obj_h */

