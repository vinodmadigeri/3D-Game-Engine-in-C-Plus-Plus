#ifndef __MESH_DATA_H
#define __MESH_DATA_H

#include <d3d9.h>

namespace Engine
{
	struct ColorRGBA
	{
		unsigned char r, g, b, a;

		ColorRGBA()
		{}

		ColorRGBA(unsigned char i_r, unsigned char i_g, unsigned char i_b, unsigned char i_a) :
			r(i_r),
			g(i_g),
			b(i_b),
			a(i_a)
		{}
	};

	// The vertex information that is sent to the display adaptor must match what the vertex shader expects
	typedef struct _sVertexData
	{
		float x, y, z;
		D3DCOLOR color; // D3DCOLOR = 4 bytes, or 8 bits [0,255] per RGBA channel
	}sVertexData;

	struct DrawInfo
	{
		D3DPRIMITIVETYPE	m_PrimitiveType;
		UINT				m_indexOfFirstVertexToRender;
		UINT				m_indexOfFirstIndexToUse;
		UINT				m_PrimitiveCount;
		UINT				m_NumOfVertices;
		sVertexData*		m_pVerticesData;
		unsigned int		m_VertexStride;
		
		//IndexBufferInfo
		DWORD32 *			m_pIndices;
		UINT				m_IndexCount;


		DrawInfo()
		{
			m_pVerticesData = NULL;
			m_pIndices = NULL;
		}
	};

	struct PrimitiveDrawInfo
	{
		D3DPRIMITIVETYPE	m_PrimitiveType;
		UINT				m_indexOfFirstVertexToRender;
		UINT				m_indexOfFirstIndexToUse;
		UINT				m_PrimitiveCount;
		UINT				m_NumOfVertices;
		unsigned int		m_VertexStride;

		//IndexBufferInfo
		UINT				m_IndexCount;
	};

	//Read from binary file structures
	//--------------------------------

	struct MeshData
	{
		DWORD32 VertexCount;
		DWORD32 IndexCount;
		sVertexData *mVertices;
		DWORD32  *mIndices;

		MeshData()
		{
			mVertices = NULL;
			mIndices = NULL;
		}
	};
}

#endif // __MESH_DATA_H