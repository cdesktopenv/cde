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
 * File:         spcE.h $XConsortium: spcE.h /main/3 1995/10/26 15:43:58 rswiston $
 * Language:     C
 *
 * (c) Copyright 1988, Hewlett-Packard Company, all rights reserved.
 *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc.				*
 */

#ifndef _spcE_h
#define _spcE_h

/* Keep this up to date with the first error number declared below */
#define SPC_First_Error_Number 100
#define SPC_Min_Error          SPC_First_Error_Number

#define SPC_Out_Of_Memory      100
#define SPC_Bad_Argument       101
#define SPC_Active_Channel     102
#define SPC_Inactive_Channel   103
#define SPC_Internal_Error     104
#define SPC_Cannot_Fork        105
#define SPC_Cannot_Exec        106
#define SPC_Cannot_Execute     107
#define SPC_No_Pipe            108
#define SPC_No_Pty             109
#define SPC_Bad_Connector      110
#define SPC_Reading            111
#define SPC_Writing            112
#define SPC_Bad_Service        113
#define SPC_Bad_Port           114
#define SPC_Unknown_Host       115
#define SPC_Bad_Socket         116
#define SPC_Bad_Connect        117
#define SPC_Bad_Bind           118
#define SPC_Bad_Listen         119
#define SPC_Bad_Accept         120
#define SPC_Bad_Linger         121
#define SPC_Bad_Reuse          122
#define SPC_Bad_Sockname       123
#define SPC_Bad_Open           124
#define SPC_Connection_EOF     125
#define SPC_Timeout            126
#define SPC_Protocol           127
#define SPC_Unexpected_Reply   128
#define SPC_No_Channel         129
#define SPC_Illegal_Iomode     130
#define SPC_No_Signal_Handler  131
#define SPC_Bad_Operation      132
#define SPC_Bad_Fd             133
#define SPC_Bad_Ioctl          134
#define SPC_Bad_Select         135
#define SPC_Bind_Timeout       136
#define SPC_Arg_Too_Long       137
#define SPC_Write_Prot         138
#define SPC_Bad_Username       139
#define SPC_Bad_Password       140
#define SPC_Client_Not_Valid   141
#define SPC_Cannot_Open_Slave  142
/* #define SPC_Register_Error     143 */ /* This has been obsoleted by the
                                            expanded register errors below */
#define SPC_Protocol_Abort     144
#define SPC_Env_Too_Big        145
#define SPC_Unlink_Logfile     146
#define SPC_Closed_Channel     147
#define SPC_Bad_Authentication 148
#define SPC_Cannot_Open_Log    149
#define SPC_Connection_Reset   150
#define SPC_Register_Username  151
#define SPC_Register_Netrc     152
#define SPC_Register_Open      153
#define SPC_Register_Handshake 154

#define SPC_Bad_Termios_Mode   155
#define SPC_Bad_Termios_Speed  156
#define SPC_Bad_Termios_CC     157
#define SPC_Bad_Termios_Proto  158

#define SPC_Bad_Signal_Name    159
#define SPC_Bad_Signal_Value   160
#define SPC_Bad_Signal_Format  161

#define SPC_Bad_tc_Call        162

#define SPC_cannot_Chdir       163

#define SPC_Bad_Permission     164
#define SPC_Cannot_Create_Netfilename	165
#define SPC_Protocol_Version_Error 	166

/* JET - a special error code for goobers trying to overflow our buffers. */
/* VU#172583 */
#define SPC_Buffer_Overflow    167

/* Keep this up to date with the last error number declared above */
#define SPC_Max_Error          168

/* The definition of the SPC Error structure has been moved to spc.h
   (to make it public) */

#endif /*  _spcE_h */
