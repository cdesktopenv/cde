// $XConsortium: oid_t.hh /main/3 1996/06/11 16:30:40 cde-hal $
#ifndef _oid_t_hh
#define _oid_t_hh

// Temporary code to be replaced with lend...

class oid_t
{
public: // functions
  oid_t (const u_int32 code)
    : f_code (code) { }
  const u_int32 icode() const
    { return (f_code); }
  bool eq (const oid_t &oid) const
    { return (f_code == oid.f_code); }

protected: // variables
  u_int32 f_code;
};

#endif /* _oid_t_hh */
/* DO NOT ADD ANY LINES AFTER THIS #endif */
