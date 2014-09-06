/*
	This file contains all of the function declarations for this example program
*/

#ifndef __EAE2014_WIN32_H
#define __EAE2014_WIN32_H

// Header Files
//=============
#include "precompiled.h"

// There are two things that have to happen:
//	* The main window must be created
//		(this will happen quickly)
//	* The main window must run until it gets closed
//		(this is up to the user)

namespace Win32Management
{
	class WindowsManager
	{
	private:
		static WindowsManager * m_Instance;
		
		// Instead of a pointer Windows provides a "handle"
		// to represent each window that is created
		// ("HWND" == "window handle").
		// In Windows every separate element
		// (buttons, text fields, scroll bars, etc.) is a "window",
		// and so a typical Windows program will have many different windows.
		// In our class, however, we will only have a single main window.
		static HWND m_mainWindow;

		const HINSTANCE m_thisInstanceOfTheProgram;
		const int m_initialWindowDisplayState;

		// Window classes are almost always identified by name;
		// there is also a unique ATOM associated with them,
		// but in practice Windows expects to use the class name as an identifier.
		const std::string m_mainWindowClass_name;

		// The window's "caption"
		// (The text that is displayed in the title bar)
		const std::string m_windowCaption;

		const unsigned int m_windowWidth;
		const unsigned int m_windowHeight;
		const bool m_shouldRenderFullScreen;

		WindowsManager(const HINSTANCE i_thisInstanceOfTheProgram,
						const int i_initialWindowDisplayState,
						const std::string& i_MainWindowClassName,
						const std::string& i_windowCaption,
						const unsigned int i_windowWidth,
						const unsigned int i_windowHeight, 
						const bool i_shouldRenderFullScreen);
		~WindowsManager();

		bool CreateMainWindow(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState);
		
		// CreateMainWindow
		//-----------------
		HWND CreateMainWindowHandle(const HINSTANCE i_thisInstanceOfTheProgram, const int i_initialWindowDisplayState);
		ATOM RegisterMainWindowClass(const HINSTANCE i_thisInstanceOfTheProgram);
		//------------------------------------------

		bool CleanupMainWindow();
		bool OnMainWindowClosed(const HINSTANCE i_thisInstanceOfTheProgram);
		bool UnregisterMainWindowClass(const HINSTANCE i_thisInstanceOfTheProgram);
		void ShutdownMainWindow(const HINSTANCE i_thisInstanceOfTheProgram);

		// Windows reports errors in a way that's a little complicated;
		// the following function makes it convenient to get a
		// human-readable error message
		static std::string GetLastWindowsError();
		static LRESULT CALLBACK OnMessageReceived(HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam);

	public:
		static bool CreateInstance(const HINSTANCE i_thisInstanceOfTheProgram,
			const int i_initialWindowDisplayState,
			const std::string& i_MainWindowClassName,
			const std::string& i_windowCaption,
			const unsigned int i_windowWidth = 800,
			const unsigned int i_windowHeight = 600,
			const bool i_shouldRenderFullScreen = false);
		static WindowsManager* GetInstance();
		static void Destroy();
		void UpdateMainWindow(int& o_exitCode, bool& o_QuitRequested);
		HWND GetReferenceToMainWindowHandle();
	};
}

#endif	// __EAE2014_WIN32_H
