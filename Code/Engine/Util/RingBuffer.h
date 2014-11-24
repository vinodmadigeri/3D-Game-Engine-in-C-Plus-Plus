#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "PreCompiled.h"
#include <math.h>

#include "Debug.h"

namespace Engine
{

	template<class T, int N>
	class RingBuffer
	{
	private:
		T m_Array[N];					//Array of data
		unsigned long ulUsedCount;		//Total used count in ringbuffer, max value is the max capacity of ring buffer
		unsigned long ulSize;			//Max Size of ringbuffer
		long lCurrentIndex;				//latest index used in ring buffer
	public:
		RingBuffer(void);
		~RingBuffer(void);

		unsigned long Size(void) const;
		unsigned long Count(void) const;
		void Push(const T & i_item);
		T operator[](const unsigned long i_index) const;
		void Clear(void);
	};

	void RingBuffer_UnitTest( void );
}

	#include "RingBuffer.inl"

#endif //__RING_BUFFER_H