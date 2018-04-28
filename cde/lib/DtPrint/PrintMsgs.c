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
/* $TOG: PrintMsgs.c /main/14 1999/10/14 13:54:53 mgreess $ */
/*
 * DtPrint/PrintMsgs.c
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

#ifdef I18N_MSG

#include <stdio.h>
#include <nl_types.h>
#include <X11/Intrinsic.h>
#include <Dt/PrintI.h>

#if !defined(NL_CAT_LOCALE)
#define NL_CAT_LOCALE 0
#endif 

#define _DTPRINT_CAT_NAME "DtPrint"

#endif /* I18N_MSG */

/*
 * ------------------------------------------------------------------------
 * PrintSetupB module messages
 */
const char _DtPrMsgPrintSetupBox_0000[] =
   "Invalid workAreaLocation resource setting.";
const char _DtPrMsgPrintSetupBox_0001[] =
   "More Printers...";
const char _DtPrMsgPrintSetupBox_0002[] =
   "Print to File";
const char _DtPrMsgPrintSetupBox_0003[] =
   "Select File...";
const char _DtPrMsgPrintSetupBox_0004[] =
   "Cancel";
const char _DtPrMsgPrintSetupBox_0005[] =
   "Copies:";
const char _DtPrMsgPrintSetupBox_0006[] =
   "Help";
const char _DtPrMsgPrintSetupBox_0007[] =
   "Print";
const char _DtPrMsgPrintSetupBox_0008[] =
   "Printer Name:";
const char _DtPrMsgPrintSetupBox_0009[] =
   "Setup...";
const char _DtPrMsgPrintSetupBox_0010[] =
   "Printer Description:";
const char _DtPrMsgPrintSetupBox_0011[] =
   "Copy count out of range.";
const char _DtPrMsgPrintSetupBox_0012[] =
   "Invalid print setup mode.";
const char _DtPrMsgPrintSetupBox_0013[] =
   "Print setup mode cannot be changed.";
const char _DtPrMsgPrintSetupBox_0014[] =
   "Printer Info...";
const char _DtPrMsgPrintSetupBox_0015[] =
   "Invalid Printer";
const char _DtPrMsgPrintSetupBox_0016[] =
   "Printer %s not found on X print server %s";
const char _DtPrMsgPrintSetupBox_0017[] =
   "Unable to connect to X print server %s";
const char _DtPrMsgPrintSetupBox_0018[] =
   "X server %s does not support printing";
const char _DtPrMsgPrintSetupBox_0019[] =
   "A default printer could not be determined";
const char _DtPrMsgPrintSetupBox_0020[] =
   "An X print server for %s could not be found";
const char _DtPrMsgPrintSetupBox_0021[] =
   "Printer name missing from specifier: %s";
const char _DtPrMsgPrintSetupBox_0022[] =
   "Invalid print destination.";
const char _DtPrMsgPrintSetupBox_0023[] =
   "Unable to convert from compound text.";
const char _DtPrMsgPrintSetupBox_0024[] =
   "Arguments not needed for resource conversion.";
const char _DtPrMsgPrintSetupBox_0025[] =
   "Select Printer Error";
const char _DtPrMsgPrintSetupBox_0026[] =
   "Unable to find any printers.";
const char _DtPrMsgPrintSetupBox_0027[] =
   "description unavailable";
const char _DtPrMsgPrintSetupBox_0028[] =
   "Select File";
const char _DtPrMsgPrintSetupBox_0029[] =
   "Printer Information";
const char _DtPrMsgPrintSetupBox_0030[] =
   "Format:";
const char _DtPrMsgPrintSetupBox_0031[] =
   "Printer Model:";
const char _DtPrMsgPrintSetupBox_0032[] =
   "Printers:";
const char _DtPrMsgPrintSetupBox_0033[] =
   "More Printers";
const char _DtPrMsgPrintSetupBox_0034[] =
   "Print to Printer";
const char _DtPrMsgPrintSetupBox_0035[] =
   "File Name:";
const char _DtPrMsgPrintSetupBox_0036[] =
   "Print Setup - Help";



/*
 * ------------------------------------------------------------------------
 * _DtPrint Frame widget messages
 */
const char _DtPrMsgPrintOption_0000[] = "Margins";
const char _DtPrMsgPrintOption_0001[] = "Top:";
const char _DtPrMsgPrintOption_0002[] = "Right:";
const char _DtPrMsgPrintOption_0003[] = "Bottom:";
const char _DtPrMsgPrintOption_0004[] = "Left:";
 
const char _DtPrMsgPrintOption_0005[] = "Headers and Footers";
const char _DtPrMsgPrintOption_0006[] = "Top Left:";
const char _DtPrMsgPrintOption_0007[] = "Top Right:";
const char _DtPrMsgPrintOption_0008[] = "Bottom Left:";
const char _DtPrMsgPrintOption_0009[] = "Bottom Right:";
 
/*
 * ------------------------------------------------------------------------
 * Dt Print Dialog Manager module messages
 */
const char _DtPrMsgPrintDlgMgr_0000[] = "Send Mail When Done";
const char _DtPrMsgPrintDlgMgr_0001[] = "Banner Page Title:";
const char _DtPrMsgPrintDlgMgr_0002[] = "Print Command Options:";

#if defined(I18N_MSG)

#if defined(hpV4)
/*
 * Wrapper around catgets -- this makes sure the message string is saved
 * in a safe location; so repeated calls to catgets() do not overwrite
 * the catgets() internal buffer.  This has been a problem on HP systems.
 */
static char *catgets_cached(nl_catd catd, int set, int num, char *dflt)
{
#define INITIAL_NMSGS_PER_SET	300
#define INITIAL_NSETS		50

  /* array to hold messages from catalog */
  static char	***cached_msgs = NULL;
  static int	nmsgs_per_set = INITIAL_NMSGS_PER_SET;
  static int	nsets = INITIAL_NSETS;
  
  char		**setptr;
  int		i, multiplier;
  Cardinal	size;

  char* message;

  /* convert to a zero based index */
  int		setIdx = set - 1;
  int		numIdx = num - 1;

  _DtPrintProcessLock();
  
  if (NULL == cached_msgs)
  {
      size = sizeof(char**) * nsets;
      cached_msgs = (char***) XtMalloc(size);
      memset((char*) cached_msgs, 0, size);
  }
  else if (setIdx >= nsets)
  {
      for (multiplier=2; setIdx > multiplier*nsets; multiplier++) {}
      size = sizeof(char**) * nsets;
      cached_msgs = (char***) XtRealloc((char*) cached_msgs, multiplier * size);
      memset((char*) (cached_msgs + size), 0, multiplier * size);
      nsets *= multiplier;
  }

  if (NULL == cached_msgs[setIdx])
  {
      size = sizeof(char*) * nmsgs_per_set;
      cached_msgs[setIdx] = (char**) XtMalloc(size);
      memset((char*) cached_msgs[setIdx], 0, size);
  }
  else if (numIdx >= nmsgs_per_set)
  {
      for (multiplier=2; numIdx > multiplier*nsets; multiplier++) {}
      size = sizeof(char*) * nmsgs_per_set;

      for (i=0; i<nmsgs_per_set; i++)
      {
	  if (NULL != cached_msgs[i])
	  {
              cached_msgs[i] =
		(char**) XtRealloc((char*) cached_msgs[i], multiplier * size);
              memset((char*) (cached_msgs[i] + size), 0, multiplier * size);
	  }
      }
      nmsgs_per_set *= multiplier;
  }

  setptr = cached_msgs[setIdx];
  if (NULL == setptr[numIdx])
    setptr[numIdx] = strdup(catgets(catd, set, num, dflt));
  
  message = setptr[numIdx];

  _DtPrintProcessUnlock();

  return message;
}
#endif /* hpV4 */


/*
 * ------------------------------------------------------------------------
 * Name: _DtPrintGetMessage
 *
 * Parameters:
 *
 *   int	set -		The message catalog set number.
 *
 *   int	n - 		The message number.
 *
 *   char	*s -		The default message if the message is not
 * 				retrieved from a message catalog.
 *
 * Returns: the string for set 'set' and number 'n'.
 */
char *
_DtPrintGetMessage(
		   int set,
		   int n,
		   char * s)
{
        char *msg;
	nl_catd catopen();
	char *catgets();
	static int first = 1;
	static nl_catd nlmsg_fd;

	if(first)
	{
	    _DtPrintProcessLock();
	    if(first) 
	    {
		first = 0;
		nlmsg_fd = catopen(_DTPRINT_CAT_NAME, NL_CAT_LOCALE);
	    }
	    _DtPrintProcessUnlock();
	}
#if defined(hpV4)
	msg=catgets_cached(nlmsg_fd,set,n,s);
#else
	msg=catgets(nlmsg_fd,set,n,s);
#endif
	return (msg);

}
#endif /* I18N_MSG */
