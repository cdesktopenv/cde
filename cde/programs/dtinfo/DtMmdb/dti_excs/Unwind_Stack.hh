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
// $XConsortium: Unwind_Stack.hh /main/5 1996/10/04 09:34:56 drk $
class Destructable;

#define UNWIND_STACK_SIZE 512

class Unwind_Record
{
public:
  Destructable *f_object;
  unsigned short f_size;
  void set (Destructable *object, unsigned int size)
    { f_object = object; f_size = size; }
  unsigned long object_start() const
    { return ((unsigned long) f_object); }
  unsigned long object_end() const
    { return ((unsigned long) f_object + f_size); }
};


class Unwind_Stack
{
public:
  Unwind_Stack()
    { f_bottom = g_top;
      PRINTF (("+++ New TOS = %d\n", f_bottom)); }

  ~Unwind_Stack()
    { g_top = f_bottom;
      PRINTF (("--- Reset TOS = %d\n", g_top)); }

  void push (Destructable *object, unsigned int size);

  void pop()
    { g_top--;
      PRINTF (("  Pop [ %d ]\n", g_top)); }

  int empty() const
    { return (g_top == f_bottom); }

  const Unwind_Record &top() const
    { return (g_stack[g_top-1]); }
  
private:
  unsigned short f_bottom;

  static unsigned short g_top;

#ifdef C_API
  static Unwind_Record*  g_stack;

  friend void initialize_exception();
  friend void quit_exception();
#else
  static Unwind_Record  g_stack[UNWIND_STACK_SIZE];
#endif
};


inline void
Unwind_Stack::push (Destructable *object, unsigned int size)
{
  void terminate();
  if (g_top > UNWIND_STACK_SIZE)
    {
      Exceptions::error (Exceptions::f_msg_out_of_obj_stack_memory,
			 Exceptions::INTERNAL_ERROR);
      terminate();
    }
  PRINTF (("  Push [ %d ] = %p\n", g_top, object));
  g_stack[g_top++].set (object, size);
}
