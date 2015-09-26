
#include "polycode/view/win/core/PolycodeView.h"
#include "polycode/core/PolyWinCore.h"
#include "polycode/core/PolyCoreServices.h"
#include "polycode/core/PolyCoreInput.h"
#include "polycode/core/PolyRenderer.h"
//#include "polycode/core/PolyOpenGLGraphicsInterface.h"
#include "polycode/core/PolyLogger.h"
#include <io.h>
#include <fcntl.h>
#include <ios>

using namespace Polycode;

Win32Core *core = NULL;

static void OpenConsole()
{
    int outHandle, errHandle, inHandle;
    FILE *outFile, *errFile, *inFile;
    AllocConsole();
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 9999;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE),_O_TEXT);
    inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE),_O_TEXT );

    outFile = _fdopen(outHandle, "w" );
    errFile = _fdopen(errHandle, "w");
    inFile =  _fdopen(inHandle, "r");

    *stdout = *outFile;
    *stderr = *errFile;
    *stdin = *inFile;

    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );
    setvbuf( stdin, NULL, _IONBF, 0 );

    std::ios::sync_with_stdio();

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int nWidth, nHeight;
	bool useDefault = false;

	if (!core)
		return DefWindowProc(hWnd, message, wParam, lParam);

	switch (message) {
	case WM_COPYDATA:
	{
		COPYDATASTRUCT *cp = (COPYDATASTRUCT*)lParam;
		wchar_t *stringData = (wchar_t*)cp->lpData;
		core->copyDataString = String(stringData);
		core->hasCopyDataString = true;
	}
	break;
	case WM_SIZE:
		nWidth = LOWORD(lParam);
		nHeight = HIWORD(lParam);
		if (core) {
			core->handleViewResize(nWidth, nHeight);
		}
		break;

	case WM_MOUSEMOVE:
		if (core)
			core->handleMouseMove(lParam, wParam);
		break;

	case WM_MOUSEWHEEL:
		if (core)
			core->handleMouseWheel(lParam, wParam);
		break;

	case WM_LBUTTONDOWN:
		if (core)
			core->handleMouseDown(CoreInput::MOUSE_BUTTON1, lParam, wParam);
		break;
	case WM_LBUTTONUP:
		if (core)
			core->handleMouseUp(CoreInput::MOUSE_BUTTON1, lParam, wParam);
		break;

	case WM_RBUTTONDOWN:
		if (core)
			core->handleMouseDown(CoreInput::MOUSE_BUTTON2, lParam, wParam);
		break;
	case WM_RBUTTONUP:
		if (core)
			core->handleMouseUp(CoreInput::MOUSE_BUTTON2, lParam, wParam);
		break;

#ifndef NO_TOUCH_API 
#ifdef NO_PEN_API
	case WM_TOUCH:
		if(core) {
			if(core->isMultiTouchEnabled()) {
				core->handleTouchEvent(lParam, wParam);
			}
		}
		break;
#else
	case WM_POINTERUPDATE:
	case WM_POINTERUP:
	case WM_POINTERDOWN:
		if (core)
			core->handlePointerUpdate(lParam, wParam);
		break;
#endif
#endif

	case WM_MBUTTONDOWN:
		if (core)
			core->handleMouseDown(CoreInput::MOUSE_BUTTON3, lParam, wParam);
		break;
	case WM_MBUTTONUP:
		if (core)
			core->handleMouseUp(CoreInput::MOUSE_BUTTON3, lParam, wParam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (core) {
			wchar_t unicodeChar = 0;
			MSG m;
			m.hwnd = hWnd;
			m.message = message;
			m.wParam = wParam;
			m.lParam = lParam;
			m.time = 0;
			if (PeekMessage(&m, hWnd, 0, WM_USER, PM_NOREMOVE) && (m.message == WM_CHAR)) {
				GetMessage(&m, hWnd, 0, WM_USER);
				unicodeChar = (wchar_t)m.wParam;
			}

			core->handleKeyDown(lParam, wParam, unicodeChar);
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (core)
			core->handleKeyUp(lParam, wParam);
		break;
	case WM_CLOSE:
		if (core)
			core->Shutdown();
		useDefault = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		useDefault = true;
		break;
	}

	if (useDefault)
		return DefWindowProc(hWnd, message, wParam, lParam);
	else
		return 0;
}


PolycodeView::PolycodeView(HINSTANCE hInstance, int nCmdShow, LPCTSTR windowTitle, bool resizable, bool showDebugConsole) : PolycodeViewBase() {

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"POLYCODEAPPLICATION";
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);

	if (resizable) {
		hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"POLYCODEAPPLICATION", windowTitle, WS_OVERLAPPEDWINDOW | WS_SYSMENU, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);
	} else {
		hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"POLYCODEAPPLICATION", windowTitle, WS_CAPTION | WS_POPUP | WS_SYSMENU, 0, 0, 640, 480, NULL, NULL, hInstance, NULL);
	}

	windowData = (void*)&hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (showDebugConsole) {
		OpenConsole();
	}

	this->resizable = resizable;
	this->hInstance = hInstance;
	this->nCmdShow = nCmdShow;
	this->changed = false;

	initContext();
}

PolycodeView::~PolycodeView() {
	destroyContext();
}

void PolycodeView::handleChange() {
	Services()->getRenderMutex()->lock();

	if (modeInfo) {
		if (modeInfo->fullScreen) {

			SetWindowLongPtr(hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
			ShowWindow(hWnd, SW_SHOW);
			MoveWindow(hWnd, 0, 0, modeInfo->xRes, modeInfo->yRes, TRUE);

			DEVMODE dmScreenSettings;					// Device Mode
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
			dmScreenSettings.dmPelsWidth = modeInfo->xRes;			// Selected Screen Width
			dmScreenSettings.dmPelsHeight = modeInfo->yRes;			// Selected Screen Height
			dmScreenSettings.dmBitsPerPel = 32;				// Selected Bits Per Pixel
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		} else {
			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = modeInfo->xRes;
			rect.bottom = modeInfo->yRes;
			if (resizable) {
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE);
				AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_SYSMENU, FALSE);
			} else {
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_CAPTION | WS_POPUP | WS_SYSMENU | WS_VISIBLE);
				AdjustWindowRect(&rect, WS_CAPTION | WS_POPUP | WS_SYSMENU, FALSE);
			}
			MoveWindow(hWnd, 0, 0, rect.right - rect.left, rect.bottom - rect.top, TRUE);

			ChangeDisplaySettings(0, 0);
		}

		if (oldModeInfo){
			if (modeInfo->aaLevel != oldModeInfo->aaLevel) {
				initContext();
			}
			delete oldModeInfo;
		} else {
			initContext();
		}

		oldModeInfo = modeInfo;
	}

	changed = false;
	Services()->getRenderMutex()->unlock();
	Services()->getRenderer()->getRenderThread()->enqueueJob(RenderThread::JOB_REQUEST_CONTEXT_CHANGE, new VideoModeChangeInfo(*modeInfo));
}

void PolycodeView::initContext() {
	destroyContext();

	if (!(hDC = GetDC(hWnd))) {
		Logger::log("Can't Create A GL Device Context.\n");
		return;
	}

	const EGLint configAttributes[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};

	const EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	const EGLint surfaceAttributes[] =
	{
		/*EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,*/
		EGL_NONE
	};

	const EGLint defaultDisplayAttributes[] =
	{
		// These are the default display attributes, used to request ANGLE's D3D11 renderer.
		// eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
		EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

		// EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
		// Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,

		// EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
		// the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
		// Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
		/*EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,*/
		EGL_NONE,
	};

	//const EGLint fl9_3DisplayAttributes[] =
	//{
	//	// These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
	//	// These attributes are used if the call to eglInitialize fails with the default display attributes.
	//	EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
	//	EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
	//	EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
	//	EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
	//	//EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
	//	EGL_NONE,
	//};

	//const EGLint warpDisplayAttributes[] =
	//{
	//	// These attributes can be used to request D3D11 WARP.
	//	// They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
	//	EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
	//	EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
	//	EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
	//	//EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
	//	EGL_NONE,
	//};

	EGLConfig config = NULL;

	// eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
	if (!eglGetPlatformDisplayEXT) {
		Logger::log("Failed to get function eglGetPlatformDisplayEXT");
	}

	mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, hDC, defaultDisplayAttributes);
	if (mEglDisplay == EGL_NO_DISPLAY) {
		Logger::log("Failed to get EGL display");
	}

	if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE) {
		//This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
		//mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
		//if (mEglDisplay == EGL_NO_DISPLAY) {
		//	Logger::log("Failed to get EGL display");
		//}

		//if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE) {
		//	// This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
		//	mEglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
		//	if (mEglDisplay == EGL_NO_DISPLAY) {
		//		Logger::log("Failed to get EGL display");
		//	}

		//	if (eglInitialize(mEglDisplay, NULL, NULL) == EGL_FALSE) {
		//		// If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
				Logger::log("Failed to initialize EGL");
				return;
		//	}
		//}
	}

	EGLint numConfigs = 0;
	if ((eglChooseConfig(mEglDisplay, configAttributes, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0)) {
		Logger::log("Failed to choose first EGLConfig");
	}

	mEglSurface = eglCreateWindowSurface(mEglDisplay, config, hWnd, surfaceAttributes);

	if (mEglSurface == EGL_NO_SURFACE) {
		Logger::log("Failed to create EGL fullscreen surface");
	}

	mEglContext = eglCreateContext(mEglDisplay, config, NULL, contextAttributes);
	if (mEglContext == EGL_NO_CONTEXT) {
		Logger::log("Failed to create EGL context");
	}
}

void PolycodeView::destroyContext() {
	if (mEglDisplay == NULL)
		return;

	eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(mEglDisplay, mEglContext);
	eglDestroySurface(mEglDisplay, mEglSurface);
	ReleaseDC(hWnd, hDC);
	hDC = 0;
	if (modeInfo->fullScreen)
		ChangeDisplaySettings(NULL, 0);
}