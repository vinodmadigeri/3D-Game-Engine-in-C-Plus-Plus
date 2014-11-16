/*
	This class defines a Maya file translator
*/

#ifndef __CMAYAEXPORTER_H
#define __CMAYAEXPORTER_H

// Header Files
//=============

#include <maya/MPxFileTranslator.h>
#include <vector>

// Forward Declarations
//=====================

namespace
{
	struct sVertex;
}

// Class Definition
//=================

namespace MayaExporter
{
	class cMayaExporter : public MPxFileTranslator
	{
		// Inherited Interface
		//====================

	public:

		// The writer method is what exports the file
		virtual bool haveWriteMethod() const { return true; }
		virtual MStatus writer( const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode );

		// You can choose what the default file extension of an exported mesh is
		virtual MString defaultExtension() const { return "mesh.lua"; }

		// Interface
		//==========

	public:

		// This function is used by Maya to create an instance of the exporter (see registerFileTranslator() in EntryPoint.cpp)
		static void* Create()
		{
			return new cMayaExporter;
		}
	};
}

#endif	// __CMAYAEXPORTER_H
