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
	return BuilderHelper::Build<Generic::cGenericBuilder>(i_arguments, i_argumentCount);
}
