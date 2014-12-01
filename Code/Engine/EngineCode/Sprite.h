
#ifndef __SPRITES_H
#define __SPRITES_H

#include "Precompiled.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "MeshData.h"

namespace Engine
{

	struct sRectangle
	{
		float left, right, top, bottom;
		sRectangle(float i_left, float i_right, float i_top, float i_bottom)
			: left(i_left), right(i_right), top(i_top), bottom(i_bottom) {}
	};

	struct SpriteDrawInfo
	{
		D3DPRIMITIVETYPE	m_PrimitiveType;
		UINT				m_indexOfFirstVertexToRender;
		UINT				m_PrimitiveCount;
		UINT				m_NumOfVertices;
		unsigned int		m_VertexStride;
		Engine::sVertexData	m_pVerticesData[4];

		SpriteDrawInfo(){}

		SpriteDrawInfo(const sRectangle &i_position, const sRectangle &i_texcoords):
			m_PrimitiveType(D3DPT_TRIANGLESTRIP), m_indexOfFirstVertexToRender(1), m_PrimitiveCount(2), m_NumOfVertices(6), m_VertexStride(sizeof(Engine::sVertexData))
		{
			memset(m_pVerticesData, 0, sizeof(Engine::sVertexData) * m_NumOfVertices);

			FillSingleVertexData(i_position.left, i_position.top, i_texcoords.left, i_texcoords.top, m_pVerticesData[0]);
			FillSingleVertexData(i_position.right, i_position.bottom, i_texcoords.right, i_texcoords.bottom, m_pVerticesData[1]);
			FillSingleVertexData(i_position.left, i_position.bottom, i_texcoords.left, i_texcoords.bottom, m_pVerticesData[2]);
			FillSingleVertexData(i_position.right, i_position.bottom, i_texcoords.right, i_texcoords.bottom, m_pVerticesData[3]);
			FillSingleVertexData(i_position.left, i_position.top, i_texcoords.left, i_texcoords.top, m_pVerticesData[4]);
			FillSingleVertexData(i_position.right, i_position.top, i_texcoords.right, i_texcoords.top, m_pVerticesData[5]);
		}

		void FillSingleVertexData(const float ix, const float iy, const float iU, const float iV, Engine::sVertexData & m_pVerticesData)
		{
			m_pVerticesData.x = ix;
			m_pVerticesData.y = iy;
			m_pVerticesData.z = 0.0f;
			m_pVerticesData.U = iU;
			m_pVerticesData.V = iV;
		}
	};

}

namespace Engine
{
	class Sprite
	{
	private:
		std::string m_name;

		IDirect3DDevice9 * m_direct3dDevice;

		IDirect3DVertexShader9* m_vertexShader;

		IDirect3DPixelShader9* m_fragmentShader;
		ID3DXConstantTable* m_pfragmentShaderConsts;

		IDirect3DTexture9* m_texture;
		DWORD m_samplerRegister;

		IDirect3DVertexBuffer9* m_vertexBuffer;

		bool LoadFragmentShader(const char* i_FragmentShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadFragmentShaderConstants(const char* i_FragmentShaderpath, ID3DXConstantTable* i_pfragmentShaderConsts, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool LoadVertexShader(const char* i_VertexShaderpath, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);
		bool LoadTextureAndSamplerRegister(const char* iTexturePath, const char* iSamplerName, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

	public:
		SpriteDrawInfo m_spriteDrawIfo;

		Sprite(const char *iTextureName, IDirect3DDevice9 *i_direct3dDevice, IDirect3DVertexBuffer9* i_vertexBuffer, const SpriteDrawInfo i_spriteDrawInfo);

		~Sprite();
		inline std::string GetName() const{ return m_name; }
		inline IDirect3DVertexBuffer9* GetVertexBuffer() const
		{
			assert(m_vertexBuffer);
			return m_vertexBuffer;
		}

		bool Load(const char* i_TexturePath, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

		static bool CreateSpriteInfo(const sRectangle *i_positionRect, const sRectangle *i_texcoordsRect, SpriteDrawInfo &i_spriteDrawIfo);


		HRESULT Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);
	};
}

#endif //__SPRITES_H