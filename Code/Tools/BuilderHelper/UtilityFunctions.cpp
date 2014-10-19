// Header Files
//=============

#include "UtilityFunctions.h"

#include <iostream>
#include <ShlObj.h>
#include <sstream>

// Interface
//==========

bool BuilderHelper::CopyAssetFile(const char* i_path_source, const char* i_path_target,
	const bool i_shouldFunctionFailIfTargetAlreadyExists, std::string* o_errorMessage )
{
	if ( ::CopyFile( i_path_source, i_path_target, i_shouldFunctionFailIfTargetAlreadyExists ) != FALSE )
	{
		return true;
	}
	else
	{
		if ( o_errorMessage )
		{
			*o_errorMessage = GetLastWindowsError();
		}
		return false;
	}
}

bool BuilderHelper::CreateDirectoryIfNecessary(const std::string& i_path, std::string* o_errorMessage)
{
	// If the path is to a file (likely), remove it so that only the directory remains
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
	// Get the path in a form Windows likes (without any ".."s).
	// Windows requires a character buffer
	// to copy the path variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = MAX_PATH;
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
				if ( result == ERROR_SUCCESS )
				{
					std::cout << "Created directory " << buffer << "\n";
					return true;
				}
				else if ( ( result == ERROR_FILE_EXISTS ) || ( result== ERROR_ALREADY_EXISTS ) )
				{
					return true;
				}
				else
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = BuilderHelper::GetFormattedWindowsError(result);
					}
					return false;
				}
			}
			else
			{
				// If you're seeing this error you will need to increase maxCharacterCount
				if ( o_errorMessage )
				{
					std::stringstream errorMessage;
					errorMessage << "The full path of \"" << directory << "\" requires " << characterCount <<
						" characters and the provided buffer only has room for " << maxCharacterCount;
					*o_errorMessage = errorMessage.str();
				}
				return false;
			}
		}
		else
		{
			if ( o_errorMessage )
			{
				*o_errorMessage = BuilderHelper::GetLastWindowsError();
			}
			return false;
		}
	}
}

bool BuilderHelper::DoesFileExist( const char* i_path, std::string* o_errorMessage, DWORD* o_errorCode )
{
	// Try to get information about the file
	WIN32_FIND_DATA fileData;
	HANDLE file = FindFirstFile( i_path, &fileData );
	if ( file != INVALID_HANDLE_VALUE )
	{
		if ( FindClose( file ) == FALSE )
		{
			std::stringstream errorMessage;
			errorMessage << "Windows couldn't close the file handle to \"" << i_path << "\": " << GetLastWindowsError();
			OutputErrorMessage( errorMessage.str().c_str() );
		}
		return true;
	}
	else
	{
		if ( o_errorMessage )
		{
			*o_errorMessage = BuilderHelper::GetLastWindowsError( o_errorCode );
		}
		return false;
	}
}

bool BuilderHelper::GetEnvironmentVariable( const char* i_key, std::string& o_value, std::string* o_errorMessage )
{
	// Windows requires a character buffer
	// to copy the environment variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = MAX_PATH;
	char buffer[maxCharacterCount];
	// Ask Windows for the environment variable
	const DWORD characterCount = ::GetEnvironmentVariable( i_key, buffer, maxCharacterCount );
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
			if ( o_errorMessage )
			{
				std::stringstream errorMessage;
				errorMessage << "The environment variable \"" << i_key << "\" requires " << characterCount <<
					" characters and the provided buffer only has room for " << maxCharacterCount;
				*o_errorMessage = errorMessage.str();
			}
			return false;
		}
	}
	else
	{
		if ( o_errorMessage )
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
				std::stringstream errorMessage;
				errorMessage << "The environment variable \"" << i_key << "\" doesn't exist";
				*o_errorMessage = errorMessage.str();
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to get the environment variable \"" << i_key << "\": " << errorString;
				*o_errorMessage = errorMessage.str();
			}
		}
		return false;
	}
}

std::string BuilderHelper::GetFormattedWindowsError( const DWORD i_errorCode )
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

std::string BuilderHelper::GetLastWindowsError( DWORD* o_optionalErrorCode )
{
	// Windows stores the error as a code
	const DWORD errorCode = GetLastError();
	if ( o_optionalErrorCode )
	{
		*o_optionalErrorCode = errorCode;
	}
	return GetFormattedWindowsError( errorCode );
}

bool BuilderHelper::GetLastWriteTime( const char* i_path, uint64_t& o_lastWriteTime, std::string* o_errorMessage )
{
	// Get the last time that the file was written to
	ULARGE_INTEGER lastWriteTime;
	{
		WIN32_FIND_DATA fileData;
		{
			HANDLE file = FindFirstFile( i_path, &fileData );
			if ( file != INVALID_HANDLE_VALUE )
			{
				if ( FindClose( file ) == FALSE )
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = GetLastWindowsError();
					}
					return false;
				}
			}
			else
			{
				if ( o_errorMessage )
				{
					*o_errorMessage = GetLastWindowsError();
				}
				return false;
			}
		}
		FILETIME fileTime = fileData.ftLastWriteTime;
		lastWriteTime.HighPart = fileTime.dwHighDateTime;
		lastWriteTime.LowPart = fileTime.dwLowDateTime;
	}
	o_lastWriteTime = static_cast<uint64_t>( lastWriteTime.QuadPart );
	return true;
}

void BuilderHelper::OutputErrorMessage( const char* i_errorMessage, const char* i_optionalFileName )
{
	std::cerr << ( i_optionalFileName ? i_optionalFileName : "Asset Build" ) <<
		": error: " << i_errorMessage << "\n";
}

bool BuilderHelper::ExecuteCommand(const char* i_command, DWORD* o_exitCode, std::string* o_errorMessage)
{
	return ExecuteCommand(NULL, i_command, o_exitCode, o_errorMessage);
}


bool BuilderHelper::ExecuteCommand(const char* i_command, const char* i_optionalArguments, DWORD* o_exitCode, std::string* o_errorMessage)
{
	bool wereThereErrors = false;

	// Copy the const arguments into a non-const buffer
	const DWORD bufferSize = 512;
	char arguments[bufferSize] = { 0 };
	if (i_optionalArguments)
	{
		size_t argumentLength = strlen(i_optionalArguments);
		if (bufferSize > argumentLength)
		{
			strcpy_s(arguments, bufferSize, i_optionalArguments);
		}
		else
		{
			if (o_errorMessage)
			{
				std::ostringstream errorMessage;
				errorMessage << "Couldn't copy the command (of length " << argumentLength
					<< ") into a non-const buffer of size " << bufferSize;
				*o_errorMessage = errorMessage.str();
			}
			return false;
		}
	}

	// Start a new process
	SECURITY_ATTRIBUTES* useDefaultAttributes = NULL;
	const BOOL dontInheritHandles = FALSE;
	const DWORD createDefaultProcess = 0;
	void* useCallingProcessEnvironment = NULL;
	const char* useCallingProcessCurrentDirectory = NULL;
	STARTUPINFO startupInfo = { 0 };
	{
		startupInfo.cb = sizeof(startupInfo);
	}

	PROCESS_INFORMATION processInformation = { 0 };

	if (CreateProcess(i_command, arguments, useDefaultAttributes, useDefaultAttributes,
		dontInheritHandles, createDefaultProcess, useCallingProcessEnvironment, useCallingProcessCurrentDirectory,
		&startupInfo, &processInformation) != FALSE)
	{
		// Wait for the process to finish
		if (WaitForSingleObject(processInformation.hProcess, INFINITE) != WAIT_FAILED)
		{
			// Get the exit code
			if (o_exitCode)
			{
				if (GetExitCodeProcess(processInformation.hProcess, o_exitCode) == FALSE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the exit code of the process \"" << i_command <<
							"\": " << GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
				}
			}
		}
		else
		{
			wereThereErrors = true;
			if (o_errorMessage)
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to wait for the process \"" << i_command <<
					"\" to finish: " << GetLastWindowsError();
				*o_errorMessage = errorMessage.str();
			}
		}

		// Close the process handles
		if (CloseHandle(processInformation.hProcess) == FALSE)
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to close the handle to the process \"" << i_command <<
				"\": " << GetLastWindowsError();
			MessageBox(NULL, errorMessage.str().c_str(), "Error Closing Process Handle", MB_OK | MB_ICONERROR);
		}

		if (CloseHandle(processInformation.hThread) == FALSE)
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to close the handle to the process \"" << i_command <<
				"\" thread: " << GetLastWindowsError();
			MessageBox(NULL, errorMessage.str().c_str(), "Error Closing Process Thread Handle", MB_OK | MB_ICONERROR);
		}

		return !wereThereErrors;
	}
	else
	{
		if (o_errorMessage)
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to start the process \"" << i_command << "\": " << GetLastWindowsError();
			*o_errorMessage = errorMessage.str();
		}
		return false;
	}
}