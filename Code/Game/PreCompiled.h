/*
This file will be #included before anything else in the project
*/

#ifndef __EAE2014_PRECOMPILED_H
#define __EAE2014_PRECOMPILED_H

// Exclude extraneous Windows stuff
#define WIN32_LEAN_AND_MEAN
// Prevent Windows from creating min/max macros
#define NOMINMAX

// Initialize Windows
#include <Windows.h>

#include <assert.h>

#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

const unsigned int g_windowWidth = 800;
const unsigned int g_windowHeight = 600;
const bool g_shouldRenderFullScreen = false;

#endif	// __EAE2014_PRECOMPILED_H
