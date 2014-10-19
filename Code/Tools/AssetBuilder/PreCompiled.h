/*
This file will be #included before anything else in the project
*/

#ifndef __ASSET_PRECOMPILED_H
#define __ASSET_PRECOMPILED_H

// Exclude extraneous Windows stuff
#define WIN32_LEAN_AND_MEAN
// Prevent Windows from creating min/max macros
#define NOMINMAX

// Initialize Windows
#include <Windows.h>

#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

#include <cassert>
#include <string>
#include <iostream>
#include <ShlObj.h>
#include <sstream>
#endif	// __ASSET_PRECOMPILED_H
