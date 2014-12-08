
#ifndef __SPRITES_H
#define __SPRITES_H

#include "Precompiled.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "MeshData.h"
#include "../UserSettings/UserSettings.h"

namespace Engine
{

	struct sSprite
	{
		float left, top;
		float desiredtexturewidth;
		unsigned int horizontalSpriteCount;
		unsigned int verticalSpriteCount;
		sSprite(float i_left, float i_top, float i_desiredtexturewidth, unsigned int i_horizontalSpriteCount = 1, unsigned int i_verticalSpriteCount = 1) :
			left(i_left), top(i_top), desiredtexturewidth(i_desiredtexturewidth), horizontalSpriteCount(i_horizontalSpriteCount), verticalSpriteCount(i_verticalSpriteCount)
		{

		}
	};

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
		unsigned int		m_MaxHorizontalCount;
		unsigned int		m_MaxVerticalCount;

		SpriteDrawInfo(){}

		SpriteDrawInfo(const sSprite &i_spriteDetails, const sRectangle &i_texcoords, const float i_TextureOriginalwidth, const float i_TextureOriginalheight) :
			m_PrimitiveType(D3DPT_TRIANGLESTRIP), 
			m_indexOfFirstVertexToRender(0), 
			m_PrimitiveCount(2), 
			m_NumOfVertices(4), 
			m_VertexStride(sizeof(Engine::sVertexData)),
			m_MaxHorizontalCount(i_spriteDetails.horizontalSpriteCount),
			m_MaxVerticalCount(i_spriteDetails.verticalSpriteCount)
		{
			assert(m_MaxHorizontalCount > 0 && m_MaxVerticalCount > 0);

			float Texcoordleft = i_texcoords.left / i_spriteDetails.horizontalSpriteCount;
			float Texcoordright = i_texcoords.right / i_spriteDetails.horizontalSpriteCount;
			float Texcoordtop = i_texcoords.top / i_spriteDetails.verticalSpriteCount;
			float Texcoordbottom = i_texcoords.bottom / i_spriteDetails.verticalSpriteCount;

			float left = i_spriteDetails.left;
			float right = i_spriteDetails.left + i_spriteDetails.desiredtexturewidth; //Right is left + Desired width
			float top = i_spriteDetails.top;
			//Bottom is top - Adjusted height maintaining the original ratio of the texture resolution
			float bottom = i_spriteDetails.top - i_TextureOriginalheight * i_spriteDetails.desiredtexturewidth / i_TextureOriginalwidth;

			//Adjust the Sprite height and width based on aspect ratio
			float AspectRatio = static_cast<float>(UserSettings::GetWidth()) / static_cast<float>(UserSettings::GetHeight());

			if (AspectRatio > 1.0f)
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
		
		static const char *s_VertexShaderPath;
		static const char *s_FragmentShaderPath;
		static const char* s_SamplerName;

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

		bool Load(const char* i_TexturePath, const sRectangle &i_texcoords, const sSprite &i_spriteDetails, IDirect3DDevice9* i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage = NULL
#endif
			);

		bool CreateSpriteDrawInfoandFillVertexBuffer(const sSprite &i_spriteDetails, const sRectangle &i_texcoords);
		bool FillVertexBuffer();
		bool FillSpriteSheet(unsigned int i_HorizontalCount = 0, unsigned int i_VerticalCount = 0);

		HRESULT Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

	};
}

#endif //__SPRITES_H