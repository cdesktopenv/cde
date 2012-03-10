// $XConsortium: WXmOpButton.cc /main/3 1996/06/11 16:46:51 cde-hal $
#include "WXmOpButton.h"

/* Other activate options:
   Add another callback to a single function in the query editor
   which will snag new text and insert in the text field.  This
   means that we'll need to track what's to both sides of the cursor
   to be able to figure out how much text to grab.
*/

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

WXmOpButton::WXmOpButton (WComposite &father, const char *name,
			  PRoot *&root, long &position,
			  is_sensitive_func_t sfunc, insert_op_func_t ifunc)
: WXmPushButton (father, name, WAutoManage),
  f_root (root), f_position (position),
  f_is_sensitive (sfunc), f_insert (ifunc), f_sensitive (True)
{
  SetActivateCallback (this, (WWL_FUN) &WXmOpButton::insert_op);
}

// /////////////////////////////////////////////////////////////////
// set_sensitive - check sensitivity and set
// /////////////////////////////////////////////////////////////////

void
WXmOpButton::set_sensitive ()
{
  bool sense;
  if (f_position == -1)
    sense = FALSE;
  else
    sense = (*f_is_sensitive)(f_root, f_position);
  if (sense != f_sensitive)
    {
      SetSensitive (sense);
      f_sensitive = sense;
    }
}

// /////////////////////////////////////////////////////////////////
// insert_op - insert operation
// /////////////////////////////////////////////////////////////////

void
WXmOpButton::insert_op (WCallback *)
{
  (*f_insert)(f_root, f_position);
}
