// $XConsortium: WString.hh /main/4 1996/08/23 20:43:34 cde-hal $
#ifndef __WCHAR_H_
#define __WCHAR_H_

#include <wchar.h>
#include <string.h>

class WStringRep
{
    friend class WString;
private:
    WStringRep(const wchar_t* data, int length);
    ~WStringRep();

    void incref();
    void decref();

private:
    int f_refcount;
    wchar_t* f_data;
    int f_length;
};

inline
WStringRep::WStringRep(const wchar_t* data, int length) :
    f_refcount(0), f_length(length)
{
    f_data = new wchar_t[f_length + 1];
    memcpy((void*)f_data, (void*)data, f_length*sizeof(wchar_t));
    f_data[f_length] = 0;
}

inline
WStringRep::~WStringRep()
{
    delete[] f_data;
}

inline void
WStringRep::decref()
{
    if (--f_refcount == 0)
	delete this;
}

inline void
WStringRep::incref()
{
    ++f_refcount;
}

class WString
{
public:
    WString();

    WString(const WString &);
    WString(const char *,   int length = -1);
    WString(const wchar_t*, int length = -1);

    ~WString();

    WString& operator=(const WString&);

    operator wchar_t*() const;

    char*    get_mbstr(); // user is responsible for freeing memory
    wchar_t* get_wstr();  // user is responsible for freeing memory

    int length() const;

protected:
    void set_wstring(WStringRep*);
    void unset_wstring();

private:

    WStringRep* f_wstringrep;
};
#endif

