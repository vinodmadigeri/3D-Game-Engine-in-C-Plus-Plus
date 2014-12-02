
#ifndef __SPRITES_H
#define __SPRITES_H

#include "Precompiled.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "MeshData.h"
#include "../UserSettings/UserSettings.h"

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
		unsigned int m_MaxHorizontalCount;
		unsigned int m_MaxVerticalCount;

		SpriteDrawInfo(){}

		SpriteDrawInfo(const sRectangle &i_position, const sRectangle &i_texcoords, unsigned int i_MaxHorizontalCount = 1, unsigned int i_MaxVerticalCount = 1) :
			m_PrimitiveType(D3DPT_TRIANGLESTRIP), 
			m_indexOfFirstVertexToRender(0), 
			m_PrimitiveCount(2), 
			m_NumOfVertices(4), 
			m_VertexStride(sizeof(Engine::sVertexData)),
			m_MaxHorizontalCount(i_MaxHorizontalCount), 
			m_MaxVerticalCount(i_MaxVerticalCount)
		{
			assert(m_MaxHorizontalCount > 0 && m_MaxVerticalCount > 0);

			float Texcoordleft = i_texcoords.left / i_MaxHorizontalCount;
			float Texcoordright = i_texcoords.right / i_MaxHorizontalCount;
			float Texcoordtop = i_texcoords.top / i_MaxVerticalCount;
			float Texcoordbottom = i_texcoords.bottom / i_MaxVerticalCount;

			float left = i_position.left;
			float right = i_position.right;
			float top = i_position.top;
			float bottom = i_position.bottom;

			//Adjust the Sprite height and width based on aspect ratio
			float AspectRatio = static_cast<float>(UserSettings::GetWidth()) / static_cast<float>(UserSettings::GetHeight());

			if (AspectRatio > 0.0f)
			{
				left = left / AspectRatio;
				right = right / AspectRatio;
			}
			else
			{
				top = top * AspectRatio;
				bottom = bottom * AspectRatio;
			}

			memset(m_pVerticesData, 0, sizeof(Engine::sVertexData) * m_NumOfVertices);
			
			FillSingleVertexData(left, bottom, Texcoordleft, Texcoordbottom, m_pVerticesData[0]);
			FillSingleVertexData(left, top, Texcoordleft, Texcoordtop, m_pVerticesData[1]);
			FillSingleVertexData(right, bottom, Texcoordright, Texcoordbottom, m_pVerticesData[2]);
			FillSingleVertexData(right, top, Texcoordright, Texcoordtop, m_pVerticesData[3]);
		}

		void FillSingleVertexData(const float ix, const float iy, const float iU, const float iV, Engine::sVertexData & m_pVerticesData)
		{
			m_pVerticesData.x = ix;
			m_pVerticesData.y = iy;
			m_pVerticesData.z = 0.0f;
			m_pVerticesData.U = iU;
			m_pVerticesData.V = iV;
			m_pVerticesData.color = D3DCOLOR_XRGB(0, 0, 0);
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
		SpriteDrawInfo m_spriteDrawInfo;

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


		bool DrawFromSpriteSheet(unsigned int i_HorizontalCount = 0, unsigned int i_VerticalCount = 0);

		static bool CreateSpriteInfo(const sRectangle *i_positionRect, const sRectangle *i_texcoordsRect,
			SpriteDrawInfo &i_spriteDrawIfo, unsigned int i_MaxHorizontalCount = 1, unsigned int i_MaxVerticalCount = 1);

		HRESULT Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

	};
}

#endif //__SPRITES_H