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
#ifndef _DT_MSG_P_H 
#define _DT_MSG_P_H
/*
 *  MsgP.h -- Private header file for DT messaging library 
 *
 *  $XConsortium: MsgP.h /main/3 1995/10/26 15:24:51 rswiston $
 *  $XConsortium: MsgP.h /main/3 1995/10/26 15:24:51 rswiston $
 *
 * (C) Copyright 1993, Hewlett-Packard, all rights reserved.
 */
#include <Dt/Msg.h>


/*
 * Definitions
 */

/* max property size (bytes) */
#define DT_MSG_MAX_PROP_SIZE	(32768)

/* atom names */
#define DT_MSG_XA_REQUEST		"_DT_REQUEST"
#define DT_MSG_XA_NOTIFY		"_DT_NOTIFY"
#define DT_MSG_XA_BROADCAST_REGISTRY	"_DT_BROADCAST_REGISTRY"

/*
 * _DtMessage handle
 */
typedef struct _DtMsgHandle {
    char *		pchName;	/* handle "name" */
    Atom		atom;		/* Selection/Broadcast atom  */
    Widget		widget;		/* Widget registering this handle */
    char *		pchPropName;	/* property name */
    Atom		property;	/* atomized property name */

    /* other data */
    struct _DtMsgServiceContext *
			service_data;	/* service data assoc w/ handle */
    struct _DtMsgBroadcastData *
			broadcast_data;	/* broadcast data assoc w/ handle */
} DtMsgHandle;

/*
 * _DtMessage handle accessor "functions" 
 */
#define DtMsgH_Name(h)		((h)->pchName)
#define DtMsgH_Widget(h)	((h)->widget)
#define DtMsgH_Atom(h)		((h)->atom)
#define DtMsgH_SvcData(h)	((h)->service_data)
#define DtMsgH_BcData(h)	((h)->broadcast_data)
#define DtMsgH_PropertyName(h)	((h)->pchPropName)
#define DtMsgH_PropertyAtom(h)	((h)->property)
#define DtMsgH_Shandle(h)	((h)->service_data->handle)
#define DtMsgH_RequestProc(h)	((h)->service_data->request_proc)
#define DtMsgH_ReceiveCD(h)	((h)->service_data->receive_client_data)
#define DtMsgH_LoseProc(h)	((h)->service_data->lose_proc)
#define DtMsgH_LoseCD(h)	((h)->service_data->lose_client_data)
#define DtMsgH_RegistryAtom(h)	((h)->broadcast_data->aRegistry)
#define DtMsgH_SharedWindow(h)	((h)->broadcast_data->winShared)
#define DtMsgH_SharedWidget(h)	((h)->broadcast_data->wShared)
#define DtMsgH_Listener(h)	((h)->broadcast_data->wListener)
#define DtMsgH_BreceiveProc(h)	((h)->broadcast_data->Breceive_proc)
#define DtMsgH_BclientData(h)	((h)->broadcast_data->Bclient_data)
#define DtMsgH_Bprops(h)	((h)->broadcast_data->props)
#define DtMsgH_BnumProps(h)	((h)->broadcast_data->numProps)
#define DtMsgH_BsizeProps(h)	((h)->broadcast_data->sizeProps)
#define DtMsgH_Breceivers(h)	((h)->broadcast_data->pReceivers)
#define DtMsgH_BnumReceivers(h)	((h)->broadcast_data->numReceivers)
#define DtMsgH_BsizeReceivers(h)	((h)->broadcast_data->sizeReceivers)
#define DtMsgH_BSenderInit(h) ((h)->broadcast_data->bSenderInitialized)

/*
 * Service context data 
 * (for client that offers a service)
 */
typedef struct _DtMsgServiceContext {
    DtMsgHandle	handle;
    DtMsgReceiveProc	request_proc;
    DtMsgStatusProc	lose_proc;
    Pointer		receive_client_data;
    Pointer		lose_client_data;
} DtMsgServiceContext;

/*
 * Request context data
 * (for client that makes a service request)
 */
typedef struct _DtMsgRequestContext {
    DtMsgHandle	handle;
    DtMsgReceiveProc	reply_proc;
    Pointer		client_data;
} DtMsgRequestContext;


/*
 * Reply message context data
 * (for client replying to a request)
 */
typedef struct _DtMsgReplyMessageContext {
    DtMsgHandle	handle;
    Window		window;
    Atom		target;
    Atom		property;
} DtMsgReplyMessageContext;


/*
 * Broadcast sender data
 */

/* number of props to allocate per memory request */
#define DT_MSG_PROP_INC_AMT	10

typedef struct _DtMsgBroadcastPerReceiverData {
    Widget		widget;			/* widget of receiver */
    Atom *		propsUnread;		/* list of props */
    int			numPropsUnread;		/* number of props */
    int			sizePropsUnread;	/* amt of space allocated */
} DtMsgBroadcastPerReceiverData;

typedef struct _DtMsgBroadcastData {
    Atom		aRegistry;	/* registry atom name */
    Window		winShared;	/* shared window */
    Widget		wShared;	/* shared widget */
    Widget		wListener;	/* child of shared window */
    DtMsgReceiveProc	Breceive_proc;	/* broadcast receive proc */
    Pointer		Bclient_data;	/* broadcast client data */

    Boolean		bSenderInitialized;	/* true if ready for send */
    Atom *		props;		/* props to use for messages */
    int			numProps;	/* number of message props */
    int			sizeProps;	/* number of props allocated */
    DtMsgBroadcastPerReceiverData *	pReceivers;	/* rcvr data */
    int			numReceivers;	/* number of receivers */
    int			sizeReceivers;	/* amt of rcvr space allocated */

} DtMsgBroadcastData;


/* 
 * Broadcast _DtMessage Registry definitions
 */

/* registry entry size, in words */
#define DT_MSG_REGISTRY_ENTRY_SIZE	2

/* offsets within each registry entry */
#define DT_MSG_REGISTRY_NAME_OFFSET	0
#define DT_MSG_REGISTRY_WINDOW_OFFSET	1

/* Macros to compute offset */
#define MSG_GROUP_NAME(R, i) \
    (R[((i)*DT_MSG_REGISTRY_ENTRY_SIZE)+DT_MSG_REGISTRY_NAME_OFFSET])

#define MSG_GROUP_WINDOW(R, i) \
    (R[((i)*DT_MSG_REGISTRY_ENTRY_SIZE)+DT_MSG_REGISTRY_WINDOW_OFFSET])


#endif /* not defined _DT_MSG_P_H */
/***** END OF FILE ****/
