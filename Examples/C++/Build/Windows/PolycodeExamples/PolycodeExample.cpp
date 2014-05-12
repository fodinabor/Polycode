#include <Polycode.h>
#include "HelloPolycodeApp.h"
#include "PolycodeView.h"
#include "windows.h"

using namespace Polycode;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Polycode Example",false,true);
#else 
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"Polycode Example");
#endif
	HelloPolycodeApp *app = new HelloPolycodeApp(view);

	MSG Msg;
	do {
		if(PeekMessage(&Msg, NULL, 0,0,PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	} while(app->Update());
	return Msg.wParam;
}