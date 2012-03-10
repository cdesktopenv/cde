/* $XConsortium: SGMLName.h /main/2 1996/07/18 15:19:02 drk $ */

#ifndef __SGMLName_h
#define __SGMLName_h

class SGMLName{

 public:
  static int intern(const char*, int upcase = 0);

  static const char *lookup(int) /* throw(KeyError) */;

  enum DeclaredValue { IMPLIED, CDATA, NOTATION, TOKEN, ENTITY, qty };

  static void init();
};

#endif
