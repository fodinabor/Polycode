 /*
Copyright (C) 2011 by Ivan Safrin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

// Polycode CORE LIBRARY CONFIGURATION SECTION

// Compile support for lua bindings.
//#define _COMPILE_LUA

#define POLYCODE_VERSION_STRING "0.8.5_dev"

#define USE_POSIX_MUTEX 1

#define COMPILE_GL_RENDERER
typedef float PolyRendererVertexType;
typedef unsigned int PolyRendererIndexType;

#ifdef _WINDOWS
	#define WIN32_LEAN_AND_MEAN

	#pragma warning(disable:4251)
	#pragma warning(disable:4305)
	#pragma warning(disable:4244)
	#pragma warning(disable:4018)
	#pragma warning(disable:4996)
	#pragma warning(disable:4309)
	#ifndef NULL
	#define NULL 0
	#endif
	// Prevent windows.h includes from generating min/max macros that
	// clash with the templates in <algorithm>
	#ifndef NOMINMAX
	#define NOMINMAX
	#endif
#endif
#include <stdint.h>
 
#ifndef NULL
	#define NULL 0
#endif
 
#define PI 3.14159265
#define RADIANS 57.2957795

#define TODEGREES 57.2957795
#define TORADIANS 0.0174532925

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC	  2
#define PLATFORM_UNIX	  3
#define PLATFORM_IOS	  4
#define PLATFORM_ANDROID  5

#if defined(_WINDOWS) || defined(WINAPI_FAMILY)	 || defined(WIN32)
	#define PLATFORM PLATFORM_WINDOWS
#elif __APPLE__ && __MACH__
	#ifndef CLDOC
		#include <TargetConditionals.h>
	#endif
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define PLATFORM PLATFORM_IOS
		#define STRICT_OPENGLES2 1
		#define POLYCODE_NUMBER_IS_SINGLE 1
	#else
		#define PLATFORM PLATFORM_MAC
	#endif
#elif defined(__ANDROID__)
	#define PLATFORM PLATFORM_ANDROID
	#define POLYCODE_NUMBER_IS_SINGLE 1
#else
	#include <cstddef>
	#define PLATFORM PLATFORM_UNIX
#endif

#if (defined(_WINDOWS) || defined(WINAPI_FAMILY)) && defined(POLYCODE_EXPORTS)
	#define _PolyExport __declspec(dllexport)
#else
	#define _PolyExport
#endif


#ifdef POLYCODE_NUMBER_IS_SINGLE
typedef float Number;
#else
typedef double Number;
#endif

#ifdef _MSC_VER
#if _MSC_VER<=1700

#include <cmath>	//cmath for "round / floor"

inline int round(Number x) { 
	return floor(x + 0.5); 
}
#endif 
#endif

#define RANDOM_NUMBER ((Number)rand()/(Number)RAND_MAX)

inline Number clampf(Number x, Number a, Number b)
{
	return x < a ? a : (x > b ? b : x);
}

#ifndef MIN
	#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
	#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#if PLATFORM == PLATFORM_IOS
//	  #define BGRA_TEXTURE_FORMAT
#endif

// Special flag read by create_lua_library parser, suppresses Lua bindings for item.
#define POLYIGNORE

class _PolyExport PolyBase {};

