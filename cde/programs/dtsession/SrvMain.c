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
static char rcsid[] =
  "$XConsortium: SrvMain.c /main/4 1995/10/30 09:40:18 rswiston $";
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */
/*
 $Log$
 * Revision 1.10  95/02/24  13:47:53  13:47:53  mcnichol_ibm_austin
 * 7602 - code needs 4 line CDE partner copyright notices
 * 
 * Revision 1.9  94/07/25  15:52:01  15:52:01  mcbill_hp_cv
 * remove call to DtSetUserMessageHandler (obsolete)
 *  
 * 
 * Revision 1.8  94/05/05  16:13:58  16:13:58  arg_sun
 * DevEnv_cleanup
 * 
 * Revision 1.7  93/08/12  20:25:17  20:25:17  xbuild (See Marc Ayotte)
 * additional CoeToDt check-in
 * 
 * Revision 1.6  93/08/12  20:17:58  20:17:58  xbuild (See Marc Ayotte)
 * CoeToDt check-in
 * 
 * Revision 1.5  93/03/08  14:39:58  14:39:58  xbuild (See Marc Ayotte)
 * "duplicate rev for sending to ibm"
 * 
 * Revision 1.4  93/03/05  11:19:52  11:19:52  ronv (Ronald Voll)
 * ========= Changed strings: =============
 * 	vue --> dt 
 * 	Vue --> Dt 
 * 	VuE --> CoE 
 * 	VUE --> DT 
 * 
 * Revision 1.3  93/01/28  15:47:47  15:47:47  xbuild (See Marc Ayotte)
 * XmpToDt check-in
 * 
 * Revision 1.2  93/01/27  19:50:39  19:50:39  xbuild (See Marc Ayotte)
 * XueToDt check-in
 * 
 * Revision 1.1  93/01/12  10:42:51  10:42:51  xbuild (See Marc Ayotte)
 * Initial revision
 * 
 * Revision 1.1  93/01/07  16:40:10  16:40:10  xbuild (See Marc Ayotte)
 * Initial revision
 * 
 * Revision 1.1  91/10/31  16:26:30  16:26:30  keith (Keith Taylor)
 * Initial revision
 * 
 * Revision 3.3  91/05/22  14:02:22  14:02:22  julie (Julie Skeen)
 * protos and domain fixes
 * 
 * Revision 3.2  90/09/24  17:20:33  17:20:33  fredh (Fred Handloser)
 * change to static message catalogs
 * 
 * Revision 3.1  90/07/27  11:37:34  11:37:34  ted (Ted Ransom)
 * added the new parameter for InitializeCoecolor
 * 
 * Revision 3.0  90/07/10  12:08:42  12:08:42  kimd (Kim Drongesen)
 * first release after strider/tw
 * useAsyncGeometry and font changes for topcat added
 * 
 * Revision 2.6  90/04/26  14:00:03  14:00:03  ted
 * bullet proofing
 * 
 * Revision 2.5  90/03/29  08:08:46  08:08:46  ted (Ted Ransom)
 * now calls InitializeCoecolor .. does CheckMonitor, InitializePalettes, and
 * AllocateColors
 * 
 * Revision 2.4  90/03/26  14:34:45  14:34:45  ted (Ted Ransom)
 * registered error message handler .. registered XeProgName
 * 
 * Revision 2.3  90/03/14  17:23:40  17:23:40  ted (Ted Ransom)
 * no change
 * 
 * Revision 2.2  90/03/14  11:25:43  11:25:43  jenny (Jennefer Wood)
 * added appContext to XtOpenDisplay and XtAppMainLoop
 * 
 * Revision 2.1  90/03/13  09:04:30  09:04:30  ted (Ted Ransom)
 * added support for Selections .. use -DSELECTION
 * 
 * Revision 2.0  90/03/12  09:17:25  09:17:25  jenny (Jennefer Wood)
 * NCGA
 * 
 * Revision 1.5  90/02/28  11:27:02  11:27:02  ted (Ted Ransom)
 * updated to support multiple screens
 * 
 * Revision 1.4  90/02/26  17:25:02  17:25:02  ted (Ted Ransom)
 * no changes ... sorry
 * 
 * Revision 1.3  90/02/21  13:41:08  13:41:08  root ()
 * added malloc_check and malloc_trace under ifdef for USERHELP
 * 
 * Revision 1.2  90/02/12  16:04:09  16:04:09  ted (Ted Ransom)
 * changed c_server.h Srv.h
 * 
 * Revision 1.1  90/02/12  15:20:45  15:20:45  ted (Ted Ransom)
 * Initial revision
 * 
*/
#include "Srv.h"
#include "Sm.h"

void 
main(
        int argc,
        char **argv )
{
    Display *display;
    int result;

#ifdef USERHELP
    malloc_check(1);
    malloc_trace(0);
#endif
    XtAppContext app;

    /* Initialize the toolkit and open the display */
    XtToolkitInitialize();

    app = XtCreateApplicationContext();

    display = XtOpenDisplay(app, NULL, argv[0], "Customize", NULL, 0,
                                         &argc, argv);

    /* Go register the DT error handler */
    XeProgName = argv[0];

    result = InitializeDtcolor(display, DEFAULT_SESSION);

    if(result != 0)
      exit(0);

    XtAppMainLoop(app);
}

