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
 *  Msg.h -- Header file for DT messaging library 
 *
 *  $XConsortium: Msg.h /main/5 1996/03/01 16:36:15 drk $
 *
 * (C) Copyright 1993, Hewlett-Packard, all rights reserved.
 */
#ifndef _DT_MSG_H 
#define _DT_MSG_H

#include <X11/Intrinsic.h>
#include <Dt/DataTypes.h>

/*
 * _DtMessage handle definitions
 */
typedef struct _DtMsgHandle *DtMsgHandle;

/*
 * Return values
 */
#define dtmsg_NO_LISTENERS		(102)
#define dtmsg_SUCCESS			(1)
#define dtmsg_FAIL			(-1)
#define dtmsg_NO_SERVICE		(-102)
#define dtmsg_WRONG_FORMAT		(-103)
#define dtmsg_ANOTHER_PROVIDER		(-104)
#define dtmsg_LOST_SERVICE		(-105)

#define DtMsgContext   Pointer

#endif /* not defined _DT_MSG_H */
/***** END OF FILE ****/
