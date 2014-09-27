#ifndef __SHARED_POINTER_HEADER
#define __SHARED_POINTER_HEADER
#include "PreCompiled.h"

namespace Engine
{
	template<class T>
	class SharedPointer
	{
	private:
		T * m_pPtr;
		unsigned long * m_pulReferenceCount;
		void Destroy(void);
	public:
		SharedPointer(void);
		SharedPointer(T * i_pPtr);
		SharedPointer(const SharedPointer<T> & i_Other);
		SharedPointer<T> & operator=(const SharedPointer<T> &i_rhs);
	
		T* operator->(void)
		{
			assert(m_pPtr != nullptr);
			return m_pPtr;
		}

		T & operator*(void)
		{
			assert(m_pPtr != nullptr);
			return *m_pPtr;
		}

		bool operator!=(void * Ptr)
		{
			if (m_pPtr != Ptr)
			{
				return true;
			}

			return false;
		}

		bool operator==(void * Ptr)
		{
			if (m_pPtr == Ptr)
			{
				return true;
			}

			return false;
		}

		~SharedPointer(void);

	} ;
}

#include "SharedPointer.inl"

#endif //__SHARED_POINTER_HEADER