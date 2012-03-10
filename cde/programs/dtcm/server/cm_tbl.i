/*******************************************************************************
**
**  cm_tbl.i
**
**  static char sccsid[] = "@(#)cm_tbl.i 1.1 94/09/05 Copyr 1991 Sun Microsystems, Inc.";
**
**  $XConsortium: cm_tbl.i /main/3 1995/11/03 11:08:33 rswiston $
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

/*
 * Program table for calendar manager rpc protocol version 5
 */

struct rpcgen_table tableprog_5_table[] = {

	(char *(*)())RPCGEN_ACTION(cms_ping_5_svc),
	(xdrproc_t)xdr_void,			0,
	(xdrproc_t)xdr_void,			0,

	(char *(*)())RPCGEN_ACTION(cms_list_calendars_5_svc),
	(xdrproc_t)xdr_void,			0,
	(xdrproc_t)xdr_cms_list_calendars_res,	sizeof ( cms_list_calendars_res ),

	(char *(*)())RPCGEN_ACTION(cms_open_calendar_5_svc),
	(xdrproc_t)xdr_cms_open_args,		sizeof ( cms_open_args ),
	(xdrproc_t)xdr_cms_open_res,		sizeof ( cms_open_res ),

	(char *(*)())RPCGEN_ACTION(cms_create_calendar_5_svc),
	(xdrproc_t)xdr_cms_create_args,		sizeof ( cms_create_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),

	(char *(*)())RPCGEN_ACTION(cms_remove_calendar_5_svc),
	(xdrproc_t)xdr_cms_remove_args,		sizeof ( cms_remove_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),

	(char *(*)())RPCGEN_ACTION(cms_register_5_svc),
	(xdrproc_t)xdr_cms_register_args,		sizeof ( cms_register_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),

	(char *(*)())RPCGEN_ACTION(cms_unregister_5_svc),
	(xdrproc_t)xdr_cms_register_args,		sizeof ( cms_register_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),

	(char *(*)())RPCGEN_ACTION(cms_enumerate_calendar_attr_5_svc),
	(xdrproc_t)xdr_cms_name,			sizeof ( cms_name ),
	(xdrproc_t)xdr_cms_enumerate_calendar_attr_res,sizeof ( cms_enumerate_calendar_attr_res ),

	(char *(*)())RPCGEN_ACTION(cms_get_calendar_attr_5_svc),
	(xdrproc_t)xdr_cms_get_cal_attr_args,	sizeof ( cms_get_cal_attr_args ),
	(xdrproc_t)xdr_cms_get_cal_attr_res,	sizeof ( cms_get_cal_attr_res ),

	(char *(*)())RPCGEN_ACTION(cms_set_calendar_attr_5_svc),
	(xdrproc_t)xdr_cms_set_cal_attr_args,	sizeof ( cms_set_cal_attr_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),

	(char *(*)())RPCGEN_ACTION(cms_archive_5_svc),
	(xdrproc_t)xdr_cms_archive_args,		sizeof ( cms_archive_args ),
	(xdrproc_t)xdr_cms_archive_res,		sizeof ( cms_archive_res ),

	(char *(*)())RPCGEN_ACTION(cms_restore_5_svc),
	(xdrproc_t)xdr_cms_restore_args,		sizeof ( cms_restore_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),

	(char *(*)())RPCGEN_ACTION(cms_lookup_reminder_5_svc),
	(xdrproc_t)xdr_cms_reminder_args,		sizeof ( cms_reminder_args ),
	(xdrproc_t)xdr_cms_reminder_res,		sizeof ( cms_reminder_res ),

	(char *(*)())RPCGEN_ACTION(cms_lookup_entries_5_svc),
	(xdrproc_t)xdr_cms_lookup_entries_args,	sizeof ( cms_lookup_entries_args ),
	(xdrproc_t)xdr_cms_entries_res,		sizeof ( cms_entries_res ),

	(char *(*)())RPCGEN_ACTION(cms_enumerate_sequence_5_svc),
	(xdrproc_t)xdr_cms_enumerate_args,		sizeof ( cms_enumerate_args ),
	(xdrproc_t)xdr_cms_entries_res,		sizeof ( cms_entries_res ),

	(char *(*)())RPCGEN_ACTION(cms_get_entry_attr_5_svc),
	(xdrproc_t)xdr_cms_get_entry_attr_args,	sizeof ( cms_get_entry_attr_args ),
	(xdrproc_t)xdr_cms_get_entry_attr_res,	sizeof ( cms_get_entry_attr_res ),

	(char *(*)())RPCGEN_ACTION(cms_insert_entry_5_svc),
	(xdrproc_t)xdr_cms_insert_args,		sizeof ( cms_insert_args ),
	(xdrproc_t)xdr_cms_entry_res,		sizeof ( cms_entry_res ),

	(char *(*)())RPCGEN_ACTION(cms_update_entry_5_svc),
	(xdrproc_t)xdr_cms_update_args,		sizeof ( cms_update_args ),
	(xdrproc_t)xdr_cms_entry_res,		sizeof ( cms_entry_res ),

	(char *(*)())RPCGEN_ACTION(cms_delete_entry_5_svc),
	(xdrproc_t)xdr_cms_delete_args,		sizeof ( cms_delete_args ),
	(xdrproc_t)xdr_u_int,		sizeof ( u_int ),
};
int tableprog_5_nproc =
	sizeof(tableprog_5_table)/sizeof(tableprog_5_table[0]);

