/*	Copyright (c) 1994 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: PrefMgr.hh /main/3 1996/06/11 16:27:17 cde-hal $
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

class PrefAgent;
class IntegerPref;
class BooleanPref;
class WindowGeometryPref;
class WindowGeometry;
class UserPreference;

typedef char *PrefSymbol;

class PrefMgr : public Long_Lived
{
public:
  static PrefSymbol BrowseGeometry;
  static PrefSymbol FontScale;
  static PrefSymbol BrowseLock;
  static PrefSymbol MapGeometry;
  static PrefSymbol MapAutoUpdate;
  static PrefSymbol NodeHistSize;
  static PrefSymbol SearchHistSize;
  static PrefSymbol MaxSearchHits;
  static PrefSymbol DefaultMarkBase;
  static PrefSymbol DisplayFirstHit;
  static PrefSymbol AutomaticHelp;
#ifdef UseWideChars
  // Preferences for Japanese hyphenation - 4/6/94 kamiya
  static PrefSymbol HyphenMethod;
  static PrefSymbol HyphenLevel;
#endif

  enum { INT_CHANGED = FolioObject::_LAST, BOOLEAN_CHANGED,
	 STRING_CHANGED, GEOMETRY_CHANGED };

  PrefMgr();
  ~PrefMgr();

  void display();
  void sync();

  int get_int (const PrefSymbol sym);
  void set_int (const PrefSymbol sym, int value);

  bool get_boolean (const PrefSymbol sym);
  void set_boolean (const PrefSymbol sym, bool value);

  const char *get_string (const PrefSymbol sym);
  void set_string (const PrefSymbol sym, const char *value);

  const WindowGeometry &get_geometry (const PrefSymbol sym);
  void set_geometry (const PrefSymbol sym, const WindowGeometry &value);

private:
  PrefAgent	       *f_pref_agent;

  PrefSymbol f_last_symbol;
  UserPreference *f_last_preference;

  LONG_LIVED_HH(PrefMgr,pref_mgr);
};

LONG_LIVED_HH2(PrefMgr,pref_mgr);

inline
PrefMgr::PrefMgr()
: f_pref_agent (NULL), f_last_symbol (NULL), f_last_preference (NULL)
{
}

