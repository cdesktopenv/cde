/* $XConsortium: random_gen.h /main/3 1996/06/11 17:42:42 cde-hal $ */

#ifndef _random_gen_h
#define _random_gen_h

#include "utility/pm_random.h"
#include "object/oid_t.h"

class random_gen
{
protected:
   pm_random rand_gen;

protected:
   void _random_string(ostream& out, int low, int high);

public:
   random_gen();
   ~random_gen();

   pm_random& random_generator() { return rand_gen; };
   int pick_a_value(int low, int high);

   const char* get_random_string(int low, int high);

   void random_string(ostream& out, int len);
   void random_string(ostream& out, int low, int high, 
		      Boolean out_string_code = true);
   void random_string_to_be_compressed(ostream&, int l, int h, const oid_t& = ground);
   void random_oid(ostream&, int class_code = 0, int instance_code = 0);
   void random_integer(ostream&, int value = 0);
};

#endif
