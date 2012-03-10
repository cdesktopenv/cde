/* $XConsortium: StyleValidate.cc /main/2 1996/07/18 16:18:13 drk $ */
#include <stdio.h>
#include <iostream.h>
#include <stream.h>
#include <unistd.h>
#include <sys/wait.h>

#include "DataBase.h"
#include "StyleValidate.h"

//---------------------------------------------------------------------
int 
validate_stylesheet( const char *buf, int buf_size, enum RENDERER_ENGINE_T t )
{

  char *styleFile = form( "/usr/tmp/style_sheet.%d", getpid() );
  
  FILE *fp = fopen( styleFile, "w" );
  if ( !fp ) {
    throw( PosixError(1, "Unable to open style_sheet\n") );
  }

  fwrite( (char *)buf, buf_size, 1, fp );
  fclose( fp );

  char* renderer = 0;

  switch ( t ) {
    case ONLINE:
     renderer = "online";
     break;
    case PRINT:
     renderer = "hardcopy";
     break;
    default:
     throw( PosixError(1, "Unknown renderer engine\n") );
  }

  char *cmd = form("validator %s %s", renderer, styleFile);
  int status = system(cmd);
  int exit_status = WEXITSTATUS(status);

  unlink( styleFile );

  return (exit_status);
  
}

