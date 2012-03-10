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
