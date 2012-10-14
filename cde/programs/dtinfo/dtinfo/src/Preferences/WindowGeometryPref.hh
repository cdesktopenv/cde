/*
 * $XConsortium: WindowGeometryPref.hh /main/3 1996/06/11 16:31:41 cde-hal $
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


class WindowGeometry
{
public:
  WindowGeometry()
    : ulx(0), uly(0), width(100), height(100)
    { }
  bool operator == (const WindowGeometry &wg)
    { return (ulx == wg.ulx && uly == wg.uly &&
	      width == wg.width && height == wg.height); }
  int ulx, uly;
  unsigned int width, height;
};

class WindowGeometryPref : public UserPreference
{
public:
  WindowGeometryPref (const char *name);

  const WindowGeometry &value()
    { return (f_value); }

  void value (const WindowGeometry &v)
    { f_value = v; save(); }

private:
  void save();
  void restore();

  WindowGeometry f_value;
};


inline
WindowGeometryPref::WindowGeometryPref (const char *name)
: UserPreference(name)
{
  restore();
}
