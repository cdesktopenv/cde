/*	Copyright (c) 1995 FUJITSU LIMITED	*/
/*	All Rights Reserved			*/

/*
 * $XConsortium: FolioObject.hh /main/5 1996/09/10 17:26:30 barstow $
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

// Base Object for ALL Folio Objects

class Dependent;
class DependOnList;
class FolioObject;

typedef void (FolioObject::* notify_handler_t)
  (FolioObject *object, u_int notify_type,
   void *call_data, void *client_data);

#if defined(__STDC__) || defined(hpux) || defined(__osf__)
#define DEF_CLASS(C) \
  virtual ClassType type() const; \
  static ClassType C##Class
#define INIT_CLASS(C) \
  ClassType C::type() const { return (C##Class); } \
  ClassType C::C##Class = (size_t) &C::C##Class
#else
#define DEF_CLASS(C) \
  virtual ClassType type() const; \
  static ClassType C/**/Class
#define INIT_CLASS(C) \
  ClassType C::type() const { return (C/**/Class); } \
  ClassType C::C/**/Class = (ClassType) &C::C/**/Class
#endif

#ifdef __osf__
  typedef char * ClassType;
#else
  typedef int ClassType;
#endif

class FolioObject
{
public: // enums
#define FolioObjectLast 1
  enum notify_types { DESTROYED = 0, _LAST = FolioObjectLast };
  DEF_CLASS (FolioObject);

public: // functions
  FolioObject();
  virtual ~FolioObject();

  virtual const char *display_as();

#define AddDependent(CB, TYPE) \
  add_dependent (this, (notify_handler_t)CB, TYPE)
#define AddDependentd(CB, TYPE, DATA) \
  add_dependent (this, (notify_handler_t)CB, TYPE, DATA)

#if defined(SC3) || defined(__osf__) || defined(USL) || defined(__SunOS)
  void add_dependent (FolioObject *dependent, notify_handler_t handler,
		      u_int notify_type, void *dependent_data = NULL);
#else
  void add_dependent (void *dependent, notify_handler_t handler,
		      u_int notify_type, void *dependent_data = NULL);
#endif

#define RemoveDependent(CB, TYPE) \
  remove_dependent(this, (notify_handler_t)CB, TYPE)
#define RemoveDependentd(CB, TYPE, DATA) \
  remove_dependent(this, (notify_handler_t)CB, TYPE, DATA)

#if defined(SC3) || defined(__osf__) || defined(USL) || defined(__SunOS)
  void remove_dependent (FolioObject *dependent, notify_handler_t handler,
			 u_int notify_type, void *dependent_data = NULL);
#else
  void remove_dependent (void *dependent, notify_handler_t handler,
			 u_int notify_type, void *dependent_data = NULL);
#endif
  void remove_depend_on (FolioObject *target, Dependent *d);
  void release_dependents();
  void release_depend_on_list();

#define Observe(TARG, TYPE, CB) \
  observe (this, TARG, (notify_handler_t) CB, TYPE)
#define Observed(TARG, TYPE, CB, DATA) \
  observe (this, TARG, (notify_handler_t) CB, TYPE, DATA)

#if defined(SC3) || defined(__osf__) || defined(USL) || defined(__SunOS)
  void observe (FolioObject *real_this, FolioObject *target,
		notify_handler_t callback, u_int notify_type,
		void *client_data = NULL);
#else
  void observe (void *real_this, FolioObject *target,
		notify_handler_t callback, u_int notify_type,
		void *client_data = NULL);
#endif
  
  bool initialized()
    { return (f_initialized); }
  ErrorCode status()
    { return (f_status); }

protected: // functions
  // Object states that it has changed - dependents notified.
  // notify_type should be an enum provided by the object.  Dependents
  // should know about various object change types that it cares about.
  // Dependents should ignore change types that it doesn't care about. 
  void notify (u_int notify_type, void *notify_data = NULL);

  void self_destruct();
  void setInitialized()
    { f_initialized = TRUE; }
  void unsetInitialized()
    { f_initialized = FALSE; }
  void setStatus (ErrorCode status)
    { f_status = status; }

protected: // variables
  ErrorCode	 f_status;
  bool	 f_initialized;
  Dependent     *f_dependents;
  DependOnList  *f_depend_on_list;
};
