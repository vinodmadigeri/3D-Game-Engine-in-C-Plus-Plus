#ifndef __FRAGMENT_SHADER_BUILDER_H
#define __FRAGMENT_SHADER_BUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace Shader
{
	class cFragmentShaderBuilder : public BuilderHelper::cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build(const std::vector<const std::string>&);
	};
}


#endif //__FRAGMENT_SHADER_BUILDER_H