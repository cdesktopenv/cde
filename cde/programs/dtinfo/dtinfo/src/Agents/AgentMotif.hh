/*
 * $XConsortium: AgentMotif.hh /main/4 1996/06/11 16:10:58 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include "WWL/wwl.h"
class WXmApplicationShell;
class Graphic;

#include "UAS.hh"

class Agent : public WWL, public FolioObject
{
public: // functions
  ~Agent() ;
  WApplicationShell &toplevel()      
     { return (window_system().toplevel()); } ;
  Display *xdisplay()   
     { return (window_system().display()); } ;

  // for refreshing of graphics...overloaded by NodeWindowAgent,
  // ViewportAgent, and GraphicAgent
  virtual void refresh(const UAS_Pointer<Graphic> &);
};
