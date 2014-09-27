#ifndef __HASHED_STRING_HEADER__
#define __HASHED_STRING_HEADER__
#include "PreCompiled.h"

namespace Engine
{
	class HashedString
	{
#ifdef DEBUG_HASH_STRING
		std::string mString;
#endif
		unsigned int mHash;
		static unsigned int Hash(const void * i_bytes, const unsigned int i_bytecount);
	public:
		HashedString(void);
		HashedString(const char * i_string);
		HashedString(const HashedString &i_other);
		~HashedString(void);

		HashedString &  operator=(const HashedString &i_rhs);
		bool operator==(const HashedString &i_rhs) const;
		unsigned int Get(void) const;
#ifdef DEBUG_HASH_STRING
		std::string HashedString::GetString(void) const;
#endif
		static unsigned int Hash(const char * i_String);

	} ;
}

#include "HashedString.inl"

#endif //__HASHED_STRING_HEADER__