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
/* $XConsortium: Prompt.h /main/3 1995/11/06 09:44:30 rswiston $ */
/*                                                                      *
 * (c) Copyright 1993, 1994 Hewlett-Packard Company                     *
 * (c) Copyright 1993, 1994 International Business Machines Corp.       *
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.                      *
 * (c) Copyright 1993, 1994 Novell, Inc.                                *
 */

#ifndef PROMPT_H
#define PROMPT_H

#include "MotifUI.h"

class Prompt : public MotifUI {

 private:

   Widget _caption;
   Widget _text;
   PromptType _prompt_type;
   char *_default_value;
   char *_value;
   ValidationCallback CB;
   void * validation_data;

 public:

   Prompt(MotifUI * parent,
          char *name = "prompt",
          boolean editable = true,
          PromptType prompt_type = STRING_PROMPT, 
          char *default_value = NULL,
          ValidationCallback CB = NULL,
          void * validation_data = NULL,
          boolean echo_input = true,
          int n_column = 20,
          int n_rows = 1,
          int captionWidth = 0);
   ~Prompt();

   void SetFocus()             { DoSetFocus(_text); }
   void Reset();               // Reset to default value

   char * Value();             // Access value as STRING
   void Value(int *);          // Access value as INTEGER
   void Value(float *);        // Access value as REAL_NUMBER

   boolean Value(int);         // Set value
   boolean Value(float);       // Set value
   boolean Value(char *);      // Set value

   void DefaultValue(char *);  // Set value

   char * DefaultValue()             { return _default_value; }

   const UI_Class UIClass()          { return PROMPT; }
   const int UISubClass()            { return _prompt_type; }
   const char *const UIClassName()   { return "Prompt"; }

};

#endif /* PROMPT_H */
