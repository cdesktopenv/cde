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
/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: PrefMgr.cc /main/5 1996/06/11 16:27:12 cde-hal $
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

#define C_UserPreference
#define C_IntegerPref
#define C_BooleanPref
#define C_WindowGeometryPref
#define C_StringPref
#define L_Preferences

#define C_PrefAgent
#define L_Agents

#define C_PrefMgr
#define C_MessageMgr
#define L_Managers

#include <Prelude.h>

#ifndef ORIGINAL
LONG_LIVED_CC(PrefMgr,pref_mgr);
#else
PrefMgr *PrefMgr::g_pref_mgr;
PrefMgr &
pref_mgr()
{
  message_mgr().warning_dialog ("PrefMgr access detected.");
  if (PrefMgr::g_pref_mgr == NULL)
    PrefMgr::g_pref_mgr = new PrefMgr();
  return (*PrefMgr::g_pref_mgr);
}
#endif

#if defined(__STDC__) || defined(hpux) || defined(__osf__)
# define DEFSYM(S) PrefSymbol PrefMgr::S = (PrefSymbol) #S
#else
# define DEFSYM(S) PrefSymbol PrefMgr::S = (PrefSymbol) "S"
#endif

DEFSYM (BrowseGeometry);
DEFSYM (FontScale);
DEFSYM (BrowseLock);
DEFSYM (MapGeometry);
DEFSYM (MapAutoUpdate);
DEFSYM (NodeHistSize);
DEFSYM (SearchHistSize);
DEFSYM (MaxSearchHits);
DEFSYM (DefaultMarkBase);
DEFSYM (DisplayFirstHit);
DEFSYM (AutomaticHelp);
#ifdef UseWideChars
// preferences for Japanese hyphenation - 4/6/94 kamiya
DEFSYM (HyphenMethod);
DEFSYM (HyphenLevel);
#endif


PrefMgr::~PrefMgr()
{
  delete f_last_preference;
}

void
PrefMgr::display()
{
  if (f_pref_agent == NULL)
    f_pref_agent = new PrefAgent();

  f_pref_agent->display();
}


void
PrefMgr::sync()
{
  mtry
    {
      UserPreference::flush_preferences();
    }
  mcatch_any()
    {
      message_mgr().error_dialog ((char*)"Unable to save preferences.");
    }
  end_try;
}


int
PrefMgr::get_int (const PrefSymbol sym)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new IntegerPref (sym);
    }

  return (((IntegerPref *) f_last_preference)->value());
}

void
PrefMgr::set_int (const PrefSymbol sym, int value)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new IntegerPref (sym);
    }

  ((IntegerPref *) f_last_preference)->value (value);
  notify (INT_CHANGED);
}


bool
PrefMgr::get_boolean (const PrefSymbol sym)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new BooleanPref (sym);
    }

  return (((BooleanPref *) f_last_preference)->value());
}

void
PrefMgr::set_boolean (const PrefSymbol sym, bool value)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new BooleanPref (sym);
    }

  ((BooleanPref *) f_last_preference)->value (value);
  notify (BOOLEAN_CHANGED);
}


const char *
PrefMgr::get_string (const PrefSymbol sym)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new StringPref (sym);
    }

  return (((StringPref *) f_last_preference)->value());
}

void
PrefMgr::set_string (const PrefSymbol sym, const char *value)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new StringPref (sym);
    }

  ((StringPref *) f_last_preference)->value (value);
  notify (STRING_CHANGED);
}


const WindowGeometry &
PrefMgr::get_geometry (const PrefSymbol sym)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new WindowGeometryPref (sym);
    }

  return (((WindowGeometryPref *) f_last_preference)->value());
}

void
PrefMgr::set_geometry (const PrefSymbol sym, const WindowGeometry &value)
{
  if (f_last_symbol != sym)
    {
      f_last_symbol = sym;
      delete f_last_preference;
      f_last_preference = new WindowGeometryPref (sym);
    }

  ((WindowGeometryPref *) f_last_preference)->value (value);
  notify (GEOMETRY_CHANGED);
}
