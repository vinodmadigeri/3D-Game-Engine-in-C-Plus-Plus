/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cGenericBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	return eae6320::Build<eae6320::cGenericBuilder>( i_arguments, i_argumentCount );
}
