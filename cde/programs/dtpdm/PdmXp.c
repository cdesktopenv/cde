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
/* $XConsortium: PdmXp.c /main/4 1996/08/12 18:43:03 cde-hp $ */
/*
 * dtpdm/PdmXp.c
 */
/*
 * (c) Copyright 1996 Digital Equipment Corporation.
 * (c) Copyright 1996 Hewlett-Packard Company.
 * (c) Copyright 1996 International Business Machines Corp.
 * (c) Copyright 1996 Sun Microsystems, Inc.
 * (c) Copyright 1996 Novell, Inc. 
 * (c) Copyright 1996 FUJITSU LIMITED.
 * (c) Copyright 1996 Hitachi.
 */
#include <stdio.h>
#include <sys/stat.h>
#include "PdmXp.h"

#include <X11/Intrinsic.h>

typedef enum {
    PDMXP_JOB, PDMXP_DOC, PDMXP_PRINTER, PDMXP_SERVER,
    PDMXP_BAD_POOL /* should always be last in list */
} PdmXpPoolIndex;


/*
 * static function declarations
 */
static const char* PdmXpGetQualifier(PdmXp* me);
static char* PdmXpBuildResourceName(PdmXp* me, PdmOid id_att);
#if 0 && defined(PRINTING_SUPPORTED)
static XrmDatabase PdmXpLoadPool(PdmXp* me, XPAttributes type);
#endif /* PRINTING_SUPPORTED */


/*
 * ------------------------------------------------------------------------
 * Name: PdmXpNew
 *
 * Description:
 *
 *     Creates a new PdmXp instance structure.
 *
 * Return value:
 *
 *     The new PdmXp instance structure.
 *
 */
PdmXp*
PdmXpNew(void)
{
    return (PdmXp*)XtCalloc(1, sizeof(PdmXp));
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpDelete
 *
 * Description:
 *
 *     Closes an existing Xp connection, and frees the passed
 *     PdmXp instance structure.
 *
 * Return value:
 *
 *     None.
 *
 */
void
PdmXpDelete(PdmXp* me)
{
    PdmXpClose(me);
    XtFree((char*)me);
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpOpen
 *
 * Description:
 *
 *     This function opens the passed print display specifier and sets
 *     the passed print context on the newly opened print display.
 *
 * Return value:
 *
 *     If successful, the print display pointer is returned. If unable to
 *     open the display, or if the display does not support the Xp
 *     extension, NULL is returned.
 *
 */
Display*
PdmXpOpen(PdmXp* me,
	  char* display_spec,
	  char* context_str)
{
    /*
     * only maintain one connection
     */
    PdmXpClose(me);
    /*
     * open the passed display spec
     */
    me->display = XOpenDisplay(display_spec);
    if(me->display)
    {
	int error_base;
	int event_base;
	/*
	 * check to see if the display is a print server
	 */
#if 0 && defined(PRINTING_SUPPORTED)
	if(XpQueryExtension(me->display, &event_base, &error_base))
	{
	    /*
	     * set the passed print context on the print display
	     */
	    me->context = strtoul(context_str, (char**)NULL, 0);
	    /*
	     * load the resource DB qualifier
	     */
	    PdmXpGetQualifier(me);
	}
	else
	{
#endif /* PRINTING_SUPPORTED */
	    XCloseDisplay(me->display);
	    me->display = (Display*)NULL;
#if 0 && defined(PRINTING_SUPPORTED)
	}
#endif /* PRINTING_SUPPORTED */
    }

    return me->display;
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpClose
 *
 * Description:
 *
 *     Closes the print display.
 *
 * Return value:
 *
 *     None.
 *
 */
void
PdmXpClose(PdmXp* me)
{
    if(me->display)
    {
	int i;
	
	for(i = 0; i < PDMXP_POOL_COUNT; i++)
	{
	    if(me->pool[i] != (XrmDatabase)NULL)
	    {
		XrmDestroyDatabase(me->pool[i]);
		me->pool[i] = (XrmDatabase)NULL;
	    }
	}
	XCloseDisplay(me->display);
	me->display = NULL;
#if 0 && defined(PRINTING_SUPPORTED)
	me->context = (XPContext)NULL;
#endif /* PRINTING_SUPPORTED */
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpLoadPool
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
#if 0 && defined(PRINTING_SUPPORTED)
static XrmDatabase
PdmXpLoadPool(PdmXp* me, XPAttributes type)
{
    PdmXpPoolIndex i;
    /*
     * determine the index into the pool array based on the Xp pool type
     */
    switch(type)
    {
    case XPJobAttr:
	i = PDMXP_JOB;
	break;
    case XPDocAttr:
	i = PDMXP_DOC;
	break;
    case XPPrinterAttr:
	i = PDMXP_PRINTER;
	break;
    case XPServerAttr:
	i = PDMXP_SERVER;
	break;
    default:
	return (XrmDatabase)NULL;
	break;
    }
    /*
     * get the attributes from the X print server
     */
    if(me->pool[i] == (XrmDatabase)NULL)
    {
	XTextProperty text_prop;
	char** list;
	int count;
	
	text_prop.value = (unsigned char*)
	    XpGetAttributes(me->display, me->context, type);
	text_prop.encoding = XInternAtom(me->display, "COMPOUND_TEXT", False);
	text_prop.format = 8;
	text_prop.nitems = strlen((char*)text_prop.value);
	if(Success ==
	   XmbTextPropertyToTextList(me->display, &text_prop, &list, &count))
	{
	    if(count > 0)
		me->pool[i] = XrmGetStringDatabase(list[0]);

	    XFreeStringList(list);	    
	}
    }

    return me->pool[i];
}
#endif /* PRINTING_SUPPORTED */

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpGetQualifier
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *
 */
static const char*
PdmXpGetQualifier(PdmXp* me)
{
    if(me->qualifier == (char*)NULL)
    {
#if 0 && defined(PRINTING_SUPPORTED)
	if(PdmXpLoadPool(me, XPPrinterAttr) != (XrmDatabase)NULL)
	{
	    char* str_type;
	    XrmValue value;
	    
	    if(XrmGetResource(me->pool[PDMXP_PRINTER],
			      "qualifier", "qualifier", &str_type, &value))
	    {
		me->qualifier = XtNewString((char*)value.addr);
		me->qualifier_len = strlen(me->qualifier);
	    }
	}
#endif /* PRINTING_SUPPORTED */
    }
    return me->qualifier;
}



/*
 * ------------------------------------------------------------------------
 * Name: PdmXpBuildResourceName
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     A new fully-qualified resource name. It is the caller's
 *     responsibility to free the returned string by calling XtFree.
 *
 */
static char*
PdmXpBuildResourceName(PdmXp* me, PdmOid id_att)
{
    char* ptr;
    char* res_name;
    int oid_str_len;
    /*
     * allocate memory for the resource name
     */
    oid_str_len = PdmOidStringLength(id_att);
    ptr = res_name =
	XtMalloc(me->qualifier_len + 1 + oid_str_len + 1);
    /*
     * build the resource name from the printer name and the string value
     * for the passed attribute id
     */
    strncpy(ptr, me->qualifier, me->qualifier_len);
    ptr += me->qualifier_len;
    *ptr = '.';
    ptr += 1;
    strncpy(ptr, PdmOidString(id_att), oid_str_len);
    ptr += oid_str_len;
    *ptr = '\0';
    /*
     * return
     */
    return res_name;
}


/*
 * ------------------------------------------------------------------------
 * Name: PdmXpGetValue
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     pdmoid_none if the attribute value is not found.
 *
 */
#if 0 && defined(PRINTING_SUPPORTED)
PdmOid
PdmXpGetValue(PdmXp* me,
	      XPAttributes type,
	      PdmOid id_att)
{
    const char* value;
    
    value = PdmXpGetStringValue(me, type, id_att);

    return PdmOidFromString(value);
}
#endif /* PRINTING_SUPPORTED */

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpGetStringValue
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     NULL if the attribute value is not found, or if the resource
 *     representation type is not a string.
 *
 */
#if 0 && defined(PRINTING_SUPPORTED)
const char*
PdmXpGetStringValue(PdmXp* me,
		    XPAttributes type,
		    PdmOid id_att)
{
    char* res_name;
    char* str_type;
    XrmValue value;
    Bool found;
    
    XrmDatabase pool;
    
    pool = PdmXpLoadPool(me, type);
    if(pool == (XrmDatabase)NULL)
	return (const char*)NULL;

    res_name = PdmXpBuildResourceName(me, id_att);
    found = XrmGetResource(pool, res_name, res_name, &str_type, &value);
    XtFree(res_name);
    
    /*
     * return
     */
    if(found)
	return (const char*)value.addr;
    else
	return (const char*)NULL;
}
#endif /* PRINTING_SUPPORTED */

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpSetValue
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
#if 0 && defined(PRINTING_SUPPORTED)
void
PdmXpSetValue(PdmXp* me,
	      XPAttributes type,
	      PdmOid id_att,
	      PdmOid id_val)
{
    PdmXpSetStringValue(me, type, id_att, PdmOidString(id_val));
}
#endif /* PRINTING_SUPPORTED */

/*
 * ------------------------------------------------------------------------
 * Name: PdmXpSetStringValue
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
#if 0 && defined(PRINTING_SUPPORTED)
void
PdmXpSetStringValue(PdmXp* me,
		    XPAttributes type,
		    PdmOid id_att,
		    const char* str_val)
{
    char* res_name;
   
    XrmDatabase pool;
    
    pool = PdmXpLoadPool(me, type);
    if(pool == (XrmDatabase)NULL)
	return;

    res_name = PdmXpBuildResourceName(me, id_att);
    XrmPutStringResource(&pool, res_name, (char*)str_val);
    XtFree(res_name);
}
#endif /* PRINTING_SUPPORTED */


/*
 * ------------------------------------------------------------------------
 * Name: PdmXpUpdateAttributes
 *
 * Description:
 *
 *     
 *
 * Return value:
 *
 *     
 *
 */
void
PdmXpUpdateAttributes(PdmXp* me)
{
#if 0 && defined(PRINTING_SUPPORTED)
    char fname[L_tmpnam];
    
    if(tmpnam(fname))
    {
	int i;
	XrmDatabase pool;
	XPAttributes type;
	FILE* fp;
	struct stat stbuf;
	int retlen;
	char* data = NULL;
	int data_size = 0;
	XTextProperty text_prop;

	for(i = 0; i < PDMXP_POOL_COUNT; i++)
	{
	    if(me->pool[i] != (XrmDatabase)NULL)
	    {
		switch(i)
		{
		case PDMXP_JOB:
		    type = XPJobAttr;
		    break;
		case PDMXP_DOC:
		    type = XPDocAttr;
		    break;
		default:
		    continue;
		}
		/*
		 * write out the attribute pool to a file Xrm DB
		 */
		XrmPutFileDatabase(me->pool[i], fname);
		/*
		 * open the new file Xrm DB
		 */
		if(fp = fopen(fname, "r"))
		{
		    /*
		     * read the file to create a string Xrm DB
		     */
		    fstat(fileno(fp), &stbuf);
		    if(stbuf.st_size + 1 > data_size)
		    {
			data_size = stbuf.st_size + 1;
			data = XtRealloc(data, data_size);
		    }
		    retlen = read(fileno(fp), data, stbuf.st_size);
		    fclose(fp);
		    unlink(fname);
		    data[retlen] = '\0';
		    /*
		     * convert to compund text
		     */
		    if(Success == 
		       XmbTextListToTextProperty(me->display,
						 &data, 1,
						 XCompoundTextStyle,
						 &text_prop))
		    {
			/*
			 * use the string Xrm DB to update the Xp server
			 */
			XpSetAttributes(me->display, me->context,
					type, (char*)text_prop.value,
					XPAttrMerge);
			if(text_prop.value)
			    XFree(text_prop.value);
		    }
		}
	    }

	}
	XtFree(data);
    }
#endif /* PRINTING_SUPPORTED */
}
