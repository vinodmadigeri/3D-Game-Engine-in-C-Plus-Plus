#ifndef __LINE_RENDERER_H
#define __LINE_RENDERER_H

#include "PreCompiled.h"
#include <vector>
#include <d3d9.h>
#include "Vector3.h"
#include "ILine.h"

namespace Engine
{
	class LineRenderer: public ILine
	{
		std::vector<sLine> mLines;

	public:

		LineRenderer(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines) :
			ILine(iName, i_direct3dDevice, iMaxLines)
		{
			assert(iName && i_direct3dDevice);
		}

		~LineRenderer()
		{
			
		}

		virtual void Render();
	};
}

#endif //__LINE_RENDERER_H