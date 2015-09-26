#pragma once

#include "polycode/core/PolyCore.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

//#define STRICT_OPENGLES2
#include <angle_gl.h>
#include <windows.h>

using namespace Polycode;

namespace Polycode {

class _PolyExport PolycodeView : public PolycodeViewBase {
public:
	PolycodeView(HINSTANCE hInstance, int nCmdShow, LPCTSTR windowTitle, bool resizable=false, bool showDebugConsole = false);
	~PolycodeView();

	void handleChange();

	void initContext();
	void destroyContext();

	HWND hWnd;
	HDC hDC;
	HINSTANCE hInstance;

	EGLDisplay mEglDisplay;
	EGLContext mEglContext;
	EGLSurface mEglSurface;

	//LPCTSTR windowTitle;
	int nCmdShow;
	//bool showDebugConsole;

	bool changed;
	VideoModeChangeInfo* modeInfo;
	VideoModeChangeInfo* oldModeInfo;
};

}