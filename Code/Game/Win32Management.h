/*
	This file contains all of the function declarations for this example program
*/

#ifndef __EAE2014_WIN32_H
#define __EAE2014_WIN32_H

// Header Files
//=============

#include <string>

// Main Function
//==============

int CreateMainWindowAndReturnExitCodeWhenItCloses( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState );

// Helper Functions
//=================

// There are two things that have to happen:
//	* The main window must be created
//		(this will happen quickly)
//	* The main window must run until it gets closed
//		(this is up to the user)

bool CreateMainWindow( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState );
int WaitForMainWindowToCloseAndReturnExitCode( const HINSTANCE i_thisInstanceOfTheProgram );

// Windows reports errors in a way that's a little complicated;
// the following function makes it convenient to get a
// human-readable error message
std::string GetLastWindowsError();

// CreateMainWindow
//-----------------

HWND CreateMainWindowHandle( const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState );
ATOM RegisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram );

// WaitForMainWindowToCloseAndReturnExitCode
//------------------------------------------

bool CleanupMainWindow();
bool OnMainWindowClosed( const HINSTANCE i_thisInstanceOfTheProgram );
LRESULT CALLBACK OnMessageReceived( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam );
bool UnregisterMainWindowClass( const HINSTANCE i_thisInstanceOfTheProgram );
bool WaitForMainWindowToClose( int& o_exitCode );
void UpdateMainWindow( int& o_exitCode, bool& o_QuitRequested);
void ShutdownMainWindow(const HINSTANCE i_thisInstanceOfTheProgram);


#endif	// __EAE2014_WIN32_H
