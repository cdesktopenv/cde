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


