
#include "PreCompiled.h"

#include <stdio.h>

#include "Debug.h"
#include "MemoryPool.h"


namespace Engine
{
	/******************************************************************************
		Function     : MemoryPool
		Description  : Constructor for memory pool class, set the memory pool pointer and bitarray
		Input        : char *i_MemoryPointer, BitArray *i_BitArray, const unsigned long i_ulSize, 
					const unsigned long i_ulNumOfItems
		Output       : 
		Return Value : 
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	MemoryPool::MemoryPool
	(
		char *i_MemoryPointer, 
		BitArray *i_BitArray, 
		const unsigned long i_ulSize, 
		const unsigned long i_ulNumOfItems,
		const bool i_bIsPlacementCreate
	):
		m_pMemoryPool(i_MemoryPointer),
		m_BitArray(i_BitArray),
		m_ulSize(i_ulSize), 
		m_ulNumOfItems(i_ulNumOfItems),
		bIsPlacementCreate(i_bIsPlacementCreate)
	{

	}

	/******************************************************************************
		Function     : ~MemoryPool
		Description  : Deletes the memory pool array and bitarray
		Input        : void
		Output       : 
		Return Value : 
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	MemoryPool::~MemoryPool(void)
	{
	
	}

	/******************************************************************************
		Function     : Destroy
		Description  : Deletes the memory pool array and bitarray based on if 
					   created using placement new or regular new
		Input        : unsigned long *o_ulOutPutLen
		Output       : 
		Return Value : void
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void MemoryPool::Destroy(unsigned long *o_ulOutPutLen)
	{
		*o_ulOutPutLen = 0;

		if (NULL != m_BitArray)
		{
			assert(o_ulOutPutLen != NULL);

			m_BitArray->Destroy(o_ulOutPutLen);
		}

		if ( false == bIsPlacementCreate)
		{
			if (NULL != m_pMemoryPool)
			{
				delete [] m_pMemoryPool;
			}

			delete this;
		}
		else
		{
			*o_ulOutPutLen += m_ulSize * m_ulNumOfItems + sizeof(MemoryPool);
		
			this->~MemoryPool();
		}
	}

	/******************************************************************************
		Function     : Create
		Description  : Static function to alllocate pool memory and create a new memory 
					pool
		Input        : const unsigned long i_ulSize, 
					const unsigned long i_ulNumOfItems
		Output       : 
		Return Value : MemoryPool * 
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	MemoryPool * MemoryPool::Create
	(
		const unsigned long i_ulSize, 
		const unsigned long i_ulNumOfItems
	)
	{
		char *pNewMemoryPool = new char[i_ulSize * i_ulNumOfItems];

		if(NULL == pNewMemoryPool)
		{
			return NULL;
		}

		pNewMemoryPool[0] = '\0';

		BitArray *pNewBitArray = BitArray::Create(i_ulNumOfItems);
		bool bIsPlacementCreate = false;

		if (NULL == pNewBitArray)
		{
			delete [] pNewMemoryPool;
			return NULL;
		}

		return new MemoryPool(pNewMemoryPool, pNewBitArray, i_ulSize, i_ulNumOfItems, bIsPlacementCreate);
	}

	/******************************************************************************
		Function     : CreateFromExistingMemory
		Description  : Static function to create pool memory with an existing
					memory for Memory Manager
		Input        : const unsigned long i_ulSize, 
					const unsigned long i_ulNumOfItems
		Output       : 
		Return Value : MemoryPool * 
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	MemoryPool * MemoryPool::CreateFromExistingMemory
	(
		void * i_MemoryBlock,
		const unsigned long i_ulSize, 
		const unsigned long i_ulNumOfItems,
		unsigned long *o_pulUsed
	)
	{
		if ((NULL == i_MemoryBlock) || (NULL == o_pulUsed))
		{
			return NULL;
		}

		*o_pulUsed = 0;
		bool bIsPlacementCreate = true;

		uintptr_t pMemPointer = reinterpret_cast<uintptr_t> (i_MemoryBlock);

		BitArray *pBitArray = BitArray::CreateFromExistingMemory(i_MemoryBlock, i_ulNumOfItems, o_pulUsed);
		
		pMemPointer += *o_pulUsed;

		CONSOLE_PRINT("*Memory Used by BitArray: %d [%X]", *o_pulUsed, pMemPointer);

		*o_pulUsed += i_ulSize * i_ulNumOfItems + sizeof(MemoryPool);
		
		CONSOLE_PRINT("*Total Memory Used By Memorymanager: %d", *o_pulUsed);

		return new (reinterpret_cast<void *>(pMemPointer)) MemoryPool((reinterpret_cast<char *>(pMemPointer + sizeof(MemoryPool))), pBitArray, i_ulSize, i_ulNumOfItems, bIsPlacementCreate);
	}

	/******************************************************************************
		Function     : Allocate
		Description  : Function to allocate memory from the pool, marks the memory pool 
					bit as used.
		Input        : const size_t i_size
		Output       : 
		Return Value : void *  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void * MemoryPool::Allocate(const size_t i_size)
	{
		unsigned long ulFirstFreeBit;

		//assert(i_size < (m_ulSize * m_ulNumOfItems));
		assert(i_size == m_ulSize);
		ulFirstFreeBit = m_BitArray->GetAndSetFirstFreeBitIndex();
		
		if (-1 == ulFirstFreeBit)
		{
			CONSOLE_PRINT("Memory full, cannot find free bit");
			return NULL;
		}
		void *ptr = m_pMemoryPool + ulFirstFreeBit * m_ulSize;
		return ptr;

	//this Memory pool doesnt work for array allocation
	#if 0
		if( 1 == (i_size / m_ulSize ))
		{
			assert(i_size == m_ulSize);
			ulFirstFreeBit = m_BitArray->GetAndSetFirstFreeBitIndex();
		
			if (-1 == ulFirstFreeBit)
			{
				CONSOLE_PRINT("Memory full, cannot find free bit");
				return NULL;
			}

			return m_pMemoryPool + ulFirstFreeBit * i_size;
		}
		else
		{
			ulInputNumItems = (i_size / m_ulSize);
			ulFirstFreeBit = m_BitArray->GetAndSetFirstFreeBitIndex();
			
			unsigned long ulCount;

			for(ulCount = ulFirstFreeBit; ulCount < (m_ulNumOfItems - ulInputNumItems); ulCount++)
			{
				unsigned long ulInnerCountCount;
				for( ulInnerCountCount= ulCount; ulInnerCountCount < ulInputNumItems; ulInnerCountCount++)
				{
					if (true == m_BitArray->CheckBit(ulCount))
					{
						break;
					}
				}

				if(ulInnerCountCount != ulInputNumItems)
				{
					continue;
				}

				for(ulInnerCountCount= ulCount; ulInnerCountCount < ulInputNumItems; ulInnerCountCount++)
				{
					m_BitArray->SetBit(ulInnerCountCount);
				}

				return m_pMemoryPool + ulCount * i_size;
			}
		}

		CONSOLE_PRINT("Memory full, cannot find free bit for the array");
		return NULL;
	#endif

	}

	/******************************************************************************
		Function     : DeAllocate
		Description  : Function to deallocate the memory, set the used bit to free
		Input        : const void *pPointer
		Output       : 
		Return Value : void  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void MemoryPool::DeAllocate(const void *pPointer)
	{
		assert(pPointer !=NULL);

		for (unsigned long ulCount = 0; ulCount < m_ulNumOfItems; ulCount++)
		{
			if(pPointer == (m_pMemoryPool + ulCount * m_ulSize))
			{
				m_BitArray->ClearSetBit(ulCount);
				break;
			}
		}

		return;
	}
}