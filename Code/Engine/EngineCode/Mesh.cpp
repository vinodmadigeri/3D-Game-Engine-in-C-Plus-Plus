
#include "PreCompiled.h"
#include <fstream>

#include "Mesh.h"
#include "MeshData.h"

namespace Engine
{
	Mesh::Mesh(const char * iName, 
		const DrawInfo &i_DrawInfo,
		IDirect3DVertexDeclaration9* i_vertexDeclaration,
		IDirect3DVertexBuffer9* i_vertexBuffer,
		IDirect3DIndexBuffer9* i_indexBuffer) :
		mName(iName),
		mHashedName(iName),
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

		assert(i_DrawInfo.m_NumOfVertices > 0 && i_DrawInfo.m_IndexCount > 0);
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


	bool Mesh::GetDrawInfoFromMeshFile(const char * iMeshPath, DrawInfo & iDrawInfo)
	{
		std::ifstream ReadMeshFile;
		ReadMeshFile.open(iMeshPath, std::ios::in | std::ios::binary);
		assert(ReadMeshFile.good());
		ReadMeshFile.seekg(0, ReadMeshFile.end);
		unsigned int TotalReadSize = static_cast<unsigned int>(ReadMeshFile.tellg());
		assert(TotalReadSize);
		ReadMeshFile.seekg(0, ReadMeshFile.beg);


		char* RawBinaryData = new char[TotalReadSize];
		assert(RawBinaryData);
		ReadMeshFile.read(RawBinaryData, TotalReadSize);

		char* pBinData = RawBinaryData;

		iDrawInfo.m_NumOfVertices = *(reinterpret_cast<DWORD32 *>(pBinData));
		pBinData += sizeof(DWORD32); //Vertices Count

		iDrawInfo.m_IndexCount = *(reinterpret_cast<DWORD32 *>(pBinData));
		pBinData += sizeof(DWORD32); //Indices Count;

		//Fill the read data to structure
		iDrawInfo.m_pVerticesData = new sVertexData[iDrawInfo.m_NumOfVertices]; //deleted in the destructor of mesh
		iDrawInfo.m_pIndices = new DWORD32[iDrawInfo.m_IndexCount]; //deleted in the destructor of mesh

		assert(iDrawInfo.m_pVerticesData && iDrawInfo.m_pIndices);

		memcpy(iDrawInfo.m_pVerticesData, reinterpret_cast<sVertexData *>(pBinData), sizeof(sVertexData) * iDrawInfo.m_NumOfVertices);
		pBinData += sizeof(sVertexData) * iDrawInfo.m_NumOfVertices;

		memcpy(iDrawInfo.m_pIndices, reinterpret_cast<unsigned int *>(pBinData), sizeof(DWORD32) * iDrawInfo.m_IndexCount);
		pBinData += sizeof(DWORD32) * iDrawInfo.m_IndexCount;


		iDrawInfo.m_PrimitiveType = D3DPT_TRIANGLELIST;
		unsigned int numberOfVerticesPerTriangle = 3;
		iDrawInfo.m_PrimitiveCount = iDrawInfo.m_IndexCount / numberOfVerticesPerTriangle;
		iDrawInfo.m_indexOfFirstVertexToRender = 0;
		iDrawInfo.m_indexOfFirstIndexToUse = 0;
		iDrawInfo.m_VertexStride = sizeof(sVertexData);

		delete[] RawBinaryData;
		ReadMeshFile.close();

		return true;
	}
}