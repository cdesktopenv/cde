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
 * File:         spc.h $XConsortium: spc.h /main/3 1995/10/26 15:48:38 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _spc_h
#define _spc_h

/* -------------------------------------------- */
/* Requires:					*/
#include <stdio.h>
#include <bms/XeUserMsg.h>

#include <termios.h>

/* -------------------------------------------- */

typedef struct _SPC_Channel *SPC_Channel_Ptr;

typedef struct _XeHostInfo {
    XeString	os;
    XeString    os_ver;
    XeString    hw_arch;
} *XeHostInfo;

extern FILE *SPC_Print_Protocol;
extern FILE *spc_logF;
extern XeString spc_user_environment_file;

/* Error returns for SPC routines */

#define SPC_ERROR       FALSE  /* Use this value for error checking */

/*
 * These are the channel connector definitions
 */

#define STDIN			0
#define STDOUT			1
#define STDERR			2

#define MASTER_SIDE	0
#define SLAVE_SIDE	1

/* These are the sub-process notification identifiers */

#define SPC_PROCESS_STOPPED	1 /* Child process is in background */
#define SPC_PROCESS_EXITED	2 /* Child process called exit(cause); */
#define SPC_PROCESS_SIGNALLED	3 /* Child process received signal: cause */
#define SPC_PROCESS_INTERRUPT	4 /* Child process WAIT was interrupted */
#define SPC_PROCESS_DUMPED(a)	((a) & 0200) /* True when core dumped */

/* This is the maximum size of an SPC I/O Buffer */
#define SPC_BUFSIZ		4096

/*
 * These macros define the bit field portion of an SPC_IOMode
 */

#define SPCIO_ALL_MASK		0xffffffff

/* The IO Modes that define the input and output sources */

#define SPCIO_SOURCE_MASK	0xf
#define SPCIO_NOIO		0x0 /* The default - no input/output */
#define SPCIO_WRITEONLY		0x1 /* Only write app stdin */
#define SPCIO_READONLY		0x2 /* Only read app stdout */
#define SPCIO_READWRITE		0x3 /* Read stdout, write stdin */
#define SPCIO_ERRORONLY		0x4 /* Only read stderr */
#define SPCIO_WRITEERROR	0x5 /* Write stdin, read stderr */
#define SPCIO_READERROR		0x6 /* Only read stdout/stderr */
#define SPCIO_READWRITEERROR	0x7 /* Full std (in, out, err) */

/* Use this bit with above IO Mode for splitting stdout and stderr data */

#define SPCIO_SEPARATEREADERROR	0x8 /* Separate stdout & stderr */

/* The IO Modes that deal with communication styles (features) */
#define SPCIO_STYLE_MASK	0x70
#define SPCIO_PTY               0x10 /* Use a PTY */
#define SPCIO_PIPE		0x20 /* Use pipe() - no line editing */
#define SPCIO_NOIOMODE          0x40 /* Use neither */

#define SPCIO_LINEEDIT          0x80 /* Valid only with PTY */

/* Other flags */

#define SPCIO_SYSTEM		0x100 /* Use system() - Spawns a SHELL */
#define SPCIO_LINEORIENTED	0x200 /* Invoke callback on line bounds */
                                      /* It is possible to break two byte
					 characters.  See note on XeSPCRead. */
#define SPCIO_WAIT		0x400 /* Wait for process to finish */
#define SPCIO_USE_XTOOLKIT	0x800 /* Use the X toolkit */
#define SPCIO_SYNC_TERMINATOR  0x1000 /* Handle termination synchronously */
#define SPCIO_USE_LOGFILE      0x2000 /* Use logfile for stderr -- only
					 valid with SPCIO_NOIO */

#define SPCIO_SIGNAL_PGRP     0x20000 /* Propagate signals to entire process
					 group. */
#define SPCIO_FORCE_CONTEXT   0x40000 /* Error on SPC Spawn if context dir
					 is not present */

/*
 * Routines
 */


/* spc.c */


#if defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
# define EXTERN_DECL(type, name, arglist) type name arglist
# if defined(__cplusplus) && defined(__c_callable)
#  define EXTERN_C_CALLABLE(type, name, arglist) \
          extern "C" { type name arglist ; }
# else
#  define EXTERN_C_CALLABLE(type, name, arglist) \
          EXTERN_DECL(type, name, arglist)
# endif
#else
#ifdef _AIX
# define EXTERN_C_CALLABLE(type, name, arglist) \
	 extern type name arglist 
# define EXTERN_DECL(type, name, arglist) \
	 type name arglist
#else /* _AIX */
# define EXTERN_DECL(type, name, arglist) name arglist
# define EXTERN_C_DECL(type, name, arglist) EXTERN_DECL(type, name, arglist)

#endif /* (_AIX) */
#endif

EXTERN_C_CALLABLE(SPC_Channel_Ptr, XeSPCOpen, (XeString hostname, int iomode));

/*
  Open an SPC channel.  Process will run on 'hostname' (or the local
  host if value is NULL), with the specified iomode.
*/

EXTERN_C_CALLABLE(int, XeSPCClose, (SPC_Channel_Ptr channel));

/*
  Close an SPC channel.  Closing a channel will automatically deactivate it
  (meaning that any subprocess associated with the channel is terminated).
*/

EXTERN_DECL(int, XeSPCReset, (SPC_Channel_Ptr channel));

/*
  Reset an SPC channel.  This will allow it to be used in a subsequent
  spawn or exec call.
*/


EXTERN_DECL(int, XeSPCRead,
	    (SPC_Channel_Ptr channel, int connector,
	     XeString buffer, int length));

/*
  Read length characters from an SPC channel into some preallocated buffer.
  Note that it is possible to split a two-byte character, if the first
  byte if the character is read in just at buffer[length].  However, the
  next read will return the second byte (just like Unix read).  The
  'connector' value is either STDOUT or STDERR.
*/

EXTERN_DECL(int, XeSPCWrite,
	    (SPC_Channel_Ptr channel, XeString buffer, int length));

/*
  Write length characters from buffer to the standard input of a
  process on the other side of an SPC channel.
*/

EXTERN_C_CALLABLE(int, XeSPCActive, (SPC_Channel_Ptr channel));

/*
  Returns True when channel is active, False otherwise
*/  

EXTERN_DECL(int, XeSPCData, (SPC_Channel_Ptr channel));

/*
  Returns True when channel be read from, False otherwise
*/  

EXTERN_C_CALLABLE(int, XeSPCSpawn,
		  (XeString pathname, XeString context_dir, XeString *argv,
		   XeString *envp, SPC_Channel_Ptr channel));

/*
  Spawn an application under SPC
*/

EXTERN_DECL(SPC_Channel_Ptr, XeSPCOpenAndSpawn,
	    (XeString hostname, int iomode, XeString pathname,
	     XeString context_dir, XeString *argv, XeString *envp));
	    
/*
  Combine the Open and Spawn channel operations
*/

EXTERN_DECL(int, XeSPCExecuteProcess, (SPC_Channel_Ptr channel));

/*
  Restart a new subprocess on a channel
*/

EXTERN_C_CALLABLE(void, XeSPCKillProcesses, (int wait));

/*
  Kill all known executing processes (useful for catching SIGTERM, etc)
*/

EXTERN_DECL(int, XeSPCKillProcess, (SPC_Channel_Ptr channel, int wait));

/*
  Kill executing process on an SPC channel.  'wait' TRUE means don't
  return from call until process is completely terminated (including
  after user specified callbacks are called).
*/

EXTERN_DECL(int, XeSPCInterruptProcess, (SPC_Channel_Ptr channel));

/*
  Interrupt executing process on an SPC channel (send SIGINT).
*/

EXTERN_DECL(int, XeSPCSignalProcess, (SPC_Channel_Ptr channel, int sig));

/*
  Send an arbitrary signal to executing process on an SPC channel.
*/  

typedef
  EXTERN_DECL(void, (*SbInputHandlerProc),
	      (void *client_data, XeString buf, int nchars, int connector));

EXTERN_C_CALLABLE(int, XeSPCAddInput,
                  (SPC_Channel_Ptr    channel,
                   SbInputHandlerProc handler,
                   void               *client_data));

/*
  Add an input channel handler
*/

/*
 * The user input handler takes the following form:
 *
 * void UserInputHandler(client_data, text, size, connection)
 *      void * client_data; *** Useful for passing widet destination ***
 *      XeString text;      *** The text coming from the SPC channel ***
 *      int size;           *** The number of character in passed text ***
 *      int connection;     *** The connection where data was received ***
 *                          ***  (STDOUT or STDERR)                    ***
 */

typedef
  EXTERN_DECL(void, (*SPC_TerminateHandlerType),
              (SPC_Channel_Ptr  chan,
               int  pid,
               int  type,
               int  cause,
               void *Terminate_Data));

EXTERN_C_CALLABLE(int, XeSPCRegisterTerminator,
                  (SPC_Channel_Ptr          channel,
                   SPC_TerminateHandlerType teminator,
                   void *                   client_data));

/*
  Add a termination handler to a channel (called when sub-process dies)
*/

/*
 * The user termination handler takes the following form:
 *
 * void UserTerminator(channel, pid, type, cause, client_data)
 *      SPC_CHannel_ptr channel;
 *      int pid;            *** The Process ID of the terminated appl. ***
 *      int type;           *** The type of termination (see above)    ***
 *      int cause;          *** The number associated w/termination    ***
 *      void * client_data; *** User specified client data             ***
 */

/*
 * These are the channel access routines
 */

EXTERN_DECL(XeString, XeSPCGetDevice,
	    (SPC_Channel_Ptr channel, int connector, int side));

/*
  Return the device name associated with a side of a channel device
  pair.  'connector' is either STDIN, STDOUT, or STDERR, and 'side' is
  either MASTER_SIDE or SLAVE_SIDE.  This call is valid only in PTY
  channels.
*/  

EXTERN_DECL(int, XeSPCGetProcessStatus,
	    (SPC_Channel_Ptr channel, int *type, int *cause));

/*
  Fill in the type and cause of a process termination.
*/

EXTERN_DECL(int, XeSPCAttach, (SPC_Channel_Ptr channel, int pid));

/*
  Returns True if a process ID was associated with an SPC channel.
*/

EXTERN_DECL(int, XeSPCDetach, (SPC_Channel_Ptr channel));

EXTERN_DECL(int, XeSPCGetPID, (SPC_Channel_Ptr channel));

/*
  Returns the Process ID of the channel or NULL if none
*/  

EXTERN_DECL(int, XeSPCGetLogfile,
	    (SPC_Channel_Ptr channel, XeString *host, XeString *file));

/*
  Return the logfile for the channel.  If the channel was not opened
  with SPCIO_USE_LOGFILE specified, it will return NULL.  Also note
  that it returns an XeString *, not an XeString.
*/  

EXTERN_DECL(int, XeSPCRemoveLogfile, (SPC_Channel_Ptr channel));

/*
  Remove the logfile associated with the channel
*/

/*
 *
 * Features currently not implemented:
 *
 *  SPCIO_WAIT with ptys
 *
 *  SEPARATEREADERROR with ptys
 *
 *  Complete error checking.  For example, there
 *  are no checks for trying to write to a channel
 *  opened W/O SPCIO_WRITE specified
 *
 */

EXTERN_DECL(int, XeSPCGetChannelSyncFd, (SPC_Channel_Ptr channel));

/*
  Get the file descriptor for checking synchronous termination.  This
  is used for interfacing with event loops.
*/

EXTERN_DECL(SPC_Channel_Ptr, XeSPCHandleTerminator, (int fd));

/*
  Handle a synchronous termination condition

  This routine is to be used with the file descriptor returned by
  XeSPCGetChannelSyncFd.  The idea is that one opens a channel using
  SPCIO_SYNC_TERMINATOR set in the iomode, and then at some point
  checks for input available on the returned file descriptor (possibly
  using a select(2) system call).  If there is input, it means that
  some SYNC_TERMINATOR channel had a subprocess die.  The program then
  calls XeSPCHandleTerminator to get the termination handler invoked.
  IT IS THE RESPONSIBILITY OF THE USER PROGRAM TO EVENTUALLY CALL
  XeSPCHandleTerminator.  IF IT DOES NOT, THE PROGRAM MAY DEADLOCK
  ITSELF.
*/


/* SPC Error handling */

typedef struct _SPCError { /* An SPC Error message */
  XeString text;     /* The text */
  XeString format;   /* How to format args */
  XeSeverity severity;     /* How bad is it, doc? */
  char use_errno;          /* Whether to use the system errno */
} SPCError;

/* Use this to get the current error number */

extern int XeSPCErrorNumber;

EXTERN_DECL(SPCError *, XeSPCLookupError, (int errnum));

/*
  Returns the SPCError structure associated with the passed error number or
   NULL if the passed error number is not a valid SPC error.  The
   error structure returned will be overwritten by a subsequent
   XeSPCLookupError call.
*/

EXTERN_DECL(void, XeSPCShutdownCallbacks, (void));

EXTERN_DECL(void, XeSPCRestartCallbacks, (void));

/* These two routines are used to temporarily suspend SPC callbacks */

EXTERN_DECL(int, XeSetpgrp, (int read_current_termio));

/*
  This routine will do the following:

  1. open /dev/tty
  2. get the termio information from the file descriptor just opened
  3. close /dev/tty
  3. allocate a master / slave pty pair, opening the master side
  4. set the termio info of the master side to be the result of step 2
  5. setpgrp
  6. open the slave side.
  
  All of this has the effect of making the process which called this
  routine immune to interrupts, etc., but also passing on the termio
  characteristics of the original tty.

  If read_current_termio is non-zero, steps 1-3 will NOT be performed, but
  instead this routine will get the information from the following termio
  struct:
  
*/

EXTERN_DECL(XeHostInfo, SPC_GetHostinfo, (SPC_Channel_Ptr channel));

/* 
  Return information about the host (os, os-ver, hw) to which "channel"
  is currently connected.  The return is to a static structure of static
  strings.  Do not modify or free and of them!
*/

/*
 **
 ** New B.00 functions
 **
*/

EXTERN_C_CALLABLE(int, XeSPCSendEOF, (SPC_Channel_Ptr channel));

/*
 Close the standard input of the process on the other side of the channel
*/

EXTERN_C_CALLABLE(int, XeSPCSetTermio,
		  (SPC_Channel_Ptr channel,
		   int connection,
		   int side,
		   struct termios *termio));

/*
 Set the termio value of the PTY associated with 'connection' (STDIN,
 STDOUT, or STDERR), on 'side' (MASTER_SIDE or SLAVE_SIDE) to the
 value pointed to by termio.  This call must be made before the
 subprocess is spawned.
*/

#endif /* #ifdef _spc_h */
