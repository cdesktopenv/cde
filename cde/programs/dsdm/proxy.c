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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $TOG: proxy.c /main/6 1998/08/03 09:33:32 mgreess $ */
/*	Copyright (c) 1990, 1991, 1992, 1993 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1985, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF     	*/
/*	UNIX System Laboratories, Inc.                     	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* 
 * The Proxy Agent is used to translate Drag And Drop (dnd)
 * messages between Motif and OLIT clients.
 *
 * LIMITATIONS:
 * Any Motif or OLIT client running before the proxy was started will have to
 * be restarted to participate in drag and drop.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xproto.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#ifdef    DEBUG
#define DPRINTF(args) (void) printf args
#define a2s(atom)	(atom ? XGetAtomName(dpy, atom) : "0")
#else  /* DEBUG */
#define DPRINTF(a)
#endif /* DEBUG */

#define OLIT_DROP_SELECTION	0
#define OLIT_DROP_TIME		1
#define OLIT_DROP_COORDINATE	2
#define OLIT_DROP_SITE_ID	3
#define OLIT_DROP_OPERATION	4

#define GET_PROPERTY_MAX	1000000
#define MOTIF_DRAG_PROTOCOL_VERSION 0
#define DROP_TABLE_MAX	10
#define MOTIF_MESSAGE_TYPE_MASK	0x7f
#define MOTIF_RECEIVER_BIT	0x80
#define MOTIF_DROP_SITE_STATUS_MASK ((CARD16) 0xFF0F)
#define MOTIF_DROP_SITE_STATUS_VALID ((CARD16) 0x0030)

enum {
    MOTIF_TOP_LEVEL_ENTER,
    MOTIF_TOP_LEVEL_LEAVE,
    MOTIF_DRAG_MOTION,
    MOTIF_DROP_SITE_ENTER,
    MOTIF_DROP_SITE_LEAVE,
    MOTIF_DROP_START,
    MOTIF_DROP_FINISH,
    MOTIF_DRAG_DROP_FINISH,
    MOTIF_OPERATION_CHANGED
};

enum {
        XmDRAG_NONE,
        XmDRAG_DROP_ONLY,
        XmDRAG_PREFER_PREREGISTER,
        XmDRAG_PREREGISTER,
        XmDRAG_PREFER_DYNAMIC,
        XmDRAG_DYNAMIC,
        XmDRAG_PREFER_RECEIVER
};

typedef struct {
    Atom	source_handle;
    Atom	proxy_handle;
    Time 	time_stamp;		/* time of drop_start or trigger */
    Window	source_win;
    Window	proxy_sel_req_win;	/* needed for concurrent drops */
    Window	receiver_win;		/* was motif_receiver_win!!!
					 * This field was only for the olit
					 * source and was initialized in
					 * HandleOlitTrigger().
					 *
					 * Now this field is also served as
					 * the olit destination for the
					 * SUN_DRAGDROP_DONE situation and
					 * this field is initialized in
					 * ProxyMain:SelectionRequest... */
    CARD32	motif_action;		/* for olit source only */
    INT16	trigger_x;		/* for olit source only */
    INT16	trigger_y;		/* for olit source only */
    Atom	status;			/* for olit source only!
					 * ATOM_MOTIF_SUCESS or
					 * ATOM_MOTIF_FAILURE.
					 * This field will set in
					 * ProxyMain:SelectionRequest... */
} 	drop_info_t;
    
typedef struct {
    Atom	target;
    Atom	property;
}    target_prop_t;

typedef struct {
    BYTE        byte_order;
    BYTE        protocol_version;
    CARD16      num_target_lists B16;
    CARD32      size B32;
} motif_targets_t;

typedef struct {
    BYTE	 message_type;
    BYTE	 byte_order;
    CARD16	 flags;
    CARD32	 time;
    INT16	 x;
    INT16	 y;
    CARD32 	 icc_handle;
    CARD32	 src_window;
} motif_drop_start_t;

typedef struct {
    BYTE	byte_order;
    BYTE	protocol_version;
    CARD16	targets_index B16;
    CARD32	icc_handle B32;
} motif_initiator_t;

typedef struct {
    BYTE	byte_order;
    BYTE	protocol_version;
    BYTE	drag_protocol_style;
    BYTE	pad1;
    CARD32	proxy_window B32;
    CARD16	num_drop_sites B16;
    CARD16	pad2 B16;
    CARD32	heap_offset B32;
} motif_receiver_t;

typedef struct _box {
    short x1, x2, y1, y2;
} BOX;

typedef struct {
    long size;
    long numRects;
    BOX *rects;
    BOX extents;
} REGION;

typedef struct _site {
    int screen;
    unsigned long site_id;
    Window window_id;
    unsigned long flags;
    Region region;
    struct _site *next;
} drop_site_t;

extern char *ProgramName;
extern drop_site_t *MasterSiteList;
extern Window proxy_win;
extern Atom ATOM_DRAGDROP_INTEREST;
extern Atom ATOM_WM_STATE;

unsigned char *GetInterestProperty();

static ForwardConversion();

static drop_info_t drop_table[DROP_TABLE_MAX];
Atom ATOM_SUN_DND_TRIGGER;
Atom ATOM_SUN_DND_DONE;
Atom ATOM_SUN_SELECTION_END;
Atom ATOM_SUN_SELECTION_ERROR;
Atom ATOM_SUN_DND_ACK;
Atom ATOM_MOTIF_DRAG_WIN;
Atom ATOM_MOTIF_PROXY_WIN;
Atom ATOM_MOTIF_DND_MESS;
Atom ATOM_MOTIF_INITIATOR_INFO;
Atom ATOM_MOTIF_RECEIVER_INFO;
Atom ATOM_MOTIF_SUCCESS;
Atom ATOM_MOTIF_FAILURE;
Atom ATOM_MOTIF_TARGETS;
Atom ATOM_TARGETS;
Atom ATOM_MULTIPLE;
Window motif_drag_win;
char motif_byte_order;

void 
InitializeByteOrder()
{
    unsigned int		endian = 1;

    /* if LSByte is set, then hardware uses little endian addressing */
    if (*((char *)&endian))
	motif_byte_order = 'l';
    else
	motif_byte_order = 'B';
}

/* swap 2 bytes of data if the byte order is different */
Swap2Bytes(byte_order, data)
    char byte_order;
    CARD16 data;
{
    static CARD16 ret_data;
    char *old_p = (char *) &data;
    char *new_p = (char *) &ret_data;

    if (byte_order == motif_byte_order)
	return data;

    new_p[0] = old_p[1];
    new_p[1] = old_p[0];
    return ret_data;
}
	
/* swap 4 bytes of data if the byte order is different */
Swap4Bytes(byte_order, data)
    char byte_order;
    CARD32 data;
{
    static CARD32 ret_data;
    char *old_p = (char *) &data;
    char *new_p = (char *) &ret_data;

    if (byte_order == motif_byte_order)
	return data;

    new_p[0] = old_p[3];
    new_p[1] = old_p[2];
    new_p[2] = old_p[1];
    new_p[3] = old_p[0];
    return ret_data;
}
	

/* Get a new drop record */
/* use source_handle to indicate whether record is free or in use */
static drop_info_t *
NewDropInfo()
{
    int i, oldest_index=0;

    for(i=0; i<DROP_TABLE_MAX; i++)
	if (drop_table[i].source_handle == 0) {
	    return drop_table+i;
	}

    /* if there are no more free records then use the oldest record */
    for(i=1; i<DROP_TABLE_MAX; i++)
	if (drop_table[i].time_stamp < drop_table[oldest_index].time_stamp)
	    oldest_index = i;

    return drop_table+oldest_index;
}

/* Search for a drop record */
/* use source_handle as the key to the drop_info record */
static drop_info_t *
GetDropInfo(proxy_sel_req_win)
    Window proxy_sel_req_win;
{
    int i;

    for(i=0; i<DROP_TABLE_MAX; i++)
	if (drop_table[i].proxy_sel_req_win == proxy_sel_req_win) {
	    return drop_table+i;
	}
    return NULL;
}

/* Search for a drop record using the proxy_handle as the key */
static drop_info_t *
GetDropInfoUsingProxyHandle(proxy_handle)
    Atom proxy_handle;
{
    int i;

    for(i=0; i<DROP_TABLE_MAX; i++)
	if (drop_table[i].proxy_handle == proxy_handle
	    && drop_table[i].source_handle)
	    return drop_table+i;
    return NULL;
}

/* Delete a drop record */
static
ClearDropInfo(proxy_handle)
    Atom proxy_handle;
{
    int i;
    for(i=0; i<DROP_TABLE_MAX; i++)
	if (drop_table[i].proxy_handle == proxy_handle) {
	    drop_table[i].source_handle = 0; /* reset flag */
	    break;
	}
}
	    
/* get the window that has the atom associated with it */
Window 
GetAtomWindow(dpy, win, atom)
        Display *dpy;
        Window win;
        Atom atom;
{
    Window 		root, parent;
    Window 		*children;
    unsigned int 	nchildren;
    int		 	i;
    Atom 		type = None;
    int 		format;
    unsigned long 	nitems, after;
    unsigned char 	*data;
    Window 		inf = 0;

    XGetWindowProperty(dpy, win, atom, 0, 0, False, AnyPropertyType,
		       &type, &format, &nitems, &after, &data);
    if (type)
      return win;
    else {
	if (!XQueryTree(dpy, win, &root, &parent, &children, &nchildren) ||
	    (nchildren == 0))
	  return 0;
	for (i = nchildren - 1; i >= 0; i--) {
	    if (inf = GetAtomWindow(dpy, children[i], atom))
	      return inf;
	}
    }
    return 0;
}

/* get the window that is a property of the inputed window */
Window
GetPropertyWindow(dpy, in_win, atom)
    Display *dpy;
    Window in_win;
    Atom atom;
{
    Atom            type;
    int             format;
    unsigned long   lengthRtn;
    unsigned long   bytesafter;
    Window         *property = NULL;
    Window          win = None;

    if ((XGetWindowProperty (dpy,
			     in_win,
			     atom,
                             0L, 1,
                             False,
                             AnyPropertyType,
                             &type,
                             &format,
                             &lengthRtn,
                             &bytesafter,
                             (unsigned char **) &property) == Success) &&
         (type == XA_WINDOW) &&
         (format == 32) &&
         (lengthRtn == 1)) {
        win = *property;
    }
    if (property) {
        XFree ((char *)property);
    }

    return (win);
}

/* startup initialization */
ProxyInit(dpy, dsdm_win)
    Display *dpy;
    Window dsdm_win;
{
    enum { XA_MOTIF_DRAG_WINDOW, XA_MOTIF_DRAG_PROXY_WINDOW,
	   XA_MOTIF_DRAG_AND_DROP_MESSAGE,
	   XA_MOTIF_DRAG_INITIATOR_INFO, XA_MOTIF_DRAG_RECEIVER_INFO,
	   XAXmTRANSFER_SUCCESS, XAXmTRANSFER_FAILURE,
	   XA_MOTIF_DRAG_TARGETS, XA_SUN_DRAGDROP_TRIGGER,
	   XA_SUN_DRAGDROP_DONE, XA_SUN_SELECTION_END, XA_SUN_SELECTION_ERROR,
	   XA_SUN_DRAGDROP_ACK, XATARGETS, XAMULTIPLE,  NUM_ATOMS };
    static char* atom_names[] = {
           "_MOTIF_DRAG_WINDOW", "_MOTIF_DRAG_PROXY_WINDOW",
	   "_MOTIF_DRAG_AND_DROP_MESSAGE",
	   "_MOTIF_DRAG_INITIATOR_INFO", "_MOTIF_DRAG_RECEIVER_INFO",
	   "XmTRANSFER_SUCCESS", "XmTRANSFER_FAILURE",
	   "_MOTIF_DRAG_TARGETS", "_SUN_DRAGDROP_TRIGGER",
	   "_SUN_DRAGDROP_DONE", "_SUN_SELECTION_END", "_SUN_SELECTION_ERROR",
	   "_SUN_DRAGDROP_ACK", "TARGETS", "MULTIPLE" };

    int i;
    enum { ATOM_BUF_LEN = 25 };
    char buf[ATOM_BUF_LEN * DROP_TABLE_MAX];
    char *buf_names[DROP_TABLE_MAX];
    Atom buf_atoms[DROP_TABLE_MAX];
    XSetWindowAttributes attr;
    Atom atoms[NUM_ATOMS];

    InitializeByteOrder();

    /* make motif_drag_win and proxy_win persistant */
    XSetCloseDownMode(dpy, RetainPermanent);
    XInternAtoms(dpy, atom_names, NUM_ATOMS, False, atoms);

    ATOM_MOTIF_DRAG_WIN = atoms[XA_MOTIF_DRAG_WINDOW];
    ATOM_MOTIF_PROXY_WIN = atoms[XA_MOTIF_DRAG_PROXY_WINDOW];
    ATOM_MOTIF_DND_MESS = atoms[XA_MOTIF_DRAG_AND_DROP_MESSAGE];
    ATOM_MOTIF_INITIATOR_INFO = atoms[XA_MOTIF_DRAG_INITIATOR_INFO];
    ATOM_MOTIF_RECEIVER_INFO = atoms[XA_MOTIF_DRAG_RECEIVER_INFO];
    ATOM_MOTIF_SUCCESS = atoms[XAXmTRANSFER_SUCCESS];
    ATOM_MOTIF_FAILURE = atoms[XAXmTRANSFER_FAILURE];
    ATOM_MOTIF_TARGETS = atoms[XA_MOTIF_DRAG_TARGETS];

    ATOM_SUN_DND_TRIGGER = atoms[XA_SUN_DRAGDROP_TRIGGER];
    ATOM_SUN_DND_DONE = atoms[XA_SUN_DRAGDROP_DONE];
    ATOM_SUN_SELECTION_ERROR = atoms[XA_SUN_SELECTION_ERROR];
    ATOM_SUN_SELECTION_END = atoms[XA_SUN_SELECTION_END];
    ATOM_SUN_DND_ACK = atoms[XA_SUN_DRAGDROP_ACK];

    ATOM_TARGETS = atoms[XATARGETS];
    ATOM_MULTIPLE = atoms[XAMULTIPLE];

    for (i=0; i<DROP_TABLE_MAX; i++) {
        buf_names[i] = buf + (ATOM_BUF_LEN * i);
	sprintf(buf_names[i], "DND_PROXY_HANDLE_%d", i);
    }
    XInternAtoms(dpy, buf_names, DROP_TABLE_MAX, False, buf_atoms);

    for (i=0; i<DROP_TABLE_MAX; i++) {
	drop_table[i].proxy_handle = buf_atoms[i];
	drop_table[i].proxy_sel_req_win = 
	    XCreateWindow(dpy,
			  DefaultRootWindow(dpy), 0, 0, 1, 1, 0, 0,
			  InputOnly, CopyFromParent, 0, &attr);
    }
    motif_drag_win = GetPropertyWindow(dpy, DefaultRootWindow(dpy),
                             ATOM_MOTIF_DRAG_WIN);

    /* if motif_drag_win doesn't exist then define and declare it */
    if (motif_drag_win == None) {
	motif_drag_win = dsdm_win;
        XChangeProperty (dpy,
                     DefaultRootWindow(dpy),
                     ATOM_MOTIF_DRAG_WIN,
		     XA_WINDOW,
		     32,
		     PropModeReplace,
		     (unsigned char *) &motif_drag_win,
		     1);

    }

    /* define and declare the proxy_window */
    proxy_win = dsdm_win;
    XChangeProperty(dpy, motif_drag_win, 
		    ATOM_MOTIF_PROXY_WIN, /* property */
		    XA_WINDOW, /* type */
		    32,               /* format */
		    PropModeReplace, /* mode */
		    (unsigned char *) &proxy_win, /* data */
		    1
		    );

#ifdef DEBUG
    printf("drag_win=0x%lx, proxy_win=0x%lx\n", motif_drag_win, proxy_win);
#endif
    /*  Watch for mapping of top level windows */
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureNotifyMask);

    /*  Watch for someone taking away proxy_win ownership */
    XSelectInput(dpy, proxy_win, PropertyChangeMask);

    for(i=0; i<DROP_TABLE_MAX; i++)
	XSetSelectionOwner(dpy, drop_table[i].proxy_handle,
			   proxy_win, CurrentTime);
}

/* Place the motif receiver property on the Openlook receiver's top
   level window */
AdvertiseMotifDropSite(dpy, win)
    Display *dpy;
    Window win;
{
    motif_receiver_t receiver_info;
    unsigned long nitems;
    CARD32 *data;
    int	motif_receiver = False;

#define OL_PREVIEW_INDEX 4
#define MOTIF_RECEIVER_FLAG	0x80000000
    /* make sure receiver is not a Motif application */
    data = (CARD32 *) GetInterestProperty(dpy, win, &nitems);
    if (data != NULL) {
	if (nitems > OL_PREVIEW_INDEX &&
	    (data[OL_PREVIEW_INDEX] & MOTIF_RECEIVER_FLAG))
	    motif_receiver = True;
	XFree(data);
    }
    if (motif_receiver)
	return;

#ifdef DEBUG
    printf("AdvertiseMotifDropSite\n");
#endif

    receiver_info.byte_order = motif_byte_order;
    receiver_info.protocol_version = MOTIF_DRAG_PROTOCOL_VERSION;
    receiver_info.drag_protocol_style = XmDRAG_DROP_ONLY;
    receiver_info.proxy_window = proxy_win;
    receiver_info.num_drop_sites = 0;  /* not used in drag_drop_only mode */
    receiver_info.heap_offset = 0;     /* not used in drag_drop_only mode */

    XChangeProperty(dpy,
		    win,
		    ATOM_MOTIF_RECEIVER_INFO, /* property */
		    ATOM_MOTIF_RECEIVER_INFO, /* type */
		    8,               /* format */
		    PropModeReplace, /* mode */
		    (unsigned char *) &receiver_info, /* data */
		    sizeof(motif_receiver_t)  /* size of data in bytes */
		    );
}

/* Determine if the drop coordinate is within a rectangle */
static char
InRect(drop_x, drop_y, x, y, width, height)
    int drop_x, drop_y, x, y;
    unsigned int width, height;
{
    if ((drop_x >= x) && (drop_x <= (x+width)) &&
	(drop_y >= y) && (drop_y <= (y+height)))
	return True;
    else
	return False;
}

#define DRAGDROP_VERSION	0	/* OLIT dnd version */
#define INTEREST_RECT		0
#define INTEREST_WINDOW		1
#define NEXTWORD(result) if (index >= nitems)	{		\
                             XFree(data);                       \
                             return;                            \
			 }                                      \
			 (result) = data[index++]

/* get the drop site given the window and coordinate */
static void
GetOlitDropSite(dpy, top_level_win, drop_x, drop_y, ret_site_id, ret_event_win)
    Display *dpy;
	    Window top_level_win;
    INT16 drop_x, drop_y;
    unsigned long *ret_site_id;
    Window *ret_event_win;
{
    unsigned long *data, nitems, version, nsites, event_win, site_id, flags;
    unsigned long areatype, nrects, areawin;
    int	index = 0;
    int i,j, junk;
    char done = False;
    Window wjunk;
    int x,y;
    unsigned int width, height, border, ujunk;
    Window root_win;

    *ret_event_win = *ret_site_id = 0;
    data = (unsigned long *) GetInterestProperty(dpy, top_level_win, &nitems);
    if (data == NULL)	return;

    /* get root window */
    if (XGetGeometry(dpy, top_level_win, &root_win, &junk, &junk,
		     &ujunk, &ujunk, &ujunk, &ujunk) == 0)
	return;

    NEXTWORD(version);
    if (version != DRAGDROP_VERSION)	return;
    NEXTWORD(nsites);
    for (i=0; i<nsites && !done; ++i) {
	NEXTWORD(event_win);	NEXTWORD(site_id);	NEXTWORD(flags);
	NEXTWORD(areatype);
	switch (areatype) {
	case INTEREST_RECT:
	    NEXTWORD(nrects);
	    for (j=0; j<nrects && !done; ++j) {
		NEXTWORD(x);
		NEXTWORD(y);
		NEXTWORD(width);
		NEXTWORD(height);
		if (XTranslateCoordinates(dpy, top_level_win, root_win, x, y,
					     &x, &y, &wjunk))
		    done = InRect((int)drop_x,(int)drop_y, x, y, width,height);
	    }
	    break;
	case INTEREST_WINDOW:
	    NEXTWORD(nrects);
	    for (j=0; j<nrects && !done; ++j) {
		NEXTWORD(areawin);
		if (0 == XGetGeometry(dpy, areawin, &wjunk, &junk, &junk,
				      &width, &height, &border, &ujunk)) {
		    fprintf(stderr,
			    "%s: XGetGeometry failed on window 0x%lx\n",
			    ProgramName, areawin);
		    return;
		}
		if (XTranslateCoordinates(dpy, areawin, root_win, 0, 0,
					     &x, &y, &wjunk)) {
		    x = x - border;
		    y = y - border;
		    width = width + border;
		    height = height + border;
		    done = InRect((int)drop_x, (int)drop_y, x, y,width,height);
		}
	    }
	    break;
	default:
	    fprintf(stderr,
		    "%s: unknown site area type on window 0x%lx\n",
		    ProgramName, top_level_win);
	    return;
	}
    }
    if (done) {
	*ret_site_id = site_id;
	*ret_event_win = event_win;
    }
}

#define MOTIF_DROP_MOVE_ACTION 	(1 << 0)
#define MOTIF_DROP_COPY_ACTION	(1 << 1)
#define MOTIF_DROP_LINK_ACTION	(1 << 2)
#define MOTIF_DROP_ALLOWED_ACTIONS	  0x0F00
#define OLIT_DROP_ACKNOWLEDGE_ACTION	(1 << 1)
#define OLIT_DROP_TRANSIENT_ACTION	(1 << 2)
#define OLIT_DROP_MOVE_ACTION	(1 << 0)
#define OLIT_DROP_COPY_ACTION	0
#define OLIT_DROP_LINK_ACTION	(1 << 10)

/* translate an OLIT operation to a Motif operation */
static CARD16
ConvertOlitAction(olit_action)
    CARD32 olit_action;
{
    CARD16 motif_action;

    motif_action = MOTIF_DROP_COPY_ACTION | (MOTIF_DROP_COPY_ACTION << 8);

    if (olit_action & OLIT_DROP_MOVE_ACTION)
	motif_action |= MOTIF_DROP_MOVE_ACTION | (MOTIF_DROP_MOVE_ACTION << 8);
    else if (olit_action & OLIT_DROP_LINK_ACTION)
	motif_action |= MOTIF_DROP_LINK_ACTION | (MOTIF_DROP_LINK_ACTION << 8);

    return motif_action;
}

/* translate a Motif operation to an OLIT operation */
static CARD32
ConvertMotifAction(motif_action)
    CARD16 motif_action;
{
    CARD32 olit_action;

    olit_action = OLIT_DROP_TRANSIENT_ACTION;
    if (motif_action & MOTIF_DROP_MOVE_ACTION)
	olit_action |= OLIT_DROP_MOVE_ACTION;
    else if (motif_action & MOTIF_DROP_COPY_ACTION)
	olit_action |= OLIT_DROP_COPY_ACTION;
    else if (motif_action & MOTIF_DROP_LINK_ACTION)
	olit_action |= OLIT_DROP_LINK_ACTION;
    else
	olit_action = 0;
    return olit_action;
}

/* Process Motif ClientMessage */
HandleMotifMessage(dpy, event)
    Display *dpy;
    XClientMessageEvent *event;
{
    motif_drop_start_t info;
    drop_info_t *drop_info;
    Window dest_win;

    dest_win = event->window;
    info.message_type = event->data.b[0];
    info.byte_order = event->data.b[1];

    if (info.message_type & MOTIF_RECEIVER_BIT) {
	/* receiver message
	   If initiator is Motif then forward it this message. */
	if (event->window != proxy_win)
	    XSendEvent(dpy, event->window, False,
		       NoEventMask, (XEvent *) event);
    }
    else {
	/* initiator message */
	if ((info.message_type & MOTIF_MESSAGE_TYPE_MASK)== MOTIF_DROP_START) {
	    unsigned long site_id;
	    Window event_win;
	    XClientMessageEvent out_event;
	    CARD32 	olit_action;

	    /* receiver will never be MOTIF because MOTIF receivers
	       don't set the proxy field of RECEIVER_INFO atom
	       */

	    info.flags = Swap2Bytes(info.byte_order, event->data.s[1]);

            /* since dropSiteStatus can't be determined in a short amount
               of time, always return a valid drop site */
            info.flags &= MOTIF_DROP_SITE_STATUS_MASK;
            info.flags |= MOTIF_DROP_SITE_STATUS_VALID;

	    info.time = Swap4Bytes(info.byte_order, event->data.l[1]);
	    info.x = Swap2Bytes(info.byte_order, event->data.s[4]);
	    info.y = Swap2Bytes(info.byte_order, event->data.s[5]);
	    info.icc_handle = Swap4Bytes(info.byte_order, event->data.l[3]);
	    info.src_window = Swap4Bytes(info.byte_order, event->data.l[4]);

	    /* send a drop start ack here
	       to give ourselves more time to process the drop
	       because GetOlitDropSite or 
	       selection processing might take a long time
	       */
	    info.message_type = event->data.b[0] |= MOTIF_RECEIVER_BIT;
            event->data.s[1] = Swap2Bytes(info.byte_order, info.flags);
	    event->window = info.src_window;
	    XSendEvent(dpy, event->window, False,
		       NoEventMask, (XEvent *) event);

	    /* get olit drop site */
	    GetOlitDropSite(dpy, 
			    dest_win, info.x, info.y, &site_id, &event_win);
	    drop_info = NewDropInfo();
	    olit_action = ConvertMotifAction(info.flags);
	    if (event_win && olit_action) {
		drop_info->source_handle = info.icc_handle;
		drop_info->time_stamp = info.time;

		out_event.type = ClientMessage;
		out_event.send_event = True;
		out_event.display = dpy;
		out_event.window = event_win;
		out_event.message_type = ATOM_SUN_DND_TRIGGER;
		out_event.format = 32;

		out_event.data.l[OLIT_DROP_SELECTION]= drop_info->proxy_handle;
		out_event.data.l[OLIT_DROP_TIME] = info.time;
		out_event.data.l[OLIT_DROP_COORDINATE] = 
		    (info.x << 16) | info.y;
		out_event.data.l[OLIT_DROP_SITE_ID] = site_id;
		out_event.data.l[OLIT_DROP_OPERATION] = olit_action;
		XSendEvent(dpy, out_event.window, False,
		       NoEventMask, (XEvent *) &out_event);
	    }
	    else {
		/* tell motif initiator that the drop failed */
		XConvertSelection(dpy,
				  info.icc_handle,
				  ATOM_MOTIF_FAILURE,
				  None,
				  proxy_win,
				  info.time);
		
	    }
	}
    }
}

/* Process OLIT ClientMessage */
static
HandleOlitTrigger(dpy, event)
    Display *dpy;
    XClientMessageEvent *event;
{
    drop_site_t *site;
    char success = False;
    Atom source_handle = event->data.l[OLIT_DROP_SELECTION];
    Time time_stamp = event->data.l[OLIT_DROP_TIME];

    /* proxy agent only gets ATOM_SUN_DND_TRIGGER if
       receiver is motif */
    
    /* map site_id to site_window */
    for (site = MasterSiteList; site != NULL; site = site->next) {
	if (site->site_id == event->data.l[OLIT_DROP_SITE_ID]) {
	    drop_info_t *drop_info = NewDropInfo();
	    
	    drop_info->motif_action = 
		ConvertOlitAction(event->data.l[OLIT_DROP_OPERATION]);
	    drop_info->receiver_win = site->window_id;
	    drop_info->source_handle = source_handle;
	    drop_info->trigger_x = (event->data.l[OLIT_DROP_COORDINATE]) >> 16;
	    drop_info->trigger_y = 
		(event->data.l[OLIT_DROP_COORDINATE]) & 0x0000ffff;
	    drop_info->time_stamp = time_stamp;

	    if ((event->data.l[OLIT_DROP_OPERATION])
		& OLIT_DROP_ACKNOWLEDGE_ACTION) {
		XConvertSelection(dpy,
			      drop_info->source_handle, /* selection */
			      ATOM_SUN_DND_ACK,
			      ATOM_SUN_DND_ACK,	/* property */
			      drop_info->proxy_sel_req_win,
			      drop_info->time_stamp /* time */
			      );
	    }

	    /* get olit initiator targets */
	    XConvertSelection(dpy,
			      drop_info->source_handle, /* selection */
			      ATOM_TARGETS,
			      drop_info->proxy_handle,	/* property */
			      drop_info->proxy_sel_req_win,
			      drop_info->time_stamp /* time */
			      );
	    success = True;
	    break;
	}
    }      	    
    if (!success)
	/* tell olit initiator we are done */
	XConvertSelection(dpy, source_handle,
			  ATOM_SUN_DND_DONE,
			  ATOM_SUN_DND_DONE, /* property */
			  proxy_win,
			  time_stamp);
}


/* Place the Motif initiator property on the initiator's top level window */
static void
UpdateInitiatorAtom(dpy, win, channel, targets_index)
    Display *dpy;
    Window win;
    Atom channel;
    CARD16	targets_index;
{
    motif_initiator_t initiator_info;

    initiator_info.byte_order = motif_byte_order;
    initiator_info.protocol_version = MOTIF_DRAG_PROTOCOL_VERSION;
    initiator_info.targets_index = targets_index;
    initiator_info.icc_handle = channel;

    XChangeProperty(dpy, win,
		    channel, /* property */
		    ATOM_MOTIF_INITIATOR_INFO, /* type */
		    8,               /* format */
		    PropModeReplace, /* mode */
		    (unsigned char *) &initiator_info, /* data */
		    sizeof(motif_initiator_t) /* size of data in bytes */
		    );
}

/* Send Motif DROP_START ClientMessage to receiver */
SendStartDrop(dpy, src_win, drop_info)
    Display *dpy;
    Window src_win;
    drop_info_t *drop_info;
{
    motif_drop_start_t info;
    XClientMessageEvent	event;

    info.message_type = MOTIF_DROP_START;
    info.byte_order = motif_byte_order;
    info.flags = drop_info->motif_action;
    info.time = drop_info->time_stamp;
    info.x = drop_info->trigger_x;
    info.y = drop_info->trigger_y;
    info.icc_handle = drop_info->proxy_handle;
    info.src_window = src_win;

    event.type = ClientMessage;
    event.send_event = True;
    event.display = dpy;
    event.window = drop_info->receiver_win;
    event.message_type = ATOM_MOTIF_DND_MESS;
    event.format = 8;
    memcpy(&event.data, &info, sizeof(info));
    XSendEvent(dpy, event.window, False, NoEventMask, (XEvent *) &event);
}

/* Compare data of type Atom */
static int
AtomCompare(atom1, atom2)
    Atom *atom1, *atom2;
{
    return(*atom1 - *atom2);
}

/* Get the Motif TARGETS */
static void
GetTargetsList(dpy, targets_list_p)
    Display *dpy;
    motif_targets_t 	**targets_list_p;
{
    motif_targets_t 	*targets_list;
    Atom type;
    int format, status;
    unsigned long length, bytes_after;

    status = XGetWindowProperty(dpy,
                             motif_drag_win,
			     ATOM_MOTIF_TARGETS,
                             0L, GET_PROPERTY_MAX,
                             False,
                             ATOM_MOTIF_TARGETS,
                             &type,
                             &format,
                             &length,
                             &bytes_after,
                             (unsigned char **) &targets_list);
    if ((status != Success) || (type != ATOM_MOTIF_TARGETS) || (length == 0)) {
	/* allocate targets_list of zero targets */
	targets_list = (motif_targets_t *) malloc(sizeof(motif_targets_t));
	targets_list->byte_order = motif_byte_order;
	targets_list->protocol_version = MOTIF_DRAG_PROTOCOL_VERSION;
	targets_list->num_target_lists = 0;
	targets_list->size = sizeof(motif_targets_t);
    }
    *targets_list_p = targets_list;
}

static void
MatchTargets(targets_list, atoms, atom_cnt, targets_index_p)
    motif_targets_t	*targets_list;
    Atom		*atoms;
    unsigned long	atom_cnt;
    CARD16		*targets_index_p;
{
    int target_index;
    CARD32	*data;
    int num_sub_list_items, sub_list_index;
    CARD16	*num_sub_p;
    char byte_order = targets_list->byte_order;
    int num_target_lists;
    CARD32 target, tmp_data;

    num_target_lists = Swap2Bytes(byte_order, targets_list->num_target_lists);
    data = (CARD32 *) (targets_list+1);
    for (target_index=0; target_index < num_target_lists; target_index++) {
	num_sub_p = (CARD16 *) data;
	num_sub_list_items = (int) Swap2Bytes(byte_order, *num_sub_p);
	data = (CARD32 *) (num_sub_p+1);

	if (num_sub_list_items == atom_cnt) {
	    for (sub_list_index=0; sub_list_index < num_sub_list_items;
		 sub_list_index++) {
	       /* address isn't long word aligned so copy one byte at a time */
		memcpy(&tmp_data, data+sub_list_index, 4);
		target = Swap4Bytes(byte_order, tmp_data);
		if (target != atoms[sub_list_index])
		    break;
	    }
	    /* if all the atoms match then we got a match */
	    if (sub_list_index >= num_sub_list_items) {
		break;
	    }
	}
	data += num_sub_list_items;
    }

    *targets_index_p = target_index;
}

/* Put OLIT targets on the Motif TARGETS targets list */
static void
GetTargetsIndex(dpy, in_data, atom_cnt, targets_index_p)
    Display		*dpy;
    unsigned char *in_data;
    unsigned long atom_cnt;
    CARD16		*targets_index_p;  /* RETURN */
{
    Atom 		*atoms = (Atom *) in_data;
    motif_targets_t 	*new_list, *targets_list;
    int 		i, old_size, new_size;
    CARD16 		*target_cnt;
    CARD32		*target_data, tmp_data;
    int			malloc_length;
    motif_initiator_t	initiator_info;
    char 		byte_order;
    CARD16		num_target_lists;

#ifdef DEBUG
    for (i=0; i<atom_cnt; i++) {
	printf("GetTargetsIndex: target[%d]=%s\n", i, a2s(atoms[i]));
    }
#endif

    /* search existing targets list for a match, otherwise add our
       targets to the list */
    qsort((void *)atoms, (size_t)atom_cnt, (size_t)sizeof(Atom), AtomCompare);
    GetTargetsList(dpy, &targets_list);
    MatchTargets(targets_list, atoms, atom_cnt, targets_index_p);
    byte_order = targets_list->byte_order;
    num_target_lists = Swap2Bytes(byte_order,targets_list->num_target_lists);
    if (*targets_index_p < num_target_lists)
	return;

    /* no match, add our targets to the list */
    old_size = Swap4Bytes(byte_order, targets_list->size);
    /* new size = old_list_size + target_cnt(CARD16) + targets(CARD32) */
    new_size = old_size + sizeof(CARD16) + (sizeof(CARD32) * atom_cnt);
    new_list = (motif_targets_t *) 
	realloc(targets_list, new_size);

    /* change the new data's byte order to the existing data's byte order */
    /* put data in motif target list format */
    num_target_lists++;
    new_list->num_target_lists = Swap2Bytes(byte_order, num_target_lists);
    new_list->size = Swap4Bytes(byte_order, new_size);

    target_cnt = (CARD16 *) (((BYTE *) new_list) + old_size);
    *target_cnt = Swap2Bytes(byte_order, atom_cnt); /* target_cnt */
    target_data = (CARD32 *) (target_cnt + 1);
    for (i=0; i<atom_cnt; i++) {
	/* address isn't long word aligned so copy one byte at a time */
	tmp_data = Swap4Bytes(byte_order, atoms[i]);
	memcpy(target_data+i, &tmp_data, 4);
    }

    /* overwrite existing targets_list */
    XChangeProperty(dpy, motif_drag_win, ATOM_MOTIF_TARGETS,
		    ATOM_MOTIF_TARGETS, 8,
		    PropModeReplace,
		    (unsigned char *) new_list, new_size);
}

/* 
  * update target list property
  * advertise psuedo motif initiator info property
  * send DROP_START message to motif receiver
  */
static void
ContinueHandleOlitTrigger(dpy, data, length, drop_info)
    Display *dpy;
    unsigned char *data;
    unsigned long length;
    drop_info_t		*drop_info;
{
    CARD16		targets_index;
    Window psuedo_motif_initiator_win;

    GetTargetsIndex(dpy, data, length, &targets_index);
    psuedo_motif_initiator_win = proxy_win;
    UpdateInitiatorAtom(dpy, psuedo_motif_initiator_win, 
			drop_info->proxy_handle, targets_index);
    SendStartDrop(dpy, psuedo_motif_initiator_win, drop_info);
}

/* Get OLIT initiator's TARGETS */
ProcessOlitInitiatorConversion(event, drop_info)
    XSelectionEvent *event;
    drop_info_t		*drop_info;
{
    Atom type;
    int format;
    unsigned long length, bytes_after;
    unsigned char *data;


    /* XView needs this */
    if (event->target == ATOM_SUN_DND_ACK)
	XChangeProperty(event->display, event->requestor, event->property,
		  event->target, 32, PropModeReplace, NULL, 0);

    /* get OLIT targets
       sometimes type is TARGETS and sometimes type is ATOM so don't bother
       checking type */
    if ((event->target == ATOM_TARGETS) &&
	event->property && (XGetWindowProperty (event->display,
			     event->requestor,
			     event->property,
			     0L, GET_PROPERTY_MAX,
			     False,
			     AnyPropertyType,
			     &type,
			     &format,
			     &length,
			     &bytes_after,
			     (unsigned char **) &data) == Success)
	&& length) {

	ContinueHandleOlitTrigger(event->display, data, length, drop_info);
	XFree(data);
    }
}

/* copy a property and the associated data from one window to another.
   Returns True if successful else returns False */
static int
CopyProperty(dpy, property, old_win, new_win)
    Display *dpy;
    Atom property;
    Window old_win, new_win;
{
    Atom type;
    int format;
    unsigned long length, bytes_after;
    unsigned char *data;

    if (property && (XGetWindowProperty (dpy, old_win, property,
			     0L, GET_PROPERTY_MAX,
			     False, 
			     AnyPropertyType,
			     &type,
			     &format,
			     &length,
			     &bytes_after,
			     (unsigned char **) &data) == Success)
	&& format) {
	XChangeProperty(dpy, new_win, property,
			type, format, PropModeReplace, data, (int) length);
	XFree(data);
	return True;
    }
    else
	return False;
}
    
/* copy the targets property and the associated data 
   from one window to another.
   Returns True if we get ATOM_SUN_DND_DONE or ATOM_SUN_SELECTION_END */
static int
CopyTargets(dpy, prop, old_win, new_win)
    Display *dpy;
    Atom prop;
    Window old_win, new_win;
{
    Atom type;
    int format;
    unsigned long length, bytes_after;
    unsigned char *data;
    target_prop_t *target_info;
    int	i;
    int ol_done = False;

    if ((XGetWindowProperty (dpy, old_win, prop,
			     0L, GET_PROPERTY_MAX,
			     False, 
			     AnyPropertyType,
			     &type,
			     &format,
			     &length,
			     &bytes_after,
			     &data) == Success)
	&& format) {
	target_info = (target_prop_t *) data;
	for(i=0; i < (length / 2); i++, target_info++) {
	    DPRINTF(("CopyTargets: targets[%d]=%s\n", i, a2s(target_info->target)));
	    CopyProperty(dpy, target_info->property, old_win, new_win);
	    if (target_info->target == ATOM_SUN_DND_DONE ||
		(target_info->target == ATOM_SUN_SELECTION_END))
		ol_done = True;
	}
	XFree(data);
    }
    return ol_done;
}
    
/* Forward the SelectionNotify to the receiver.
   Returns True if we get ATOM_SUN_DND_DONE or ATOM_SUN_SELECTION_END */
static
ForwardConversion(event, drop_info)
    XSelectionEvent *event;
    drop_info_t		*drop_info;
{
    int ol_done = False;

    if (CopyProperty(event->display, event->property,
		     event->requestor, drop_info->source_win)) {

	if (event->target == ATOM_MULTIPLE) {
	    /* get target name and property pair
	       copy all these properties */
	    ol_done = CopyTargets(event->display, event->property,
				  event->requestor, drop_info->source_win);
	}
    }
    else {
	/* initiator couldn't convert property, tell receiver this */
	event->property = 0;
    }

    event->requestor = drop_info->source_win;
    event->selection = drop_info->proxy_handle;
    XSendEvent(event->display, event->requestor, False, 0, (XEvent*)event);
    return ol_done;
}

/* This routine handles the selectionrequest for the target MULTIPLE.
   XtGetSelectionValues generates a selection request with the target
   MULTIPLE. This is how multiple targets selection
   (a selection request that asks for multiple targets) work:
   Window A calls XtGetSelectionValues
   Xt places a property on window A which contains all the targets.
   Xt asks selection owner of selection 1 to convert.
   Window B which is the selection owner of selection 1
       gets a selectionrequest.
   Window B reads the property on window A to get the targets
       and then converts them.
   Window B places the converted targets data in properties on window A.
   Window B places the target names and the properties holding
      the target data (target/property pair) in a property on window A.

   With single target selection (XtGetSelectionValue), window B would
   get the target from the selectionrequest event structure and convert it.

   The following is how the proxy agent behaves when there is a multiple
   targets selection:
   Window A calls XtGetSelectionValues
   Xt places a property on window A which contains all the targets.
   Xt asks selection owner of selection 1 to convert.
   Proxy agent which is the selection owner of selection 1
      gets a selectionrequest.
   Proxy agent reads the property on window A to get the targets.
   Proxy agent places a property on itself which contains all the targets.
   Proxy agent asks selection owner of selection 2 to convert.
   Window B which is the selection owner of selection 2 
      gets a selectionrequest.
   Window B reads the property on proxy agent to get the targets
   and then converts them.
   Window B places the converted targets data in properties on proxy agent.
   Window B places the target names and the properties holding
   the target data in a property proxy agent.
   Proxy agent reads the targets data and places the targets data
      in a property on window A.
   Proxy agent reads the target names and the properties holding
      the target data and places this info in a property on window A.
*/
ForwardMultpleSelectionRequest(event, drop_info)
    XSelectionRequestEvent *event;
    drop_info_t *drop_info;
{
    int format, status;
    unsigned long length, bytes_after;
    unsigned char *data;

    drop_info->source_win = event->requestor;

    if (CopyProperty(event->display, event->property, 
		     event->requestor, drop_info->proxy_sel_req_win)) {

	/* change requestor and selection */
	event->requestor = drop_info->proxy_sel_req_win;
	event->selection = drop_info->source_handle;

	/* forward selection request to initiator */
	XConvertSelection(event->display,
			  event->selection,
			  event->target,
			  event->property,
			  event->requestor,
			  event->time
			  );
    }
}


/* tell the initiator to clean up and do internal cleanup */
DndDone(dpy, status, drop_info)
    Display *dpy;
    Atom status;
    drop_info_t *drop_info;
{
    XConvertSelection(dpy,
		      drop_info->source_handle,
		      status,
		      drop_info->proxy_handle,
		      drop_info->proxy_sel_req_win,
		      drop_info->time_stamp);

    /* XView needs this */
    if (status == ATOM_SUN_DND_DONE)
	XConvertSelection(dpy,
		      drop_info->source_handle,
		      ATOM_SUN_SELECTION_END,
		      drop_info->proxy_handle,
		      drop_info->proxy_sel_req_win,
		      drop_info->time_stamp);
}


ProxyMain(dpy, event)
    Display *dpy;
    XEvent *event;
{
    switch(event->type) {
    case ClientMessage:
#ifdef DEBUG
	printf("ClientMessage\n");
#endif
	if (event->xclient.message_type == ATOM_SUN_DND_TRIGGER)
	    HandleOlitTrigger(dpy, &event->xclient);
	else if (event->xclient.message_type == ATOM_MOTIF_DND_MESS)
	    HandleMotifMessage(dpy, &event->xclient);
	break;

    case MapNotify: {
	Window client_win;

	client_win = GetAtomWindow(dpy, event->xcreatewindow.window,
				     ATOM_WM_STATE);
	if (client_win) {
	    /* if client has olit dnd interest then attach motif dnd
	       interest to client */
	    if (GetAtomWindow(dpy, client_win, ATOM_DRAGDROP_INTEREST)) {
		AdvertiseMotifDropSite(dpy, client_win);
	    }
	    /* look for PropertyChange because olit receivers
	       don't always declare ATOM_DRAGDROP_INTEREST at MapNotify time
	       */
	    XSelectInput(dpy, client_win, PropertyChangeMask);
	}
	break;
    }

    case PropertyNotify:
/*	printf("PropertyNotify, win=0x%lx\n", event->xproperty.window);*/
	/* if a OLIT drop site is advertised
	   then advertise MOTIF drop site also */
	if (event->xproperty.atom == ATOM_DRAGDROP_INTEREST) {
	    AdvertiseMotifDropSite(dpy, event->xproperty.window);
	}
	else if (event->xproperty.atom == ATOM_MOTIF_PROXY_WIN) {
	    if ((motif_drag_win == event->xproperty.window) &&
		(proxy_win != GetPropertyWindow(dpy, motif_drag_win,
                             ATOM_MOTIF_PROXY_WIN)))
		/* proxy_win has been stolen by another client,
		   inform user */

		/* maybe the following should be a X message */
		fprintf(stderr, "%s: lost proxy control\n", ProgramName);
	}

	/* In the future, we might want to handle the case where
	 * a client removes the ATOM_DRAGDROP_INTEREST or the
	 * ATOM_MOTIF_RECEIVER_INFO atom
	  */
	break;

    case SelectionRequest: {
	drop_info_t *drop_info;
#ifdef DEBUG
	printf("SelectionRequest, selection=%s, target=%s, prop=%s, win=0x%lx, time=%ld\n",
		   a2s(event->xselectionrequest.selection),
		   a2s(event->xselectionrequest.target),
		   a2s(event->xselectionrequest.property),
		   event->xselectionrequest.requestor,
		   event->xselectionrequest.time
	       );
#endif
	drop_info = GetDropInfoUsingProxyHandle
	    (event->xselectionrequest.selection);
	if (drop_info == NULL)
	    /* unknown selection, ignore the SelectionRequest */
	    break;
	if (event->xselectionrequest.target == ATOM_SUN_DND_DONE ||
	    (event->xselectionrequest.target == ATOM_SUN_SELECTION_END)) {
		/* tell motif initiator we are done OLIT does not have a
		 * Failure; so all dnd exit status map to SUCCESS.
		 *
		 * Also record `requestor' id, so that we know who to
		 * forward when receiving SelectionNotify. */
	    drop_info->receiver_win = event->xselectionrequest.requestor;
	    DndDone(dpy, ATOM_MOTIF_SUCCESS, drop_info);
	}	
	else if (event->xselectionrequest.target == ATOM_MOTIF_SUCCESS
		 || (event->xselectionrequest.target == ATOM_MOTIF_FAILURE)) {
		/* tell olit initiator we are done.
		 *
		 * We will have to remember this target value so that
		 * we know what to forward when receiving SelectionNotify.
		 *
		 * Note that olit protocol doesn't have a XmTRANSFER_FAILURE
		 * equivalence, the closest is _SUN_DRAGDROP_ERROR. We
		 * may want to consider it otherwise olit app will
		 * think the drop was successful...
		 */
	    drop_info->status = event->xselectionrequest.target;
	    DndDone(dpy, ATOM_SUN_DND_DONE, drop_info);
	}	
	else if (event->xselectionrequest.target == ATOM_SUN_SELECTION_ERROR)
	{
		/* tell motif initiator OLIT can not handle this drop.
		 * we have a failure.
		 */
	    DndDone(dpy, ATOM_MOTIF_FAILURE, drop_info);
	}
	else if (event->xselectionrequest.target == ATOM_MULTIPLE)
	    ForwardMultpleSelectionRequest(
				  &(event->xselectionrequest), drop_info);
	else {
	    /* forward selection request to initiator */
	    drop_info->source_win = event->xselectionrequest.requestor;
	    XConvertSelection(event->xselectionrequest.display,
				  drop_info->source_handle,
				  event->xselectionrequest.target,
				  event->xselectionrequest.property,
				  drop_info->proxy_sel_req_win,
				  event->xselectionrequest.time
				  );
	}
	break;
    } /* end of case SelectionRequest */

    case SelectionNotify:
#ifdef DEBUG
	printf("SelectionNotify, sel=%s, target=%s, prop=%s, w=0x%lx, time=%ld\n",
	       a2s(event->xselection.selection),
	       a2s(event->xselection.target),
	       a2s(event->xselection.property), event->xselection.requestor,
	       event->xselection.time);
#endif
    {
	drop_info_t *drop_info = GetDropInfo(event->xselection.requestor);

	if (drop_info == NULL || 
	    (event->xselection.time < drop_info->time_stamp))
	    /* got SelectionNotify of unknown selection (source) */
	    break;

		/* Trap these two targets, so that the destination side
		 * will not see selection-time-out */
	if (event->xselection.target == ATOM_SUN_DND_DONE ||  /* olit->motif */
	    event->xselection.target == ATOM_MOTIF_SUCCESS) { /* motif->olit */

		XSelectionEvent * xe = &event->xselection;

			/* Currently, in both olit dnd protocol and
			 * motif dnd protocol, the destination side
			 * DONT check the value from the source side!
			 *
			 * We will have to copy xe->property to
			 * drop_info->receiver_win if this is no longer
			 * TRUE in the future... */
		xe->requestor = drop_info->receiver_win;
		xe->selection = drop_info->proxy_handle;
		xe->target = xe->target == ATOM_SUN_DND_DONE ?
				drop_info->status : ATOM_SUN_DND_DONE;
		XSendEvent(xe->display, xe->requestor, False,
						NoEventMask, event);

		ClearDropInfo(drop_info->proxy_handle);
		break;
	}
	/* if event->property is proxy_handle then the convert request
	   was from the proxy agent
	   else the convert request was from the receiver */
	if (event->xselection.property == drop_info->proxy_handle ||
	    (event->xselection.property == ATOM_SUN_DND_ACK)) {
	    ProcessOlitInitiatorConversion(&event->xselection, drop_info);
	} else {
	    if (ForwardConversion(event, drop_info)) {

			/* We are here ONLY if the target is ATOM_MULTIPLE and
			 * the target list contains either _SUN_DRAGDROP_DONE or
			 * _SUN_SELECTION_END. Probably for XView support?! */
		DndDone(dpy, ATOM_MOTIF_SUCCESS, drop_info);
	    }
	}
    	break;
    }	/* end of case SelectionNotify */
    }	/* end of switch */
}
