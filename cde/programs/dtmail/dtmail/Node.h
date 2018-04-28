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
 *	$XConsortium: Node.h /main/4 1996/04/21 19:42:44 drk $
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

#ifndef	__NODESERVANT_HH
#define	__NODESERVANT_HH

#include <sys/types.h>

#include <Xm/Xm.h>
#include <DtMail/DtMail.hh>

// Nodes are of critical importance to the MailTool architecture 
// since all classes rely heavily on Nodes.
// It is possible for the client to maintain a list of Nodes for
// each Folder that it is interested in.  Each Folder has a list of
// Nodes, a Node for each Message the Folder's mailfile contains.
// Each Message has a list of Nodes, a Node for each Attachment the 
// Message contains. And so on...
// Nodes are circular lists and the root node is the only one that
// has a "home-p" set to TRUE.  Each Node has a my_previous_node and
// a my_next_node which point to the previous and next nodes in the
// circular list.  Each Node has data which is of type CORBA::ObjRef.
// The data in the Node depends on which level the node exists - 
// Nodes maintained by Folders contain Messages, Nodes maintained by
// Messages contain Attachments, ...
// This implementation of Nodes is abstract enough for *any* CORBA::ObjRef
// to be placed in Nodes.  Hence, it can be easily used or adapted-for-use
// in any other environment requiring its functionality.
// Semantics:
//
// set_homep()			:  Set self to be home-node.
// is_homep()			:  Is self home-node?
// get_home()			:  Traverse through hierarchy until 
//				   home-node is reached.
// set/get_data()		:  Set or get Node's data.
// next(), prev()		:  Go to next (or previous) Node.
// append(), prepend()		:  Append or prepend a Node to self.`
// remove()			:  Remove node from list.  
// destroy()			:  Destroy node, its contents and release objrefs.


class Node {

    public:
				Node();  
				Node(
				    DtMailMessageHandle msg_num,
				    char * str
				);

				~Node();

	void			set_homep(
				);
	boolean			is_home(
				);
	Node*			get_home(
				);

	void			set_message_handle(
					DtMailMessageHandle a_hndl
				);
    
    
        DtMailMessageHandle		get_message_handle(
				);
    
        void			set_message_header(char* hdr);
	char*			get_message_header();

	void			set_number(int i);
	int			get_number();

	Node*			next(
				);

	Node*			prev(
				);

	void			set_previous_node(
					Node* anyNode
				);
	void			set_next_node(
					Node* anyNode
				);

	void			append(
					Node* anyNode
				);
	void			prepend(
					Node* anyNode
				);
       
	void			remove(
				);

    private:
    
	DtMailMessageHandle		my_message_handle;
        char*			my_message_header;
	int			bogus_number;
	boolean			homep;
	boolean			deleted_p;
	Node*			my_previous_node;
	Node*			my_next_node;
};


#endif			//__NODESERVANT_HH




