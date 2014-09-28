#ifndef __MEMORY_POOL_HEADER
#define __MEMORY_POOL_HEADER

#include "PreCompiled.h"


#include "BitArray.h"


namespace Engine
{
	class MemoryPool
	{
		unsigned long m_ulSize;
		unsigned long m_ulNumOfItems;
		BitArray *m_BitArray;
		char *m_pMemoryPool;
		bool bIsPlacementCreate;
		MemoryPool(char *i_MemoryPointer, BitArray *i_BitArray, const unsigned long i_ulSize, 
				   const unsigned long i_ulNumOfItems, const bool i_bIsPlacementCreate);
		~MemoryPool();
	public:
	
		static MemoryPool *Create(const unsigned long ulSize, const unsigned long ulNumOfItems);

		static MemoryPool *CreateFromExistingMemory(void * i_MemoryBlock,	const unsigned long i_ulSize, 
												  const unsigned long i_ulNumOfItems, unsigned long *o_pulUsed);

		void Destroy(unsigned long *o_ulOutPutLen);

		void *Allocate(const size_t i_size);
		void DeAllocate(const void *pPointer);

		inline void * operator new(size_t i_size)
		{
			return malloc(i_size);
		} 

		inline void operator delete(void * i_ptr)
		{
			if (i_ptr)
			{
				free(i_ptr);
			}
		}

		inline void * operator new(size_t i_size, void * i_Where)
		{
			return i_Where;
		} 

		inline void operator delete(void * i_ptr, void * i_Where)
		{
			return;
		}
	};
}
#endif //__MEMORY_POOL_HEADER