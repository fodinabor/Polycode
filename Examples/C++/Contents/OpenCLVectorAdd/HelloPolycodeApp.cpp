#include "HelloPolycodeApp.h"
#include "iostream"
#include <time.h>

HelloPolycodeApp::HelloPolycodeApp(PolycodeView *view) : EventHandler() {

	core = new POLYCODE_CORE(view, 640,480,false,false,0,0,90, 1, true);

	CoreServices::getInstance()->getResourceManager()->addArchive("Resources/default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);
	
	clTest();

	std::string clDevName;
	cl::Device dev = pCl->getDefaultDevice();
	dev.getInfo(CL_DEVICE_NAME, &clDevName);

	Scene *scene = new Scene(Scene::SCENE_2D);
    scene->getActiveCamera()->setOrthoSize(640, 480);
	SceneLabel *label = new SceneLabel("Name: " + clDevName, 32);
	scene->addChild(label);
}

HelloPolycodeApp::~HelloPolycodeApp() {
}

bool HelloPolycodeApp::Update() {
    return core->updateAndRender();
}

void HelloPolycodeApp::vectorAdd(float* a, float* b, float* result, int i) {
	for (i = 0; i < NENTRIES; i++) {
		result[i] = a[i] + b[i];
	}
	return;
}

void HelloPolycodeApp::clTest() {

	String OCLSource =
		"__kernel void vector_add ("
		"  const __global float *a"
		"  , const __global float *b"
		"  , const __global int *i"
		"  , __global float * result"
		"){"
		"  size_t pos = get_global_id(0);"
		"    result[pos] = a[pos] + b[pos] + i[0];"
		"}";

	float a[NENTRIES];
	float b[NENTRIES];
	float result[NENTRIES];
	int ii[1] = { 100 };

	//std::srand(time(NULL));
	
	for (int i = 0; i<NENTRIES; i++) {
		a[i] = i;
		b[i] = i;
		result[i] = 0.f; // result[i] should be 2*i
	}

	std::vector<void*> arrays;
	arrays.push_back(a);
	arrays.push_back(b);
	arrays.push_back(ii);

	String name = "vector_add";

	std::vector<size_t> size;
	size.push_back(sizeof(a));
	size.push_back(sizeof(b));
	size.push_back(sizeof(ii));

	pCl = new PolyCL<float, NENTRIES>(OCLSource, name, arrays, size, result);

	/*float* res = new float[NENTRIES];
	
	int i;
	std::cout << "i:";
	std::cin >> (int)i;

	unsigned int stop_CPU = 0;
	unsigned int start_CPU = core->getTicks();
	vectorAdd(a, b, res, i);
	stop_CPU = core->getTicks();
	unsigned int time_CPU = stop_CPU - start_CPU;
	Logger::log("%d\n %d\n", start_CPU, stop_CPU);
	Logger::log("Time CPU: %i\n", time_CPU);*/
	

	if (pCl->getSuccess()) {
		float* res = pCl->getResult();
		Logger::log("Got Result!\n");
		for (int k = 0; k<NENTRIES; k++) {
			Logger::log("R at %i: %f\n",k, res[k]);
		}
	}
	else {
		Logger::log("BuildError\n");
		exit(EXIT_FAILURE);
	}
}