#include <Polycode.h>
#include "PolycodeView.h"

const unsigned long NENTRIES = 1000;

using namespace Polycode;

class HelloPolycodeApp : public EventHandler {
public:
    HelloPolycodeApp(PolycodeView *view);
    ~HelloPolycodeApp();
    bool Update();
    
	void clTest();

	void vectorAdd(float* a, float* b, float* result, int i);

private:
	Core *core;
	PolyCL<float, NENTRIES> *pCl;
	float* res;
};
