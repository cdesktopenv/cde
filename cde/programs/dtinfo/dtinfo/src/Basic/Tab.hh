// $XConsortium: Tab.hh /main/3 1996/06/11 16:22:18 cde-hal $

class ObjectId;
class NodeHandle;

class Tab : public FolioObject
{
public:
  Tab (const char *name, const ObjectId &locator);
    
  ~Tab();

  const char *name () const
    { return (f_name); }
  const ObjectId &locator () const
    { return *f_locator; }

protected:
  char       *f_name;
  ObjectId   *f_locator;
};
