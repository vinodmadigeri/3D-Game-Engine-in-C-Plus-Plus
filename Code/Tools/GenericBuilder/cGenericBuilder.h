/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_CGENERICBUILDER_H
#define EAE6320_CGENERICBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace eae6320
{
	class cGenericBuilder : public cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build( const std::vector<const std::string>& );
	};
}

#endif	// EAE6320_CGENERICBUILDER_H
