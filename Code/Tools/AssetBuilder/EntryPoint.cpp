/*
	The main() function is where the program starts execution
*/

// Header Files
//=============
#include "PreCompiled.h"
#include <cstdlib>
#include "AssetBuilder.h"

// Entry Point
//============

int main( int argumentCount, char** arguments )
{
	std::string RelativeScriptPath = "BuildAssets.lua";
	AssetBuilder * pAssetBuilder = AssetBuilder::Create(RelativeScriptPath);

	assert(pAssetBuilder != NULL);

	bool wereThereErrors = false;

	// The command line should have a list of assets to build
	for ( int i = 1; i < argumentCount; ++i )
	{
		const char* relativePath = arguments[i];
		if (!pAssetBuilder->BuildAsset(relativePath))
		{
			wereThereErrors = true;
		}
	}

	delete pAssetBuilder;

	if ( !wereThereErrors )
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}
