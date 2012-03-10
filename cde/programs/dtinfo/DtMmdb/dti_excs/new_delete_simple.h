/* $XConsortium: new_delete_simple.h /main/3 1996/06/11 16:52:04 cde-hal $ */


#ifndef _new_delete_simple_h
#define _new_delete_simple_h 1


#define NEW_AND_DELETE_SIGNATURES(class_name) \
   void* operator new( size_t ); \
   void* operator new( size_t, void* ); \
   void operator delete( void* ) 

#ifdef C_API
#include <stdlib.h>

#define NEW_AND_DELETE_BODIES_SIMPLE(class_name) \
\
void* class_name::operator new( size_t sz )\
{\
   return (void*)malloc(sz); \
}\
\
void* class_name::operator new( size_t sz, void* ptr )\
{\
   return (void*)ptr; \
}\
\
void class_name::operator delete( void* p )\
{\
   ::operator delete(p); \
}


#else
#include <new.h>
#endif


#endif


