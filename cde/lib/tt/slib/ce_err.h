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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: ce_err.h /main/3 1995/10/23 11:48:51 rswiston $ 			 				 */
/* @(#)ce_err.h	1.2 @(#) 
 * See ce.h in this directory for an explanation of why this piece of
 * the Classing Engine is in the ToolTalk directory.
 */

/* 
 * Error messages used by the CE
 */
#ifndef ce_err_h
#define ce_err_h

#define CE_ERR_INTERNAL_ERROR			-1
#define CE_ERR_ERROR_READING_DB			1
#define CE_ERR_WRITE_IN_PROGRESS		2
#define CE_ERR_DB_NOT_LOADED			3
#define CE_ERR_WRITE_NOT_STARTED		4
#define CE_ERR_NAMESPACE_EXISTS			5
#define CE_ERR_NAMESPACE_NOT_EMPTY		6
#define CE_ERR_NAMESPACE_DOES_NOT_EXIST		7
#define CE_ERR_NS_ENTRY_EXISTS			8
#define CE_ERR_ENTRY_NOT_ALLOCED		9
#define CE_ERR_ATTRIBUTE_EXISTS			10
#define CE_ERR_NO_MEMORY			11
#define CE_ERR_NO_PERMISSION_TO_WRITE		12
#define CE_ERR_WRONG_ARGUMENTS			13
#define CE_ERR_ERROR_WRITING_DB			14
#define CE_ERR_OPENING_DB			15
#define CE_ERR_DB_LOCKED			16
#define CE_ERR_ERROR_OPENING_FILE		17
#define CE_ERR_WRONG_DATABASE_VERSION		18
#define CE_ERR_UNKNOWN_DATABASE_NAME		19
#define CE_ERR_BAD_DATABASE_FILE		20

#endif ce_err_h
