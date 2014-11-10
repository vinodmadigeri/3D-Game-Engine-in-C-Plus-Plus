/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cTextureBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	return BuilderHelper::Build<Texture::cTextureBuilder>(i_arguments, i_argumentCount);
}
