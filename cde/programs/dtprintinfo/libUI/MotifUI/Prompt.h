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
