#pragma once

#include "polycode/core/PolyCore.h"
#include <windows.h>

using namespace Polycode;

namespace Polycode {

class _PolyExport PolycodeView : public PolycodeViewBase {
public:
	PolycodeView(HINSTANCE hInstance, int nCmdShow, LPCTSTR windowTitle, bool resizable=false, bool showDebugConsole = false);
	~PolycodeView();

	HWND hwnd;
};

}