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
 *	$XConsortium: ComposeCmds.hh /main/3 1995/11/06 16:05:11 rswiston $
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

#ifndef COMPOSECMDS_HH
#define COMPOSECMDS_HH

class ComposeFamily : public RoamCmd {
  private:
    RoamMenuWindow *	_parent;
    
  public:
    ComposeFamily(char * name, char *label, int active, RoamMenuWindow *);
#ifndef CAN_INLINE_VIRTUALS
    ~ComposeFamily( void );
#endif /* ! CAN_INLINE_VIRTUALS */
    void Display_entire_msg(DtMailMessageHandle, SendMsgDialog *, char *);
    void appendSignature(SendMsgDialog *);
    char * valueToAddrString(DtMailValueSeq & value);
};

class ComposeCmd : public ComposeFamily {
  private:
    RoamMenuWindow *	_parent;

  public:
    virtual void doit();   
    ComposeCmd( char *, char *, int, RoamMenuWindow * );
    virtual const char *const className () { return "ComposeCmd"; }
};

class ForwardCmd : public ComposeFamily {
  private:
	RoamMenuWindow *_parent;
	int _forward;
  public:
    virtual void doit();   
    ForwardCmd( char *, char *, int, RoamMenuWindow *, int );
    virtual const char *const className () { return "ForwardCmd"; }
};

class ReplyAllCmd : public ComposeFamily {
  private:
	RoamMenuWindow *_parent;
	int _include;
  public:
    virtual void doit();   
    ReplyAllCmd( char *, char *, int, RoamMenuWindow *, int );
    virtual const char *const className () { return "ReplyAllCmd"; }
};

class ReplyCmd : public ComposeFamily {
  private:
	RoamMenuWindow *_parent;
	int _include;
  public:
    virtual void doit();
    ReplyCmd( char *, char *, int, RoamMenuWindow *, int );
    virtual const char *const className () { return "ReplyCmd"; }
};

class TemplateCmd : public NoUndoCmd {
  private:
    SendMsgDialog	*_compose;
    char		*_file;

  public:
    virtual void doit();
    TemplateCmd(char * name, 
		char *label, 
		int active, 
		SendMsgDialog *, 
		const char * file);
    virtual ~TemplateCmd();
    virtual const char *const className() { return "TemplateCmd"; }
};

class HideShowCmd : public NoUndoCmd {
  private:
    SendMsgDialog	*_compose;
    char		*_header;

  public:
    virtual void doit(void);
    HideShowCmd(char * name,
		char *widgetlabel,
		int active,
		SendMsgDialog *,
		const char * label);
    virtual ~HideShowCmd(void);
    virtual const char *const className() { return "HideShowCmd"; }
};

#endif
