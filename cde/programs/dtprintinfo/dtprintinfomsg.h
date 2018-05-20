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
/* $XConsortium: dtprintinfomsg.h /main/3 1995/11/06 09:33:49 rswiston $ */
#ifndef DTPRINTINFOMSG_H
#define DTPRINTINFOMSG_H

/*
 * if __cplusplus is defined, the system header files take care
 * of themselves and putting this "extern C" here causes inconsistent linkage
 * specifications.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <nl_types.h> 
#include <locale.h> 

#include "dtprintinfo_msg.h"
#include "dtprintinfo_cat.h"

extern nl_catd dtprintinfo_cat;

#ifdef hpux
extern char *Catgets(nl_catd catd, int set_num, int msg_num, char *s);
#define MESSAGE(msg) \
   Catgets(dtprintinfo_cat,DTPRINTER_SET,msg,TXT_DTPRINTER_SET_ ## msg)
#else
#define MESSAGE(msg) \
   catgets(dtprintinfo_cat,DTPRINTER_SET,msg,TXT_DTPRINTER_SET_ ## msg)
#endif

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* DTPRINTINFOMSG_H */
