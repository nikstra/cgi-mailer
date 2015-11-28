///////////////////////////////////////////////////////////////////////////////
//
//	FILE:
//		CAutoPtr.h
//
//	DATE:
//		1999-03-18
//
//	DESCRIPTION:
//		CAutoPtr is a template class to be used as a pointer.
//		It allocates an array of TYPE in the constructor and
//		deallocates it in the destructor.
//
//	DEFINITIONS:
//		class CAutoPtr
//
//	FUNCTIONS:
//		CAutoPtr::CAutoPtr()
//		CAutoPtr::~CAutoPtr()
//		CAutoPtr::operator TYPE * ()
//
//	PURPOSE:
//		To avoid memory leaks, CAutoPtr uses RAII ( Resource Acquisition
//		Is Initialization ). By default, when declaring a CAutoPtr it
//		allocates memory for an object of TYPE, if a value n is given in
//		the constructor it allocates an array of TYPE with the size of n.
//		When CAutoPtr is destroyed, all memory previously allocated is
//		released in the destructor, thus preventing the programmer from
//		forgeting to release allocated memory.
//		The operator TYPE * () function is called when casting from
//		class CAutoPtr to TYPE.
//
//	NOTES:
//		All member functions are defined outside the class definition
//		to make the code more readable and to provide space for comments
//		for each function.
//
//	AUTHOR:
//		Niklas Strand
//

#ifndef CAUTOPTR_H
#define CAUTOPTR_H

///////////////////////////////////////////////////////////////////////////////
//
//	CLASS:		CAutoPtr
//
//	PURPOSE:	Template class for an auto pointer, that deallocates
//				memory on destruction.
//

template<class TYPE>
class CAutoPtr
{
public :
	CAutoPtr( int len = 1 );
	~CAutoPtr();
	operator TYPE*();

private :
	TYPE *ptr;
};

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CAutoPtr::CAutoPtr()
//
//	PURPOSE:	Constructor, allocates memory for an array of TYPE.
//

template<class TYPE>
inline CAutoPtr<TYPE>::CAutoPtr( int len )
{
	ptr = new TYPE[len];
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CAutoPtr::~CAutoPtr()
//
//	PURPOSE:	Destructor, releases memory allocated in the constructor.
//

template<class TYPE>
inline CAutoPtr<TYPE>::~CAutoPtr()
{
	delete [] ptr;
}

///////////////////////////////////////////////////////////////////////////////
//
//	FUNCTION:	CAutoPtr::operator TYPE * ()
//
//	PURPOSE:	Returns a pointer to the allocated array, so that you can
//				use an instance of CAutoPtr as a normal pointer.
//

template<class TYPE>
inline CAutoPtr<TYPE>::operator TYPE * ()
{
	return ptr;
}

#endif // CAUTOPTR_H
