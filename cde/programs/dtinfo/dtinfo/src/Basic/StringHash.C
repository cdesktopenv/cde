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
 * $XConsortium: StringHash.cc /main/3 1996/06/11 16:21:55 cde-hal $
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

#include <Prelude.h>

    // Static lookup table

static int lookup_table[] =
  { 1,87,49,12,176,178,102,166,121,193,6,84,249,230,44,163,14,
    197,213,181,161,85,218,80,64,239,24,226,236,142,38,200,
    110,177,104,103,141,253,255,50,77,101,81,18,45,96,31,222,
    25,107,190,70,86,237,240,34,72,242,20,214,244,227,149,235,
    97,234,57,22,60,250,82,175,208,5,127,199,111,62,135,248,
    174,169,211,58,66,154,106,195,245,171,17,187,182,179,0,243,
    132,56,148,75,128,133,158,100,130,126,91,13,153,246,216,219,
    119,68,223,78,83,88,201,99,122,11,92,32,136,114,52,10,
    138,30,48,183,156,35,61,26,143,74,251,94,129,162,63,152,
    170,7,115,167,241,206,3,150,55,59,151,220,90,53,23,131,
    125,173,15,238,79,95,89,16,105,137,225,224,217,160,37,123,
    118,73,2,157,46,116,9,145,134,228,207,212,202,215,69,229,
    27,188,67,124,168,252,42,4,29,108,21,247,19,205,39,203,
    233,40,186,147,198,192,155,33,164,191,98,204,165,180,117,76,
    140,36,210,172,41,54,159,8,185,232,113,196,231,47,146,120,
    51,65,28,144,254,221,93,189,194,139,112,43,71,109,184,209 };



u_int
string_hash (const char *key)
{
  assert (key != NULL);

  register u_int pos, sum = 0;
  register char c;

  for (pos = 0; (c = *key++); pos++)
    sum = (sum << 5) + (sum >> (sizeof(int) * 8 - 6))
	  + (c ^ lookup_table[(int)c]);

  // was:
  // sum = c ^ lookup_table [(pos * c) % 256]; 

  return (sum);
}
