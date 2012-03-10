/* $XConsortium: prolatex.c /main/3 1996/06/11 17:38:30 cde-hal $ */

#include <stdio.h> 
#include <string.h> 

#define BUFSIZ 1000

int replace();

main(argc, argv)
int argc;
char* argv[];
{
   char buf[BUFSIZ];

   while ( fgets(buf, BUFSIZ, stdin) != NULL ) {
       buf[strlen(buf)-1] = '\0';
       if ( replace(buf, "\\epsffile{", argv[1]) == 0 )
          continue;
       if ( replace(buf, "\\input{", argv[1]) == 0 )
          continue;
       else {
         fputs(buf, stdout);
         fputs("\n", stdout);
      }
   }
}

int replace(buf, pattern, s)
char* buf;
char* pattern;
char* s;
{
   int loc = strlen(pattern);
   char c = buf[loc];
   buf[loc] = '\0';
   if ( strcmp(buf, pattern) == 0 ) {
     fputs(buf, stdout);
     /*fputs(getenv("PWD"), stdout);*/
     fputs(s, stdout);
     fputs("/", stdout);
     buf[loc] = c;
     fputs(buf + loc, stdout);
     fputs("\n", stdout);
      return 0;
   } else  {
     buf[loc] = c;
     return -1;
   }
}
