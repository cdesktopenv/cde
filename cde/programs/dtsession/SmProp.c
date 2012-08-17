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
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * (c) Copyright 1995 Digital Equipment Corporation.
 * (c) Copyright 1993, 1994, 1995 Hewlett-Packard Company
 * (c) Copyright 1993, 1994, 1995 International Business Machines Corp.
 * (c) Copyright 1993, 1994, 1995 Sun Microsystems, Inc.
 * (c) Copyright 1993, 1994, 1995 Novell, Inc. 
 * (c) Copyright 1995 FUJITSU LIMITED.
 * (c) Copyright 1995 Hitachi.
 *
 * $XConsortium: SmProp.c /main/4 1996/10/07 10:29:51 drk $
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/SM/SMlib.h>
#include "SmXSMP.h"

/*
 * Forward declarations
 */
static SmPropValue * CopyPropValues (
	int			numPropValues,
	SmPropValue		* thePropValues);


void
SetPropertiesProc (
	SmsConn         	smsConn,
	SmPointer       	managerData,
	int             	numProps,
	SmProp          	**theProps)
{
	ClientRecPtr   		pClient = (ClientRecPtr) managerData;
	PropertyRecPtr   	pProp;
	int 			i, j;
	Boolean			found;

#ifdef DEBUG
	(void) printf ("Received SET PROPERTIES [%p]\n", smsConn);
#endif /* DEBUG */

	if (numProps == 0)
		return;

	for (i = 0; i < numProps; i++) {

		found = False;

		if (theProps[i]->num_vals == 0)
			continue;

		for (pProp = pClient->props; 
		     pProp != NULL; 
		     pProp = pProp->next) {
			
			if (!strcmp (theProps[i]->name, pProp->prop.name) &&
			    !strcmp (theProps[i]->type, pProp->prop.type)) {
				/* 
				 * Reuse this property but replace its
				 * property values
				 */

				for (j = 0; j < pProp->prop.num_vals; j++) {
					XtFree (pProp->prop.vals[j].value);
				}

				if (pProp->prop.num_vals > 0)
					XtFree ((char *) pProp->prop.vals);

				pProp->prop.vals = CopyPropValues (
					theProps[i]->num_vals, 
					theProps[i]->vals);

				pProp->prop.num_vals = theProps[i]->num_vals;

				found = True;
				break;
			}	
		}

		if (!found) {

			/*
			 * Put it at the end of the list (or the beginning
			 * if this client has no properties)
			 */
			PropertyRecPtr		trail = pClient->props;
			for (pProp = pClient->props; 
			     pProp != NULL; 
			     trail = pProp, pProp = pProp->next);


			pProp = (PropertyRecPtr)XtMalloc (sizeof (PropertyRec));
			if (!pProp)
				return;

			if (trail)
				trail->next = pProp;
			else
				pClient->props = pProp;

			pProp->prop.name = XtNewString (theProps[i]->name);
			pProp->prop.type = XtNewString (theProps[i]->type);
			pProp->prop.num_vals = theProps[i]->num_vals;
			pProp->next = NULL;

			pProp->prop.vals = CopyPropValues (
				theProps[i]->num_vals, theProps[i]->vals);
		}

		SmFreeProperty (theProps[i]);
	}

	if (theProps)
		free ((char *) theProps);

#ifdef DEBUG
	for (i = 0, pProp = pClient->props; 
	     pProp != NULL; 
	     i++, pProp = pProp->next) {
		(void) printf ("\t[%2d] name = %s\n", i + 1, 
			pProp->prop.name);
		(void) printf ("\t[%2d] type = %s\n", i + 1, 
			pProp->prop.type);
		(void) printf ("\t[%2d] num props = %d\n", i + 1, 
			pProp->prop.num_vals);
		for (j = 0; j < pProp->prop.num_vals; j++) {
			(void) printf ("\t\t [%2d] (%3d bytes) = %s\n", j + 1,
				pProp->prop.vals[j].length,
				pProp->prop.vals[j].value);
		}
	}
#endif /* DEBUG */
}


void 
DeletePropertiesProc (
	SmsConn         	smsConn,
	SmPointer       	managerData,
	int             	numProps,
	char            	**propNames)
{
	ClientRecPtr   		pClient = (ClientRecPtr) managerData;
	int			i;

#ifdef DEBUG
	(void) printf ("Received DELETE PROPERTIES [%p] - %d properties\n", 
			smsConn, numProps);
	for (i = 0; i < numProps; i++)
		(void) printf ("\t%s\n", propNames[i]);
#endif /* DEBUG */

	if (numProps == 0)
		return;

	for (i = 0; i < numProps; i++) {

		PropertyRecPtr   	pProp;
		PropertyRecPtr   	tmp;
		PropertyRecPtr   	trail;

		for (pProp = pClient->props, trail = pClient->props ; 
		     pProp != NULL;
		     trail = pProp, pProp = pProp->next) {

			if (!strcmp (pProp->prop.name, propNames[i])) {

				int		j;

				XtFree (pProp->prop.name);
				XtFree (pProp->prop.type);

				for (j = 0; j < pProp->prop.num_vals; j++) {
					XtFree (pProp->prop.vals[j].value);
				}

				if (pProp->prop.num_vals > 0)
					XtFree ((char *) pProp->prop.vals);
				/*
			 	 * Remove the record from the list and then
			 	 * free the record
			 	 */
				trail->next = pProp->next;
				XtFree ((char *) pProp);
				pProp = trail;
			}
		}
		
		free (propNames[i]);
	}

	free ((char *) propNames);
}


void
GetPropertiesProc (
	SmsConn         	smsConn,
	SmPointer       	managerData)
{
	ClientRecPtr   		pClient = (ClientRecPtr) managerData;
	PropertyRecPtr   	pProp;
	PropertyRecPtr   	trail;
	int			numProps;
	int			i, j;
	SmProp			**pPropsRet;

#ifdef DEBUG
	(void) printf ("Received GET PROPERTIES [%p]\n", smsConn);
#endif /* DEBUG */

	if (!pClient->props) {
		SmsReturnProperties (smsConn, 0, NULL);
		return;
	}

	for (pProp = pClient->props, numProps = 0; 
	     pProp != NULL;
	     pProp = pProp->next, numProps++);

	pPropsRet = (SmProp **) XtMalloc (numProps * sizeof (SmProp *));

	if (!pPropsRet) {
		SmsReturnProperties (smsConn, 0, NULL);
		return;
	}

	/*
	 * This function must return an array of pointers to SmProp
	 * structures and since the properties are actually stored
	 * in a linked list, a transformation is required.
	 */
	for (i=0, pProp = pClient->props; 
	     pProp != NULL; 
	     i++, pProp = pProp->next) {

		pPropsRet[i] = (SmProp *) XtMalloc (sizeof (SmProp));
		if (!pPropsRet[i]) {
			SmsReturnProperties (smsConn, 0, NULL);
			return;
		}

		pPropsRet[i]->name = XtNewString (pProp->prop.name);
		pPropsRet[i]->type = XtNewString (pProp->prop.type);
		pPropsRet[i]->num_vals = pProp->prop.num_vals;
		pPropsRet[i]->vals = (SmPropValue *) XtMalloc (
			pProp->prop.num_vals * sizeof (SmPropValue));

		if (!pPropsRet[i]->vals) {
			SmsReturnProperties (smsConn, 0, NULL);
			return;
		}

		for (j = 0; j < pProp->prop.num_vals; j++) {

			pPropsRet[i]->vals[j].length = 
				pProp->prop.vals[j].length;
			pPropsRet[i]->vals[j].value = (SmPointer)
				XtMalloc (pProp->prop.vals[j].length);

			if (!pPropsRet[i]->vals[j].value) {
				SmsReturnProperties (smsConn, 0, NULL);
				return;
			}

			memcpy (pPropsRet[i]->vals[j].value,
				pProp->prop.vals[j].value,
				pProp->prop.vals[j].length);
		}
	}

	SmsReturnProperties (smsConn, numProps, pPropsRet);

#ifdef DEBUG
	for (i = 0; i < numProps; i++) {
		(void) printf ("\t[%2d] name = %s\n", i + 1, 
			pPropsRet[i]->name);
		(void) printf ("\t[%2d] type = %s\n", i + 1, 
			pPropsRet[i]->type);
		(void) printf ("\t[%2d] num props = %d\n", i + 1, 
			pPropsRet[i]->num_vals);
		for (j = 0; j < pPropsRet[i]->num_vals; j++) {
			(void) printf ("\t\t [%2d] (%3d bytes) = %s\n", j + 1,
				pPropsRet[i]->vals[j].length,
				pPropsRet[i]->vals[j].value);
		}
	}
#endif /* DEBUG */

	for (i = 0; i < numProps; i++)
		SmFreeProperty (pPropsRet[i]);

	XtFree ((char *) pPropsRet);
}


static SmPropValue *
CopyPropValues (
	int			numPropValues,
	SmPropValue		* thePropValues)
{
	SmPropValue		* pPropValueRet;
	int			i;

	pPropValueRet = (SmPropValue *) XtMalloc (numPropValues *
		  sizeof (SmPropValue));

	if (!pPropValueRet)
		return (NULL);

	for (i = 0; i < numPropValues; i++) {

		pPropValueRet[i].length = thePropValues[i].length;
		pPropValueRet[i].value = (SmPointer) 
			XtMalloc(thePropValues[i].length + 1);

		if (!pPropValueRet[i].value)
			return (NULL);

		memcpy (pPropValueRet[i].value,
			thePropValues[i].value,
			thePropValues[i].length);

		((char *) pPropValueRet[i].value)[pPropValueRet[i].length]='\0';
	}

	return (pPropValueRet);
}


/*
 * Returns a ptr to the property record list for the given
 * property.
 */
PropertyRecPtr
GetPropertyRec (
	ClientRecPtr		pClientRec,
	char			* propName)
{
	PropertyRecPtr		pPropRec;

	for (pPropRec = pClientRec->props; pPropRec; 
			pPropRec = pPropRec->next) {

		if (!strcmp (propName, pPropRec->prop.name))
			return (pPropRec);

	}

	return (pPropRec);
}


/*
 * If the given property exists, it returns a ptr to the first
 * value of the property if the type of the property is SmARRAY8
 * (char *).
 */
char *
GetArrayPropertyValue (
	ClientRecPtr		pClientRec,
	char			* propName)
{
	PropertyRecPtr		pPropRec;

	if ((pPropRec = GetPropertyRec (pClientRec, propName)) == NULL)
		return (NULL);

	if ((!strcmp (pPropRec->prop.type, SmARRAY8)) &&
	    (pPropRec->prop.num_vals >= 1))
		return (pPropRec->prop.vals[0].value);

	return (NULL);
}


/*************************************<->*************************************
 *
 *  GetListOfArrayPropertyValue -
 *
 *  Description: If the specified property exits and its type is
 * 	a LISTofARRAY8, then a NULL-terminated array of pointers
 *	to the values is returned.
 *
 *  Returns: NULL-terminated array of values or NULL
 *
 *  Comments: caller is responsible for using XtFree to free the returned
 *	pointer if it is not NULL
 *
 *************************************<->***********************************/
char **
GetListOfArrayPropertyValue (
	ClientRecPtr		pClientRec,
	char			* propName)
{
	PropertyRecPtr		pPropRec;
	char			** ppchar;
	int			i;

	if ((pPropRec = GetPropertyRec (pClientRec, propName)) == NULL)
		return (NULL);

	if ((strcmp (pPropRec->prop.type, SmLISTofARRAY8)) ||
	    		(pPropRec->prop.num_vals <= 0))
		return (NULL);

	ppchar = (char **) XtMalloc ((pPropRec->prop.num_vals + 1) *
				   sizeof (char *));
	if (!ppchar)
		return (NULL);

	for (i = 0; i < pPropRec->prop.num_vals; i++)
		ppchar[i] = pPropRec->prop.vals[i].value;

	ppchar[pPropRec->prop.num_vals] = NULL;

	return (ppchar);
}


/*************************************<->*************************************
 *
 *  GetCardPropertyValue -
 *
 *  Description: If the specified property exits and its type is
 * 	a CARD8, then return its value.
 *
 *  Returns: True if the property is found; False otherwise
 *
 *************************************<->***********************************/
Boolean
GetCardPropertyValue (
	ClientRecPtr		pClientRec,
	char			* propName,
	int			* propValue)		/* MODIFIED */
{
	PropertyRecPtr		pPropRec;

	if ((pPropRec = GetPropertyRec (pClientRec, propName)) == NULL)
		return (False);

	if ((!strcmp (pPropRec->prop.type, SmCARD8)) &&
	    (pPropRec->prop.num_vals >= 1)) {
		int	hint =  (int) *((char *) (pPropRec->prop.vals[0].value));
		*propValue = hint;
		return (True);
	}

	return (False);
}
