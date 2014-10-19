#ifndef __VERTEX_SHADER_BUILDER_H
#define __VERTEX_SHADER_BUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace Shader
{
	class cVertexShaderBuilder : public BuilderHelper::cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build(const std::vector<const std::string>&);
	};
}


#endif //__VERTEX_SHADER_BUILDER_H