
#include "PreCompiled.h"

#include "Mesh.h"
#include "MeshData.h"

namespace Engine
{

	Mesh::Mesh(const DrawInfo &i_DrawInfo,
		IDirect3DVertexDeclaration9* i_vertexDeclaration,
		IDirect3DVertexBuffer9* i_vertexBuffer,
		IDirect3DIndexBuffer9* i_indexBuffer) :
		mDrawInfo(i_DrawInfo),
		m_vertexDeclaration(i_vertexDeclaration),
		m_vertexBuffer(i_vertexBuffer),
		m_indexBuffer(i_indexBuffer)
	{

	}

	Mesh::~Mesh()
	{
		if (m_vertexDeclaration)
		{
			m_vertexDeclaration->Release();
			m_vertexDeclaration = NULL;
		}

		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = NULL;
		}

		if (m_indexBuffer)
		{
			m_indexBuffer->Release();
			m_indexBuffer = NULL;
		}
	}


	IDirect3DVertexDeclaration9* Mesh::GetVertexDeclaration() const
	{
		assert(m_vertexDeclaration);
		return m_vertexDeclaration;
	}

	IDirect3DVertexBuffer9* Mesh::GetVertexBuffer() const
	{
		assert(m_vertexBuffer);
		return m_vertexBuffer;
	}
	
	IDirect3DIndexBuffer9* Mesh::GetIndexBuffer() const
	{
		assert(m_indexBuffer);
		return m_indexBuffer;
	}
}