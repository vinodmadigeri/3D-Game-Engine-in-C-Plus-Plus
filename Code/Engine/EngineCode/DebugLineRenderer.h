#ifndef __DEBUG_LINE_RENDERER_H
#define __DEBUG_LINE_RENDERER_H

#include "PreCompiled.h"
#include <vector>
#include <d3d9.h>
#include "Vector3.h"
#include "ILine.h"

namespace Engine
{
	class DebugLineRenderer: private ILine
	{
		std::vector<sLine> mLines;
		
		DebugLineRenderer(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines) :
			ILine(iName, i_direct3dDevice, iMaxLines)
		{
			assert(iName && i_direct3dDevice);
		}

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
			mLines.push_back(iLine);
		}

		void Render(const std::vector<sLine> & iLines);
	};
}

#endif //__DEBUG_LINE_RENDERER_H