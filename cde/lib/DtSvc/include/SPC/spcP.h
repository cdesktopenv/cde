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
 * $XConsortium: spcP.h /main/4 1996/05/15 12:01:55 drk $
 * Language:     C
 *
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1988,1993,1994,1996 Hewlett-Packard Company.
 * (c) Copyright 1993,1994,1996 International Business Machines Corp.
 * (c) Copyright 1993,1994,1996 Sun Microsystems, Inc.
 * (c) Copyright 1993,1994,1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */

#ifndef _spcP_h
#define _spcP_h

#include <netdb.h>		/* hostent */
#include <termios.h>

#include <bms/bms.h>
#include <bms/Symbolic.h>
#include <bms/XeUserMsg.h>
#include <bms/SbEvent.h>
#include <bms/spc.h>
#include <bms/scoop.h>
#include <bms/connect.h>
#include <bms/MemoryMgr.h>

#include "chars.h"
#include "spcE.h"
#include "spc-obj.h"

/* ---------------------------------------------------------------------- */

#ifndef CDE_CONFIGURATION_TOP
#define CDE_CONFIGURATION_TOP 		"/etc/opt/dt"
#endif
#ifndef CDE_INSTALLATION_TOP
#define CDE_INSTALLATION_TOP 		"/usr/dt"
#endif

#define Stdin(chn)	chn->file_descs[STDIN]
#define Stdout(chn)	chn->file_descs[STDOUT]
#define Stderr(chn)	chn->file_descs[STDERR]

/* Some necessary identifiers */
#define OK		0
#define ERROR		-1
#define Undefined	-1

/* Some character constants */

#define Channel_ByteID	(XeChar)'\077'

/* And finally, the common channel identifier */
#ifndef BITSPERBYTE
#define BITSPERBYTE 8
#endif
#define CID_High_Bits	(Channel_ByteID << (BITSPERBYTE * (sizeof(int) - 1)))
#define CID_Middle_Bits	(0x1010 << BITSPERBYTE)
#define CID_Low_Bits	(Channel_ByteID & 0xff)

#define Channel_Identifier	(CID_High_Bits | CID_Middle_Bits | CID_Low_Bits)

/*** Note: If device names ever increase from: /dev/ptym/ptyp0, look here ***/
#define PTY_NAMLEN		20
#define EXCEPT_FLAG             -1 /* Used for TIOC handling in PTY's */

/* Define lengths of buffers for protocol requests and other sizes of things */

#define REQUEST_HEADER_LENGTH 20
#define MAXREQLEN  (SPC_BUFSIZ+REQUEST_HEADER_LENGTH)
#define DEFAULT_ENVP_SIZE     50

/*
 **
 ** Definitions for select.
 **
*/

extern int max_fds;

/*
 * Macros to access SPC_IOMode bit fields
 */

/* These are derived from above spc bit specifications */
#define IS_SPCIO_STDIN(a)	(a & SPCIO_WRITEONLY)
#define IS_SPCIO_STDOUT(a)	(a & SPCIO_READONLY)
#define IS_SPCIO_STDERR(a)	(a & SPCIO_ERRORONLY)
#define IS_SPCIO_SEPARATE(a)	(a & SPCIO_SEPARATEREADERROR)

#define IS_SPCIO_NOIO(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_NOIO)
#define IS_SPCIO_WRITEONLY(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_WRITEONLY)
#define IS_SPCIO_READONLY(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_READONLY)
#define IS_SPCIO_READWRITE(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_READWRITE)
#define IS_SPCIO_ERRORONLY(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_ERRORONLY)
#define IS_SPCIO_WRITEERROR(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_WRITEERROR)
#define IS_SPCIO_READERROR(a)	((a & SPCIO_SOURCE_MASK) == SPCIO_READERROR)
#define IS_SPCIO_READWRITEERROR(a)((a&SPCIO_SOURCE_MASK) == SPCIO_READWRITEERROR)

/* Style flags (mutually exclusive) */
#define IS_SPCIO_PIPE(a)	((a & SPCIO_STYLE_MASK) == SPCIO_PIPE)
#define IS_SPCIO_PTY(a)         ((a & SPCIO_STYLE_MASK) == SPCIO_PTY)
#define IS_SPCIO_NOIOMODE(a)	((a & SPCIO_STYLE_MASK) == SPCIO_NOIOMODE)

#define IS_SPCIO_LINEEDIT(a)	(a & SPCIO_LINEEDIT)

/* Other non-mutually exclusive flags */
#define IS_SPCIO_SYSTEM(a)	(a & SPCIO_SYSTEM)
#define IS_SPCIO_LINEORIENTED(a)(a & SPCIO_LINEORIENTED)
#define IS_SPCIO_WAIT(a)	(a & SPCIO_WAIT)
#define IS_SPCIO_TOOLKIT(a)     (a & SPCIO_USE_XTOOLKIT)
#define IS_SPCIO_SYNC_TERM(a)   (a & SPCIO_SYNC_TERMINATOR)
#define IS_SPCIO_USE_LOGFILE(a) (a & SPCIO_USE_LOGFILE)

#define IS_SPCIO_SIGNAL_PGRP(a) (a & SPCIO_SIGNAL_PGRP)
#define IS_SPCIO_FORCE_CONTEXT(a) (a & SPCIO_FORCE_CONTEXT)

#define SPCIO_HAS_DATA          0x2000  /* Used for line-oriented IO */
#define HAS_DATA(chn)           (((chn->IOMode) & SPCIO_HAS_DATA) == SPCIO_HAS_DATA)

#define SPCIO_UNUSED3           0x80000

/* Flags for internal use only */

#define SPCIO_DEALLOC_ARGV         0x4000
#define IS_SPCIO_DEALLOC_ARGV(a)   (a & SPCIO_DEALLOC_ARGV)

#define SPCIO_DELAY_CLOSE       0x8000
#define IS_SPCIO_DELAY_CLOSE(a) (a & SPCIO_DELAY_CLOSE)

#define SPCIO_DO_CLOSE         0x10000
#define IS_SPCIO_DO_CLOSE(a)   (a & SPCIO_DO_CLOSE)

/* Make sure that STATE_OFFSET is always such that you will clear the
   IOMode flags */

#define STATE_OFFSET            24  
#define SPCIO_ACTIVE		(1<<STATE_OFFSET) /* channel has subprocess */
#define SPCIO_DATA		(2<<STATE_OFFSET) /* channel has data */

#define IS_SPCIO_ACTIVE(a)	(a & SPCIO_ACTIVE)
#define IS_SPCIO_DATA(a)        (a & SPCIO_DATA)

#define IS_ACTIVE(chn)          (IS_SPCIO_ACTIVE(chn->IOMode))
#define IS_DATA(chn)            (IS_SPCIO_DATA(chn->IOMode))

#define IS_REMOTE(chn)          (chn->connection)

/* SPC state machine */

#define CHANNEL_STATE(io_mode)              ((io_mode)>>STATE_OFFSET & 0x3)
#define MAKE_CHANNEL_STATE(io_mode, _state) \
  (((_state)<<STATE_OFFSET)   | (io_mode & ~(0x3<<STATE_OFFSET)))
#define MAKE_STATE(_data,_proc)             ((_data)<<1    | (_proc))
#define MAKE_STATE_INDEX(_old,_new)         ((_old)<<2     | (_new))
#define DATA_LINE(_state)                   ((_state) >> 1)
#define PROC_LINE(_state)                   ((_state) &  1)

/* Defaults */

#define DEFAULT_CHANNEL_CLASS pty_channel_class
#define IS_SPCIO_DEFAULT(a)   !((a) & SPCIO_STYLE_MASK)
#define SPCIO_DEFAULT         SPCIO_PTY

/*
 * Type definitions for SPC file descriptors
 */

/* Definitions for wires */

#define READ_SIDE       0
#define WRITE_SIDE      1

/*
 * Constants and external declarations for managing the SPC daemon's
 * logfile list, exit timer and process id list.
 */
#define SPCD_DEFAULT_TIMEOUT   			10
#define SPCD_DEAD_PROCESS      			-1
#define SPCD_NO_TIMER          			-1
#define SPCD_REQUEST_PENDING			1
#define SPCD_NO_REQUEST_PENDING			0

/*
 * Constants for the 'SPC_who_am_i' variable.
 */
#define SPC_I_AM_A_CLIENT			1
#define SPC_I_AM_A_DAEMON			2

/*
 * External variables defined in noio.c
 */
extern char **SPC_logfile_list;

/*
 * External variables defined in spc-exec.c
 */
extern pid_t *SPC_pid_list;
extern char *SPC_mount_point_env_var;

/*
 * External variables defined in spc-obj.c
 */
extern int SPC_who_am_i;

/*
 * The client version number.
 */
extern int SPC_client_version_number;

/*
 * The name of the directory used for authentication and
 * for temporary logfiles.
 */
extern XeString SPCD_Authentication_Dir;

/*
 * Definitions for file locations
 */
#define SPCD_ENV_INSTALL_DIRECTORY	CDE_INSTALLATION_TOP "/config"
#define SPCD_ENV_CONFIG_DIRECTORY       CDE_CONFIGURATION_TOP "/config"
#define SPCD_ENV_HOME_DIRECTORY		".dt"
#define SPCD_ENV_FILE			"dtspcdenv"
#define SPCD_PROG_NAME			"dtspcd"

/*
 * The name of the remove environment 'keyword'.
 */
#define SPC_REMOVE_VAR			"unset"

/*
 ***
 *** Start typedefs
 ***
*/

typedef int Files[3];           /* Standard set of file descriptors */
typedef int SPC_IOMode;		/* Sub-Process Control IO Mode Bits */

/* This is a "wire", a data structure used in creating communication paths
   to / from a subprocess.  It encapsulates all the information needed by
   all types of paths */
  
typedef struct _wire {
  SPC_IOMode flags;             /* Basically, acts like an IOMode, but
				   we are only interested in its data bits */
  int fd[2];
  XeString master_name, slave_name;
  int read_toolkit_id, except_toolkit_id;
  struct termios master_termio, slave_termio;
  struct _wire *next;
} Wire;

typedef struct _buffered_data {
  XeChar data[MAXREQLEN];
  int len, offset;
} buffered_data, *buffered_data_ptr;

typedef enum {
  SPC_Input,
  SPC_Exception,
  SPC_Terminator,
  SPC_Client
  } SPC_Callback_Condition;

/*
 * Type definition for an SPC Connection (a socket to SPCD server)
 */

typedef struct _SPC_Connection {
                                      /* The host on which other end resides */
  XeChar          hostname[MAXHOSTNAMELEN];
  int 	          sid;		      /* The Socket ID */
  XeChar 	  connected;	      /* Whether server connection is made */
  struct hostent *local;              /* socket address information for */
  struct hostent *remote;	      /* ... local, remote machine */
  int 		  termination_id;
  struct _SPC_Connection 
                  *next;	      /* The next server in the list */
  XeQueue         queued_remote_data; /* queue for remote data */
  int		  protocol_version;   /* Version of protocol of remote */
  XeString        hostinfo;   	      /* Info about remote, (type, os, etc) */

} SPC_Connection, *SPC_Connection_Ptr;

/*
 * This next macro returns a newly allocated string.  
 * The caller should free it.
 */

#define CONNECTION_HOSTNAME(conn) \
    ((conn->remote) ? XeFindShortHost((conn)->remote->h_name) : \
     Xestrdup(XeString_Empty))

/*
 * Type definition for an SPC Channel
 */

typedef int (*SPC_F_read_filter)(SPC_Channel_Ptr, int, XeString, int);

typedef struct _SPC_Channel {	/* Sub-Process Control Channel */

  /* stuff for SCOOP */
  channel_clasp class_ptr;
  root_object_part

  /* First, we place a channel identifier, so we can check cid (see below) */
  int identifier;		/* The common identifier for every channel */

  /* The setup request information */
  SPC_Connection *connection;	/* The connection structure (Local = NULL) */

  XeString context_dir;   	/* Directory to "cd" to during spawn */
  XeString path;		/* The command path pointer */
  XeString *argv;		/* The command argument list pointers */
  XeString *envp;		/* The command environment list pointers */

  SPC_IOMode IOMode;		/* The IO Control Mode settings */

  /* User defined data handling procedures */
  SbInputHandlerProc Input_Handler;
                                /* The user input handler routine */
  void * client_data;		/* The user associated input handler data */
				/* Useful (ie.) as input Widget */
  SPC_TerminateHandlerType Terminate_Handler;	/* The SIGCLD user handler routine */
  void * Terminate_Data;	/* The client_data for user handler routine */

  /* Now the recorded information */

  int pid;			/* The child Process IDentification number */
				/* Also the RPID for a Remote channel */
  int cid;			/* The remote Channel IDentification number */
  int status;			/* The Process status */
  Wire *wires[3];		/* The read side/write side file descriptors */
  Files file_descs;             /* Which file descriptors to use */
  Wire *wire_list;              /* Pointer to list of wires we have
				   (used for reset & pre_fork processing) */
  buffered_data_ptr linebufs[3]; /* Buffers for lined oriented IO */
  SPC_F_read_filter read_filter;
                                /* Function to use to read.  It may
				   be the actual read method, or it
				   may be a filter for newlines
				   (which would eventually call the read
				   method) */
  XeQueue  queued_remote_data;  /* queue for remote data */
  XeString logfile;             /* logfile name for SPCIO_USE_LOGFILE */
  int      close_timeout;       /* Amount of time to wait for a pty close */
  struct _SPC_Channel *next;	/* The next active channel */

  int    sync_pipe[2];		/* Only used in pty.c for __hpux_pty */

} SPC_Channel;

/* The default shell when none can be derived */
#ifdef hpV4
#define DEFAULT_SHELL	(XeString ) "/usr/bin/sh"
#else
#define DEFAULT_SHELL	(XeString ) "/bin/sh"
#endif /* hpV4 */

/* Some macros which define the bit field portions of a wait() status */
/* NOTE: These should be independent of integer word size */
#define WAIT_STATUS_MASK	0xff
#define IS_WAIT_STATUS_STOPPED	0177
#define IS_WAIT_STATUS_EXITED	0000
#define IS_WAIT_STATUS_DUMPED	0200

typedef unsigned long SPCInputId;

/* SPC/local.c */
int close_local_channel_object (SPC_Channel_Ptr channel);
int write_local_channel_object (SPC_Channel_Ptr channel, XeString buffer, int nbytes);
int signal_local_channel_object (SPC_Channel_Ptr channel, int sig);
int local_channel_object_wait_for_termination (SPC_Channel_Ptr channel);
int remove_logfile_local_channel_object (SPC_Channel_Ptr channel);
void local_channel_object_input_handler (void * client_data, int *source, SPCInputId *id);
int local_channel_object_send_eof(SPC_Channel_Ptr channel);

/* noio.c */
void noio_channel_class_init (object_clasp c);
SPC_Channel_Ptr open_noio_channel_object (SPC_Channel_Ptr channel, int iomode, XeString hostname);
int read_noio_channel_object (SPC_Channel_Ptr channel, int connector, XeString buffer, int nbytes);
int write_noio_channel_object (SPC_Channel_Ptr channel, XeString buffer, int nbytes);
int pre_fork_noio_channel_object (SPC_Channel_Ptr channel);
int post_fork_noio_channel_object (SPC_Channel_Ptr channel, int parentp);
int reset_noio_channel_object (SPC_Channel_Ptr channel);
int attach_noio_channel_object (SPC_Channel_Ptr channel, int);
void noio_channel_object_input_handler (void * client_data, int *source, SPCInputId *id);
int send_eof_noio_channel_object(SPC_Channel_Ptr channel);
int set_termio_noio_channel_object(SPC_Channel_Ptr, int, int, struct termios *);

/* pipe.c */
void pipe_channel_class_init (object_clasp c);
Wire *getpipe (Wire *prevwire);
SPC_Channel_Ptr open_pipe_channel_object (SPC_Channel_Ptr channel, int iomode, XeString hostname);
int read_pipe_channel_object (SPC_Channel_Ptr channel, int connector, XeString buffer, int nbytes);
int pre_fork_pipe_channel_object (SPC_Channel_Ptr channel);
int post_fork_pipe_channel_object (SPC_Channel_Ptr channel, int parentp);
int reset_pipe_channel_object (SPC_Channel_Ptr channel);
int attach_pipe_channel_object (SPC_Channel_Ptr channel, int);
int add_input_pipe_channel_object (SPC_Channel_Ptr channel, SbInputHandlerProc handler, void *data);


/* pty.c */
void pty_channel_class_init (object_clasp c);
SPC_Channel_Ptr open_pty_channel_object (SPC_Channel_Ptr channel, int iomode, XeString hostname);
int read_pty_channel_object (SPC_Channel_Ptr channel, int connector, XeString buffer, int nbytes);
int pre_fork_pty_channel_object (SPC_Channel_Ptr channel);
int post_fork_pty_channel_object (SPC_Channel_Ptr channel, int parentp);
int reset_pty_channel_object (SPC_Channel_Ptr channel);
int attach_pty_channel_object (SPC_Channel_Ptr channel, int pid);
int add_input_pty_channel_object (SPC_Channel_Ptr channel, SbInputHandlerProc handler, void *data);
int set_termio_pty_channel_object(SPC_Channel_Ptr channel, int, int,
				  struct termios *);
struct termios *SPC_Get_Current_Termio(void);
int SPC_Setpgrp(int);

/* remote.c */
void remote_channel_class_init (object_clasp c);
Wire *get_new_remote_wire (Wire *prevwire);
SPC_Channel_Ptr open_remote_channel_object (SPC_Channel_Ptr channel, int iomode, XeString hostname);
int close_remote_channel_object (SPC_Channel_Ptr channel);
int reset_remote_channel_object (SPC_Channel_Ptr channel);
int exec_proc_remote_channel_object (SPC_Channel_Ptr channel);
int write_remote_channel_object (SPC_Channel_Ptr channel, XeString buffer, int len);
int read_remote_channel_object (SPC_Channel_Ptr channel, int connection, XeString buffer, int len);
int signal_remote_channel_object (SPC_Channel_Ptr channel, int sig);
int attach_remote_channel_object (SPC_Channel_Ptr channel, int pid);
int remove_logfile_remote_channel_object (SPC_Channel_Ptr channel);
int add_input_remote_channel_object (SPC_Channel_Ptr channel, SbInputHandlerProc handler, void *data);
int SPC_Debug_Mode (SPC_Channel_Ptr channel, XeString file);
int send_eof_remote_channel_object(SPC_Channel_Ptr channel);
int set_termio_remote_channel_object(SPC_Channel_Ptr channel,
				     int, int, struct termios *);


/* spc-env.c */
XeString SPC_Getenv (XeString var, XeString *envp);
XeString *SPC_Putenv (XeString val, XeString *envp);
XeString *SPC_Add_Env_File (XeString filename, XeString *envp);
XeString *SPC_Create_Default_Envp (XeString *old_envp);
XeString *SPC_Fixup_Environment (XeString *envp, SPC_Channel_Ptr channel);
XeString *SPC_Merge_Envp (XeString *dest_envp, XeString *source_envp);
void SPC_Free_Envp (XeString *envp);


/* spc-exec.c */
int SPC_Setup_Synchronous_Terminator (void);
SPC_Connection_Ptr SPC_Channel_Terminator_Connection (SPC_Channel_Ptr channel);
void SPC_Close_Unused (void);
int SPC_MakeSystemCommand (SPC_Channel_Ptr channel);
void SPC_Child_Terminated(int);
int exec_proc_local_channel_object (SPC_Channel_Ptr channel);


/* spc-net.c */
Boolean SPC_Init_Local_Host_Info (void);
int SPC_Local_Hostname (XeString hostname);
SPC_Connection_Ptr SPC_Open_Connection (XeString hostname);
int SPC_Open_Socket (SPC_Connection_Ptr conn, int type);
int SPC_Contact_Server (SPC_Connection_Ptr connection);
SPC_Connection_Ptr SPC_Init_Child (SPC_Connection_Ptr conn, int from);
SPC_Connection_Ptr SPC_Standalone_Daemon (SPC_Connection_Ptr conn);
int SPC_Inetd_Daemon (SPC_Connection_Ptr conn);
SPC_Connection_Ptr SPC_Start_Daemon (int standalone);


/* spc-obj.c */
void spc_init_fds(void);
object *alloc_channel_object (object_clasp c);
void channel_class_init (object_clasp c);
int SPC_ResetTerminator(void);
int SPC_Initialize (void);
SPC_Channel_Ptr SPC_Initialize_Channel (XeString hostname, int iomode);
void SPC_Channel_Terminated (SPC_Channel_Ptr channel);
int SPC_Check_Style (int iomode);
int SPC_Transform_Iomode (int iomode);
int SPC_Newline_Filter (SPC_Channel_Ptr channel, int connector, XeString buffer, int ntoread);
int SPC_Input_Handler (SPC_Channel_Ptr channel, int connector);
SPC_Channel_Ptr open_channel_object (SPC_Channel_Ptr channel, int iomode, XeString hostname);
int close_channel_object (SPC_Channel_Ptr channel);
int read_channel_object (SPC_Channel_Ptr channel, int connector, XeString buffer, int nbytes);
int write_channel_object (SPC_Channel_Ptr channel, XeString buffer, int nbytes);
int reset_channel_object (SPC_Channel_Ptr channel);
int pre_fork_channel_object (SPC_Channel_Ptr channel);
int post_fork_channel_object (SPC_Channel_Ptr channel, int parentp);
int exec_proc_channel_object (SPC_Channel_Ptr channel);
int signal_channel_object (SPC_Channel_Ptr channel, int sig);
int channel_object_wait_for_termination (SPC_Channel_Ptr channel);
int attach_channel_object (SPC_Channel_Ptr channel, int pid);
int add_input_channel_object (SPC_Channel_Ptr channel, SbInputHandlerProc handler, void *data);
int remove_logfile_channel_object (SPC_Channel_Ptr channel);

/* spc-sm.c */
int SPC_Change_State (SPC_Channel_Ptr channel, int connector, int data_line, int process_line);
int error_fun (SPC_Channel_Ptr channel, int connector);
int connector_eof (SPC_Channel_Ptr channel, int connector);
int connector_eof_with_reset (SPC_Channel_Ptr channel, int connector);
int sigcld_with_reset (SPC_Channel_Ptr channel, int connector);

/* spc-xt.c */

typedef void (*spc_handler_func_type) (void *, int *, SPCInputId *);
int SPC_Wait_For_Termination (SPC_Channel_Ptr channel);
void SPC_XtBreak (void);
void SPC_XtAddInput (SPC_Channel_Ptr channel, int *id_addr, int fd, spc_handler_func_type handler, SPC_Callback_Condition condition);
void SPC_XtRemoveInput (int *id_addr, SPC_Callback_Condition condition);


/* spc-util.c */
Wire *get_new_wire (void);
void free_wire (Wire *wire);
SPC_Channel_Ptr SPC_Find_PID (int pid);
int spc_close (int fd);
int spc_dup2 (int from, int to);
int SPC_fd_to_connector (SPC_Channel_Ptr channel, int fd);
XeString *Alloc_Argv (int n);
void SPC_Conditional_Packet_Handler (void * client_data, int *source, SPCInputId *id);
int sprintf_len (XeString s, XeString format, ...);
typedef Boolean (path_search_predicate)(XeString, XeString, XeString);
Boolean path_search (XeString path, XeString filename, path_search_predicate p);


/* spc-error.c */
XeString SPC_copy_string (XeString str);
void SPC_Error (int error, ...);
int SPC_Make_Log_Filename (XeString name, int unique);
int SPC_Open_Log (XeString filename, int unique);
int SPC_Close_Log (void);
int SPC_Write_Log (XeString str);
int SPC_Format_Log (XeString format, ...);
SPCError *SPC_Lookup_Error (int errornum);
SPCError *SPC_Lookup_Error (int errornum);


/* spc-termio.c */
XeString SPC_Decode_Termios( struct termios * );
void SPC_Encode_Termios(XeString buff, struct termios *tio);

#endif /* _spcP_h */
