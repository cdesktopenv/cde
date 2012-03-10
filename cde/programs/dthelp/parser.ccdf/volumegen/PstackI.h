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


















