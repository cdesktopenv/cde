// $XConsortium: tst_form.C /main/4 1996/08/21 15:55:30 drk $

#include <stdio.h>
#include "utility/c_stream.h"

main()
{
    fprintf(stderr, "%s\n", form("%d %c %s", 1, 'c', "form"));
}
