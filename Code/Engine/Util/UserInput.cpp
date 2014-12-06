#include "PreCompiled.h"

#include "UserInput.h"

namespace Engine
{
	UserInput* UserInput::mInstance = NULL;

	UserInput::UserInput()
	{

	}

	bool UserInput::IsVirtualKeyPressed(const int i_virtualKeyCode)
	{
		short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		return (keyState & isKeyDownMask) != 0;
	}

	bool UserInput::IsVirtualKeyReleased(const int i_virtualKeyCode)
	{
		short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = 1;
		return (keyState & isKeyDownMask) != 0;
	}

	bool UserInput::IsKeyPressed(const int i_virtualKeyCode)
	{
		return IsVirtualKeyPressed(i_virtualKeyCode);
	}

	bool UserInput::IsKeyReleased(const int i_virtualKeyCode)
	{
		return IsVirtualKeyReleased(i_virtualKeyCode);

	}

	bool UserInput::IsMouseButtonPressed(const int i_virtualButtonCode)
	{
		return IsVirtualKeyPressed(i_virtualButtonCode);
	}

	bool UserInput::CreateInstance()
	{
		if (mInstance == NULL)
		{
			mInstance = new UserInput();

			assert(mInstance);
			return true;
		}

		return false;
	}

	UserInput* UserInput::GetInstance()
	{
		if (mInstance)
		{
			return mInstance;
		}

		assert(false);
		return NULL;
	}

	void UserInput::Destroy()
	{
		if (mInstance)
		{
			delete mInstance;
		}
	}
}
