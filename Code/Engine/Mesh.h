#ifndef __MESH_H
#define __MESH_H

#include "PreCompiled.h"
#include <d3d9.h>
#include "MeshData.h"
#include "HashedString.h"

namespace Engine
{

	class Mesh
	{
	private:
		IDirect3DVertexDeclaration9* m_vertexDeclaration;
		IDirect3DVertexBuffer9* m_vertexBuffer;
		IDirect3DIndexBuffer9* m_indexBuffer;
		
	public:
		const HashedString mHashedName;
		PrimitiveDrawInfo mDrawInfo;

		Mesh(const char * iName,
			const DrawInfo &i_DrawInfo,
			IDirect3DVertexDeclaration9* i_vertexDeclaration,
			IDirect3DVertexBuffer9* i_vertexBuffer,
			IDirect3DIndexBuffer9* i_indexBuffer);

		~Mesh();

		IDirect3DVertexDeclaration9* GetVertexDeclaration() const;
		IDirect3DVertexBuffer9* GetVertexBuffer() const;
		IDirect3DIndexBuffer9* GetIndexBuffer() const;

		static bool GetDrawInfoFromMeshFile(const char * iMeshPath, DrawInfo & iDrawInfo);
	};
}


#endif //__MESH_H