// $TOG: Destructable_il.hh /main/9 1998/04/20 10:06:23 mgreess $

// /////////////////////////////////////////////////////////////////
// class constructor
// /////////////////////////////////////////////////////////////////

inline
Destructable::Destructable()
{
  // Make sure the application has initialized this library.
#ifdef DEBUG
  Exceptions::check_initialized();
#endif

  if (in_stack_set_size())
    Jump_Environment::register_object (this);
  PRINTF (("  Constructed obj @ %p\n", this));
}


// NOTE:  Both the copy and assignment constructors below init
// f_constructed to 0, which is going to be bogus if the derived
// class checks this value.  This is a problem because we don't
// know if the derived class uses the default copy constructor or
// not.  If it does we want f_constructed to be set to 1, otherwise
// we want it to be set to 0 until its methods complete.
// Ugh! 

// /////////////////////////////////////////////////////////////////
// copy constructor
// /////////////////////////////////////////////////////////////////

inline
Destructable::Destructable (const Destructable &)
{
  if (in_stack_set_size())
    Jump_Environment::register_object (this);
  PRINTF (("  Copy ctor @ %p\n", this));
}


// /////////////////////////////////////////////////////////////////
// assignment operator
// /////////////////////////////////////////////////////////////////

inline Destructable &
Destructable::operator = (const Destructable &)
{
  // Assignment operator does nothing to preserve state of original
  // creation of this object.  State information is permanently
  // associated with a Destructable object and cannot be assigned to. 
  PRINTF (("Assign op @ %p\n", this));
  return (*this);
}


// /////////////////////////////////////////////////////////////////
// class destructor
// /////////////////////////////////////////////////////////////////

// There is currently no checking here for objects that have not
// been constructed.  It is possible for this to be called for such
// objects if an exception is thrown in a constructor.  See test18.C. 

inline
Destructable::~Destructable()
{
  if (in_stack())
    Jump_Environment::unregister_object (this);
}

// /////////////////////////////////////////////////////////////////
// destruct function
// /////////////////////////////////////////////////////////////////

// This code relies on the fact that in cfront 2.1 this qualified
// call to the destructor will actually call the virtual destructor. 

#if (!defined(hpux)) && (!defined(__uxp__)) && (CC_VERSION < 30)
inline void
Destructable::destruct()
{
#if defined(linux) || defined(CSRG_BASED) || defined(sun)
  delete this;
#else
  // Have to call this here since some compilers don't allow
  // qualified calls through object pointer.
  Destructable::~Destructable();
#endif
}
#endif
