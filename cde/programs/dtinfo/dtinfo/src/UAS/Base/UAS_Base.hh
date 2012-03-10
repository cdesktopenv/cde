// $TOG: UAS_Base.hh /main/9 1998/04/17 11:40:28 mgreess $
#ifndef _UAS_Base_h_
#define _UAS_Base_h_

# include "Exceptions.hh"
# include "UAS_Pointer.hh"

class UAS_Base: public Destructable {
    public:
	UAS_Base ();
	virtual ~UAS_Base ();
	int operator == (const UAS_Base &);
#if (defined(sparc) && defined(SC3)) || defined(__uxp__) || defined(__osf__) || defined(USL) || defined(linux)
	/* SC++ 4.0.1 does not like these being protected  */
#else
    protected:
    template <class T> friend class UAS_Pointer;
#endif
	virtual void reference ();
	virtual void unreference ();
    private:
	unsigned int fReferenceCount;
};

#endif
