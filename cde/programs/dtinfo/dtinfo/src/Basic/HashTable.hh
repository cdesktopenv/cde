// $XConsortium: HashTable.hh /main/3 1996/06/11 16:18:51 cde-hal $
// NOTE: quickly hacked up

class FolioObject;

#define HASH_TABLE_SIZE 255

class HashObject ;
class HashTable {
  public:
    HashTable();
    ~HashTable();
    
    void 	add(const char *key, FolioObject *object);
  // void	remove(const char *key);
    void       *find (const char *key) ; // return object associated with key

  private:			// functions
    unsigned int hash(const char *key);

  protected:			// variables
    xList<HashObject *> *f_table[HASH_TABLE_SIZE] ;

};



