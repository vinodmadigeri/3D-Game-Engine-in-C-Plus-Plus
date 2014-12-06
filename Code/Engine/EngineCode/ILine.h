#ifndef __INTERFACE_LINE
#define __INTERFACE_LINE

#include "PreCompiled.h"
#include "Vector3.h"
#include "GraphicsSystem.h"

namespace Engine
{
	struct sLine
	{
		Vector3 mBegin;
		Vector3 mEnd;
		D3DCOLOR mColor;

		sLine(const Vector3 & iBegin, const Vector3 &iEnd)
		{
			mBegin = iBegin;
			mEnd = iEnd;
		}
	};

	class ILine
	{
	protected:
		std::string m_name;
		IDirect3DDevice9 * m_direct3dDevice;
		IDirect3DVertexShader9* m_vertexShader;
		ID3DXConstantTable* m_pvertexShaderConsts;
		D3DXHANDLE m_transform_WorldToView_ConstHandle;
		D3DXHANDLE m_transform_ViewToScreen_ConstHandle;

		IDirect3DPixelShader9* m_fragmentShader;
		ID3DXConstantTable* m_pfragmentShaderConsts;

		IDirect3DVertexBuffer9* m_vertexBuffer;


		static const char *s_VertexShaderPath;
		static const char *s_FragmentShaderPath;
		static unsigned int s_MaxLines;
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

		bool LoadVertexShaderConstants(const char* i_VertexShaderpath, ID3DXConstantTable* i_pvertexShaderConsts, IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

	public:
		ILine(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines);
		
		virtual ~ILine();

		inline std::string GetName() const{ return m_name; }
		inline IDirect3DVertexBuffer9* GetVertexBuffer() const
		{
			assert(m_vertexBuffer);
			return m_vertexBuffer;
		}

		HRESULT Set(IDirect3DDevice9 * i_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			, std::string* o_errorMessage
#endif
			);

		bool CreateVertexBufferForLine(const std::vector<sLine> &i_LineInfo);

		bool FillVertexBuffer(const std::vector<sLine> &i_LineInfo);

		virtual void Render(const std::vector<sLine> & iLines) = 0;
	};
}

#endif //__INTERFACE_LINE