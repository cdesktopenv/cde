/*
 * $XConsortium: PrintMgr.hh /main/6 1996/12/06 14:44:18 rcs $
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
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 * 
 */

#include "UAS.hh"

class PrintPanelAgent;
class NodePrintAgent;

// print_handle is used the BitHandle used by the PrintPanelAgent to determine
// which nodes to print
// standard_print_handle is a BitHandle reserved by the PrintMgr so that
// something may set that bit handle in the Library outline list to indicate
// it is for printing. Then the print_handle() is set to this...


// Example: if we are printing selected from an OutlineListView,  then the
// display bit handle of the OLV is set in the PrintMgr before calling
// display().
// If we just wish to print one node, then we can get the standard BitHandle
// from the PrintMgr, set it on in the node we wish to print (in the library
// list) and then do:
//
// print_handle(standard_print_handle());
// display()

class WTopLevelShell;
template <class T> class xList;

class PrintMgr : public Long_Lived
{
public:

    PrintMgr();
    
    void print(UAS_Pointer<UAS_Common> &node_ptr);
    void destroy_agent();
    NodeViewInfo *load(UAS_Pointer<UAS_Common> &node_ptr);
    // for old style.sheet 
    void set_font_scale(int);  

private:
    NodePrintAgent * f_npa;      
    UAS_Pointer<UAS_Common> f_last_printed;	
    int	f_font_scale ; // for old style.sheet    

private:
    LONG_LIVED_HH(PrintMgr,print_mgr);
};

LONG_LIVED_HH2(PrintMgr,print_mgr);

