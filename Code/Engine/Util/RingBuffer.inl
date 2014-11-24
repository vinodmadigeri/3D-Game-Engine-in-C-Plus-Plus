namespace Engine
{
	/******************************************************************************
	 Function     : RingBuffer()
	 Description  : Ctor
	 Input        : void
	 Output       : 
	 Return Value :  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	RingBuffer<T,N>::RingBuffer()
	{
		lCurrentIndex = -1;
		ulSize = N;
		ulUsedCount = 0;
	}

	/******************************************************************************
	 Function     : ~RingBuffer()
	 Description  : Dtor
	 Input        : void
	 Output       : 
	 Return Value :  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	RingBuffer<T,N>::~RingBuffer()
	{
		lCurrentIndex = -1;
		ulSize = N;
		ulUsedCount = 0;
	}

	/******************************************************************************
	 Function     : Size
	 Description  : Returns the size of the ringbuffer
	 Input        : void
	 Output       : 
	 Return Value : unsigned long  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	unsigned long RingBuffer<T,N>::Size(void) const
	{
		assert(ulSize != 0);
		return ulSize;
	}

	/******************************************************************************
	 Function     : Count
	 Description  : Returns the UsedCount of the ringbuffer
	 Input        : void
	 Output       : 
	 Return Value : unsigned long  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	unsigned long RingBuffer<T,N>::Count(void) const
	{
		return ulUsedCount;
	}

	/******************************************************************************
	 Function     : Push
	 Description  : Adds the input item to the ringbuffer
	 Input        : const T & i_item
	 Output       : 
	 Return Value : void  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	void RingBuffer<T,N>::Push(const T & i_item)
	{
		assert(ulSize != 0);

		if ( ulUsedCount < ulSize)
			ulUsedCount++;

		lCurrentIndex = (lCurrentIndex + 1) % (ulSize);
		m_Array[lCurrentIndex] = i_item;

		return;
	}

	/******************************************************************************
	 Function     : operator[]
	 Description  : Overloads the operator to return the latest stored value 
					starting with zero index
	 Input        : const unsigned long i_index
	 Output       : 
	 Return Value : T  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	T RingBuffer<T,N>::operator[](const unsigned long i_index) const
	{
		long lCurrTempIndex = lCurrentIndex;
		unsigned long ulFinalIndex = lCurrentIndex;
		unsigned long ulMultiple = 1;

		assert(ulSize != 0);
		assert(i_index <= ulSize);

		for(unsigned long ulCount = 0; ulCount < i_index; ulCount++)
		{
			--lCurrTempIndex;
			// For modulo operation, we can have the following function: int mod(int a, int b) { int r = a % b; return r < 0 ? r + b : r; }
			if(lCurrTempIndex >= 0)
			{
				ulFinalIndex = (lCurrTempIndex) % (ulSize);
			}
			else
			{
				if ( 0 == (abs(lCurrTempIndex) % ulSize))
				{
					ulMultiple++;
				}

				ulFinalIndex = (ulSize * ulMultiple + lCurrTempIndex);
			}
		}

		return m_Array[ulFinalIndex];
	}

	/******************************************************************************
	 Function     : Clear
	 Description  : Clears the ringbuffer by resetting size, usedcount and currentindex
	 Input        : void
	 Output       : 
	 Return Value : void  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	template<class T, int N>
	void RingBuffer<T,N>::Clear(void)
	{
		lCurrentIndex = -1;
		ulSize = 0;
		ulUsedCount = 0;
	}

}