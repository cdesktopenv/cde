// $XConsortium: dict_driver.cc /main/3 1996/06/11 16:42:53 cde-hal $
#include <string.h>

#include <iostream.h>

#include "Exceptions.hh"
#include "UAS_Exceptions.hh"
#include "UAS_String.hh"

#include "Dict.hh"
#include "DictIter.hh"

main()
{
    INIT_EXCEPTIONS();

    Dict<UAS_String, int> map(NULL, 0);

    for (;;) {
	char string[128];
	*string = '\0';
	cin.getline(string, 128);
	while (! strlen(string)) {
	    cin.getline(string, 128);
	}
	if (strcasecmp(string, "end") == 0)
	    break;
	UAS_Pointer<UAS_String> uas_string
		= new UAS_String(string, strlen(string), UAS_OWNER);
	map[*uas_string]++;
	*string = '\0';
    }

    printf("map size = %d\n", map.size());

    DictIter<UAS_String, int> mapiter;
    for (mapiter = map.first(); mapiter() ;mapiter++) {
	printf("%s, %d\n", (const char*)mapiter.key(), mapiter.value());
    }
}
