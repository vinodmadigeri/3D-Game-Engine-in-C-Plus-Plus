
#include "PreCompiled.h"
#include <fstream>

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
		assert(i_DrawInfo.m_NumOfVertices > 0 && i_DrawInfo.m_IndexCount > 0);

		mDrawInfo.m_pVerticesData = i_DrawInfo.m_pVerticesData;
		mDrawInfo.m_pIndices = i_DrawInfo.m_pIndices;
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
			delete (mDrawInfo.m_pVerticesData);
		}

		if (mDrawInfo.m_pIndices)
		{
			delete (mDrawInfo.m_pIndices);
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

		MeshData binaryMeshData;
		binaryMeshData.VertexCount = *(reinterpret_cast<unsigned int *>(pBinData));
		pBinData += sizeof(unsigned int); //Vertices Count

		binaryMeshData.IndexCount = *(reinterpret_cast<unsigned int *>(pBinData));
		pBinData += sizeof(unsigned int); //Indices Count;

		binaryMeshData.mVertices = reinterpret_cast<VertexData *>(pBinData);
		pBinData += sizeof(VertexData) * binaryMeshData.VertexCount;

		binaryMeshData.mIndices = reinterpret_cast<unsigned int *>(pBinData);
		pBinData += sizeof(unsigned int) * binaryMeshData.IndexCount;

		//Fill the read data to structure
		iDrawInfo.m_NumOfVertices = binaryMeshData.VertexCount;
		iDrawInfo.m_IndexCount = binaryMeshData.IndexCount;
		iDrawInfo.m_pVerticesData = new sVertexData[iDrawInfo.m_NumOfVertices]; //deleted in the destructor of mesh
		iDrawInfo.m_pIndices = new DWORD32[iDrawInfo.m_IndexCount]; //deleted in the destructor of mesh

		assert(iDrawInfo.m_pVerticesData && iDrawInfo.m_pIndices);

		for (unsigned int i = 0; i < iDrawInfo.m_NumOfVertices; i++)
		{
			iDrawInfo.m_pVerticesData[i].x = binaryMeshData.mVertices[i].Position[0];
			iDrawInfo.m_pVerticesData[i].y = binaryMeshData.mVertices[i].Position[1];
			iDrawInfo.m_pVerticesData[i].z = binaryMeshData.mVertices[i].Position[2];
			iDrawInfo.m_pVerticesData[i].color = D3DCOLOR_XRGB(binaryMeshData.mVertices[i].Color[0], binaryMeshData.mVertices[i].Color[1], binaryMeshData.mVertices[i].Color[2]);
		}

		for (unsigned int i = 0; i < iDrawInfo.m_IndexCount; i++)
		{
			iDrawInfo.m_pIndices[i] = static_cast<DWORD32>(binaryMeshData.mIndices[i]);
		}

		iDrawInfo.m_PrimitiveType = D3DPT_TRIANGLELIST;
		unsigned int numberOfVerticesPerTriangle = 3;
		iDrawInfo.m_PrimitiveCount = iDrawInfo.m_IndexCount / numberOfVerticesPerTriangle;
		iDrawInfo.m_indexOfFirstVertexToRender = 0;
		iDrawInfo.m_indexOfFirstIndexToUse = 0;
		iDrawInfo.m_VertexStride = sizeof(sVertexData);

		delete RawBinaryData;
		ReadMeshFile.close();

		return true;
	}
}