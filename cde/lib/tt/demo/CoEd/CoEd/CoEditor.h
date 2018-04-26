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
/*%%  (c) Copyright 1993, 1994 Hewlett-Packard Company			 */
/*%%  (c) Copyright 1993, 1994 International Business Machines Corp.	 */
/*%%  (c) Copyright 1993, 1994 Sun Microsystems, Inc.			 */
/*%%  (c) Copyright 1993, 1994 Novell, Inc. 				 */
/*%%  $XConsortium: CoEditor.h /main/3 1995/10/20 17:04:44 rswiston $ 			 				 */
#ifndef CoEditor_h
#define CoEditor_h

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Tt/tttk.h>

class CoEditor {
    public:
				CoEditor(
					Widget	       *parent
				);
				CoEditor(
					Widget	       *parent,
					const char     *file
				);
				CoEditor(
					Widget	       *parent,
					Tt_message	msg,
					const char     *docname,
					Tt_status      &status
				);
				CoEditor(
					Widget	       *parent,
					Tt_message	msg,
					int		readOnly,
					const char     *file,
					const char     *docname,
					Tt_status      &status
				);
				CoEditor(
					Widget	       *parent,
					Tt_message	msg,
					int		readOnly,
					unsigned char  *contents,
					int             len,
					const char     *docname,
					Tt_status      &status
				);
				~CoEditor();

	static Tt_message	loadISOLatin1_(
					Tt_message	msg,
					void           *clientData,
					Tttk_op		op,
					Tt_status	diagnosis,
					unsigned char  *contents,
					int             len,
					char	       *file,
					char	       *docname
				);
	static Tt_message	contractCB_(
					Tt_message	msg,
					void	       *coEditor,
					Tt_message	contract
				);
	Tt_message		_contractCB(
					Tt_message	msg,
					Tt_message	contract
				);
	static Tt_message	subContractCB_(
					Tt_message	msg,
					void	       *coEditor,
					Tt_message	subContract
				);
	Tt_message		_subContractCB(
					Tt_message	msg,
					Tt_message	subContract
				);

    private:
	void			_init();
	Tt_status		_init(
					Tt_message	msg
				);
	void			_init(
					Widget	       *parent
				);
	Tt_status		_unload();
	Tt_status		_load(
					const char     *file
				);
	Tt_status		_load(
					unsigned char  *contents,
					int		len
				);
	Tt_status		_save();
	Tt_status		_revert();
	static void		_destroyCB_(
					Widget    w,
					XtPointer coEditor,
					XtPointer call_data
				);
	void			_destroyCB(
					Widget    w,
					XtPointer call_data
				);
	int			_quit(
					int		silent = 0,
					int		force  = 0
				);
	static int		_quitAll(
					int		silent,
					int		force
				);
	static void		_wmProtocolCB_(
					Widget    w,
					XtPointer coEditor,
					XmAnyCallbackStruct *cbs
				);
	void			_wmProtocolCB();
	static void		_fileButsCB_(
					Widget    button,
					XtPointer coEditor,
					XtPointer call_data
				);
	void			_fileButsCB(
					Widget    button,
					XtPointer call_data
				);
	static void		_editButsCB_(
					Widget    button,
					XtPointer coEditor,
					XtPointer call_data
				);
	void			_editButsCB(
					Widget    button,
					XtPointer call_data
				);
	char		       *_contents(
					int	       *len
				);
	char		       *_selection(
					int	       *len
				);
	Tt_status		_acceptContract(
					Tt_message	msg
				);
	static Tt_message	_mediaLoadMsgCB_(
					Tt_message	msg,
					void           *clientData,
					Tttk_op		op,
					unsigned char  *contents,
					int             len,
					char           *file
				);
	Tt_message		_mediaLoadMsgCB(
					Tt_message	msg,
					Tttk_op		op,
					unsigned char  *contents,
					int             len,
					char           *file
				);
	static void		_textUpdateCB_(
					Widget			coEditor,
					XtPointer		pTextBuffer,
					XmTextVerifyCallbackStruct *cbs
				);
	void			_textUpdateCB();
	static Tt_message	_fileCB_(
					Tt_message	msg,
					Tttk_op		op,
					char           *pathname,
					void	       *clientData,
					int		trust,
					int		me
				);
	Tt_message		_fileCB(
					Tt_message	msg,
					Tttk_op		op,
					char	       *pathname,
					int		trust,
					int		me
				);
	static void		_textModifyCB_(
					Widget		text,
					XtPointer	,
					XmTextVerifyCallbackStruct *mod
				);
	void			_textModifyCB(
					XmTextVerifyCallbackStruct *mod
				);
	void			_adviseUser(
					const char     *msg,
					Tt_status	status
				);
	Tt_status		_read_file(
					Widget	widget,
					char	*file
				);
	Tt_status		_write_file(
					Widget widget,
					char	*file
				);
    public:
	Widget			shell;
	static CoEditor	       *editors[];
	static int		numEditors;

    private:
	Widget			_baseFrame;
	Widget			_controls;
	Widget			_fileBut;
	Widget			_editBut;
	Widget			_text;
	Boolean			_modifiedByMe;
	int			_modifiedByOther; // >1 == user has been asked
	Tt_message		_contract;
	Tt_message		_subContract;
	Tt_pattern	       *_contractPats;
	Tt_pattern	       *_filePats;
	char		       *_file;		// free w/ free()
	int			_x; 		// geometry of parent
	int			_y;
	int			_w;
	int			_h;
};

#endif
