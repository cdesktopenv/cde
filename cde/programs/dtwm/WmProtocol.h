/* 
 * (c) Copyright 1989, 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
*/ 
/* 
 * Motif Release 1.2
*/ 
/*   $XConsortium: WmProtocol.h /main/4 1995/11/01 11:49:37 rswiston $ */
/*
 * (c) Copyright 1987, 1988, 1989, 1990 HEWLETT-PACKARD COMPANY */

extern void SetupWmICCC (void);
extern void SendConfigureNotify (ClientData *pCD);
extern void SendClientOffsetMessage (ClientData *pCD);
extern void SendClientMsg (Window window, long type, long data0, Time time, long *pData, int dataLen);
extern Boolean AddWmTimer (unsigned int timerType, unsigned long timerInterval, ClientData *pCD);
extern void DeleteClientWmTimers (ClientData *pCD);
extern void TimeoutProc (caddr_t client_data, XtIntervalId *id);
