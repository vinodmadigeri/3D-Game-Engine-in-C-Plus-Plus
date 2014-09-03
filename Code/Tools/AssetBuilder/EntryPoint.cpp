/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include <cstdlib>
#include "HelperFunctions.h"

// Entry Point
//============

int main( int argumentCount, char** arguments )
{
	if ( !InitializeAssetBuild() )
	{
		return EXIT_FAILURE;
	}

	bool wereThereErrors = false;

	// The command line should have a list of assets to build
	for ( int i = 1; i < argumentCount; ++i )
	{
		const char* relativePath = arguments[i];
		if ( !BuildAsset( relativePath ) )
		{
			wereThereErrors = true;
		}
	}

	if ( !wereThereErrors )
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}
