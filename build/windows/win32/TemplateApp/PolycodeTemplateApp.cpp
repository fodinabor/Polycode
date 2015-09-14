#include "PolycodeTemplateApp.h"

PolycodeTemplateApp::PolycodeTemplateApp(PolycodeView *view) {
	core = new Win32Core(view, 640, 480, false, false, 0, 0, 60);

	core->addFileSource("archive", "default.pak");
	Logger::log("load one");
	ResourcePool *globalPool = Services()->getResourceManager()->getGlobalPool();
	Logger::log("load two");
	globalPool->loadResourcesFromFolder("default", true);

	// Write your code here

	Logger::log("loaded");

	Scene *scene = new Scene(Scene::SCENE_2D);
	scene->useClearColor = true;

	ScenePrimitive *test = new ScenePrimitive(ScenePrimitive::TYPE_VPLANE, 0.5, 0.5);
	test->setMaterialByName("Unlit");
	
	scene->addChild(test);

	Logger::log("finished");
}
PolycodeTemplateApp::~PolycodeTemplateApp() {
    
}

bool PolycodeTemplateApp::Update() {
	return core->updateAndRender();
}