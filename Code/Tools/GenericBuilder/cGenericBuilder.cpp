// Header Files
//=============

#include "cGenericBuilder.h"

#include <iostream>
#include <sstream>

// Interface
//==========

// Build
//------

bool Generic::cGenericBuilder::Build( const std::vector<const std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		const bool dontFailIfTargetAlreadyExists = false;
		std::string errorMessage;
		if (!BuilderHelper::CopyAssetFile(m_path_source, m_path_target))
		{
			wereThereErrors = true;
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Windows failed to copy \"" << m_path_source << "\" to \"" << m_path_target << "\": " << errorMessage;
			BuilderHelper::OutputErrorMessage(decoratedErrorMessage.str().c_str(), __FILE__);
		}
	}
	
	return !wereThereErrors;
}