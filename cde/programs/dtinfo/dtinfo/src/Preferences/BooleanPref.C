/*
 * $TOG: BooleanPref.C /main/7 1998/04/21 11:21:44 mgreess $
 *
 * Copyright (c) 1992 HAL Computer Systems International, Ltd.
 * All rights reserved.  Unpublished -- rights reserved under
 * the Copyright Laws of the United States.  USE OF A COPYRIGHT
 * NOTICE IS PRECAUTIONARY ONLY AND DOES NOT IMPLY PUBLICATION
 * OR DISCLOSURE.
 * 
 * THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND TRADE
 * SECRETS OF HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.  USE,
 * DISCLOSURE, OR REPRODUCTION IS PROHIBITED WITHOUT THE
 * PRIOR EXPRESS WRITTEN PERMISSION OF HAL COMPUTER SYSTEMS
 * INTERNATIONAL, LTD.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject
 * to the restrictions as set forth in subparagraph (c)(l)(ii)
 * of the Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013.
 *
 *          HAL COMPUTER SYSTEMS INTERNATIONAL, LTD.
 *                  1315 Dell Avenue
 *                  Campbell, CA  95008
 * 
 */

#define C_List

#define C_TOC_Element
#define L_Basic

#define C_WindowSystem
#define L_Other

#define C_BooleanPref
#define L_Preferences


#include <string.h>

#if defined(AIX) || defined(USL)
/* For strcasecmp() */
#include <strings.h>
#endif
#include <Prelude.h>

void
BooleanPref::save()
{
  if (f_value == 0)
    strcpy (g_buffer, "false");
  else
    strcpy (g_buffer, "true");
  set_value (g_buffer);
}

void 
BooleanPref::restore()
{
  // If no data, fetch default from resource db.
  if (*(UserPreference::value()) == '\0')
    f_value = window_system().get_boolean_default (key());
  else
    {
      const char *s = UserPreference::value();
      if (strcasecmp ("true", s) == 0)
	f_value = 1;
      else
	f_value = 0;
    }
}
