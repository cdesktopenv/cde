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
/* $XConsortium: PstackI.h /main/4 1995/11/08 11:47:58 rswiston $ */
/************************************<+>*************************************
 ****************************************************************************
 *
 * File:         PstackI.h
 *
 * Project:      Cache Creek (Rivers) Project
 * Description:  Header file for Utility functions for PStack.h
 * Author:       Brian Cripe
 *               Modifications by (Mike Wilson)
 * Language:     C
 *
 * (C) Copyright 1992, Hewlett-Packard, all rights reserved.
 *
 ****************************************************************************
 ************************************<+>*************************************/

struct PStackRec {
    int size;
    int count;
    void **items;
};

typedef struct PStackRec *PStack;





/*****************************************************************************
 * Function:	    PStack PStackCreate();
 *
 * Parameters:      NONE.
 *
 * Return Value:    PStack.
 *
 * Purpose:         Generates a PStack structure and returns it to the 
 *                  caller.
 *
 *****************************************************************************/
extern PStack PStackCreate();


/*****************************************************************************
 * Function:	    PStack PStackDestroy();
 *
 * Parameters:      stack:
 *
 * Return Value:    void.
 *
 * Purpose:         Destroys a PStack structure.
 *
 *****************************************************************************/
extern void PStackDestroy(
     PStack stack);


/*****************************************************************************
 * Function:	    PStack PStackPush();
 *
 * Parameters:      stack:
 *                  ptr:
 *
 * Return Value:    void
 *
 * Purpose:         Pushes a PStack structure on the current stack.
 *
 *****************************************************************************/
extern void PStackPush(
     PStack stack,
     void *ptr);


/*****************************************************************************
 * Function:	    PStack *PStackPeek();
 *
 * Parameters:      stack:
 *
 * Return Value:    void.
 *
 * Purpose:         Gets a PStack.
 *
 *****************************************************************************/
extern void *PStackPeek(
     PStack stack);


/*****************************************************************************
 * Function:	    PStack *PStackPop();
 *
 * Parameters:      stack:
 *
 * Return Value:    void.
 *
 * Purpose:         Pops the top element off the stack
 *
 *****************************************************************************/
extern void *PStackPop(
     PStack stack);


















