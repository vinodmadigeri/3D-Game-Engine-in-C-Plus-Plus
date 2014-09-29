#ifndef __INPUT_H
#define __INPUT_H

#include "PreCompiled.h"

namespace Engine
{
	// For standard letter or number keys, the representative ascii char can be used:
	// IsKeyPressed( 'A' ) or IsKeyPressed( '6' )

	class UserInput
	{
		static UserInput * mInstance;
		UserInput();
		UserInput & operator=(const UserInput & i_rhs);
		UserInput(const UserInput & i_Other);
		bool IsVirtualKeyPressed(const int i_virtualKeyCode);
	public:
		bool IsKeyPressed(const int i_virtualKeyCode);
		bool IsMouseButtonPressed(const int i_virtualButtonCode);

		static bool CreateInstance();
		static UserInput * GetInstance();
		static void Destroy();
	};

}

#endif //__INPUT_H