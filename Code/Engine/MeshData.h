#ifndef __MESH_DATA_H
#define __MESH_DATA_H

#include <d3d9.h>
#include <d3dx9math.h>

namespace Engine
{
	typedef struct _MeshData
	{
		void *				m_pVertices;
		unsigned int		m_VertexStride;
		unsigned int		m_VertexCount;

		D3DVERTEXELEMENT9 * m_VertexDecl;
		D3DPRIMITIVETYPE	m_PrimType;

		WORD *				m_pIndices;
		unsigned int		m_IndexCount;
	} MeshData;
}

#endif // __MESH_DATA_H