/* $XConsortium: db_new.h /main/3 1996/06/11 17:24:08 cde-hal $ */

#include <new.h>
#include "stoorage/abs_storage.h"

void* operator new ( size_t );
void* operator new ( size_t, abs_storage* );

void operator delete( void* ); 
void operator delete( void*, size_t );

