/* $XConsortium: printlines.c /main/3 1995/10/31 12:02:59 rswiston $ */
#include <stdio.h>


main(argc, argv)
int argc;
char *argv[];
{
  
   int i;
     for (i=0; i < 100; i++)
       printf("Testing SaveLines: This line number is %d \n", i);
}
