#include "PreCompiled.h"
#include "RingBuffer.h"

namespace Engine
{
	/******************************************************************************
	 Function     : RingBuffer_UnitTest
	 Description  : UnitTest function to test RingBuffer
	 Input        : void
	 Output       : 
	 Return Value : void  
	 Data Accessed: 
	 Data Updated : 
 
	 History      :
	 Author       : Vinod VM
	 Modification : Created function
	******************************************************************************/
	void RingBuffer_UnitTest( void )
	{
	#ifdef _DEBUG
		const unsigned int UTRingBufferSize = 12;

		RingBuffer<unsigned int, UTRingBufferSize>	UTRingBuffer;

		assert( UTRingBuffer.Size() == UTRingBufferSize );
		assert( UTRingBuffer.Count() == 0 );

		for( unsigned int i = 0; i < (UTRingBufferSize * 2.5); i++ )
		{
			UTRingBuffer.Push( i );
			unsigned int ulMaxCount = UTRingBuffer.Count();
			for( unsigned int j = 0; j < ulMaxCount; j++ )
			{
				assert( UTRingBuffer[j] == (i - j) );
			}
		}

		UTRingBuffer.Clear();
		assert( UTRingBuffer.Count() == 0 );
	#endif
	}

}