/*
 * $XConsortium: MessageMgr.hh /main/7 1996/11/23 14:04:55 cde-hal $
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

# include "UAS.hh"
#include <X11/Intrinsic.h>

class MessageAgent;
class PrefAgent;
class demoException ;

struct CancelOperation {
    int fDummy;
};

class MessageMgr : public Long_Lived, public UAS_Receiver<CancelOperation>
{
public: // functions
  // Initializing
  MessageMgr();
  ~MessageMgr();

  // Message display
  // NOTE: need way to get answer back in some cases!!
  
  Widget create_dialog(unsigned char dialog_type,
                       char *message_text, Widget parent = NULL);
  void quit_dialog    (char *message_text, Widget parent = NULL);
  void error_dialog   (char *message_text, Widget parent = NULL);
  void warning_dialog (char *message_text, Widget parent = NULL);
  void info_dialog    (char *message_text, Widget parent = NULL);
  void progress_dialog (char *message_text, Widget parent = NULL);
  void progress_dialog_done ();
  bool question_dialog (char *message_text, Widget parent = NULL);
  void assert_failed (char *statement, char *file,
                      int line, Widget parent = NULL);
  int get_integer (const char* msg, const char* title,
                   int default_value, Widget parent = NULL);
  const char *get_string (const char* msg, const char* title,
                          const char *default_value, Widget parent = NULL);
  void receive (CancelOperation &, void *client_data);

  STATIC_SENDER_HH(UAS_CancelRetrievalMsg);
  
  void demo_failure(demoException &, Widget parent = NULL);

  void set_max_length(int, Widget parent = NULL);

  void ok_cb();
  void cancel_cb();

  // Accessing

private: // functions

  void show_it(Widget dialog);
  void destroy_it(Widget dialog);
  
private: // variables
  MessageAgent		&f_booklist_message;
  MessageAgent		&f_nodeview_message;

  int                   f_booklist_process;
  int                   f_nodeview_process;

  bool			f_popped_up;
  bool			f_pressed_ok;
  bool			f_exit_flag;
  bool			f_dialog_active;

private:
  LONG_LIVED_HH(MessageMgr,message_mgr);
};

LONG_LIVED_HH2(MessageMgr,message_mgr);



#ifdef __STDC__
#define Xassert(STMT) \
if (!(STMT)) { \
  message_mgr().assert_failed ((char*)#STMT, (char*)__FILE__, __LINE__); \
  return; \
};
#else
#define Xassert(STMT) \
if (!(STMT)) { \
  message_mgr().assert_failed ("STMT", __FILE__, __LINE__); \
  return; \
};
#endif
