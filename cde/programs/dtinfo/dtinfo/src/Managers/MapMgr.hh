/*
 * $XConsortium: MapMgr.hh /main/4 1996/09/17 13:17:34 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
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

#include "UAS.hh"

class MapAgent;


class MapMgr : public Long_Lived
{
public: // functions
  MapMgr();

  void display_centered_on (UAS_Pointer<UAS_Common> &);

  void set_auto_update(int value);

  void clear_map(UAS_Pointer<UAS_Common> &);

protected: // functions

protected: // variables
  MapAgent *f_map_agent;

private:
  LONG_LIVED_HH(MapMgr,map_mgr);
};

LONG_LIVED_HH2(MapMgr,map_mgr);
