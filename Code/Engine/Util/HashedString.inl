#include "PreCompiled.h"
#include "Debug.h"

namespace Engine
{
	inline HashedString::HashedString(void):
		mHash(Hash(""))
#ifdef DEBUG_HASH_STRING
		, mString("")
#endif
	{

	}

	inline HashedString::HashedString(const char * i_string):
		mHash(Hash(i_string))
#ifdef DEBUG_HASH_STRING
		, mString(i_string)
#endif
	{

	}

	inline HashedString::~HashedString(void)
	{

	}

	inline unsigned int HashedString::Get(void) const
	{
		return mHash;
	}
#ifdef DEBUG_HASH_STRING
	inline std::string HashedString::GetString(void) const
	{
		return mString;
	}
#endif

	inline HashedString::HashedString(const HashedString &i_other):
		mHash(i_other.mHash)
#ifdef DEBUG_HASH_STRING
		, mString(i_other.mString)
#endif
	{

	}

	inline HashedString & HashedString::operator=(const HashedString &i_rhs)
	{
		mHash = i_rhs.mHash;

		#ifdef DEBUG_HASH_STRING
			mString = i_rhs.mString;
		#endif

		return *this;
	}
	
	inline bool HashedString::operator==(const HashedString &i_rhs) const
	{
		bool bRet = false;

		if (mHash == i_rhs.mHash)
			bRet = true;

		#ifdef DEBUG_HASH_STRING
		if (mString.compare(i_rhs.mString) == 0)
		{
			if (bRet != true)
			{
				CONSOLE_PRINT("Two strings are equal, %s = %s", mString.c_str(), i_rhs.mString.c_str());
				assert(false);
			}
		}
		else
		{
			if (bRet == true)
			{
				CONSOLE_PRINT("Two strings are not equal, %s = %s", mString.c_str(), i_rhs.mString.c_str());
				assert(false);
			}
		}
		#endif

		return bRet;
	}
}