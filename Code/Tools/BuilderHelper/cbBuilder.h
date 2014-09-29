/*
	This is the base class for builders of specific asset types
	(meaning that MeshBuilder, MaterialBuilder, TextureBuilder, etc.,
	should all derive from this base class)
*/

#ifndef EAE6320_CBBUILDER_H
#define EAE6320_CBBUILDER_H

// Header Files
//=============

#include <cstdlib>
#include <string>
#include <vector>

// This file doesn't need to #include UtilityFunctions.h,
// but doing so makes the utility functions available
// for every builder, which is nice
#include "UtilityFunctions.h"

// Class Declaration
//==================

namespace eae6320
{
	// The only thing a specific builder project's main() entry point should do
	// is to call the following function with the derived builder class
	// as the template argument:
	template<class builder_t>
	int Build( char** i_arguments, const unsigned int i_argumentCount )
	{
		builder_t builder;
		return builder.ParseCommandArgumentsAndBuild( i_arguments, i_argumentCount ) ?
			EXIT_SUCCESS : EXIT_FAILURE;
	}

	class cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		// The following function will be called from the Build<> templated function above
		// with the command line arguments directly from the main() entry point:
		bool ParseCommandArgumentsAndBuild( char** i_arguments, const unsigned int i_argumentCount );
		// And that function will extract the source and target paths
		// and then call this function in the derived class with any remaining (optional) arguments:
		virtual bool Build( const std::vector<const std::string>& i_optionalArguments ) = 0;

		// Initialization / Shut Down
		//---------------------------

		cbBuilder();

		// Inheritable Data
		//=================

	protected:

		const char* m_path_source;
		const char* m_path_target;
	};
}

#endif	// EAE6320_CBBUILDER_H
