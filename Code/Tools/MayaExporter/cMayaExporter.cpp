// Header Files
//=============

#include "cMayaExporter.h"

#include <cstdint>
#include <fstream>
#include <map>
#include <maya/MDagPath.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <sstream>
#include <string>

// Vertex Definition
//==================

namespace MayaExporter
{
	struct sVertex
	{
		// Position
		float x, y, z;
		// Normal
		float nx, ny, nz;
		// Tangent
		float tx, ty, tz;
		// Bitangent
		float btx, bty, btz;
		// Texture coordinates
		float u, v;
		// Color
		// (Note that each color channel is a float and [0,1], _not_ a uint8_t [0,255]!)
		float r, g, b, a;

		sVertex( const MPoint& i_position, const MFloatVector& i_normal,
			const MFloatVector& i_tangent, const MFloatVector& i_bitangent,
			const float i_texcoordU, const float i_texcoordV,
			const MColor& i_vertexColor )
			:
			x( static_cast<float>( i_position.x ) ), y( static_cast<float>( i_position.y ) ), z( static_cast<float>( i_position.z ) ),
			nx( i_normal.x ), ny( i_normal.y ), nz( i_normal.z ),
			tx( i_tangent.x ), ty( i_tangent.y ), tz( i_tangent.z ),
			btx( i_bitangent.x ), bty( i_bitangent.y ), btz( i_bitangent.z ),
			u( i_texcoordU ), v( i_texcoordV ),
			r( i_vertexColor.r ), g( i_vertexColor.g ), b( i_vertexColor.b ), a( i_vertexColor.a )
		{

		}
	};
}

// Helper Function Declarations
//=============================

namespace MayaExporter
{
	std::string CreateUniqueVertexKey( const int i_positionIndex, const int i_normalIndex,
		const int i_tangentIndex, const int i_texcoordIndex, const int i_vertexColorIndex );
	MStatus ProcessAllMeshes( std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus ProcessSelectedMeshes( std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus ProcessSingleMesh( const MFnMesh& i_mesh,
		std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus FillVertexAndIndexBuffer( const MFnMesh& i_mesh,
		std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer );
	MStatus WriteMeshToFile( const MString& i_fileName, std::vector<const sVertex>& i_vertexBuffer, std::vector<unsigned int>& i_indexBuffer );
}

// Inherited Interface
//====================

MStatus MayaExporter::cMayaExporter::writer(const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode)
{
	// Gather the vertex and index buffer information
	std::vector<const sVertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	{
		// The user decides whether to export the entire scene or just a selection
		if ( i_mode == MPxFileTranslator::kExportAccessMode )
		{
			const MStatus status = ProcessAllMeshes( vertexBuffer, indexBuffer );
			if ( !status )
			{
				return status;
			}
		}
		else if ( i_mode == MPxFileTranslator::kExportActiveAccessMode )
		{
			const MStatus status = ProcessSelectedMeshes( vertexBuffer, indexBuffer );
			if ( !status )
			{
				return status;
			}
		}
		else
		{
			MGlobal::displayError( "Unexpected file access mode" );
			return MStatus::kFailure;
		}
	}

	// Write the mesh to the requested file
	{
		const MString filePath = i_file.fullName();
		return WriteMeshToFile( filePath, vertexBuffer, indexBuffer );
	}
}

// Helper Function Definitions
//============================

namespace MayaExporter
{
	std::string CreateUniqueVertexKey( const int i_positionIndex, const int i_normalIndex,
		const int i_tangentIndex, const int i_texcoordIndex, const int i_vertexColorIndex )
	{
		std::ostringstream vertexKey;
		vertexKey << i_positionIndex << "_" << i_normalIndex << "_"
			<< i_tangentIndex << "_" << i_texcoordIndex << "_" << i_vertexColorIndex;
		return vertexKey.str();
	}

	MStatus ProcessAllMeshes( std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		for ( MItDag i( MItDag::kDepthFirst, MFn::kMesh ); !i.isDone(); i.next() )
		{
			MFnMesh mesh( i.item() );
			if ( !ProcessSingleMesh( mesh, o_vertexBuffer, o_indexBuffer ) )
			{
				return MStatus::kFailure;
			}
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessSelectedMeshes( std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		// Iterate through each selected mesh
		MSelectionList selectionList;
		MStatus status = MGlobal::getActiveSelectionList( selectionList );
		if ( status )
		{
			for ( MItSelectionList i( selectionList, MFn::kMesh ); !i.isDone(); i.next() )
			{
				MDagPath dagPath;
				i.getDagPath( dagPath );
				MFnMesh mesh( dagPath );
				if ( !ProcessSingleMesh( mesh, o_vertexBuffer, o_indexBuffer ) )
				{
					return MStatus::kFailure;
				}
			}
		}
		else
		{
			MGlobal::displayError( MString( "Failed to get active selection list: " ) + status.errorString() );
			return MStatus::kFailure;
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessSingleMesh( const MFnMesh& i_mesh,
		std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		if ( i_mesh.isIntermediateObject() )
		{
			return MStatus::kSuccess;
		}

		return FillVertexAndIndexBuffer( i_mesh, o_vertexBuffer, o_indexBuffer );
	}

	MStatus FillVertexAndIndexBuffer( const MFnMesh& i_mesh,
		std::vector<const sVertex>& o_vertexBuffer, std::vector<unsigned int>& o_indexBuffer )
	{
		MStatus status;

		// Get a list of the positions
		MPointArray positions;
		{
			status = i_mesh.getPoints( positions );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of the normals
		MFloatVectorArray normals;
		{
			status = i_mesh.getNormals( normals );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of tangents
		MFloatVectorArray tangents;
		{
			status = i_mesh.getTangents( tangents );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of bitangents
		MFloatVectorArray bitangents;
		{
			status = i_mesh.getBinormals( bitangents );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of the texture coordinates
		MFloatArray texcoordUs, texcoordVs;
		{
			status = i_mesh.getUVs( texcoordUs, texcoordVs );
			if ( !status )
			{
				MGlobal::displayError( status.errorString() );
				return status;
			}
		}

		// Get a list of the vertex colors
		MColorArray vertexColors;
		{
			int colorSetCount = i_mesh.numColorSets();
			if ( colorSetCount > 0 )
			{
				MString* useDefaultColorSet = NULL;
				MColor defaultColor( 1.0f, 1.0f, 1.0f, 1.0f );
				status = i_mesh.getColors( vertexColors, useDefaultColorSet, &defaultColor );
				if ( !status )
				{
					MGlobal::displayError( status.errorString() );
					return status;
				}
			}
		}

		// Gather vertex and triangle information
		std::map<const std::string, const sVertex> uniqueVertices;
		std::vector<const std::string> triangles;
		{
			MPointArray trianglePositions;
			MIntArray positionIndices;
			for ( MItMeshPolygon i( i_mesh.object() ); !i.isDone(); i.next() )
			{
				if ( i.hasValidTriangulation() )
				{
					// Store information for each vertex in the polygon
					std::map<int, const std::string> indexToKeyMap;
					{
						MIntArray vertices;
						status = i.getVertices( vertices );
						if ( status )
						{
							for ( unsigned int j = 0; j < vertices.length(); ++j )
							{
								const int positionIndex = vertices[j];
								const int normalIndex = i.normalIndex( j );
								const int tangentIndex = i.tangentIndex( j );
								int texcoordIndex;
								{
									status = i.getUVIndex( j, texcoordIndex );
									if ( !status )
									{
										MGlobal::displayError( status.errorString() );
										return status;
									}
								}
								int vertexColorIndex = -1;
								MColor vertexColor( 1.0f, 1.0f, 1.0f, 1.0f );
								{
									int colorSetCount = i_mesh.numColorSets();
									if ( colorSetCount > 0 )
									{
										status = i.getColorIndex( j, vertexColorIndex );
										if ( status )
										{
											vertexColor = vertexColors[vertexColorIndex];
										}
										else
										{
											MGlobal::displayError( status.errorString() );
											return status;
										}
									}
								}
								const std::string vertexKey = CreateUniqueVertexKey( positionIndex, normalIndex,
									tangentIndex, texcoordIndex, vertexColorIndex );
								indexToKeyMap.insert( std::make_pair( positionIndex, vertexKey ) );
								uniqueVertices.insert( std::make_pair( vertexKey,
									sVertex( positions[positionIndex], normals[normalIndex],
										tangents[tangentIndex], bitangents[tangentIndex],
										texcoordUs[texcoordIndex], texcoordVs[texcoordIndex],
										vertexColor ) ) );
							}
						}
						else
						{
							MGlobal::displayError( status.errorString() );
							return status;
						}
					}
					// Store information for each individual triangle in the polygon
					{
						int triangleCount = 0;
						i.numTriangles( triangleCount );
						for ( int j = 0; j < triangleCount; ++j )
						{
							i.getTriangle( j, trianglePositions, positionIndices );
							for ( unsigned int k = 0; k < positionIndices.length(); ++k )
							{
								const int positionIndex = positionIndices[k];
								std::map<int, const std::string>::iterator mapLookUp = indexToKeyMap.find( positionIndex );
								if ( mapLookUp != indexToKeyMap.end() )
								{
									const std::string& vertexKey = mapLookUp->second;
									triangles.push_back( vertexKey );
								}
								else
								{
									MGlobal::displayError( "A triangle gave a different vertex index than the polygon gave" );
									return MStatus::kFailure;
								}
							}
						}
					}
				}
				else
				{
					MGlobal::displayError( "This mesh has an invalid triangulation" );
					return MStatus::kFailure;
				}
			}
		}

		// Convert the triangle information to vertex and index buffers
		o_vertexBuffer.clear();
		o_indexBuffer.clear();
		o_indexBuffer.resize( triangles.size() );
		{
			std::map<const std::string, unsigned int> keyToIndexMap;
			for ( std::map<const std::string, const sVertex>::iterator i = uniqueVertices.begin(); i != uniqueVertices.end(); ++i )
			{
				keyToIndexMap.insert( std::make_pair( i->first, static_cast<unsigned int>( o_vertexBuffer.size() ) ) );
				o_vertexBuffer.push_back( i->second );
			}
			for ( size_t i = 0; i < triangles.size(); ++i )
			{
				const std::string& key = triangles[i];
				unsigned int index = keyToIndexMap.find( key )->second;
				o_indexBuffer[i] = index;
			}
		}

		return MStatus::kSuccess;
	}

	inline float getColorValue(float iColor)
	{
		return (iColor * 255);
	}

	MStatus WriteMeshToFile( const MString& i_fileName, std::vector<const sVertex>& i_vertexBuffer, std::vector<unsigned int>& i_indexBuffer )
	{
		// Maya's coordinate system is different than the default Direct3D behavior
		// (it is right handed and UVs have (0,0) at the lower left corner).
		// You can deal with these differences however you want,
		// but if you want to convert everything in the exported file
		// the following are probably the most natural changes to make:
		//	* POSITION	-> x, y, -z
		//	* NORMAL	-> nx, ny, -z
		//	* TANGENT	-> tx, ty, -tz
		//	* BITANGENT	-> -btx, -bty, btz
		//	* TEXCOORD	-> u, 1 - v

		std::ofstream fout( i_fileName.asChar() );
		if ( fout.is_open() )
		{
			// Open table
			fout << "-- mesh in Lua\n\n";

			fout << "return\n{\n";
			{
				fout << "\tVertexCount = " << i_vertexBuffer.size() << ",\n\n";
				fout << "\t--Each Vertex Data\n";
				fout << "\tVertices =\n";
				fout << "\t{\n";
				for (unsigned int i = 0; i < i_vertexBuffer.size(); i++)
				{
					fout << "\t\t{\n";
					fout << "\t\t\tPosition = {" << i_vertexBuffer[i].x << ", " << i_vertexBuffer[i].y << ", " << -(i_vertexBuffer[i].z) << "},\n";
					fout << "\t\t\tUV = {" << i_vertexBuffer[i].u << ", " << (1 - i_vertexBuffer[i].v) <<"},\n";
					fout << "\t\t\tNormal = {" << i_vertexBuffer[i].nx << ", " << i_vertexBuffer[i].ny << ", " << -(i_vertexBuffer[i].nz) << "},\n";
					fout << "\t\t\tColor = {" << getColorValue(i_vertexBuffer[i].r) << ", " << getColorValue(i_vertexBuffer[i].g) << ", " << getColorValue(i_vertexBuffer[i].b) << "},\n";
					fout << "\t\t},\n";
				}

				fout << "\t},\n\n";

				fout << "\tIndexCount = " << i_indexBuffer.size() << ",\n\n";
				fout << "\t--Each triangle index data\n";
				fout << "\tIndices =\n";
				fout << "\t{";
				
				for (int i = 0; i < i_indexBuffer.size(); i = i + 3)
				{
					fout << "\n\t\t";
					fout << i_indexBuffer[i] << ", ";
					fout << i_indexBuffer[i + 2] << ", ";
					fout << i_indexBuffer[i + 1] << ", ";
				}
				
				fout << "\n";
				fout << "\t},\n";
			}
			// Close table
			fout << "}\n";
			fout.close();

			return MStatus::kSuccess;
		}
		else
		{
			MGlobal::displayError( MString( "Couldn't open " ) + i_fileName + " for writing" );
			return MStatus::kFailure;
		}
	}

	
}
