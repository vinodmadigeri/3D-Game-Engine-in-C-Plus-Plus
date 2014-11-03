/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cMeshBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	return BuilderHelper::Build<Mesh::cMeshBuilder>(i_arguments, i_argumentCount);
}
