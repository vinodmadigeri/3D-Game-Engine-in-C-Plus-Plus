
#include "PreCompiled.h"
#include "DebugLineRenderer.h"
#include "GraphicsSystem.h"

namespace Engine
{
	DebugLineRenderer * DebugLineRenderer::mInstance = NULL;

	bool DebugLineRenderer::CreateInstance(const char *iName, IDirect3DDevice9 *i_direct3dDevice, unsigned int iMaxLines)
	{
		if (mInstance == NULL)
		{
			mInstance = new DebugLineRenderer(iName, i_direct3dDevice, iMaxLines);
		}

		return true;
	}

	DebugLineRenderer * DebugLineRenderer::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void DebugLineRenderer::Destroy()
	{
		if (mInstance)
		{
			delete mInstance;

			return;
		}

		assert(false);
	}

	void DebugLineRenderer::Render(const std::vector<sLine> & iLines)
	{
		//Only render when in frame
		if (GraphicsSystem::GetInstance()->CanSubmit() && iLines.size() > 0)
		{
			FillVertexBuffer(iLines);
			// Set the shaders
			{
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				std::string errorMessage;
#endif
				HRESULT result = Set(m_direct3dDevice
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
					, &errorMessage
#endif
					);
				assert(SUCCEEDED(result));
#ifdef EAE2014_SHOULDALLRETURNVALUESBECHECKED
				if (FAILED(result))
				{
					MessageBox(NULL, errorMessage.c_str(), "Error Setting Material", MB_OK | MB_ICONERROR);
				}
#endif
			}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			std::wstringstream EventMessage;
			EventMessage << "Draw Sprite " << (GetName().c_str());
			D3DPERF_BeginEvent(0, EventMessage.str().c_str());
#endif
			// Bind a specific vertex buffer to the device as a data source
			{
				// There can be multiple streams of data feeding the display adaptor at the same time
				const unsigned int streamIndex = 0;
				// It's possible to start streaming data in the middle of a vertex buffer
				const unsigned int bufferOffset = 0;
				// The "stride" defines how large a single vertex is in the stream of data
				const unsigned int bufferStride = sizeof(sVertexData);
				HRESULT result = m_direct3dDevice->SetStreamSource(streamIndex, GetVertexBuffer(), bufferOffset, bufferStride);
				assert(SUCCEEDED(result));
			}

			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = D3DPT_LINELIST;
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = 0;

				const unsigned int primitiveCountToRender = iLines.size();

				HRESULT result = m_direct3dDevice->DrawPrimitive(primitiveType, indexOfFirstVertexToRender, primitiveCountToRender);

				assert(SUCCEEDED(result));
			}
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			D3DPERF_EndEvent();
#endif
		}
	}
}