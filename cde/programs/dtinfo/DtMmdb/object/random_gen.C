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
// $XConsortium: random_gen.C /main/6 1996/08/21 15:56:48 drk $

#include <sys/time.h>
#include <sys/times.h>
#include "object/random_gen.h"


random_gen::~random_gen()
{
}

random_gen::random_gen()
{
#ifdef __uxp__
   int seed;
   struct tms tp;
   if ((seed = (int)times(&tp)) < 0)
      seed = 19;
#else
   struct timeval tp;
   struct timezone tzp;

   int seed = ( gettimeofday(&tp, &tzp) == 0 ) ? int(tp.tv_sec) : 19;
#endif

#ifdef CONTROLLED_SEED
if ( getenv("SEED") )
   seed = atoi(getenv("SEED"));

cerr << "seed=" << seed << "\n";
#endif

   rand_gen.seed(seed);
}
   
static char char_set[] =
   { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '.', '_', 
    '+', '|', '=', '\\', '~'
   };
     
void random_gen::random_string(ostream& out, int len)
{
   out << len << "\t";
   for ( int i=0; i<len; i++ )
     out << char_set[rand_gen.rand() % sizeof(char_set)]; 

   out << "\n";
}

static char* buf = 0;
static int buf_len = 0;

const char* random_gen::get_random_string(int low, int high)
{
   int x = pick_a_value(low, high);
 
   if ( buf_len < x ) {
      buf_len = x;
      delete buf;
      buf = new char[buf_len+1];
   }
   int i;
   for ( i=0; i<buf_len; i++ )
     buf[i] = char_set[rand_gen.rand() % sizeof(char_set)]; 

   buf[i] = 0;
   
   return buf;
}

int random_gen::pick_a_value(int low, int high)
{
   return ( low < high ) ? rand_gen.rand() % (high - low) + low : low;
}

void random_gen::_random_string(ostream& out, int low, int high)
{
   int l = pick_a_value(low, high);
   random_string(out, l);
}

void random_gen::random_string(ostream& out, int low, int high, Boolean x)
{
   if ( x == true )
      out << "6\n";
   _random_string(out, low, high);
}

void 
random_gen::random_string_to_be_compressed(ostream& out, int l, int h, const oid_t& id)
{
   out << "11\n";
   out << id << "\n";
   _random_string(out, l, h);
}

void random_gen::random_oid(ostream& out, int class_code, int instance_code)
{
   out << "7\n";
   out << class_code << "." << instance_code << "\n";
}

void random_gen::random_integer(ostream& out, int value)
{
   out << "4\n";
   out << value << "\n";
}
