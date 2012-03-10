/* $XConsortium: SearchEng.C /main/3 1996/08/21 15:47:17 drk $ */

/* imported interfaces */
#include "dti_cc/CC_Stack.h"
#include "OL-Data.h"
#include "Task.h"
#include "FlexBuffer.h"
#include "Token.h"
#include "OLAF.h"
#include "NodeData.h"
#include "NodeTask.h"
#include "GraphicsTask.h"
#include "BookTasks.h"
#include "Dispatch.h"
#include "SGMLName.h"

// exported interfaces
#include "SearchEng.h"

//---------------------------------------------------------------------
SearchEngine::SearchEngine( NodeData *parent , const Token & t )
{

  f_base = t.level();
  f_graphics = 0;

  CollectObject = -1;
  hasTerms      = 0;
  f_parent      = parent;
  termsBuffer   = NULL;
  
}

//---------------------------------------------------------------------
int
SearchEngine::GraphicsIsDone() const
{
  if ( f_graphics ) { return ( f_graphics->IsDone() ); }
  else {
    return 0;
  }

}


