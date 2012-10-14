/*
 * $XConsortium: StyleSheetMgr.hh /main/4 1996/08/20 12:05:09 rcs $
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

class StyleSheet ;
class FullStyleRecord ;

#include "UAS.hh"

class StyleSheetMgr : public Long_Lived
{
public:
  StyleSheetMgr();
  ~StyleSheetMgr();

  void font_preference_modified();
  void initOnlineStyleSheet (UAS_Pointer<UAS_Common>&);
  void initPrintStyleSheet (UAS_Pointer<UAS_Common>&);

private:
    UAS_Pointer<UAS_StyleSheet> fLastSS;
    StyleSheet *fCurrent;
    int fStyleSheetRead;

private:
  LONG_LIVED_HH(StyleSheetMgr,style_sheet_mgr);
};

LONG_LIVED_HH2(StyleSheetMgr,style_sheet_mgr);

