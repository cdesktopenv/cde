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
 *+SNOTICE
 *
 *	$XConsortium: DtMailDialogCallbackData.hh /main/4 1996/04/21 19:41:35 drk $
 *
 *	RESTRICTED CONFIDENTIAL INFORMATION:
 *	
 *	The information in this document is subject to special
 *	restrictions in a confidential disclosure agreement between
 *	HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
 *	document outside HP, IBM, Sun, USL, SCO, or Univel without
 *	Sun's specific written approval.  This document and all copies
 *	and derivative works thereof must be returned or destroyed at
 *	Sun's request.
 *
 *	Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
 *
 *+ENOTICE
 */

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//         This example code is from the book:
//
//           Object-Oriented Programming with C++ and OSF/Motif
//         by
//           Douglas Young
//           Prentice Hall, 1992
//           ISBN 0-13-630252-1	
//
//         Copyright 1991 by Prentice Hall
//         All Rights Reserved
//
//  Permission to use, copy, modify, and distribute this software for 
//  any purpose except publication and without fee is hereby granted, provided 
//  that the above copyright notice appear in all copies of the software.
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// DtMailDialogCallbackData.h: Auxiliary class used by DtMailGenDialog
//////////////////////////////////////////////////////////////
#ifndef DTMAILDIALOGCALLBACKDATA
#define DTMAILDIALOGCALLBACKDATA

class DtMailGenDialog;

typedef void (*DialogCallback)( void * );

class DtMailDialogCallbackData {

  private:
    
    DtMailGenDialog  *_dialog;
    DialogCallback  _ok;
    DialogCallback  _help;
    DialogCallback  _cancel;
    DialogCallback  _other;
    void           *_clientData;
    Widget          _other_w;
    
  public:
    
    DtMailDialogCallbackData ( DtMailGenDialog *dialog, 
			void          *clientData,
			DialogCallback ok,
			DialogCallback cancel,
			DialogCallback other,
			DialogCallback help,
			Widget other_w)
    {
	_dialog = dialog;
	_ok            = ok;
	_help          = help;
	_cancel        = cancel;
 	_other         = other;
	_clientData    = clientData;
	_other_w       = other_w;
    }
    
    DtMailGenDialog  *dialog() { return _dialog; }
    DialogCallback  ok() { return _ok; }
    DialogCallback  help() { return _help; }
    DialogCallback  cancel() { return _cancel; }
    DialogCallback  other() { return _other; }
    void           *clientData() { return _clientData; }
    Widget         other_w() { return _other_w; }
};
#endif

