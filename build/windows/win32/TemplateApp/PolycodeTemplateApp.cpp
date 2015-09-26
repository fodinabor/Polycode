#include "PolycodeTemplateApp.h"

PolycodeTemplateApp::PolycodeTemplateApp(PolycodeView *view) {
	core = new Win32Core(view, 1280, 720, false, false, 0, 0, 60);

	core->addFileSource("archive", "default.pak");
	ResourcePool *globalPool = Services()->getResourceManager()->getGlobalPool();
	globalPool->loadResourcesFromFolder("default", true);

	// Write your code here!

	Scene *scene = new Scene(Scene::SCENE_2D_TOPLEFT);
	scene->useClearColor = true;

	ScenePrimitive *test = new ScenePrimitive(ScenePrimitive::TYPE_CIRCLE, 500, 500);
	test->setMaterialByName("Unlit");
	//test->getShaderPass(0).shaderBinding->loadTextureForParam("diffuse", "main_icon.png");
	test->setColor(1, 1, 0, 1);
	scene->addChild(test);

	SceneLabel *testLabel = new SceneLabel("Test!", 32, "sans", Label::ANTIALIAS_FULL, 0.2);
	scene->addChild(testLabel);
}
PolycodeTemplateApp::~PolycodeTemplateApp() {
    
}

bool PolycodeTemplateApp::Update() {
	return core->updateAndRender();
}