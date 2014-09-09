#ifndef __GAME_H
#define __GAME_H

#pragma once
#include "Resources\Resource.h"

class MainGame
{
	bool mInitilized;
public:
	MainGame();
	bool Initilize(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState);
	int Run(void);
	void Shutdown(const HINSTANCE i_thisInstanceOfTheProgram);
};

#endif //__Game_H