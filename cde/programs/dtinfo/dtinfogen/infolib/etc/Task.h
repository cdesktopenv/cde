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
/* $XConsortium: Task.h /main/2 1996/07/18 15:23:13 drk $ */

#ifndef __Task_h
#define __Task_h

#include <stddef.h>

#include "Exceptions.hh"

class Unexpected : public Exception{
public:
  /* BEWARE! we keep a pointer to the string without copying it! */
  Unexpected(const char *msg)  { msg_ = msg; };

  const char *msg(void) { return msg_; };

  DECLARE_EXCEPTION(Unexpected, Exception)
  
private:
  const char *msg_;
};


class Token;

class Task{
public:
  virtual ~Task() {};
  
  virtual void markup(const Token& t) = 0 /* throw(Unexpected) */;

  virtual void data(const char *chars,
		    size_t len) = 0;

};

class ComplexTask: public Task{

public:
  virtual void markup(const Token& t) /* throw(Unexpected) */;

  virtual void data(const char *chars,
		    size_t len);
protected:
  ComplexTask();
  ~ComplexTask();

  void removeAllSubTasks();
  void addSubTask(Task *);

  void stopSubTask(Task *);

#define KILLSUBTASK(t) { if(t) { stopSubTask(t); delete t; t = NULL; } }

  Task *subtask(int i) { return subtasks[i]; };

protected:
  Task **subtasks;
  int    used;

private:
  int    alloc;

  void grow(int);
};

#define TEST_TASK 0
#if TEST_TASK

class TestTask : public Task{

public:
  virtual void markup(const Token& t) /* throw(Unexpected) */;

  virtual void data(const char *chars,
		    size_t len);
};

class TestTask2: public ComplexTask{

protected:
  int f_base;
  
public:
  virtual void markup(const Token& t) /* throw(Unexpected) */;

  virtual void data(const char *chars,
		    size_t len);
  TestTask2();
};
#endif /* TEST_TASK */

#endif /* __Task_h */
