/*******************************************************************************
**
**  cmcb.x
**
**  static char sccsid[] = "@(#)cmcb.x 1.3 94/09/22 Copyr 1991 Sun Microsystems, Inc."; 
**
**  $XConsortium: cmcb.x /main/1 1996/04/21 19:22:02 drk $
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
 * Calendar manager service callback RPC protocol.
 */

#if defined(RPC_HDR) || defined(RPC_XDR)

%
%#include "csa.h"
%#include "cm.h"
%

%
%/*
% * cal_attr_data - contain names of calendar attributes updated
% *
% */
struct cmcb_cal_attr_data {
	u_int		num_names;
	cms_name	*names;
};

%
%/*
% * add_entry_data - contain information of the deleted entry
% */
struct cmcb_add_entry_data {
	string	id<>;
};

%
%/*
% * delete_entry_data - contain information of the deleted entry
% */
struct cmcb_delete_entry_data {
	string	id<>;
	int	scope;
	time_t	time;
};

%
%/*
% * update_entry_data - contain information of the updated entry
% * if new entry id is not resulted from the update, oldid will be
% * set to a NULL string ("").
% */
struct cmcb_update_entry_data {
	string	newid<>;
	string	oldid<>;
	int	scope;
	time_t	time;
};

%
%/*
% * update_data - contains the update reason and specific information
% * about the update.  No additional data is provided for CSA_CALENDAR_LOGON,
% * CSA_CALENDAR_DELETED, and CSA_CALENDAR_ATTRIBUTE_UPDATED
% */
%struct cmcb_update_data {
%	int	reason;
%	union {
%		cmcb_cal_attr_data	*cdata; /* data for cal attrs updated */
%		cmcb_add_entry_data	*adata; /* data for CSA_ENTRY_ADDED */
%		cmcb_delete_entry_data	*ddata; /* data for CSA_ENTRY_DELETED */
%		cmcb_update_entry_data	*udata; /* data for CSA_ENTRY_UPDATED */
%	} data;
%};
%typedef struct cmcb_update_data cmcb_update_data;
%

struct cmcb_update_callback_args {
	string			calendar<>;
	string			user<>;
	cmcb_update_data	data;
};

#endif /* RPC_HDR */

/*
 * The AGENTPROG actually isn't used for callback.
 * A transient number will be generated instead.  It's
 * just declared here as a "syntax" holder for rpcgen
 */
program AGENTPROG {
        version AGENTVERS_2 {
                void CMCB_UPDATE_CALLBACK(cmcb_update_callback_args) = 1;
        } = 2;
} = 0;

