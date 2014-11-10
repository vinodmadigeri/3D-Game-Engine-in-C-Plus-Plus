
#ifndef __CTEXTUREBUILDER_H
#define __CTEXTUREBUILDER_H

// Header Files
//=============
#include <d3dx9tex.h>
#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace Texture
{
	class cTextureBuilder : public BuilderHelper::cbBuilder
	{
		// Interface
		//==========
		static IDirect3D9* s_direct3dInterface;
		static IDirect3DDevice9* s_direct3dDevice;
		static IDirect3DBaseTexture9* s_texture;

	public:

		// Build
		//------

		virtual bool Build( const std::vector<const std::string>& );
		bool Initialize(const char* i_path_source);
		bool ShutDown();
	};
}

#endif	// __CTEXTUREBUILDER_H
