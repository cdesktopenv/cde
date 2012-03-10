/*
 * $XConsortium: message_test.cc /main/3 1996/06/11 16:35:56 cde-hal $
 *
 * Copyright (c) 1993 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#define C_Receiver
#define C_Sender
#define L_Support

#define EXPAND_TEMPLATES

#include "Prelude.h"
#include <iostream.h>

struct Message
{
public:
  Message (char *m)
    : message(m) { }
  char *message;
};

struct MSender : public Sender<Message>
{
  void send (const Message &m)
    { send_message (m); }
};

struct MReceiver : public Receiver<Message>
{
  void receive (Message &m)
    { cout << m.message << endl; }
};

void
main()
{
  INIT_EXCEPTIONS();

  MSender sender;
  MReceiver r1, r2;

  sender.send (Message ("none"));
  sender.request (&r1);
  sender.send (Message ("r1 only"));
  sender.request (&r2);
  sender.send (Message ("r1 and r2"));
  sender.unrequest (&r1);
  sender.send (Message ("r2 only"));
  sender.unrequest (&r2);
  sender.send (Message ("none"));
};
