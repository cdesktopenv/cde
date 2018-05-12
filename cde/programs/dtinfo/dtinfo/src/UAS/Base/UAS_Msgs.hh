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
// $XConsortium: UAS_Msgs.hh /main/6 1996/06/11 16:38:23 cde-hal $
# ifndef _UAS_Msgs_hh_
# define _UAS_Msgs_hh_

template <class T> class UAS_Pointer;
class UAS_Common;
class UAS_Collection;

class UAS_StatusMsg {
    public:
	UAS_Pointer<UAS_Common> fDoc;
	char *fText;
};

class UAS_PartialDataMsg {
};

class UAS_DocumentRetrievedMsg {
    public:
	UAS_Pointer<UAS_Common> fDoc;
	UAS_DocumentRetrievedMsg (UAS_Common *doc): fDoc(doc) { }
};

class UAS_CollectionRetrievedMsg {
    public:
	UAS_Pointer<UAS_Collection> fCol;
	UAS_CollectionRetrievedMsg (UAS_Collection *col): fCol(col) {}
};

class UAS_CancelRetrievalMsg {
    public:
	UAS_Pointer<UAS_Common> fDoc;
	UAS_CancelRetrievalMsg (UAS_Pointer<UAS_Common> doc): fDoc(doc) {
	}
};

class UAS_ErrorMsg {
    public:
	UAS_ErrorMsg(): fErrorMsg(0) {}
	const char *fErrorMsg;
};

class UAS_LibraryDestroyedMsg {
    public:
	UAS_LibraryDestroyedMsg (): fLib (0) {
	}
        UAS_LibraryDestroyedMsg (UAS_Pointer<UAS_Common> lib):
            fLib (lib) {
        }
        UAS_Pointer<UAS_Common> fLib;
};

class UAS_SearchMsg {
    public:
        UAS_SearchMsg(char *m) : fSearchMsg(m), fNumBcases(0),
                                 fMaxNumBcases(0), fContFlag(1) {}
        char          *fSearchMsg;
        int            fNumBcases;
        int            fMaxNumBcases;
        unsigned char  fContFlag;
};

# endif
