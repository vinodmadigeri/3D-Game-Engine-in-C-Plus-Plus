
#include "PreCompiled.h"

#include "Mesh.h"
#include "MeshData.h"

namespace Engine
{
	Mesh::Mesh(const DrawInfo &i_DrawInfo,
		IDirect3DVertexDeclaration9* i_vertexDeclaration,
		IDirect3DVertexBuffer9* i_vertexBuffer,
		IDirect3DIndexBuffer9* i_indexBuffer) :
		m_vertexDeclaration(i_vertexDeclaration),
		m_vertexBuffer(i_vertexBuffer),
		m_indexBuffer(i_indexBuffer)
	{
		mDrawInfo.m_PrimitiveType = i_DrawInfo.m_PrimitiveType;
		mDrawInfo.m_indexOfFirstVertexToRender = i_DrawInfo.m_indexOfFirstVertexToRender;
		mDrawInfo.m_indexOfFirstIndexToUse = i_DrawInfo.m_indexOfFirstIndexToUse;
		mDrawInfo.m_PrimitiveCount = i_DrawInfo.m_PrimitiveCount;
		mDrawInfo.m_NumOfVertices = i_DrawInfo.m_NumOfVertices;
		mDrawInfo.m_VertexStride = i_DrawInfo.m_VertexStride;
		mDrawInfo.m_IndexCount = i_DrawInfo.m_IndexCount;
		
		assert(i_DrawInfo.m_pVerticesData && i_DrawInfo.m_pIndices);
		assert(i_DrawInfo.m_NumOfVertices > 0 && i_DrawInfo.m_VertexStride > 0);

		mDrawInfo.m_pVerticesData = (sVertexData *)malloc(i_DrawInfo.m_VertexStride * i_DrawInfo.m_NumOfVertices);
		mDrawInfo.m_pIndices = (DWORD32 *)malloc(i_DrawInfo.m_IndexCount * sizeof(DWORD32));

		assert(mDrawInfo.m_pVerticesData && mDrawInfo.m_pIndices);

		memcpy(mDrawInfo.m_pVerticesData, i_DrawInfo.m_pVerticesData, i_DrawInfo.m_VertexStride * i_DrawInfo.m_NumOfVertices);
		memcpy(mDrawInfo.m_pIndices, i_DrawInfo.m_pIndices, i_DrawInfo.m_IndexCount * sizeof(DWORD32));
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

		if (mDrawInfo.m_pVerticesData)
		{
			free(mDrawInfo.m_pVerticesData);
		}

		if (mDrawInfo.m_pIndices)
		{
			free(mDrawInfo.m_pIndices);
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