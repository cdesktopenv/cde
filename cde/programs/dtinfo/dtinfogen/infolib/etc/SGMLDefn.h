/* $XConsortium: SGMLDefn.h /main/2 1996/07/18 16:48:35 drk $ */
#ifndef SGML_DEFN_HDR
#define SGML_DEFN_HDR

enum DEFN_TYPE {
  INVALID=-1,
  ENTITY_TYPE,
  NOTATION_TYPE,
  SUBDOC_TYPE
};

class SGMLDefn {
  
friend int yylex();
friend class Dispatch;
friend class EntityList;

private:

  char *sys_id;
  char *pub_id;
  char *file_name;
  int   name;
  DEFN_TYPE type;
  SGMLDefn *next;

protected:

  void store_sys_id( char *sid );
  void store_pub_id( char *pid );
  void store_defn( DEFN_TYPE, char * );
  void store_file_name( char *file_name );
  SGMLDefn &operator=( SGMLDefn & );


public:

  int   getName() const { return(name); }
  char *getFileName() const { return(file_name); }
  
  SGMLDefn();
  ~SGMLDefn();
};


#endif
  
  
