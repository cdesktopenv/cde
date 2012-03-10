/* $XConsortium: name_oid_t.h /main/3 1996/06/11 17:22:59 cde-hal $ */
class name_oid_t {

public:
   ostring name;
   oid_t v_oid;

   name_oid_t(const char* nm, oid_t& id) : v_oid(id) { 
     name.set(nm);
   };
};
