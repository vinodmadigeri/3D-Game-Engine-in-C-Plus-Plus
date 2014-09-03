// Header Files
//=============
#include "PreCompiled.h"
#include "AssetBuilder.h"

AssetBuilder::AssetBuilder()
{
	if (!GetAssetBuilderEnvironmentVariable("AuthoredAssetDir", mAuthoredAssetDir))
	{
		assert(false);
	}
	if (!GetAssetBuilderEnvironmentVariable("BuiltAssetDir", mBuiltAssetDir))
	{
		assert(false);
	}
}

AssetBuilder::AssetBuilder(std::string & i_AuthoredAssetDir, std::string & i_BuiltAssetDir):
	mAuthoredAssetDir(i_AuthoredAssetDir), 
	mBuiltAssetDir(i_BuiltAssetDir)
{

}

AssetBuilder::~AssetBuilder()
{

}

AssetBuilder * AssetBuilder ::Create(void)
{
	std::string AuthoredAssetDir;
	std::string BuiltAssetDir;

	if (!GetAssetBuilderEnvironmentVariable("AuthoredAssetDir", AuthoredAssetDir))
	{
		return NULL;
	}
	if (!GetAssetBuilderEnvironmentVariable("BuiltAssetDir", BuiltAssetDir))
	{
		return NULL;
	}

	return new AssetBuilder(AuthoredAssetDir, BuiltAssetDir);
}


// Function Definitions
//=====================

bool AssetBuilder::BuildAsset(const char* i_relativePath)
{
	// Get the absolute paths to the source and target
	// (The "source" is the authored asset,
	// and the "target" is the built asset that is ready to be used in-game.
	// In this example program we will just copy the source to the target
	// and so the two will be the same,
	// but in a real asset build pipeline the two will usually be different:
	// The source will be in a format that is optimal for authoring purposes
	// and the target will be in a format that is optimal for real-time purposes.)
	const std::string path_source = mAuthoredAssetDir + i_relativePath;
	const std::string path_target = mBuiltAssetDir + i_relativePath;

	// Decide if the target needs to be built
	bool shouldTargetBeBuilt;
	{
		// The simplest reason a target should be built is if it doesn't exist
		bool doesTargetExist;
		{
			if ( !DoesFileExist( path_target.c_str(), doesTargetExist ) )
			{
				return false;
			}
		}
		if ( doesTargetExist )
		{
			// Even if the target exists it may be out-of-date.
			// If the source has been modified more recently than the target
			// then the target should be re-built.
			uint64_t lastWriteTime_source, lastWriteTime_target;
			{
				if ( !GetLastWriteTime( path_source.c_str(), lastWriteTime_source ) ||
					!GetLastWriteTime( path_target.c_str(), lastWriteTime_target ) )
				{
					return false;
				}
			}
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target;
		}
		else
		{
			shouldTargetBeBuilt = true;
		}
	}

	// Build the target if necessary
	if ( shouldTargetBeBuilt )
	{
		// Display a message to the user for each asset
		std::cout << "Building " << path_source << "\n";

		// Create the target directory if necessary
		if ( !CreateDirectoryIfNecessary( path_target ) )
		{
			return false;
		}

		// Copy the source to the target
		if ( !CopyAssetFile( path_source.c_str(), path_target.c_str() ) )
		{
			return false;
		}
	}

	return true;
}

void AssetBuilder::OutputErrorMessage(const char* i_errorMessage, const char* i_optionalFileName)
{
	std::cerr << ( i_optionalFileName ? i_optionalFileName : "Asset Build" ) << ": error: " <<
		i_errorMessage << "\n";
}

// Windows Functions
//------------------

bool AssetBuilder::CopyAssetFile(const char* i_path_source, const char* i_path_target)
{
	const BOOL shouldFailIfTargetFileAlreadyExists = FALSE;
	if ( CopyFile( i_path_source, i_path_target, shouldFailIfTargetFileAlreadyExists ) != FALSE )
	{
		return true;
	}
	else
	{
		OutputErrorMessage( GetLastWindowsError().c_str(), i_path_source );
		return false;
	}
}

bool AssetBuilder::CreateDirectoryIfNecessary(const std::string& i_path)
{
	// If the path is to a file (likely), remove it so only the directory remains
	std::string directory;
	{
		size_t pos_slash = i_path.find_last_of( "\\/" );
		if ( pos_slash != std::string::npos )
		{
			directory = i_path.substr( 0, pos_slash );
		}
		else
		{
			directory = i_path;
		}
	}
	// Get the path in a form Windows likes (without any ".."s)
	const DWORD maxCharacterCount = 512;	// See comments in GetEnvironmentVariable()
	char buffer[maxCharacterCount];
	{
		char** pathIsDirectory = NULL;
		DWORD characterCount = GetFullPathName( directory.c_str(), maxCharacterCount, buffer, pathIsDirectory );
		if ( characterCount > 0 )
		{
			if ( characterCount <= maxCharacterCount )
			{
				// Create the directory
				int result;
				{
					HWND noWindowIsDisplayed = NULL;
					const SECURITY_ATTRIBUTES* useDefaultSecurityAttributes = NULL;
					result = SHCreateDirectoryEx( noWindowIsDisplayed, buffer, useDefaultSecurityAttributes );
				}
				if ( ( result == ERROR_SUCCESS ) ||
					( ( result == ERROR_FILE_EXISTS ) || ( result== ERROR_ALREADY_EXISTS ) ) )
				{
					return true;
				}
				else
				{
					OutputErrorMessage( GetFormattedWindowsError( result ).c_str(), i_path.c_str() );
					return false;
				}
			}
			else
			{
				// If you're seeing this error you will need to increase maxCharacterCount
				std::stringstream errorMessage;
				errorMessage << "The full path of \"" << directory << "\" requires " << characterCount <<
					" characters and the provided buffer only has room for " << maxCharacterCount;
				OutputErrorMessage( errorMessage.str().c_str(), i_path.c_str() );
				return false;
			}
		}
		else
		{
			OutputErrorMessage( GetLastWindowsError().c_str(), i_path.c_str() );
			return false;
		}
	}
}

bool AssetBuilder::DoesFileExist(const char* i_path, bool& o_doesFileExist)
{
	// Try to get information about the file
	WIN32_FIND_DATA fileData;
	HANDLE file = FindFirstFile( i_path, &fileData );
	if ( file != INVALID_HANDLE_VALUE )
	{
		if ( FindClose( file ) != FALSE )
		{
			o_doesFileExist = true;
			return true;
		}
		else
		{
			OutputErrorMessage( GetLastWindowsError().c_str(), i_path );
			return false;
		}
	}
	else
	{
		DWORD errorCode;
		std::string errorMessage = GetLastWindowsError( &errorCode );
		if ( ( errorCode == ERROR_FILE_NOT_FOUND ) || ( errorCode == ERROR_PATH_NOT_FOUND ) )
		{
			o_doesFileExist = false;
			return true;
		}
		else
		{
			OutputErrorMessage( GetLastWindowsError().c_str(), i_path );
			return false;
		}
	}
}

bool AssetBuilder::GetAssetBuilderEnvironmentVariable(const char* i_key, std::string& o_value)
{
	// Windows requires a character buffer
	// to copy the environment variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = 512;
	char buffer[maxCharacterCount];
	// Ask Windows for the environment variable
	const DWORD characterCount = GetEnvironmentVariable( i_key, buffer, maxCharacterCount );
	if ( characterCount > 0 )
	{
		if ( characterCount <= maxCharacterCount )
		{
			o_value = buffer;
			return true;
		}
		else
		{
			// If you're seeing this error you will need to increase maxCharacterCount
			std::stringstream errorMessage;
			errorMessage << "The environment variable \"" << i_key << "\" requires " << characterCount <<
				" characters and the provided buffer only has room for " << maxCharacterCount;
			OutputErrorMessage( errorMessage.str().c_str() );
			return false;
		}
	}
	else
	{
		DWORD errorCode;
		std::string errorString = GetLastWindowsError( &errorCode );
		if ( errorCode == ERROR_ENVVAR_NOT_FOUND )
		{
			// If you're seeing this error and the environment variable is spelled correctly
			// it _probably_ means that you are debugging and haven't set up the environment.
			//	* Right click the project name and choose "Properties"
			//	* In the tree view to the left select "Configuration Properties->Debugging"
			//	* In the "Environment" field make sure that you have this environment variable set like:
			//		* someKey=$(someKey)
			std::string errorMessage = std::string( "The environment variable \"" ) +
				i_key + "\" doesn't exist";
			OutputErrorMessage( errorMessage.c_str() );
		}
		else
		{
			std::string errorMessage = std::string( "Windows failed to get the environment variable \"" ) +
				i_key + "\": " + errorString;
			OutputErrorMessage( errorMessage.c_str() );
		}
		return false;
	}
}

std::string AssetBuilder::GetFormattedWindowsError(const DWORD i_errorCode)
{
	std::string errorMessage;
	{
		const DWORD formattingOptions =
			// Get the error message from Windows
			FORMAT_MESSAGE_FROM_SYSTEM
			// The space for the error message should be allocated by Windows
			| FORMAT_MESSAGE_ALLOCATE_BUFFER
			// Any potentially deferred inserts should be ignored
			// (i.e. the error message should be in its final form)
			| FORMAT_MESSAGE_IGNORE_INSERTS;
		const void* messageIsFromWindows = NULL;
		const DWORD useTheDefaultLanguage = 0;
		char* messageBuffer = NULL;
		const DWORD minimumCharacterCountToAllocate = 1;
		va_list* insertsAreIgnored = NULL;
		const DWORD storedCharacterCount = FormatMessage( formattingOptions, messageIsFromWindows, i_errorCode,
			useTheDefaultLanguage, reinterpret_cast<LPSTR>( &messageBuffer ), minimumCharacterCountToAllocate, insertsAreIgnored );
		if ( storedCharacterCount != 0 )
		{
			errorMessage = messageBuffer;
		}
		else
		{
			// If there's an error GetLastError() can be called again,
			// but that is too complicated for this program :)
			errorMessage = "Unknown Windows Error";
		}
		// Try to free the memory regardless of whether formatting worked or not,
		// and ignore any error messages
		LocalFree( messageBuffer );
	}
	return errorMessage;
}

std::string AssetBuilder::GetLastWindowsError(DWORD* o_optionalErrorCode)
{
	// Windows stores the error as a code
	const DWORD errorCode = GetLastError();
	if ( o_optionalErrorCode )
	{
		*o_optionalErrorCode = errorCode;
	}
	return GetFormattedWindowsError( errorCode );
}

bool AssetBuilder::GetLastWriteTime(const char* i_path, uint64_t& o_lastWriteTime)
{
	// Get the last time that the file was written to
	ULARGE_INTEGER lastWriteTime_int;
	{
		WIN32_FIND_DATA fileData;
		{
			HANDLE file = FindFirstFile( i_path, &fileData );
			if ( file != INVALID_HANDLE_VALUE )
			{
				if ( FindClose( file ) == FALSE )
				{
					OutputErrorMessage( GetLastWindowsError().c_str(), i_path );
					return false;
				}
			}
			else
			{
				OutputErrorMessage( GetLastWindowsError().c_str(), i_path );
				return false;
			}
		}
		FILETIME fileTime = fileData.ftLastWriteTime;
		lastWriteTime_int.HighPart = fileTime.dwHighDateTime;
		lastWriteTime_int.LowPart = fileTime.dwLowDateTime;
	}
	o_lastWriteTime = static_cast<uint64_t>( lastWriteTime_int.QuadPart );
	return true;
}
