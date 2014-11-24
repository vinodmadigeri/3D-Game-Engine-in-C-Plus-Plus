
#include "PreCompiled.h"

namespace Engine
{
	template<class T>
	NamedBitSet<T>::NamedBitSet(void)
	{

	}

	template<class T>
	unsigned int NamedBitSet<T>::GetBitIndex(const char *i_name)
	{
		assert(i_name);

		for(unsigned int count = 0; count < NumBits; count++)
		{
			if (mHashedStrings[count] == i_name)
			{
				return count;
			}
		}

		HashedString Empty;

		for (unsigned int count = 0; count < NumBits; count++)
		{
			if (mHashedStrings[count] == Empty)
			{
				mHashedStrings[count] = i_name;
				return count;
			}
		}

		assert(false);

		return 0;
	}

	template<class T>
	bool NamedBitSet<T>::FindBitIndex(const char *i_name, unsigned int & o_BitIndex)
	{
		assert(i_name);

		for (unsigned int count = 0; count < NumBits; count++)
		{
			if(mHashedStrings[count] == i_name)
			{
				o_BitIndex = count;
				return true;
			}
		}

		return false;
	}
	
	template<class T>
	T NamedBitSet<T>::GetBitMask(const char *i_name)
	{
		assert(i_name);

		return 1 << GetBitIndex(i_name);
	}
	
	template<class T>
	bool NamedBitSet<T>::FindBitMask(const char *i_name, T & o_BitMask)
	{
		unsigned int BitIndex = 0;
		
		if ( true == FindBitIndex(i_name, BitIndex))
		{
			o_BitMask = 1 << BitIndex;
			return true;
		}

		return false;
	}
}