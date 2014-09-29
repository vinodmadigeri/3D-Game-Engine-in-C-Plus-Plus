/*
A material is a collection of shader programs
as well as the data that those shader program use
*/

#ifndef __EAE2014_GRAPHICS_MATERIAL_H
#define __EAE2014_GRAPHICS_MATERIAL_H

// Header Files
//=============
#include "PreCompiled.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "../External/Lua/Includes.h"

#include "Vector3.h"

// Class Declaration
//==================

namespace Engine
{
	class Material
	{
		// Interface
		//==========

	public:
		Material(IDirect3DDevice9 * i_direct3dDevice);
		~Material();

		// Render
		//-------
		HRESULT Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

		// Initialization / Shut Down
		//---------------------------

		bool Load(const char* i_MaterialFilepath, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

		void SetVertexShaderConstantValue(Vector3 i_Value);

		void SetFragmentShaderConstantValue(Vector3 i_Value);

		// Data
		//=====
	private:
		IDirect3DDevice9 * m_direct3dDevice;
		// The vertex shader is a program that operates on vertices.
		// Its input comes from a C/C++ "draw call" and is:
		//	* Position
		//	* Any other data we want
		// Its output is:
		//	* Position
		//		(So that the graphics hardware knows which pixels to fill in for the triangle)
		//	* Any other data we want
		IDirect3DVertexShader9* m_vertexShader;
		// The fragment shader is a program that operates on fragments
		// (or "potential pixels").
		// Its input is:
		//	* The data that was output from the vertex shader,
		//		interpolated based on how close the fragment is
		//		to each vertex in the triangle.
		// Its output is:
		//	* The final color that the pixel should be
		IDirect3DPixelShader9* m_fragmentShader;
		ID3DXConstantTable* m_pvertexShaderConsts;
		D3DXHANDLE m_vertexShaderConstHandle;
		
		ID3DXConstantTable* m_pfragmentShaderConsts;
		D3DXHANDLE m_fragmentShaderConstHandle;
		
		std::string mPathVertexShader;
		std::string mPathFragmentShader;

		bool LoadLuaAsset(const char* i_path
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

		bool LoadTableValues(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadTableValues_Shaders(lua_State& io_luaState, const char* key, std::string& o_PathShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadTableValues_Shader_paths(lua_State& io_luaState, std::string& o_PathShader
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadFragmentShader(const char* i_FragmentShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

		bool LoadVertexShader(const char* i_VertexShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

	};
}

#endif	// __EAE2014_GRAPHICS_MATERIAL_H
