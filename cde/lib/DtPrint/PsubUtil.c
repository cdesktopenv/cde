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
/* $XConsortium: PsubUtil.c /main/8 1996/10/31 02:09:44 cde-hp $ */
/*
 * DtPrint/PsubUtil.c
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
/*
 * ------------------------------------------------------------------------
 * Include Files
 *
 */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <Dt/DtNlUtils.h>

#include <Dt/PsubUtilI.h>

/*
 * ------------------------------------------------------------------------
 * Constant Definitions
 *
 */
/*
 * separator between printer name and display in an X Printer Specifier
 */
#define XPSPEC_NAME_DISP_SEP_CHAR '@'
#define XPSPEC_NAME_DISP_SEP "@"
#define XPSPEC_NAME_DISP_SEP_LEN 1
#define XPSPEC_DISP_SCREEN_SEP "."

/*
 * ------------------------------------------------------------------------
 * Static Function Declarations
 *
 */
static String* BuildStringList(
			       String list_string,
			       int i);
static XtEnum OpenXPrinterOnDisplay(
				    String printer_name,
				    String display_spec,
				    Display** new_display,
				    char** ct_printer_name);
static int SpanNonWhitespace(
			     char* string);
static int SpanWhitespace(
			  char* string);
static int StringToCompoundText(
				Display* display,
				char** compound_text,
				const char* string);
static Boolean TrimWhitespace(
			      String s);

/*
 * ------------------------------------------------------------------------
 * Name: BuildStringList
 *
 * Description:
 *
 *     Build a newly allocated array of Strings by recursively parsing
 *     whitespace delimited items out of the passed list String.
 *
 * Return value:
 *
 *     The array of strings. It is the caller's responsibility to free
 *     the returned list by calling _DtPrintFreeStringList.
 *
 */
static String*
BuildStringList(String list_string, int i)
{
    String string = NULL;
    /*
     * parse the next item out of the string list
     */
    if(list_string != (String)NULL)
    {
	int length;
	list_string += SpanWhitespace(list_string);
	length = SpanNonWhitespace(list_string);
	if(length != 0)
	{
	    string = XtCalloc(length+1, sizeof(char));
	    strncpy(string, list_string, length);
	    list_string += length;
	}
    }
    if(string == (String)NULL)
    {
	/*
	 * end of string list; allocate the array
	 */
	return (String*)XtCalloc(i+1, sizeof(String));
    }
    else
    {
	/*
	 * recurse
	 */
	String* string_list = BuildStringList(list_string, i+1);
	/*
	 * set the string in the list and return
	 */
	string_list[i] = string;
	return string_list;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: OpenXPrinterOnDisplay
 *
 * Description:
 *
 *     Opens 'printer_name' on display 'display_spec'. If successful, the
 *     passed 'new_display' is updated. This function does *not*
 *     initialize the print context.
 *
 *     The return parm 'ct_printer_name' is updated with the compound
 *     text version of the printer name used in establishing whether the
 *     printer is managed by the passed display. If conversion is not
 *     possible, 'printer_name' is used as is, and the location pointed
 *     to by 'ct_printer_name' is set to NULL. The 'ct_printer_name'
 *     return will also be set to NULL if the function return value is
 *     not DtPRINT_SUCCESS. If the location pointed to by
 *     'ct_printer_name' is updated to non-NULL, it is the caller's
 *     responsiblity to free the memory indicated by 'ct_printer_name'
 *     using XFree().
 *
 *     The 'ct_printer_name' return is provided as a convenience for the
 *     caller to subsequently pass to XpInitContext().
 *
 * Return value:
 *
 *     DtPRINT_SUCCESS
 *         An X printer connection was successfully opened.
 *
 *     DtPRINT_NO_PRINTER
 *         The display does not manage the indicated printer.
 *
 *     DtPRINT_NOT_XP_DISPLAY
 *         The display does not support printing.
 *
 *     DtPRINT_INVALID_DISPLAY
 *         The display could not be opened.
 *
 */
static XtEnum
OpenXPrinterOnDisplay(
		      String printer_name,
		      String display_spec,
		      Display** new_display,
		      char** ct_printer_name)
{
    Display* print_display;
    XPPrinterList printer_list;
    int error_base;
    int event_base;
    int printer_count;

    *ct_printer_name = (char*)NULL;
    /*
     * open the print display
     */
    print_display = XOpenDisplay(display_spec);
    if(print_display != (Display*)NULL)
    {
	if(XpQueryExtension(print_display, &event_base, &error_base))
	{
	    /*
	     * validate the printer
	     */
	    StringToCompoundText(print_display,
				 ct_printer_name,
				 printer_name);
	    if((char*)NULL == *ct_printer_name)
	    {
		printer_list =
		    XpGetPrinterList(print_display,
				     printer_name,
				     &printer_count);
	    }
	    else
	    {
		printer_list =
		    XpGetPrinterList(print_display,
				     *ct_printer_name,
				     &printer_count);
	    }
	    if(printer_list == (XPPrinterList)NULL)
	    {
		XCloseDisplay(print_display);
		if(*ct_printer_name)
		    XFree(*ct_printer_name);
		return DtPRINT_NO_PRINTER;
	    }
	    else
	    {
		*new_display = print_display;
		XpFreePrinterList(printer_list);
		return DtPRINT_SUCCESS;
	    }
	}
	else
	{
	    XCloseDisplay(print_display);
	    return DtPRINT_NOT_XP_DISPLAY;
	}
    }
    else
    {
	return DtPRINT_INVALID_DISPLAY;
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: SpanNonWhitespace
 *
 * Description:
 *
 *     Returns the length of the initial segment of the passed string
 *     that consists entirely of non-whitspace characters.
 *
 *
 */
static int
SpanNonWhitespace(char* string)
{
    char* ptr;
    for(ptr = string;
	*ptr != '\0' && !DtIsspace(ptr);
	ptr = DtNextChar(ptr));
    return ptr - string;
}

/*
 * ------------------------------------------------------------------------
 * Name: SpanWhitespace
 *
 * Description:
 *
 *     Returns the length of the initial segment of the passed string
 *     that consists entirely of whitespace characters.
 *
 *
 */
static int
SpanWhitespace(char* string)
{
    char* ptr;
    for(ptr = string;
	*ptr != '\0' && DtIsspace(ptr);
	ptr = DtNextChar(ptr));
    return ptr - string;
}

/*
 * ------------------------------------------------------------------------
 * Name: StringToCompoundText
 *
 * Description:
 *
 *     Converts a string to compund text for use with Xp.
 *
 * Return Value:
 *
 *     The value returned from XmbTextListToTextProperty; e.g. Success if
 *     successful.
 *
 *     The return parm compound_text will contain a pointer to the
 *     converted text. It is the caller's responsibility to free this
 *     string using XFree().
 *
 */
static int
StringToCompoundText(
		     Display* display,
		     char** compound_text,
		     const char* string)
{
    XTextProperty text_prop;
    int status;
    
    status = XmbTextListToTextProperty(display,
				       (char**)&string, 1,
				       XCompoundTextStyle,
				       &text_prop);
    if(Success == status)
	*compound_text = (char*)text_prop.value;
    else
	*compound_text = (char*)NULL;

    return status;
}

/*
 * ------------------------------------------------------------------------
 * Name: TrimWhitespace
 *
 * Description:
 *
 *     Remove leading and trailing whitespace from the passed string.
 *     This function is multi-byte safe.
 *
 * Return value:
 *
 *     True if the string was modified; False otherwise.
 *
 */
static Boolean
TrimWhitespace(String string)
{
    String ptr;
    int i;
    String last_non_ws;
    Boolean modified = False;

    if((String)NULL == string)
	return modified;
    /*
     * find the first non-whitespace character
     */
    for(ptr = string; *ptr != '\0' && DtIsspace(ptr); ptr = DtNextChar(ptr));
    /*
     * reposition the string
     */
    if(ptr != string)
    {
	modified = True;
	for(i = 0; ptr[i] != '\0'; i++)
	    string[i] = ptr[i];
	string[i] = '\0';
    }
    /*
     * find the last non-whitespace character
     */
    for(ptr = string, last_non_ws = NULL; *ptr != '\0'; ptr = DtNextChar(ptr))
	if(!DtIsspace(ptr))
	    last_non_ws = ptr;
    /*
     * trim any trailing whitespace
     */
    if((String)NULL != last_non_ws)
    {
	ptr = DtNextChar(last_non_ws);
	if(*ptr != '\0')
	{
	    modified = True;
	    *ptr = '\0';
	}
    }
    return modified;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintCreateXPrinterSpecifier
 *
 * Description:
 *
 *     Concatinates the passed X printer specifier components into a
 *     newly allocated String, inserting separators between components as
 *     needed. All components are optional; i.e. the String parms may be
 *     passed as NULL, and the int parms may be passed as -1.
 *
 *     Separators are inserted as follows:
 *
 *         * if both the 'printer_name' and 'host_name' are specified, a
 *           "@" will be inserted between them.
 *
 *         * if the 'display_num' is specified:
 *
 *             - if 'spec_net' is TCP_IPC or UNSPECIFIED, a ":" will be
 *               inserted immediately preceding it.
 *
 *             - if 'spec_net' is DEC_NET, a "::" will be inserted
 *               immediately preceding it.
 *           
 *         * if the 'screen_num' is specified, a "." will be inserted
 *           immediately preceding it.
 *
 * Return value:
 *
 *     A newly allocated X printer specifier. If all of the components
 *     are omitted, an empty string ("") will be returned. It is the
 *     responsibility of the caller to free the returned specifier by
 *     calling XtFree.
 */
String
_DtPrintCreateXPrinterSpecifier(
				String printer_name,
				String host_name,
				DtPrintSpecNet spec_net,
				int display_num,
				int screen_num)
{
    String printer_specifier;
    char display_string[32];
    char screen_string[32];
    int printer_name_len;
    int host_name_len;
    int specifier_len = 0;
    String separator;
    /*
     * printer name length
     */
    if(printer_name != (String)NULL)
	specifier_len += printer_name_len = strlen(printer_name);
    else
	printer_name_len = 0;
    /*
     * host name length
     */
    if(host_name != (String)NULL)
	specifier_len += host_name_len = strlen(host_name);
    else
	host_name_len = 0;
    /*
     * printer name / display separator length
     */
    if(printer_name_len != 0 && (host_name_len != 0 || display_num != -1))
    {
	separator = XPSPEC_NAME_DISP_SEP;
	specifier_len += XPSPEC_NAME_DISP_SEP_LEN;
    }
    else
	separator = (String)NULL;
    /*
     * display number
     */
    if(display_num == -1)
	display_string[0] = '\0';
    else
	specifier_len +=
	    sprintf(display_string, "%s%d",
		    spec_net == DtPRINT_DEC_NET ? "::" : ":",
		    display_num);
    /*
     * screen number
     */
    if(screen_num == -1)
	screen_string[0] = '\0';
    else
	specifier_len +=
	    sprintf(screen_string, "%s%d", XPSPEC_DISP_SCREEN_SEP, screen_num);
    /*
     * create and return the new printer specifier
     */
    printer_specifier =	XtMalloc(specifier_len + 1);
    sprintf(printer_specifier,
	    "%s%s%s%s%s",
	    printer_name ? printer_name : "",
	    separator ? separator : "",
	    host_name ? host_name : "",
	    display_string,
	    screen_string);
    return printer_specifier;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintFreeStringList
 *
 * Description:
 *
 *     Frees a string list created by BuildStringList.
 *
 * Return value:
 *
 *     None.
 *
 */
void
_DtPrintFreeStringList(
			 String* server_list)
{
    if(server_list)
    {
	int i;
	for(i = 0; server_list[i] != (String)NULL; i++)
	{
	    XtFree(server_list[i]);
	}
	XtFree((char*)server_list);
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintGetDefaultXPrinterName
 *
 * Description:
 *
 *
 * Return Value:
 *
 *     The default printer name, or NULL if no default could be
 *     determined. It is the responsibility of the caller to free the
 *     memory allocated for the returned String by calling XtFree.
 *
 */
String
_DtPrintGetDefaultXPrinterName(
			       Widget w)
{
    String default_printer;

    if((Widget)NULL == w)
    {
	default_printer = (String)NULL;
    }
    else
    {
	XtResource res_struct;
	/*
	 * initialize the resource structure
	 */
	res_struct.resource_name = "xpPrinter";
	res_struct.resource_class = "XpPrinter";
	res_struct.resource_type =  XmRString;
	res_struct.resource_size = sizeof(String);
	res_struct.resource_offset = 0;
	res_struct.default_type = XmRImmediate;
	res_struct.default_addr = (XtPointer)NULL;
	/*
	 * pick up the printer list application resource value for the
	 * passed widget
	 */
	XtGetApplicationResources(w, (XtPointer)&default_printer,
				  &res_struct, 1, (ArgList)NULL, 0);
    }
    /*
     * if the resource is undefined, search for an appropriate
     * environment variable
     */
    if(default_printer != (String)NULL);
    else if((default_printer = getenv("XPRINTER")) != (String)NULL);
    else if((default_printer = getenv("PDPRINTER")) != (String)NULL);
    else if((default_printer = getenv("LPDEST")) != (String)NULL);
    else if((default_printer = getenv("PRINTER")) != (String)NULL);
    /*
     * return a copy of the printer name
     */
    return XtNewString(default_printer);
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintGetXpPrinterList
 *
 * Description:
 *
 *     Retrieves the short list of Printers from the XpPrinterList
 *     resource, or XPPRINTERLIST environment variable.
 *
 * Return value:
 *
 *     A newly allocated array of printer name Strings. It is the caller's
 *     responsibility to free the returned array by calling
 *     _DtPrintFreeStringList.
 *
 */
String*
_DtPrintGetXpPrinterList(
			 Widget w)
{
    XtResource res_struct;
    String xp_printer_list;
    /*
     * initialize the resource structure
     */
    res_struct.resource_name = "xpPrinterList";
    res_struct.resource_class = "XpPrinterList";
    res_struct.resource_type =  XmRString;
    res_struct.resource_size = sizeof(String);
    res_struct.resource_offset = 0;
    res_struct.default_type = XmRImmediate;
    res_struct.default_addr = (XtPointer)NULL;
    /*
     * pick up the printer list application resource value for the passed
     * widget
     */
    XtGetApplicationResources(w, (XtPointer)&xp_printer_list,
			      &res_struct, 1, (ArgList)NULL, 0);
    /*
     * if the resource is undefined, use the environment variable
     */
    if(xp_printer_list == (String)NULL)
    {
	xp_printer_list = getenv("XPRINTERLIST");
    }
    /*
     * build the array of printer names
     */
    if(xp_printer_list != (String)NULL)
    {
	return BuildStringList(xp_printer_list, 0);
    }
    else
	return (String*)NULL;
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintGetXpServerList
 *
 * Description:
 *
 *     Retrieves the Xp Server list from the XpServerList resource, or if
 *     the resource is undefined, the XPSERVERLIST environment variable
 *     is used.
 *
 *     Each server name in the return list will be of the form
 *     "host:display". If any entry from XpServerList does not contain a
 *     display number, a default of 0 will be used.
 *
 * Return value:
 *
 *     A newly allocated array of server name Strings. It is the caller's
 *     responsibility to free the returned array by calling
 *     _DtPrintFreeStringList.
 *
 */
String*
_DtPrintGetXpServerList(
			Widget w)
{
    XtResource res_struct;
    String xp_server_list;
    int error_base;
    int event_base;
    String* server_list;
    int i;
    /*
     * initialize the resource structure
     */
    res_struct.resource_name = "xpServerList";
    res_struct.resource_class = "XpServerList";
    res_struct.resource_type =  XmRString;
    res_struct.resource_size = sizeof(String);
    res_struct.resource_offset = 0;
    res_struct.default_type = XmRImmediate;
    res_struct.default_addr = (XtPointer)NULL;
    /*
     * pick up the server list application resource value for the passed
     * widget
     */
    if((Widget)NULL == w)
	xp_server_list = (String)NULL;
    else
	XtGetApplicationResources(w, (XtPointer)&xp_server_list,
				  &res_struct, 1, (ArgList)NULL, 0);
    /*
     * if the resource is undefined, use the environment variable value
     */
    if(xp_server_list == (String)NULL)
    {
	xp_server_list = getenv("XPSERVERLIST");
    }
    /*
     * convert to a list of strings
     */
    if((Widget)NULL != w
       &&
       XpQueryExtension(XtDisplay(w), &event_base, &error_base))
    {
	/*
	 * the video server supports the Xp extension, add it to the front
	 * of the list.
	 */
	server_list = BuildStringList(xp_server_list, 1);
	server_list[0] = XtNewString(XDisplayString(XtDisplay(w)));
    }
    else if(xp_server_list != (String)NULL)
    {
	server_list = BuildStringList(xp_server_list, 0);
    }
    else
	server_list = (String*)NULL;
    /*
     * default the display number to ":0" if needed
     */
    for(i = 0; server_list && server_list[i]; i++)
    {
	String host_name;
	int display_num;
	/*
	 * check to see if display number is specified
	 */
	_DtPrintParseXDisplaySpecifier(server_list[i],
				       &host_name,
				       (DtPrintSpecNet*)NULL,
				       &display_num,
				       (int*)NULL);
	if(display_num == -1)
	{
	    /*
	     * display number not specified; default to ":0"
	     */
	    XtFree(server_list[i]);
	    server_list[i] =
		_DtPrintCreateXPrinterSpecifier((String)NULL, host_name,
						DtPRINT_TCP_IPC, 0, -1);
	}
	XtFree(host_name);
    }
    /*
     * return
     */
    return server_list;
}


/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintParseXDisplaySpecifier
 *
 * Description:
 *
 *     Parse the host name and the display and screen numbers from a
 *     conventional X Display Specifier (e.g. as in the DISPLAY env var).
 *
 *     This function returns the component values into the locations
 *     pointed to by the host_name, display_num, and screen_num
 *     parameters. If any component is not desired, the corresponding
 *     parm may be set to NULL. If a non-NULL host_name is passed, it is
 *     the responsibility of the caller to free the newly allocated
 *     String set in this parm by calling XtFree().
 *
 *     If the hostname component is missing from the passed display spec,
 *     an empty string is returned. If the display spec is NULL, NULL is
 *     returned. (-1) will be returned for the display or screen number
 *     if the display spec is NULL or if the display or screen component
 *     is missing from the spec.
 *
 * Return value:
 *
 *     None.
 *
 */
void
_DtPrintParseXDisplaySpecifier(
			       const String display_spec,
			       String* host_name,
			       DtPrintSpecNet* spec_net,
			       int* display_num,
			       int* screen_num)
{
    char* ptr;

    if(display_spec == (String)NULL)
    {
	/*
	 * not much to do with a NULL display spec
	 */
	if(host_name) *host_name = (String)NULL;
	if(spec_net) *spec_net = DtPRINT_NET_UNSPECIFIED;
	if(display_num) *display_num = -1;
	if(screen_num) *screen_num = -1;
	return;
    }
    /*
     * find the start of the display number in the display spec
     */
    ptr = DtStrchr(display_spec, ':');
    if(ptr == (char*)NULL)
    {
	/*
	 * not found, return -1 for display and screen
	 */
	if(spec_net) *spec_net = DtPRINT_NET_UNSPECIFIED;
	if(display_num) *display_num = -1;
	if(screen_num) *screen_num = -1;
	/*
	 * return the host name as a copy of the display spec
	 */
	if(host_name) *host_name = XtNewString(display_spec);
    }
    else
    {
	int num;
	/*
	 * skip over the ':', determine if this is a DECnet specifier,
	 * and pick up the display num if specified
	 */
	++ptr;
	if(*ptr == '\0')
	{
	    if(spec_net) *spec_net = DtPRINT_NET_UNSPECIFIED;
	    num = -1;
	}
	else
	{
	    if(*ptr == ':')
	    {
		if(spec_net) *spec_net = DtPRINT_DEC_NET;
		++ptr;
	    }
	    else
	    {
		if(spec_net) *spec_net = DtPRINT_TCP_IPC;
	    }
	    if(*ptr == '\0')
		num = -1;
	    else
		num = (int)strtol(ptr, &ptr, 10);
	}
	if(display_num) *display_num = num;
	if(screen_num)
	{
	    if(num == -1)
	    {
		*screen_num = -1;
	    }
	    else
	    {
		/*
		 * parse out the screen number
		 */
		if(*ptr == '.' && *(ptr+1) != '\0')
		{
		    ++ptr;
		    num = (int)strtol(ptr, &ptr, 10);
		    if(screen_num) *screen_num = num;
		}
		else
		{
		    /*
		     * not found, return -1 for screen
		     */
		    *screen_num = -1;
		}
	    }
	}
	if(host_name)
	{
	    /*
	     * allocate a new string containing just the host name
	     */
	    int host_name_len = DtStrcspn(display_spec, ":");
	    *host_name = XtMalloc(host_name_len+1);
	    strncpy(*host_name, display_spec, host_name_len);
	    (*host_name)[host_name_len] = '\0';
	}
    }
}


/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintParseXPrinterSpecifier
 *
 * Description:
 *
 *     Parse the printer name and display specifier components out of an
 *     X Printer Specifier. This function returns these components as
 *     newly allocated Strings into the locations pointed to by
 *     printer_name and display_spec. It is the responsibility of the
 *     caller to free the Strings by calling XtFree().
 *
 *     The printer_name or display_spec parameters may be passed as NULL
 *     if that component of the specifier is not desired.
 *
 *     If the printer specifier is NULL, the locations pointed to
 *     by the printer_name and display_spec will be set to NULL.
 *
 *     If either portion of the specifier is missing, a newly allocated
 *     empty string will be returned to printer_name or display_spec.
 *
 * Return value:
 *
 *     None.
 *
 */
void
_DtPrintParseXPrinterSpecifier(
			       const String specifier,
			       String* printer_name,
			       String* display_spec)
{
    if(specifier == (String)NULL)
    {
	if(printer_name) *printer_name = (String)NULL;
	if(display_spec) *display_spec = (String)NULL;
    }
    else
    {
	String delim_ptr;
	/*
	 * determine the offset of the printer name / display name delimiter
	 * ('@') within the X Printer Specifier
	 */
	delim_ptr = DtStrchr(specifier, XPSPEC_NAME_DISP_SEP_CHAR);
	if(delim_ptr == (String)NULL)
	{
	    /*
	     * no delimiter found; specifier consists of printer name only
	     */
	    if(printer_name) *printer_name = XtNewString(specifier);
	    if(display_spec) *display_spec = XtNewString("");
	}
	else
	{
	    /*
	     * copy the printer name portion from the specifier
	     */
	    if(printer_name)
	    {
		int printer_name_len = delim_ptr - specifier;
		*printer_name = (String)XtMalloc(printer_name_len + 1);
		strncpy(*printer_name, specifier, printer_name_len);
		(*printer_name)[printer_name_len] = '\0';
	    }
	    /*
	     * copy the display name portion from the specifier
	     */
	    if(display_spec) *display_spec = XtNewString(delim_ptr+1);
	}
    }
}

/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintVerifyXPrinter
 *
 * Description:
 *
 *     Determines if an X printer specifier is valid by establishing a
 *     connection to the printer, up to and including initializing a
 *     print context.
 *
 *     If the passed printer specifier is NULL, a default will be
 *     used. If the passed specifier is incomplete, this function will
 *     attempt to determine a fully qualified specifier based on the Xp
 *     server list environment variable or XRM resource. If the specifier
 *     does not include a display number, ":0" will be used.
 *
 *     If this function is successful, the 'new_display' return parameter
 *     will be set to the Display of the verified printer connection. If
 *     the passed 'printer_spec' was used as passed to establish the
 *     connection, the 'new_printer' parm will be set to NULL. If a
 *     default or fully-qualified version of the printer specifier was
 *     generated, the generated specifier will be returned via the
 *     'new_printer' parameter. It is the responsibility of the caller to
 *     free the generated String by calling XtFree. 'new_printer_spec'
 *     may be set whether or not _DtPrintVerifyXPrinter successfully
 *     opens an X printer connection.
 *
 * Return value:
 *
 *     DtPRINT_SUCCESS
 *         An X printer connection was successfully opened.
 *
 *     DtPRINT_PRINTER_MISSING
 *         The passed or default printer spec does not include a printer
 *         name component.
 *
 *     DtPRINT_NO_DEFAULT
 *         The passed printer spec was NULL, and no default printer could
 *         be determined.
 *
 *     DtPRINT_NO_DEFAULT_DISPLAY
 *         The passed printer spec or default did not include a display
 *         specifier, and no suitable display could be found within the
 *         Xp server list.
 *
 *     DtPRINT_NO_PRINTER
 *         The display indicated in the passed printer spec or default
 *         does not manage the indicated printer.
 *
 *     DtPRINT_NOT_XP_DISPLAY
 *         The display indicated in the passed printer spec or default
 *         does not support printing.
 *
 *     DtPRINT_INVALID_DISPLAY
 *         The display indicated in the passed printer spec or default
 *         could not be opened.
 *
 */
XtEnum
_DtPrintVerifyXPrinter(
		       Widget w,
		       String printer_spec,
		       String* new_printer_spec,
		       Display** new_display,
		       XPContext* new_context)
{
    String default_printer;
    String printer_name;
    String display_spec;
    XtEnum status;
    Display* print_display;
    char* ct_printer_name;
    String trimmed_spec;
    /*
     * initialize the printer spec return parm
     */
    *new_printer_spec = (String)NULL;
    /*
     * determine a default printer if the passed printer spec is NULL
     */
    if(printer_spec == (String)NULL)
    {
	default_printer = _DtPrintGetDefaultXPrinterName(w);
	if(default_printer == (String)NULL)
	    return DtPRINT_NO_DEFAULT;
	else
	    printer_spec = default_printer;
    }
    else
	default_printer = (String)NULL;
    /*
     * trim whitespace from the printer spec if needed
     */
    trimmed_spec = XtNewString(printer_spec);
    if(TrimWhitespace(trimmed_spec))
    {
	printer_spec = trimmed_spec;
    }
    else
    {
	XtFree(trimmed_spec);
	trimmed_spec = (String)NULL;
    }
    /*
     * break the printer specifier into its printer name and display
     * specifier components
     */
    _DtPrintParseXPrinterSpecifier(printer_spec,
				   &printer_name,
				   &display_spec);
    if(*printer_name == '\0')
    {
	/*
	 * printer name is missing
	 */
	status = DtPRINT_PRINTER_MISSING;
    }
    else
    {
	/*
	 * if the display spec is empty, search the server list for a
	 * suitable display
	 */
	if(*display_spec == '\0')
	{
	    String* server_list;
	    int i;
	    /*
	     * find a server in the server list that manages the printer
	     */
	    status = DtPRINT_NO_DEFAULT_DISPLAY;
	    if((server_list = _DtPrintGetXpServerList(w)) != (String*)NULL)
	    {
		for(i = 0; server_list[i] != (String)NULL; i++)
		{
		    if(OpenXPrinterOnDisplay(printer_name,
					     server_list[i],
					     &print_display,
					     &ct_printer_name)
		       == DtPRINT_SUCCESS)
		    {
			status = DtPRINT_SUCCESS;
			*new_printer_spec =
			    _DtPrintCreateXPrinterSpecifier(
						    printer_name,
						    server_list[i],
						    DtPRINT_NET_UNSPECIFIED,
						    -1, -1);
			break;
		    }
		}
		_DtPrintFreeStringList(server_list);
	    }
	}
	else
	{
	    String host_name;
	    int display_num;
	    /*
	     * check to see if display number is specified
	     */
	    _DtPrintParseXDisplaySpecifier(display_spec,
					   &host_name,
					   (DtPrintSpecNet*)NULL,
					   &display_num,
					   (int*)NULL);
	    if(display_num == -1)
	    {
		String new_display_spec;
		/*
		 * display number not specified; default to ":0"
		 */
		new_display_spec =
		    _DtPrintCreateXPrinterSpecifier((String)NULL, host_name,
						    DtPRINT_TCP_IPC, 0, -1);
		/*
		 * create new printer name for return, even if
		 * OpenXPrinterOnDisplay is unsuccessful
		 */
		*new_printer_spec =
		    _DtPrintCreateXPrinterSpecifier(printer_name,
						    new_display_spec,
						    DtPRINT_NET_UNSPECIFIED,
						    -1, -1);
		/*
		 * use the new display spec
		 */
		XtFree(display_spec);
		display_spec = new_display_spec;
	    }
	    XtFree(host_name);
	    /*
	     * open the print display
	     */
	    status = OpenXPrinterOnDisplay(printer_name,
					   display_spec,
					   &print_display,
					   &ct_printer_name);
	}
    }
    if(status == DtPRINT_SUCCESS)
    {
	/*
	 * initialize the print context
	 */
	if((char*)NULL != ct_printer_name)
	{
	    *new_context = XpCreateContext(print_display, ct_printer_name);
	    XFree(ct_printer_name);
	}
	else
	    *new_context = XpCreateContext(print_display, printer_name);
	XpSetContext(print_display, *new_context);
	/*
	 * update the display return parm
	 */
	*new_display = print_display;
    }
    /*
     * check to see if the trimmed spec was used
     */
    if(trimmed_spec != (String)NULL)
    {
	if(*new_printer_spec == (String)NULL)
	{
	    /*
	     * the trimmed spec was used as is; return it as the new
	     * printer specifier
	     */
	    *new_printer_spec = trimmed_spec;
	}
	else
	{
	    /*
	     * a modified version of the trimmed spec was used
	     */
	    XtFree(trimmed_spec);
	}
	XtFree(default_printer);
    }
    else if(default_printer != (String)NULL)
    {
	/*
	 * check to see if the default printer was used without
	 * modification
	 */
	if(*new_printer_spec == (String)NULL)
	{
	    /*
	     * the default printer was used as is; return it as the new
	     * printer specifier
	     */
	    *new_printer_spec = default_printer;
	}
	else
	{
	    /*
	     * a modified version of the default printer was used
	     */
	    XtFree(default_printer);
	}
    }
    /*
     * clean up and return
     */
    XtFree(printer_name);
    XtFree(display_spec);
    return status;
}
