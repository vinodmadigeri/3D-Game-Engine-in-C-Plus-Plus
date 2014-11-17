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
#include <vector>

#include "../External/Lua/Includes.h"

#include "MaterialConstantData.h"
#include "Vector3.h"
#include "HashedString.h"

// Class Declaration
//==================

namespace Engine
{
	class Material
	{
		// Interface
		//==========

	public:
		const std::string mName;
		const HashedString mHashedName;
		Material(const char *iName, IDirect3DDevice9 * i_direct3dDevice);
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

		bool SetPerInstanceConstantDataByName(const char * i_name, const void* i_pValue, const unsigned int i_count);
		bool SetPerViewConstantDataByName(const char * i_name, const void* i_pValue, const unsigned int i_count);
		inline std::string GetName() const { return mName; }
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
		ID3DXConstantTable* m_pvertexShaderConsts;
		// The fragment shader is a program that operates on fragments
		// (or "potential pixels").
		// Its input is:
		//	* The data that was output from the vertex shader,
		//		interpolated based on how close the fragment is
		//		to each vertex in the triangle.
		// Its output is:
		//	* The final color that the pixel should be
		IDirect3DPixelShader9* m_fragmentShader;
		ID3DXConstantTable* m_pfragmentShaderConsts;

		IDirect3DTexture9* m_texture;
		DWORD m_samplerRegister;
		std::vector<IMaterialConstant *> m_perMaterialConstantDatas; //optional
		std::vector<IMaterialConstant *> m_perInstanceConstantDatas; //optional
		std::vector<IMaterialConstant *> m_perViewConstantDatas; 
		
		//==========
		// Lua Logic

		//Constant
		bool LoadConstantDataTable(lua_State& io_luaState, const char* RootConstantTableName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadEachConstantData(lua_State& io_luaState
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadEachConstantDataValue(lua_State& io_luaState, const char * i_ConstantName
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);


		//Shaders
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

		bool LoadTableValues_Texture(lua_State& io_luaState, const char* key, std::string& o_PathTexture, std::string& o_NameSampler
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);
		bool LoadTableValues_TexturePathAndSampler(lua_State& io_luaState, std::string& o_PathTexture, std::string& o_NameSampler
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadTextureAndSamplerRegister(const char* iTexturePath, const char* iSamplerName, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		static bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_compiledShader
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
			
		bool LoadVertexShaderConstants(ID3DXConstantTable* i_pvertexShaderConsts, IDirect3DDevice9 * i_direct3dDevice);

		bool SetPerMaterialConstantDataFromMaterialFile(
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			std::string* o_errorMessage
#endif		
			);

		bool SetConstantDataByType(const char * i_name, const void* i_pValue, IMaterialConstant *io_materialContantData, const unsigned int i_count);
	};
}

#endif	// __EAE2014_GRAPHICS_MATERIAL_H
