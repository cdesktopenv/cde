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
 * $XConsortium: WArgList.cc /main/3 1996/06/11 17:03:16 cde-hal $
 *
 * Copyright (c) 1991 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */


#ifndef STDARGH
#include <stdarg.h>
#endif

#ifndef __STRING_H
#include <string.h>
#endif

#ifndef _wwl_h_
#include <WWL/wwl.h>
#endif

/*?class WArgList

The class \typ{WArgList} encapsulates a standard X toolkit
\typ{ArgList}. It simplifies the handling of toolkit arglists and can
be used in two ways: either directly like arglists, whithout having to
worry about allocating enough room for arguments, or in conjunction
with member functions of wrapped widgets.

The latter is useful in cases where many resources have to be changed
at the same time, like constraint resources for the \typ{Form} widget.

Each member function modifying a resource for a widget class is
overloaded to permit the buffering of requests in a \typ{WArgList}
rather than modifying the widget it at once. For instance, instead of
writing:

\expandafter\ifx\csname indentation\endcsname\relax
\newlength{\indentation}\fi
\setlength{\indentation}{0.5em}
\begin{flushleft}
\hspace*{2\indentation}CXmForm cxf(MyButton);\mbox{}\\
\mbox{}\\
\hspace*{2\indentation}cxf.TopAttachment (XmATTACH_WIDGET);\mbox{}\\
\hspace*{2\indentation}cxf.TopWidget (MyOtherButton);\mbox{}\\
\end{flushleft}
which does not work usually because the form requires the
attachment and the widget attached to in the same call, one would
write instead:
\expandafter\ifx\csname indentation\endcsname\relax
\newlength{\indentation}\fi
\setlength{\indentation}{0.5em}
\begin{flushleft}
\hspace*{2\indentation}CXmForm cxf(MyButton);\mbox{}\\
\hspace*{2\indentation}WArgList args;\mbox{}\\
\mbox{}\\
\hspace*{2\indentation}cxf.TopAttachment (XmATTACH_WIDGET, args);\mbox{}\\
\hspace*{2\indentation}cxf.TopWidget (MyOtherButton, args);\mbox{}\\
\hspace*{2\indentation}cxf.Set (args);\mbox{}\\
\end{flushleft}

The same is true for setting a list in an \typ{XmList} widget. Both the
contents and the size of the table should be set at the same time.

\begin{em}
We are not satisfied with the syntax of buffered resource modification
propopsed here. It might change, though we will try to remain
compatible with this version. If you find a better mechanism or syntax
to buffer resource changes, please, let us know.
\end{em}

?*/

#ifdef DOC

/*?
Default constructor for \typ{WArgList}.
?*/
WArgList::WArgList ()
{
	args = 0;
	num_args = alloc_args = 0;
}

/*?
Constructor for \typ{WArgList}. Preallocates a table of
resource/value, saves some time and space when the number of resource
is known. If more resources are specified than \var{Cardinal}, the
table grows.
?*/
WArgList::WArgList (Cardinal len)
{
	args = new Arg[len];
	num_args = 0;
	alloc_args = len;
}

/*?
Constructor of \typ{WArgList} from a Xt arglist. Nothing is allocated
as longs as no resource is added. The arglist is reallocated in this
case.
?*/
inline
WArgList::WArgList (ArgList a, Cardinal n)
{
	args = a;
	num_args = n;
	alloc_args = 0;
}
#endif

/*?
Vararg constructor for \typ{WArgList}. Beware to finish the argument
list with a NULL. Also, note that no checking is done. This is easier
to use and more readable in widget constructors than a static arglist.
?*/
WArgList::WArgList (String name, XtArgVal value, ...)
: args (NULL), num_args (0), alloc_args (0)
{
	String str;

	Add (name, value);
	va_list argv;
	va_start (argv, value);
	while ((str = va_arg(argv, String)))
		Add (str, va_arg (argv, XtArgVal));
	va_end (argv);
}

WArgList::WArgList(const WArgList &r)
{
	num_args = r.num_args;
	alloc_args = num_args;
	args = new Arg[num_args];
	memcpy(args, r.args, sizeof(Arg)*num_args);
}

/*?nodoc?*/
Arg&
WArgList::Grow (Cardinal n)
{
   // Return a reference to the requested arg if it is present
   if (n < alloc_args) {
      return args[n];
   }

   // Allocate more if requested arg is not present
   int	new_alloc_args = n + 16;
   ArgList new_args = new Arg[new_alloc_args];

   // Copy any existing args to the new area
   if (args && num_args) {

      memcpy (new_args, args, num_args * sizeof (Arg));

      // Delete existing args if allocated
      if (alloc_args) {
	 delete args;
      }
   }

   // Store new information
   alloc_args = new_alloc_args;
   args = new_args;

   // Return reference to requested argument
   return args[n];
}

WArgList&
WArgList::Add (String name, XtArgVal value)
{
// First look to see if this arg is already in the list.
//    Use that one if it is

   Arg		*a;
   Boolean	found = False;

// Loop til a matching name is found or we get to the end of the array
   for (unsigned int i = 0; !found && i < num_args; i++) {
      a = &(*this)[i];
      if ( strcmp(a->name, name) == 0 ) {
	 found = True;
      }
   }

// If the arg was found, then "a" is already set.  If not, set it here

   if ( !found ) {
      a = &(*this)[num_args];
      num_args++;
   }

   a->name  = name;
   a->value = value;

   return (*this); 
}
