//
// Polycode template. Write your code here.
// 

#include "PolycodeTemplateApp.h"


PolycodeTemplateApp::PolycodeTemplateApp(PolycodeView *view) {
    core = new POLYCODE_CORE(view, 800,480,false,false, 0,0,60);
    
    core->addFileSource("archive", "default.pak");
    ResourcePool *globalPool = core->getResourceManager()->getGlobalPool();
    globalPool->loadResourcesFromFolder("default", true);
    
	// Write your code here!
    
    scene = new Scene(core, Scene::SCENE_2D);
    scene->useClearColor = true;
    
    ScenePrimitive *test = new ScenePrimitive(ScenePrimitive::TYPE_VPLANE, 0.5, 0.5);
    test->setMaterial(globalPool->getMaterial("Unlit"));
    test->getShaderPass(0).shaderBinding->setTextureForParam("diffuse", globalPool->loadTexture("main_icon.png"));
    scene->addChild(test);
	test->setPositionY(0.2);
    
    SceneLabel *testLabel = new SceneLabel(globalPool->getMaterial("Unlit"), "Hello Polycode!", 32, globalPool->getFont("sans"), Label::ANTIALIAS_FULL, 0.2);
	testLabel->setPositionY(-0.2);
    scene->addChild(testLabel);
 
    core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
}

void PolycodeTemplateApp::handleEvent(Event *event) {
    InputEvent *inputEvent = (InputEvent*) event;
    
    switch(inputEvent->getKey()) {

    }
}

PolycodeTemplateApp::~PolycodeTemplateApp() {
    
}

bool PolycodeTemplateApp::Update() {
    bool res = core->Update();
	
	RenderFrame* frame = new RenderFrame(core->getViewport());
	scene->Render(frame, NULL, NULL, NULL, false);
	core->getRenderer()->submitRenderFrame(frame);
	
	return res;
}
