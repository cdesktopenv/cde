/*
 * $XConsortium: AgentStack.hh /main/3 1996/06/11 16:17:26 cde-hal $
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
 *"
 */

class Agent;

#ifndef _AgentStack_hh
#define _AgentStack_hh

class AgentStackEntry
{
public: // functions
  AgentStackEntry (Agent *a, AgentStackEntry *n)
    : f_agent (a), f_next(n) { };
  Agent *agent()
    { return f_agent; }
  AgentStackEntry *next ()
    { return f_next; }
  
protected: // variables
  Agent           *f_agent;
  AgentStackEntry *f_next;
};

class AgentStack
{
public: // functions
  AgentStack ()
    : top(NULL) {};
  //  NOTE: Need a "real" destructor
  //  ~AgentStack ();
  void push (Agent *agent)
    { top = new AgentStackEntry (agent, top); }				 
  Agent *pop ()
    { Agent *agent = top->agent ();
      AgentStackEntry *entry = top;
      top = entry->next();
      delete entry;
      return (agent);
    }

protected: // variables
  AgentStackEntry *top;
};

#endif /* _AgentStack_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
