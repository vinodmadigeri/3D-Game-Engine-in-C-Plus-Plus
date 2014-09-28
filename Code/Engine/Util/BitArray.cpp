
#include "PreCompiled.h"

#include <bitset>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "BitArray.h"
#include "Debug.h"


namespace Engine
{
	/******************************************************************************
		Function     : BitArray
		Description  : Constructor for BitArray that assigns memory for the bitarray, 
					sets the number of bits to be used and initilizes them to zero.
		Input        : unsigned long *i_pBitArray, unsigned long i_ulItemCount
		Output       : 
		Return Value :   
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	BitArray::BitArray(unsigned long *i_pBitArray, unsigned long i_ulItemCount, bool i_bIsPlacementCreate):
		m_pBitArray(i_pBitArray), m_ulItemCount(i_ulItemCount), bIsPlacementCreate(i_bIsPlacementCreate)
	{
		assert(m_pBitArray != NULL);

		for (unsigned long ulCount = 0; ulCount < ((i_ulItemCount +  SIZE_OF_LONG - 1)/ (SIZE_OF_LONG)); ulCount++)
		{
			m_pBitArray[ulCount] = 0;
		}
	}

	/******************************************************************************
		Function     : ~BitArray
		Description  : Destructor
		Input        : void
		Output       : 
		Return Value :   
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	BitArray::~BitArray(void)
	{
	
	}

	/******************************************************************************
		Function     : Destroy
		Description  : frees the memory allocated for the bitarray based on Placement 
					   new or regular new, Used instead of destructor
		Input        : unsigned long *o_ulOutputLen
		Output       : 
		Return Value :   
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void BitArray::Destroy(unsigned long *o_ulOutputLen)
	{
		if (false == bIsPlacementCreate)
		{
			if(m_pBitArray != NULL)
			{
				_aligned_free(m_pBitArray);
			}

			delete this;
		}
		else
		{
			assert(o_ulOutputLen != NULL);

			*o_ulOutputLen = (sizeof(unsigned long) * (m_ulItemCount +  SIZE_OF_LONG - 1) / (SIZE_OF_LONG)) + sizeof(BitArray);
		
			this->~BitArray();
		}

		return;
	}

	/******************************************************************************
		Function     : Create
		Description  : Static function that allocates memory for the bitarray and 
					creates it of size i_ulItemCount
		Input        : const unsigned long i_ulItemCount
		Output       : 
		Return Value : BitArray *   
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	BitArray * BitArray::Create(const unsigned long i_ulItemCount)
	{
		unsigned long * pNewBitArray = reinterpret_cast<unsigned long *> (_aligned_malloc((sizeof(unsigned long) * (i_ulItemCount +  SIZE_OF_LONG - 1)/ (SIZE_OF_LONG)), 64));
	
		if(NULL == pNewBitArray )
		{
			return NULL;
		}

		bool i_bIsPlacementCreate = false;

		return new BitArray(pNewBitArray, i_ulItemCount, i_bIsPlacementCreate);
	}

	/******************************************************************************
		Function     : CreateFromExistingMemory
		Description  : Static function that creates a bitarray from input memory of 
					size i_ulItemCount
		Input        : const unsigned long i_ulItemCount
		Output       : 
		Return Value : BitArray *   
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	BitArray * BitArray::CreateFromExistingMemory(void * i_MemoryBlock, const unsigned long i_ulItemCount, unsigned long *o_pulUsed)
	{
		if (NULL == i_MemoryBlock )
		{
			return NULL;
		}

		unsigned long * pBitArray = reinterpret_cast<unsigned long *> (reinterpret_cast<uintptr_t>(i_MemoryBlock) + sizeof(BitArray));
	
		*o_pulUsed += (sizeof(unsigned long) * (i_ulItemCount +  SIZE_OF_LONG - 1) / (SIZE_OF_LONG)) + sizeof(BitArray);
		bool i_bIsPlacementCreate = true;
		return new (i_MemoryBlock) BitArray(pBitArray, i_ulItemCount, i_bIsPlacementCreate);
	}

	/******************************************************************************
		Function     : operator[]
		Description  : Overloades the [] operator for accessing bits like bytes
		Input        : const unsigned long ulItemIndex
		Output       : 
		Return Value : bool  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	bool BitArray::operator[](const unsigned long ulItemIndex)
	{
		unsigned long ulInnerBit;
		unsigned long ucLongFromBitArray;
		unsigned long ForShifting = 0x00000001;
		
		ucLongFromBitArray = m_pBitArray[ulItemIndex / SIZE_OF_LONG];
		
		ulInnerBit = ulItemIndex % SIZE_OF_LONG;

		ForShifting = ForShifting << ulInnerBit;

		if(ucLongFromBitArray == (ucLongFromBitArray & ForShifting))
		{
			return true;
		}

		return false;
	}

	/******************************************************************************
		Function     : GetFirstFreeBitIndex
		Description  : Returns the first free bit in the bit array
		Input        : void
		Output       : 
		Return Value : long  (-1 if failed)
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	long BitArray::GetFirstFreeBitIndex(void) const
	{
		unsigned long ulLongFreeIndex = 0;
		unsigned long ulNumberOfLongs = (m_ulItemCount +  SIZE_OF_LONG - 1) / (SIZE_OF_LONG);
		unsigned long ulFirstFreeBitIndex = 0;
		unsigned long ForShifting = 0x00000001;

		while (ulLongFreeIndex < ulNumberOfLongs)
		{
			if (m_pBitArray[ulLongFreeIndex] != FULL_LONG_VALUE)
			{
				break;
			}
			
			ulLongFreeIndex++;
		}

		if(ulLongFreeIndex == ulNumberOfLongs)
		{
			CONSOLE_PRINT("No free bit, memory pool is full");
			return -1;
		}

		for (unsigned long ulCount = 0; ulCount < SIZE_OF_LONG; ulCount++)
		{
			if (0x00000000 == (m_pBitArray[ulLongFreeIndex] & (ForShifting << ulCount)))
			{
				ulFirstFreeBitIndex = ulCount;
				break;
			}
		}

		ulFirstFreeBitIndex += ulLongFreeIndex * SIZE_OF_LONG;

		return ulFirstFreeBitIndex;
	}

	/******************************************************************************
		Function     : SetBit
		Description  : Set the bit at the input bitindex as true.
		Input        : const unsigned long ulBitIndex
		Output       : 
		Return Value : void  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void BitArray::SetBit(const unsigned long ulBitIndex)
	{
		unsigned long ulInnerIndex;
		unsigned long ForShifting = 0x000000001;
		unsigned long AfterShifting;
		unsigned long ulByteIndex = ulBitIndex / SIZE_OF_LONG;
		
		ulInnerIndex = ulBitIndex % SIZE_OF_LONG;
		AfterShifting = (ForShifting << ulInnerIndex);
		m_pBitArray[ulByteIndex] |= AfterShifting;

		return;
	}

	/******************************************************************************
		Function     : CheckBit
		Description  : Returns true or false if the bit is set or unset.
		Input        : const unsigned long ulBitIndex
		Output       : 
		Return Value : bool  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	bool BitArray::CheckBit(const unsigned long ulBitIndex) const
	{
		unsigned long ulInnerIndex;
		unsigned long ForShifting = 0x000000001;
		ulInnerIndex = ulBitIndex % SIZE_OF_LONG;

		if ((ForShifting << ulInnerIndex) == (m_pBitArray[ulBitIndex / SIZE_OF_LONG] & (ForShifting << ulInnerIndex)))
		{
			return true;
		}

		return false;
	}

	/******************************************************************************
		Function     : RotateLeftShift
		Description  : Rotate Left shift operation
		Input        : unsigned long ulToRotate, const unsigned long ulNumberofShifts
		Output       : 
		Return Value : unsigned long  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	unsigned long BitArray::RotateLeftShift(unsigned long ulToRotate, const unsigned long ulNumberofShifts)
	{
		unsigned long ulMostSignificantBit;

		for (unsigned long ulCount = 0; ulCount < ulNumberofShifts; ulCount++)
		{
			//Get the most significant bit from ulToRotate but right shifting 32 -1 times
			ulMostSignificantBit = ulToRotate >> (SIZE_OF_LONG - 1);
			ulToRotate = ulToRotate << 1;
			ulToRotate = ulToRotate + ulMostSignificantBit;
		}

		return ulToRotate;

	}

	/******************************************************************************
		Function     : ClearSetBit
		Description  : Clear the set bit at input ulBitIndex
		Input        : const unsigned long ulBitIndex
		Output       : 
		Return Value : void  
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void BitArray::ClearSetBit(const unsigned long ulBitIndex)
	{
		unsigned long ulInnerIndex;
		unsigned long ForShifting = 0xFFFFFFFE; //Last bit is zero and hence use RotateLeftShift
		unsigned long ulByteIndex = ulBitIndex / SIZE_OF_LONG;
		ulInnerIndex = ulBitIndex % SIZE_OF_LONG;

		m_pBitArray[ulByteIndex] = m_pBitArray[ulByteIndex] & (RotateLeftShift(ForShifting, ulInnerIndex));

		return;
	}

	/******************************************************************************
		Function     : GetAndSetFirstFreeBitIndex
		Description  : Return the first free bit index after setting it used
		Input        : void
		Output       : 
		Return Value : long  (-1 if failed)
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	long BitArray::GetAndSetFirstFreeBitIndex(void)
	{
		unsigned long ulFirstFreeLongIndex = 0;
		unsigned long ulNumberOfLongs = (m_ulItemCount + SIZE_OF_LONG - 1) / (SIZE_OF_LONG);
		unsigned long ulFirstFreeBitIndex = 0;
		unsigned long ForShifting = 0x00000001;

		//Traverse the first byte in bitarray with free bit
		while (ulFirstFreeLongIndex < ulNumberOfLongs)
		{
			if (m_pBitArray[ulFirstFreeLongIndex] != FULL_LONG_VALUE)
			{
				break;
			}
			
			ulFirstFreeLongIndex++;
		}

		if(ulFirstFreeLongIndex == ulNumberOfLongs)
		{
			CONSOLE_PRINT("No free bit, memory pool is full");
			return -1;
		}

		for (unsigned long ulCount = 0; ulCount < SIZE_OF_LONG; ulCount++)
		{
			if (0x00000000 == (m_pBitArray[ulFirstFreeLongIndex] & (ForShifting << ulCount)))
			{
				ulFirstFreeBitIndex = ulCount;
				break;
			}
		}
		SetBit(ulFirstFreeBitIndex);

		ulFirstFreeBitIndex += ulFirstFreeLongIndex * SIZE_OF_LONG;

		return ulFirstFreeBitIndex;
	}

	/******************************************************************************
		Function     : GetFirstSetBitIndex
		Description  : Return the first set bit index
		Input        : void
		Output       : 
		Return Value : long  (-1 if failed)
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	long BitArray::GetFirstSetBitIndex(void) const
	{
		unsigned long ulLongSetIndex = 0;
		unsigned long ulNumberOfLongs = (m_ulItemCount + SIZE_OF_LONG - 1) / (SIZE_OF_LONG);
		unsigned long ulFirstSetBitIndex = 0;
		unsigned long ForShifting = 0x00000001;

		while (ulLongSetIndex < ulNumberOfLongs)
		{
			if (m_pBitArray[ulLongSetIndex] != 0x00000000)
			{
				break;
			}
			
			ulLongSetIndex++;
		}

		if(ulLongSetIndex == ulNumberOfLongs)
		{
			CONSOLE_PRINT("No bit is set, memory pool is empty");
			return -1;
		}

		for (unsigned long ulCount = 0; ulCount < SIZE_OF_LONG; ulCount++)
		{
			if (m_pBitArray[ulLongSetIndex] == (m_pBitArray[ulLongSetIndex] & (ForShifting << ulCount)))
			{
				ulFirstSetBitIndex = ulCount;
				break;
			}
		}

		//Convert the bit local index to final index by adding the full byte count
		ulFirstSetBitIndex += ulLongSetIndex * SIZE_OF_LONG;

		return ulFirstSetBitIndex;
	}

	/******************************************************************************
		Function     : GetAndClearFirstSetBitIndex
		Description  : Return the first set bit index after clearing it.
		Input        : void
		Output       : 
		Return Value : long  (-1 if failed)
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	long BitArray::GetAndClearFirstSetBitIndex(void)
	{
		unsigned long ulLongSetIndex = 0;
		unsigned long ulNumberOfLongs = (m_ulItemCount + SIZE_OF_LONG - 1) / (SIZE_OF_LONG);
		unsigned long ulFirstSetBitIndex = 0;
		unsigned long ForShifting = 0x00000001;

		while (ulLongSetIndex < ulNumberOfLongs)
		{
			if (m_pBitArray[ulLongSetIndex] != 0x00000000)
			{
				break;
			}
			
			ulLongSetIndex++;
		}

		if(ulLongSetIndex == ulNumberOfLongs)
		{
			CONSOLE_PRINT("No bit is set, memory pool is empty");
			return -1;
		}

		for (unsigned long ulCount = 0; ulCount < SIZE_OF_LONG; ulCount++)
		{
			if (m_pBitArray[ulLongSetIndex] == (m_pBitArray[ulLongSetIndex] & (ForShifting << ulCount)))
			{
				ulFirstSetBitIndex = ulCount;
				break;
			}
		}

		ClearSetBit(ulFirstSetBitIndex);

		ulFirstSetBitIndex += ulLongSetIndex * SIZE_OF_LONG;

		return ulFirstSetBitIndex;
	}

	/******************************************************************************
		Function     : ShowAllIndex
		Description  : Function to display the bits in bitarray
		Input        : void
		Output       : 
		Return Value : void
		Data Accessed: 
		Data Updated : 
 
		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void BitArray::ShowAllIndex(void) const
	{
		unsigned long ulLongSetIndex = 0;
		unsigned long ulNumberOfLongs = (m_ulItemCount + SIZE_OF_LONG - 1) / (SIZE_OF_LONG);

		while (ulLongSetIndex < ulNumberOfLongs)
		{
			unsigned long long n = m_pBitArray[0];
			std::bitset<32> x(n);
			std::cout<< x <<"\n";
			ulLongSetIndex++;
		}

		return;
	}
}