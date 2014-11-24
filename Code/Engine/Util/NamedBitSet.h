#ifndef __NAMED_BIT_SET_HEADER__
#define __NAMED_BIT_SET_HEADER__
#include "PreCompiled.h"
#include "HashedString.h"

namespace Engine
{

	template<class T>
	class NamedBitSet
	{
		static const unsigned int NumBits = sizeof(T) * 8;

		HashedString mHashedStrings[NumBits];
	public:
		NamedBitSet(void);

		unsigned int GetBitIndex(const char *i_name);
		bool FindBitIndex(const char *i_name, unsigned int & o_BitIndex);
	
		T GetBitMask(const char *i_name);
		bool FindBitMask(const char *i_name, T & o_BitMask);
	} ;
}

#include "NamedBitSet.inl"

#endif //__NAMED_BIT_SET_HEADER__