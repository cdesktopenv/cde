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
