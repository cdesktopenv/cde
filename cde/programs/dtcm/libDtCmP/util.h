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
/*******************************************************************************
**
**  util.h
**
**  static char sccsid[] = "@(#)util.h 1.38 94/12/20 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: util.h /main/5 1995/11/03 10:39:22 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <sys/types.h>
#include <X11/Xlib.h>
#include <csa.h>
#include "ansi_c.h"
#include "timeops.h"
#include "props.h"


#define DATAVER1        1       /* RPC versions 1 & 2 */
#define DATAVER2        2       /* RPC version 3 */
#define DATAVER3        3       /* RPC version 4 and RPC version 5 with
                                   old file store */
#define DATAVER4        4       /* RPC version 5 with new data store */
#define DATAVER_ARCHIVE -1	/* archive version.  Nothing is read_only */


#define MAXSTRING	80
#define CERROR		1
#define PWERROR		2
#define FILEERROR	3
#define SPACEERROR	4
#define DOTS(a)		(a[0]=='.' && (a[1]==0 || (a[1]=='.' && a[2]==0)))

typedef struct lines {
	char		*s;
	struct lines	*next;
} Lines;

typedef enum {appt_read, appt_write} Allocation_reason;

/*
**  Structure for list of properties
*/
typedef struct Pentry {
        char            *property_name;
        char            *property_value;
        struct Pentry   *next;
} Pentry;

typedef enum {
        ORDER_MDY,
        ORDER_DMY,
        ORDER_YMD
} OrderingType;
 
typedef enum {
        SEPARATOR_BLANK,
        SEPARATOR_SLASH,
        SEPARATOR_DOT,
        SEPARATOR_DASH
} SeparatorType;

/*
 * This convenience structure is used by the utility functions to set pointers
 * to the actual data array so hard coding indexes into the array elsewhere
 * in the front end isn't necessary.
 */
typedef struct {
	/*
	 * Read-only attributes
	 */
	CSA_attribute	*identifier;
	CSA_attribute	*modified_time;
	CSA_attribute	*author;
	CSA_attribute	*number_recurrence;

	/*
	 * Read-write attributes
	 */
	CSA_attribute	*time;
	CSA_attribute	*type;
	CSA_attribute	*subtype;
	CSA_attribute	*private;
	CSA_attribute	*end_time;
	CSA_attribute	*show_time;
	CSA_attribute	*what;
	CSA_attribute	*state;
	CSA_attribute	*repeat_type;
	CSA_attribute	*repeat_times;
	CSA_attribute	*repeat_interval;
	CSA_attribute	*repeat_week_num;
	CSA_attribute	*recurrence_rule;
	CSA_attribute	*beep;
	CSA_attribute	*flash;
	CSA_attribute	*mail;
	CSA_attribute	*popup;
	CSA_attribute	*sequence_end_date;

	/*
	 * The actual name array and count
	 */

	CSA_attribute_reference *names;
	int		num_names;

	/*
	 * The actual data array and count
	 */
	CSA_attribute	*attrs;
	CSA_uint32	count;

	/* Whether the structure has been filled in with a query */

	int		filled;
	Allocation_reason reason;
	
	/* the version of the back end this appointment structure 
	   was intended for */

	int		version;
} Dtcm_appointment;

/*
 * This convenience structure is used by the entry_to_cal, attrs_to_cal, and
 * cal_to_attrs utility functions
 */
typedef struct {
	/*
	 * Read-only attributes
	 */
	CSA_attribute	*cal_name;
	CSA_attribute	*server_version;
	CSA_attribute	*num_entries;
	CSA_attribute	*cal_size;

	/*
	 * Read-write attributes
	 */
	CSA_attribute	*access_list;
	CSA_attribute	*time_zone;

	/*
	 * The actual name array and count
	 */

	CSA_attribute_reference *names;
	int		num_names;

	/*
	 * The actual data array and count
	 */
	CSA_attribute		*attrs;
	CSA_uint32		count;

	/* Whether the structure has been filled in with a query */

	int		filled;
	Allocation_reason reason;
	
	/* the version of the back end this calendar structure 
	   was intended for */

	int		version;
} Dtcm_calendar;

/*
 * Utility functions to to provide pointers into the arrays passed back and
 * forth to the backend.  These functions and structures make hard coding
 * indexes into the arrays unnecessary.
 */
extern Dtcm_appointment	*allocate_appt_struct	P((Allocation_reason , int, ...));
extern CSA_return_code  query_appt_struct       P((CSA_session_handle, 
						   CSA_entry_handle, 
						   Dtcm_appointment *));
extern Dtcm_calendar	*allocate_cal_struct	P((Allocation_reason, int, ...));
extern CSA_return_code  query_cal_struct        P((CSA_session_handle, 
						   Dtcm_calendar *));
extern void		free_appt_struct	P((Dtcm_appointment**));
extern void		free_cal_struct		P((Dtcm_calendar**));

extern boolean_t	cal_ident_index_ro	P((int, int));
extern boolean_t	entry_ident_index_ro	P((int, int));
extern CSA_enum		cal_ident_index_tag	P((int));
extern CSA_enum		entry_ident_index_tag	P((int));
extern boolean_t	ident_name_ro	P((char*, int));
extern void		initialize_cal_attr	P((int, CSA_attribute*,
						   Allocation_reason, int));
extern void		initialize_entry_attr	P((int, CSA_attribute*,
						   Allocation_reason, int));
extern void		set_appt_links		P((Dtcm_appointment*));
extern void		set_cal_links		P((Dtcm_calendar*));
extern void		scrub_cal_attr_list	P((Dtcm_calendar*));
extern void		setup_range		P((CSA_attribute**,
						   CSA_enum**, int*, time_t,
						   time_t, CSA_sint32, CSA_sint32,
						   boolean_t, int));
extern void		free_range		P((CSA_attribute**,
						   CSA_enum**, int));

/*
 * Other utilty functions
 */
extern int		blank_buf		P((char*));
extern int		embedded_blank		P((char*));
extern char		*ckalloc		P(());
extern char		*cm_def_printer		P(());
extern char		*cm_get_credentials	P(());
extern char		*cm_get_local_domain	P(());
extern char		*cm_get_local_host	P(());
extern char		*cm_get_uname		P(());
extern char		*cm_pqtarget		P((char*));
extern char		*cm_strcat		P((char*, char*));
extern char		*cm_strcpy		P((char*, char*));
extern char		*cm_strdup		P((char*));
extern int		cm_strlen		P((char*));
extern char		*cm_target2domain	P((char*));
extern char		*cm_target2host		P((char*));
extern char		*cm_target2location	P((char*));
extern char		*cm_target2name		P((char*));
extern char		*cr_to_str		P((char*));
extern void		destroy_lines		P((Lines*));
extern void		expand_esc_chars	P((char*));
extern char		*get_head		P((char*, char));
extern char		*get_tail		P((char*, char));
extern void		print_tick		P((time_t));
extern boolean_t	same_path		P((char*, char*));
extern boolean_t	same_user		P((char*, char*));
extern char		*str_to_cr		P((char*));
extern char		*strncat		P(());
extern char		*substr			P((char*, int, int n));
extern void		syserr			P((char*, int, int, int));
extern Lines		*text_to_lines		P((char*, int));
extern int		get_data_version	P((CSA_session_handle));
extern int		get_server_version	P((CSA_session_handle));
extern CSA_sint32	privacy_set		P((Dtcm_appointment *));
extern CSA_sint32	showtime_set		P((Dtcm_appointment *));
extern int		max			P((int, int));
extern int		min			P((int, int));
extern int              parse_date              P((OrderingType, SeparatorType, char*, char*, char*, char*));

extern int              datestr2mdy             P((char*, OrderingType, SeparatorType, char*));
extern void             format_tick             P((time_t, OrderingType, SeparatorType, char*));
extern void		format_time		P((Tick, DisplayType, char*));

/*
 * In Motif you can't associate user data with items in a list.  To get around
 * this we have the following simple functions that maintain a list of
 * user data.  We use the intrinscs coding style to reinforce the
 * relationship these routines have to the XmList* functions.
 */
typedef struct _CmDataItem {
	struct _CmDataItem	*next;
	void			*data;
} CmDataItem;

typedef struct _CmDataList {
	CmDataItem	*head;
	CmDataItem	*tail;
	int		count;
} CmDataList;

extern CmDataList	*CmDataListCreate	P((void));
extern void		CmDataListDestroy	P((CmDataList *, int));
extern int		CmDataListAdd		P((CmDataList *, void *, int));
extern void		*CmDataListGetData	P((CmDataList *, int));
extern void		*CmDataListDeletePos	P((CmDataList *, int, int));
extern void		CmDataListDeleteAll	P((CmDataList *, int));


#endif
