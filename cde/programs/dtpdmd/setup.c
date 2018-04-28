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

/******************************************************************************
 ******************************************************************************
 **
 ** File:         setup.c
 ** RCS:          $TOG: setup.c /main/4 1997/06/18 17:33:23 samborn $
 **
 ** Description:
 **
 ** (c) Copyright 1995, Hewlett-Packard Company, all rights reserved.
 **
 ******************************************************************************
 *****************************************************************************/

#define SETUP_DOT_C

#include "dtpdmdP.h"


/*********************************************************************
 *
 * Setup PDM_MANAGER selection and TYPE atoms
 */
Bool _PdmMgrSetup( Display *sel_display, Window sel_window, Bool security_flag )
{
    enum { XA_PDM_START, XA_PDM_REPLY, XA_TARGETS, XA_TIMESTAMP,
	   XA_MULTIPLE, XA_PDM_START_OK, XA_PDM_START_VXAUTH,
	   XA_PDM_START_PXAUTH, XA_PDM_START_ERROR, XA_PDM_EXIT_OK,
	   XA_PDM_EXIT_CANCEL, XA_PDM_EXIT_VXAUTH, XA_PDM_EXIT_PXAUTH,
	   XA_PDM_EXIT_ERROR, XA_PDM_MBOX, XA_PDM_MAIL, NUM_ATOMS };
    static char *atom_names[] = { 
           "PDM_START", "PDM_REPLY", "TARGETS", "TIMESTAMP",
	   "MULTIPLE", "PDM_START_OK", "PDM_START_VXAUTH",
	   "PDM_START_PXAUTH", "PDM_START_ERROR", "PDM_EXIT_OK",
	   "PDM_EXIT_CANCEL", "PDM_EXIT_VXAUTH", "PDM_EXIT_PXAUTH",
	   "PDM_EXIT_ERROR", "PDM_MBOX", "PDM_MAIL" };

    int i;
    Atom atoms[XtNumber(atom_names)];

    /*
     * Create PDM_MANAGER atoms.
     */
    XInternAtoms( sel_display, atom_names, XtNumber(atom_names), False, atoms );

    g.pdm_selection = XInternAtom( sel_display, g.alt_selection, False );
    g.pdm_start     = atoms[XA_PDM_START];
    g.pdm_reply     = atoms[XA_PDM_REPLY];
    g.pdm_targets   = atoms[XA_TARGETS];
    g.pdm_timestamp = atoms[XA_TIMESTAMP];
    g.pdm_multiple  = atoms[XA_MULTIPLE];

    g.pdm_start_ok      = atoms[XA_PDM_START_OK];
    g.pdm_start_vxauth  = atoms[XA_PDM_START_VXAUTH];
    g.pdm_start_pxauth  = atoms[XA_PDM_START_PXAUTH];
    g.pdm_start_error   = atoms[XA_PDM_START_ERROR];

    g.pdm_exit_ok      = atoms[XA_PDM_EXIT_OK];
    g.pdm_exit_cancel  = atoms[XA_PDM_EXIT_CANCEL];
    g.pdm_exit_vxauth  = atoms[XA_PDM_EXIT_VXAUTH];
    g.pdm_exit_pxauth  = atoms[XA_PDM_EXIT_PXAUTH];
    g.pdm_exit_error   = atoms[XA_PDM_EXIT_ERROR];

    if (security_flag){
	g.pdm_mbox = atoms[XA_PDM_MBOX];
	g.pdm_mail = atoms[XA_PDM_MAIL];
    }
    else {
	g.pdm_mbox = (Atom) 0;
	g.pdm_mail = (Atom) 0;
    }

    if ( XGetSelectionOwner(sel_display, g.pdm_selection) != None ) {
	/*
	 * Someone else already owns the selection.
	 */
	return( /* cannot become owner */ False );
    }
    else {
	/*
	 * Claim ownership of the selection.
	 */
	i=0;
	while ((XGetSelectionOwner(sel_display, g.pdm_selection)!=sel_window)) {
	    if ( i++ > 100 )
		return( /* cannot become owner */ False );

	    XSetSelectionOwner( sel_display, g.pdm_selection, sel_window,
				CurrentTime );
	}
    }
    return( /* became owner */ True );
}

