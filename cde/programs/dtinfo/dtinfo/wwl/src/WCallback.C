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
 *	WWL		(c) Copyright LRI 1990
 *
 *	C++ Callbacks
 *
 *	$XConsortium: WCallback.cc /main/4 1996/06/11 17:03:21 cde-hal $
 *	$CurLog$        (Log at end)
 */

#ifndef _XtIntrinsic_h
#include <X11/Intrinsic.h>
#endif

#ifndef _WCallback_h_
#include <WWL/WCallback.h>
#endif

#ifndef _WTimeOut_h_
#include <WWL/WTimeOut.h>

#endif

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <WWL/WObject.h>
     
// ---------------- the callback

static void
cbWwl (Widget, caddr_t closure, caddr_t call_data)
{
	((WCallback*) closure) -> Call (call_data);
}

/*?class WWL
?*/

/*?nodoc?*/
WWL :: WWL ()
{
	cbList = 0;
}

/*?
Remove all callbacks set on this object.
?*/
void
WWL::removeCallbacks()
{
  WCallback* ncb;
  for (WCallback* pcb = cbList; pcb; pcb = ncb) {
    ncb = pcb->next;
    pcb->object = 0;		// avoid calling RemoveCb from ~WCallback
    delete pcb;
  }
  cbList = 0;
}

WWL :: ~WWL ()
{
  removeCallbacks();
}

/*?
  Add a member callback to this object.
  {\tt o->AddCb(cb)} is identical to {\tt cb->Obj(o)}.
  ?*/
void
WWL :: AddCb (WCallback* cb)
{
  if (cb->object)
    cb->object->RemoveCb (cb);
  cb->next = cbList;
  cb->object = this;
  cbList = cb;
}

/*?
  Remove a member callback
  from this object. Return True if the callback was found, else False.
  Note that the callback can be then added to this or another object.
  ?*/
Boolean
WWL :: RemoveCb (WCallback* cb)
{
  if (! cb)
    return True;
  if (cb == cbList) {
    cbList = cb->next;
    cb->next = 0;
    cb->object = 0;
    return True;
  }
  WCallback* ncb = 0;
  for (WCallback* pcb = cbList; pcb; pcb = ncb) {
    ncb = pcb->next;
    if (ncb == cb) {
      pcb->next = ncb->next;
      cb->next = 0;
      cb->object = 0;
      return True;
    }
  }
  return False;
}


/*?nodoc?*/
void WWL::__dummy () {}


/*?class WCallback
  ?*/


/*?
  Create a new member callback, for the callback list named \var{n} of
  the widget \var{w}. The member function to call is {\tt o->f}.
  If \var{o} is null, the member callback is created but not activated.
  It can be activated later with \fun{Obj(WWL*)} or \fun{WWL::AddCb}.
  ?*/
WCallback :: WCallback (Widget w, const char* n, WWL* o, WWL_FUN f, void* clientdata)
{
  next = 0;
  widget = w;
  wobject = NULL;
  name = n;
  object = 0;
  fun = f;
  client_data = (caddr_t) clientdata;
  XtAddCallback (widget, name, (XtCallbackProc)cbWwl, (caddr_t) this);
  if (o)
    o->AddCb (this);
}

WCallback :: WCallback (Widget w, Atom a, WWL* o, WWL_FUN f, void* clientdata)
{
  next = 0;
  widget = w;
  wobject = NULL;
  name = NULL;
  object = o;
  fun = f;
  client_data = (caddr_t) clientdata;
  XmAddWMProtocolCallback (widget, a, (XtCallbackProc)cbWwl, (caddr_t) this);
}

/*?
  Create a new member callback, for the callback list named \var{n} of
  the WObject \var{w}. The member function to call is {\tt o->f}.
  If \var{o} is null, the member callback is created but not activated.
  It can be activated later with \fun{Obj(WWL*)} or \fun{WWL::AddCb}.
  ?*/
WCallback :: WCallback (WObject &w, const char* n, WWL* o, WWL_FUN f, void* clientdata)
{
  next = 0;
  widget = w;
  wobject = &w;
  name = n;
  object = 0;
  fun = f;
  client_data = (caddr_t) clientdata;
  XtAddCallback (widget, name, (XtCallbackProc)cbWwl, (caddr_t) this);
  if (o)
    o->AddCb (this);
}

WCallback :: WCallback (WObject &w, Atom a, WWL* o, WWL_FUN f, void* clientdata)
{
  next = 0;
  widget = w;
  wobject = &w;
  name = NULL;
  object = o;
  fun = f;
  client_data = (caddr_t) clientdata;
  XmAddWMProtocolCallback (widget, a, (XtCallbackProc)cbWwl, (caddr_t) this);
}


/*?
  Remove the callback from the widget callback list,
  and from the associated object, if any.
  ?*/
WCallback :: ~WCallback ()
{
  if (name)
    XtRemoveCallback (widget, name, (XtCallbackProc)cbWwl,
		      (caddr_t) this);
  if (object)
    object->RemoveCb (this);
}

#ifdef DOC
/*?
  Get the member function of this callback.
  ?*/
WWL_FUN
WCallback :: Fun ()
{ return fun; }

/*?
  Set the member function of this callback.
  ?*/
void
WCallback :: Fun (WWL_FUN f)
{ fun = f; }

/*?
  Get the object of this callback.
  ?*/
WWL*
WCallback :: Obj ()
{
}

/*?
  Set the object of this callback.
  ?*/
void
WCallback :: Obj (WWL* o)
{
}

/*?
  Retreive the callback information.
  This is valid only from the body of the member callback.
  ?*/
caddr_t
WCallback :: CallData ()
{
}

/*?
  Retreive the widget the callback is associated with.
  ?*/
Widget
WCallback :: GetWidget ()
{
}
#endif


/*?nodoc?*/
void
WCallback :: Call (caddr_t calldata)
{
  if (object) {
    call_data = calldata;
    (object->*fun) (this);
  }
}


// callback for timeout procedure

static void
timeoutWwl (XtPointer client_data, XtIntervalId)
{
  // IntervalId already stored in WTimeOut object, so no need for it here. 
  ((WTimeOut *) client_data)->Call();
}

// constructor for WTimeOut object

WTimeOut::WTimeOut (XtAppContext app_context, unsigned long interval,
		    WWL *o, WTimeOutFunc f, XtPointer client)
: object (o), func (f), client_data (client)
{
  interval_id =
    XtAppAddTimeOut (app_context, interval, (XtTimerCallbackProc) timeoutWwl,
		     this);
}


// memeber of WTimeOut to carry out call

void
WTimeOut::Call ()
{
  if (object)
    (object->*func) (this);
  
  interval_id = 0;
  delete this;
}

// destructor for WTimeOut - just destroy object to remove a timeout
// the object is deleted automatically when the callback occurs.

WTimeOut::~WTimeOut ()
{
  if (interval_id != 0)
    {
      XtRemoveTimeOut (interval_id);
      interval_id = 0;
    }
}
