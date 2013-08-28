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
/* $XConsortium: DataTask.C /main/3 1996/08/21 15:46:25 drk $ */

/* exported interfaces... */
#include "DataTask.h"

/* imported interfaces... */
#include "Token.h"

DataTask::DataTask(const Token&) : buf()
{
  level = 1;
}

void DataTask::markup(const Token &t)
{
  switch(t.type()){
  case START:
    /*
     * be careful not to start collecting again after the relavent
     * element is done!
     */
    if(level > 0) level++;

    break;
    
  case END:
    if(level > 0) level--;
    break;

  default:
    break;
  }
}


void DataTask::data(const char *chars, size_t len)
{
  if(level > 0){
    buf.write(chars, len);
  }
}


const char * DataTask::content(size_t *len)
{
  if(len) *len = buf.GetSize();
  
  return buf.GetBuffer();
}
