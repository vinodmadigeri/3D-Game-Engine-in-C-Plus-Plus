// Header Files
//=============

#include "cGenericBuilder.h"

#include <iostream>
#include <sstream>

// Interface
//==========

// Build
//------

bool eae6320::cGenericBuilder::Build( const std::vector<const std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		const bool dontFailIfTargetAlreadyExists = false;
		std::string errorMessage;
		if (!BuildAsset())
		{
			wereThereErrors = true;
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Windows failed to copy \"" << m_path_source << "\" to \"" << m_path_target << "\": " << errorMessage;
			eae6320::OutputErrorMessage( decoratedErrorMessage.str().c_str(), __FILE__ );
		}
	}
	
	return !wereThereErrors;
}

bool eae6320::cGenericBuilder::BuildAsset(void)
{
	// Get the absolute paths to the source and target
	// (The "source" is the authored asset,
	// and the "target" is the built asset that is ready to be used in-game.
	// In this example program we will just copy the source to the target
	// and so the two will be the same,
	// but in a real asset build pipeline the two will usually be different:
	// The source will be in a format that is optimal for authoring purposes
	// and the target will be in a format that is optimal for real-time purposes.)
	const std::string path_source = m_path_source;
	const std::string path_target = m_path_target;

	// Decide if the target needs to be built
	bool shouldTargetBeBuilt;
	{
		// The simplest reason a target should be built is if it doesn't exist
		bool doesTargetExist;
		{
			DWORD errorCode;
			std::string errorMessage;

			if (!DoesFileExist(path_target.c_str(), &errorMessage, &errorCode))
			{
				if ((errorCode == ERROR_FILE_NOT_FOUND) || (errorCode == ERROR_PATH_NOT_FOUND))
				{
					doesTargetExist = false;
				}
				else
				{
					OutputErrorMessage(errorMessage.c_str(), path_target.c_str());
					return false;
				}
			}
			else
			{
				doesTargetExist = true;
			}
		}

		if (doesTargetExist)
		{
			// Even if the target exists it may be out-of-date.
			// If the source has been modified more recently than the target
			// then the target should be re-built.
			uint64_t lastWriteTime_source, lastWriteTime_target;
			{
				if (!GetLastWriteTime(path_source.c_str(), lastWriteTime_source) ||
					!GetLastWriteTime(path_target.c_str(), lastWriteTime_target))
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
	if (shouldTargetBeBuilt)
	{
		// Display a message to the user for each asset
		std::cout << "Building " << path_source << "\n";
		std::string errorMessage;

		// Create the target directory if necessary
		if (!CreateDirectoryIfNecessary(path_target))
		{
			return false;
		}

		// Copy the source to the target
		if (!CopyAssetFile(path_source.c_str(), path_target.c_str(), false, &errorMessage))
		{
			return false;
		}
	}

	return true;
}
