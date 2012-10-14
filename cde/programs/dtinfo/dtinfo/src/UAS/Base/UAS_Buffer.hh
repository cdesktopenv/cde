// $XConsortium: UAS_Buffer.hh /main/3 1996/06/11 16:36:49 cde-hal $
class UAS_Buffer
{
public:
  UAS_Buffer (unsigned int initial_size = 1024);
  ~UAS_Buffer();

  void init (unsigned int initial_size = 1024);

  char *point() const
    { return (f_point); }
  // Might want to add assert to make sure point is valid.  15:07 01/12/93 DJB 
  void point (char *new_point)
    { f_point = new_point; }
  const char *data() const
    { return (f_start); }
  unsigned int length() const
    { return (f_end_of_data - f_start); }
  // Methods for saving buffer independent position inforation, since
  // the buffer can be dynamically reallocated.  You must save offsets
  // if you want to get an existing string out of the buffer after a
  // write. 
  int offset (const char *point) const
    { return (point - f_start); }
  char *position (int offset)
    { return (f_start + offset); }
  // Return the number of bytes of real data remaining after the point. 
  unsigned int remaining() const
    { return (f_end_of_data - f_point); }
  void reset()
    { f_point = f_start; }

  void write (const ssize_t integer);
  void write (const unsigned int integer)
    { write ((ssize_t) ((void *)(ssize_t) integer)); }
  void write (const char *string);
  void write (const char *bytes, unsigned int size, unsigned int length);

  void read (int *integer);
  void read (unsigned int *integer)
    { read ((int *) integer); }
  // Versions to get copy (space must exist!): 
  unsigned int read (char *const string);
  void read (char *const bytes, unsigned int size, unsigned int length);
  // Versions to point into buffer memory:
  unsigned int read (char **string);
  void read (char **bytes, unsigned int size, unsigned int length);

private:
  void check_space (unsigned int);

private:
  static char *f_start;
  char        *f_point;
  static char *f_end;
  char        *f_end_of_data;
  static int   f_reference_count;
};
