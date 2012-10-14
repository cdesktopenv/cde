/*
 * $XConsortium: Long_Lived.hh /main/3 1996/06/11 16:20:20 cde-hal $
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


class Long_Lived : public FolioObject
{
protected:
  Long_Lived();
  virtual ~Long_Lived();

private:
  static List g_long_lived_list;
};


// Macros to easily create access functions.

#define LONG_LIVED_HH(CLASS,FUNC)   friend CLASS &FUNC(); \
                                    static CLASS * CONCAT(g_,FUNC)

#define LONG_LIVED_HH2(CLASS,FUNC)  CLASS &FUNC();

#define LONG_LIVED_CC(CLASS,FUNC)   CLASS * CLASS::CONCAT(g_,FUNC);         \
                                    CLASS &FUNC() {                         \
                                      if (CLASS::CONCAT(g_,FUNC) == NULL)   \
                                      CLASS::CONCAT(g_,FUNC) = new CLASS(); \
                                      return (*CLASS::CONCAT(g_,FUNC));     \
                                    }
