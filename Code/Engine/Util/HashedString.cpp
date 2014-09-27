#include "PreCompiled.h"
#include "HashedString.h"

namespace Engine
{
	/******************************************************************************
	Function     : Hash
	Description  : Function to hash input string
	Input        : const char * i_String
	Output       : Hashed value
	Return Value : unsigned int

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	unsigned int HashedString::Hash(const char * i_String)
	{
		assert(i_String);

		return Hash(static_cast<const void *>(i_String), strlen(i_String));
	}

	/******************************************************************************
	Function     : Hash
	Description  : Function to hash input bytes
	Input        : const void * i_bytes, const unsigned int i_bytecount
	Output       : Hashed value
	Return Value : unsigned int

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	unsigned int HashedString::Hash(const void * i_bytes, const unsigned int i_bytecount)
	{
		// FNV hash, http://isthe.com/chongo/tech/comp/fnv/

		register const unsigned char * p = static_cast<const unsigned char *>( i_bytes );
		unsigned int hash = 2166136261;

		for( unsigned int i = 0; i < i_bytecount; ++i )
			hash = 16777619 * (hash ^ p[i]);

		return hash ^ (hash >> 16);
	}
}