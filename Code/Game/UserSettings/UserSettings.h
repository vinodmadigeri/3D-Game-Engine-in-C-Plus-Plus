/*
	This file manages user-configurable settings
*/

#ifndef _USERSETTINGS_H
#define _USERSETTINGS_H

// Header Files
//=============


// Interface
//==========

namespace UserSettings
{
	unsigned int GetHeight();
	unsigned int GetWidth();
	bool IsFullScreenModeEnabled();
	bool IsAntiAliasingEnabled();
}

#endif	// _USERSETTINGS_H
