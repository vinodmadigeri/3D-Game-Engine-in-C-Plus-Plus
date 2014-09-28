#ifndef __BIT_ARRAY_HEADER
#define __BIT_ARRAY_HEADER

#include "PreCompiled.h"

#include <stdlib.h>

#define BYTE_LENGTH 8
#define SIZE_OF_LONG  (sizeof(unsigned long) * BYTE_LENGTH)
#define	FULL_LONG_VALUE 0xffffffff

namespace Engine
{

	class BitArray
	{
		unsigned long m_ulItemCount;
		unsigned long * m_pBitArray;
		bool bIsPlacementCreate;
		BitArray(unsigned long *i_pBitArray, unsigned long i_ulItemCount, bool i_bIsPlacementCreate);
		~BitArray();
		unsigned long RotateLeftShift(unsigned long ulToRotate, unsigned long ulNumberofShifts);

	public:

		void Destroy(unsigned long *o_ulOutputLen);
		static BitArray * Create(const unsigned long i_ulItemCount);
		static BitArray * CreateFromExistingMemory(void * i_MemoryBlock, const unsigned long i_ulItemCount, unsigned long *o_pulUsed);
		bool CheckBit(const unsigned long ulBitIndex) const;
		void SetBit(const unsigned long ulBitIndex);
		void ClearSetBit(const unsigned long ulBitIndex);
		long GetFirstFreeBitIndex(void) const;
		long GetAndSetFirstFreeBitIndex(void);
		long GetFirstSetBitIndex(void) const;
		long GetAndClearFirstSetBitIndex(void);
		void ShowAllIndex(void) const;
		bool operator[] (unsigned long ulItemIndex);
	};
}

#endif //__BIT_ARRAY_HEADER