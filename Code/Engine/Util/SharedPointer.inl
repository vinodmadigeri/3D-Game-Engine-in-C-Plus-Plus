
namespace Engine
{
	/******************************************************************************
	Function     : Sharedpointer functions
	Description  :  
	Input        : 
	Output       : 
	Return Value : 

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/

	template<class T>
	SharedPointer<T>::SharedPointer(void):
		m_pPtr(NULL),
		m_pulReferenceCount(new unsigned long)
	{
		assert(m_pulReferenceCount != nullptr);
		*m_pulReferenceCount = 1;
	}

	template<class T>
	SharedPointer<T>::SharedPointer(T* i_pPtr):
		m_pPtr(i_pPtr),
		m_pulReferenceCount(new unsigned long)
	{
		assert(m_pulReferenceCount != nullptr);

		*m_pulReferenceCount = 1;
	}

	template<class T>
	SharedPointer<T>::SharedPointer(const SharedPointer<T> &i_Other):
		m_pPtr(i_Other.m_pPtr),
		m_pulReferenceCount(i_Other.m_pulReferenceCount)
	{
		//assert(m_pPtr != nullptr);
		assert(m_pulReferenceCount != nullptr);

		(*m_pulReferenceCount)++;
	}

	template<class T>
	void SharedPointer<T>::Destroy(void)
	{
		assert(m_pulReferenceCount != nullptr);

		if(--(*m_pulReferenceCount) == 0)
		{
			if (m_pPtr)
				delete m_pPtr;

			delete m_pulReferenceCount;
		}
	}

	template<class T>
	SharedPointer<T> & SharedPointer<T>::operator=(const SharedPointer<T> &i_rhs)
	{
		Destroy();
		m_pPtr = i_rhs.m_pPtr;
		m_pulReferenceCount = i_rhs.m_pulReferenceCount;

		assert(m_pulReferenceCount != nullptr);

		(*m_pulReferenceCount)++;
		
		return *this;
	}

	template<class T>
	SharedPointer<T>::~SharedPointer(void)
	{
		Destroy();
	}
}