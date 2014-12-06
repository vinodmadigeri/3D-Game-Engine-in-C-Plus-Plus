#ifndef __DEBUG_LINE_RENDERER_H
#define __DEBUG_LINE_RENDERER_H

#include "PreCompiled.h"
#include <vector>
#include <d3d9.h>
#include "Vector3.h"
#include "ILine.h"
#include "Debug.h"

namespace Engine
{
#ifdef EAE2014_DEBUGLINE_SHOULDDRAW
	class DebugLineRenderer: private ILine
	{
		std::vector<sLine> mLines;
		bool mShouldDraw;
		bool mShouldRenderDebugLinesThisFrame;
		DebugLineRenderer(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines);

		~DebugLineRenderer()
		{

		}

	public:
		static DebugLineRenderer * mInstance;
		static bool CreateInstance(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines);

		static DebugLineRenderer * GetInstance();
		static void Destroy();
		
		bool AddLines(const sLine &iLine)
		{
			if (mLines.size() >= s_MaxLines)
			{
				return false;
			}
			mLines.push_back(iLine);
			mShouldRenderDebugLinesThisFrame = true;
			return true;
			
		}

		inline bool LoadShaders()
		{
#ifdef EAE2014_DEBUGLINE_SHOULDDRAW
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
			std::string ErrorMessage;
#endif
			if (!Load(m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				, &ErrorMessage
#endif
				))
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				DebugPrint(ErrorMessage.c_str());
#endif
				return false;
			}
#endif
			return true;
		}

		inline bool CreateVertexBuffer()
		{
			return CreateVertexBufferForLine();
		}

		void Render();
	};
#endif
}

#endif //__DEBUG_LINE_RENDERER_H