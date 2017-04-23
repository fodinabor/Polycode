/*
 Copyright (C) 2012 by Ivan Safrin
 
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


 
#include "polycode/ide/PolycodeMaterialEditor.h"
#include "polycode/ide/PolycodeFrame.h"
#include "polycode/core/PolyCubemap.h"

extern UIColorPicker *globalColorPicker;
extern UIGlobalMenu *globalMenu;
extern PolycodeFrame *globalFrame;

PostEditorPane::PostEditorPane(Core *core, ResourcePool *resourcePool, ResourcePool *localPool) : UIElement(core) {
	currentMaterial = NULL;
	
	bottomElement = new UIElement(core);
	
	headerBgBottom = new UIRect(core, resourcePool, 10,10);
	bottomElement->addChild(headerBgBottom);
	headerBgBottom->setAnchorPoint(-1.0, -1.0, 0.0);
	headerBgBottom->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderBgColor"));
	
	propList = new PropList(core, resourcePool, "POST EFFECT EDITOR");
	bottomElement->addChild(propList);
	
	baseProps = new PropSheet(core, resourcePool, "EFFECT OPTIONS", "");
	propList->addPropSheet(baseProps);

	nameProp = new StringProp(core, resourcePool, "Name", this);
	baseProps->addProp(nameProp);
	nameProp->addEventListener(this, Event::CHANGE_EVENT);
	
	fp16Prop = new BoolProp(core, resourcePool, "HDR targets");
	fp16Prop->set(false);
	baseProps->addProp(fp16Prop);
	fp16Prop->addEventListener(this, Event::CHANGE_EVENT);
		
	baseProps->propHeight = 300;

	targetsProps = new RenderTargetsSheet(core, resourcePool);
	propList->addPropSheet(targetsProps);
		
	passProps = new ShaderPassesSheet(core, resourcePool, localPool);
	propList->addPropSheet(passProps);
	passProps->addEventListener(this, Event::CHANGE_EVENT);
	passProps->addEventListener(this, Event::REMOVE_EVENT);
	passProps->addEventListener(this, Event::SELECT_EVENT);

	optionsPropList = new PropList(core, resourcePool, "SHADER PASS OPTIONS");
	bottomElement->addChild(optionsPropList);
	optionsPropList->setPosition(400, 0);

	targetBindingProps = new TargetBindingsSheet(core, resourcePool);
	optionsPropList->addPropSheet(targetBindingProps);
	targetBindingProps->addEventListener(this, Event::CHANGE_EVENT);
		

	shaderOptionsSheet = new ShaderOptionsSheet(core, resourcePool);
	optionsPropList->addPropSheet(shaderOptionsSheet);

	
	topElement = new UIElement(core);
	postPreview = new PostPreviewBox(core, resourcePool);
	topElement->addChild(postPreview);
	postPreview->setPosition(0, 0);
		
	mainSizer = new UIVSizer(core, resourcePool, 100, 100, 300, true);
	mainSizer->setPosition(0, 0);
	mainSizer->addTopChild(topElement);
	mainSizer->addBottomChild(bottomElement);
	mainSizer->addEventListener(this, UIEvent::CHANGE_EVENT);
	addChild(mainSizer);
	enabled = false;
		
}

void PostEditorPane::Activate() {
	postPreview->previewScene->enabled = true;
	postPreview->renderTexture->enabled = true;
}

void PostEditorPane::Deactivate() {
	postPreview->previewScene->enabled = false;
	postPreview->renderTexture->enabled = false;
}


PostEditorPane::~PostEditorPane() {

}

void PostEditorPane::Resize(Number width, Number height) {
	mainSizer->Resize(width, height);
	
	headerBgBottom->Resize(width, 30);	
	propList->Resize(width/2.0, height- mainSizer->getMainHeight());
	
	propList->updateProps();
	optionsPropList->Resize(width/2.0, height- mainSizer->getMainHeight());
	optionsPropList->setPosition(floor(width/2.0), optionsPropList->getPosition().y);
	optionsPropList->updateProps(); 
	UIElement::Resize(width, height);
	adjustPreview();
}

void PostEditorPane::adjustPreview() {
	targetsProps->normTextureWidth = getWidth();
	targetsProps->normTextureHeight = mainSizer->getMainHeight()-30;
	postPreview->Resize(getWidth(), mainSizer->getMainHeight());
}


void PostEditorPane::setMaterial(std::shared_ptr<Material> material) {
	enabled = true;
	currentMaterial = material;		
		
	nameProp->set(material->getName());
	
	if(currentMaterial->fp16RenderTargets) {
		fp16Prop->set(true);
	} else {
		fp16Prop->set(false);
	}
	
	targetsProps->material = material;
	passProps->setMaterial(material);	
	postPreview->setMaterial(material);
	
	optionsPropList->visible = false;
	optionsPropList->enabled = false;
	
	adjustPreview();
	
	for(int i=0; i < currentMaterial->getNumShaderRenderTargets(); i++) {
		ShaderRenderTarget *target = currentMaterial->getShaderRenderTarget(i);
		if(target->sizeMode == ShaderRenderTarget::SIZE_MODE_NORMALIZED) {
			target->normalizedWidth = targetsProps->normTextureWidth;
			target->normalizedHeight = targetsProps->normTextureHeight;
			currentMaterial->recreateRenderTarget(target, Vector2(core->getXRes(), core->getYRes()));
		}
	}
				
}

void PostEditorPane::handleEvent(Event *event) {
	if(event->getDispatcher() == nameProp) {
		currentMaterial->setName(nameProp->get());
		dispatchEvent(new Event(), Event::CHANGE_EVENT);					
	} else if(event->getDispatcher() == mainSizer && event->getEventCode() == UIEvent::CHANGE_EVENT) {
			propList->Resize(propList->getWidth(), getHeight()-mainSizer->getMainHeight());
			propList->updateProps();
			adjustPreview();
	} else if(event->getDispatcher() == passProps || event->getDispatcher() == targetBindingProps) {
		if(event->getEventCode() == Event::REMOVE_EVENT) {
				optionsPropList->visible = false;
				optionsPropList->enabled = false;	
		} else if(event->getEventCode() == Event::CHANGE_EVENT) {
			postPreview->clearMaterial();
			postPreview->setMaterial(currentMaterial);
		} else if(event->getEventCode() == Event::SELECT_EVENT) {
			if(passProps->selectedProp) {
				int shaderIndex = passProps->selectedProp->shaderIndex;
				std::shared_ptr<Material> material = passProps->selectedProp->material;
				std::shared_ptr<ShaderBinding> binding = material->getShaderBinding(shaderIndex);
				targetBindingProps->setShader(currentMaterial->getShader(shaderIndex), material, binding);
				shaderOptionsSheet->setShader(currentMaterial->getShader(shaderIndex), material, binding);
				optionsPropList->visible = true;
				optionsPropList->enabled = true;	
				
			}
		}
	} else if(event->getDispatcher() == fp16Prop && event->getEventCode() == Event::CHANGE_EVENT) {
		currentMaterial->fp16RenderTargets = fp16Prop->get();
		currentMaterial->recreateRenderTargets(Vector2(core->getXRes(), core->getYRes()));
	}
}



CubemapEditorPane::CubemapEditorPane(Core *core, ResourcePool *resourcePool, ResourcePool *localPool) : UIElement(core) {
	currentCubemap = NULL;

	headerBg = new UIRect(core, resourcePool, 10,10);
	addChild(headerBg);
	headerBg->setAnchorPoint(-1.0, -1.0, 0.0);
	headerBg->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderBgColor")); 
	
	propList = new PropList(core, resourcePool, "CUBEMAP EDITOR");
	addChild(propList);
	propList->setPosition(0, 0);
	
	PropSheet *previewProps = new PropSheet(core, resourcePool, "CUBEMAP PREVIEW", "");
	propList->addPropSheet(previewProps);
	MaterialPreviewProp *materialPreviewProp = new MaterialPreviewProp(core, resourcePool);
	previewProps->addProp(materialPreviewProp); 
	cubemapPreview = materialPreviewProp->previewBox;
	previewProps->propHeight = 300;		

	PropSheet *baseProps1 = new PropSheet(core, resourcePool, "CUBEMAP OPTIONS", "");
	propList->addPropSheet(baseProps1);

	nameProp = new StringProp(core, resourcePool, "Name", this);
	baseProps1->addProp(nameProp);
	
	baseProps1->propHeight = 70;
	
	PropSheet *baseProps = new PropSheet(core, resourcePool, "CUBEMAP TEXTURES", "");
	propList->addPropSheet(baseProps);

	xPosTexture = new TextureProp(core, resourcePool, "X+");
	baseProps->addProp(xPosTexture);
	xNegTexture = new TextureProp(core, resourcePool, "X-");
	baseProps->addProp(xNegTexture);	
	yPosTexture = new TextureProp(core, resourcePool, "Y+");
	baseProps->addProp(yPosTexture);		
	yNegTexture = new TextureProp(core, resourcePool, "Y-");
	baseProps->addProp(yNegTexture);
	zPosTexture = new TextureProp(core, resourcePool, "Z+");
	baseProps->addProp(zPosTexture);
	zNegTexture = new TextureProp(core, resourcePool, "Z-");
	baseProps->addProp(zNegTexture);

	nameProp->addEventListener(this, Event::CHANGE_EVENT);	
	yPosTexture->addEventListener(this, Event::CHANGE_EVENT);
	yNegTexture->addEventListener(this, Event::CHANGE_EVENT);
	xPosTexture->addEventListener(this, Event::CHANGE_EVENT);
	xNegTexture->addEventListener(this, Event::CHANGE_EVENT);
	zPosTexture->addEventListener(this, Event::CHANGE_EVENT);
	zNegTexture->addEventListener(this, Event::CHANGE_EVENT);						
	
	baseProps->propHeight = 420;
	propList->updateProps();
		
	std::shared_ptr<Material> previewMaterial = resourcePool->getMaterial("SkyBox");	
	cubemapPreview->setMaterial(previewMaterial);
	enabled = false;
	
}


void CubemapEditorPane::Activate() {
	cubemapPreview->previewScene->enabled = true;
	cubemapPreview->renderTexture->enabled = true;
}

void CubemapEditorPane::Deactivate() {
	cubemapPreview->previewScene->enabled = false;
	cubemapPreview->renderTexture->enabled = false;
}


void CubemapEditorPane::setCubemap(std::shared_ptr<Cubemap> cubemap) {
	enabled = true;
	currentCubemap = cubemap;
	
	nameProp->set(cubemap->getResourceName());

	// RENDERER_TODO
//	cubemapPreview->previewPrimitive->getLocalShaderOptions()->clearCubemap("lightCube");
//	cubemapPreview->previewPrimitive->getLocalShaderOptions()->addCubemap("lightCube", cubemap);
	
	yPosTexture->set(cubemap->getTexture(Cubemap::CUBEMAP_YPOS));
	yNegTexture->set(cubemap->getTexture(Cubemap::CUBEMAP_YNEG));
	xPosTexture->set(cubemap->getTexture(Cubemap::CUBEMAP_XPOS));
	xNegTexture->set(cubemap->getTexture(Cubemap::CUBEMAP_XNEG));
	zPosTexture->set(cubemap->getTexture(Cubemap::CUBEMAP_ZPOS));
	zNegTexture->set(cubemap->getTexture(Cubemap::CUBEMAP_ZNEG));				
	
}

void CubemapEditorPane::handleEvent(Event *event) {

	if(event->getDispatcher() == nameProp) {
		currentCubemap->setResourceName(nameProp->get());
		dispatchEvent(new Event(), Event::CHANGE_EVENT);					
	}

	if(event->getDispatcher() == yPosTexture) {
		currentCubemap->setTexture(yPosTexture->get(), Cubemap::CUBEMAP_YPOS);
		currentCubemap->recreateFromTextures();
		dispatchEvent(new Event(), Event::CHANGE_EVENT);			
	}	
	
	if(event->getDispatcher() == yNegTexture) {
		currentCubemap->setTexture(yNegTexture->get(), Cubemap::CUBEMAP_YNEG);
		currentCubemap->recreateFromTextures();
		dispatchEvent(new Event(), Event::CHANGE_EVENT);			
	}	

	if(event->getDispatcher() == xPosTexture) {
		currentCubemap->setTexture(xPosTexture->get(), Cubemap::CUBEMAP_XPOS);
		currentCubemap->recreateFromTextures();
		dispatchEvent(new Event(), Event::CHANGE_EVENT);			
	}	

	if(event->getDispatcher() == xNegTexture) {
		currentCubemap->setTexture(xNegTexture->get(), Cubemap::CUBEMAP_XNEG);
		currentCubemap->recreateFromTextures();
		dispatchEvent(new Event(), Event::CHANGE_EVENT);			
	}	

	if(event->getDispatcher() == zPosTexture) {
		currentCubemap->setTexture(zPosTexture->get(), Cubemap::CUBEMAP_ZPOS);
		currentCubemap->recreateFromTextures();
		dispatchEvent(new Event(), Event::CHANGE_EVENT);			
	}	

	if(event->getDispatcher() == zNegTexture) {
		currentCubemap->setTexture(zNegTexture->get(), Cubemap::CUBEMAP_ZNEG);
		currentCubemap->recreateFromTextures();
		dispatchEvent(new Event(), Event::CHANGE_EVENT);			
	}	
	
}

CubemapEditorPane::~CubemapEditorPane() {

}

void CubemapEditorPane::Resize(Number width, Number height) {
	headerBg->Resize(width, 30);	
	propList->Resize(width, height);
	propList->updateProps();
}


ShaderEditorPane::ShaderEditorPane(Core *core, ResourcePool *resourcePool, ResourcePool *localPool) : UIElement(core), localPool(localPool)
{
	changingShader = false;
	currentShader = NULL;

	headerBg = new UIRect(core, resourcePool, 10,10);
	addChild(headerBg);
	headerBg->setAnchorPoint(-1.0, -1.0, 0.0);
	headerBg->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderBgColor")); 
	
	propList = new PropList(core, resourcePool, "SHADER EDITOR");
	addChild(propList);
	propList->setPosition(0, 0);
	
	PropSheet *baseProps = new PropSheet(core, resourcePool, "SHADER SETTINGS", "");
	propList->addPropSheet(baseProps);
	
	nameProp = new StringProp(core, resourcePool, "Name", this);
	baseProps->addProp(nameProp);	
	nameProp->addEventListener(this, Event::CHANGE_EVENT);	
	
	screenShaderProp = new BoolProp(core, resourcePool, "Screen shader");
	baseProps->addProp(screenShaderProp);	
	screenShaderProp->addEventListener(this, Event::CHANGE_EVENT);	
	
	
	vertexProgramProp = new ComboProp(core, resourcePool, "Vertex prog.");
	baseProps->addProp(vertexProgramProp);	
	vertexProgramProp->addEventListener(this, Event::CHANGE_EVENT); 

	fragmentProgramProp = new ComboProp(core, resourcePool, "Fragment prog.");
	baseProps->addProp(fragmentProgramProp);	
	fragmentProgramProp->addEventListener(this, Event::CHANGE_EVENT);	
	
	pointLightsProp = new NumberProp(core, resourcePool, "Num point lights", this);
	baseProps->addProp(pointLightsProp);
	pointLightsProp->addEventListener(this, Event::CHANGE_EVENT);

	spotLightsProp = new NumberProp(core, resourcePool, "Num spotlights", this);
	baseProps->addProp(spotLightsProp); 
	spotLightsProp->addEventListener(this, Event::CHANGE_EVENT);

	baseProps->propHeight = 220;
	
	propList->updateProps();
	
	enabled = false;
}

ShaderEditorPane::~ShaderEditorPane() {

}

void ShaderEditorPane::handleEvent(Event *event) {
	if(!changingShader) {
	
		if(event->getDispatcher() == nameProp) {
			currentShader->setName(nameProp->get());
			dispatchEvent(new Event(), Event::CHANGE_EVENT);			
		}
	
		if(event->getDispatcher() == screenShaderProp) {
			currentShader->screenShader = screenShaderProp->get();
			dispatchEvent(new Event(), Event::CHANGE_EVENT);			
		}
				
		if(event->getDispatcher() == pointLightsProp) {
			currentShader->numPointLights = floor(pointLightsProp->get());
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}

		if(event->getDispatcher() == spotLightsProp) {
			currentShader->numSpotLights = floor(spotLightsProp->get());
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}		
		
		if(event->getDispatcher() == vertexProgramProp) {
			std::shared_ptr<ShaderProgram> vpProgram = std::static_pointer_cast<ShaderProgram>(vertexProgramProp->comboEntry->getSelectedItem()->sharedData);
			if(vpProgram) {
				currentShader->vertexProgram = vpProgram;
				dispatchEvent(new Event(), Event::CHANGE_EVENT);
			} else {
				globalFrame->assetBrowser->addEventListener(this, UIEvent::OK_EVENT);
				std::vector<String> extensions;
				extensions.push_back("vert");
				choosingVertexProgram = true;
				globalFrame->showAssetBrowser(extensions);				
			}
		}		

		if(event->getDispatcher() == fragmentProgramProp) {
			std::shared_ptr<ShaderProgram> fpProgram = std::static_pointer_cast<ShaderProgram>(fragmentProgramProp->comboEntry->getSelectedItem()->sharedData);
			if(fpProgram) {
				currentShader->fragmentProgram = fpProgram;
				dispatchEvent(new Event(), Event::CHANGE_EVENT);
			} else {
				globalFrame->assetBrowser->addEventListener(this, UIEvent::OK_EVENT);
				std::vector<String> extensions;
				extensions.push_back("frag");
				choosingVertexProgram = false;
				globalFrame->showAssetBrowser(extensions);			
			}
		}
	}
	
	if(event->getDispatcher() == globalFrame->assetBrowser && event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::OK_EVENT) {
		String newProgramPath = globalFrame->assetBrowser->getFullSelectedAssetPath();
		globalFrame->assetBrowser->removeAllHandlersForListener(this);
		

		OSFileEntry entry(newProgramPath, OSFileEntry::TYPE_FILE);
		std::shared_ptr<ShaderProgram> newProgram = std::make_shared<ShaderProgram>(newProgramPath);
		newProgram->reloadOnFileModify = true;
		if(newProgram) {
			newProgram->setResourceName(entry.name);
			newProgram->setResourcePath(newProgramPath);
			localPool->addResource(newProgram);
		}

		if(choosingVertexProgram) {
			currentShader->vertexProgram = newProgram;
		} else {
			currentShader->fragmentProgram = newProgram;
		}
		
		dispatchEvent(new Event(), Event::CHANGE_EVENT);		
		globalFrame->hideModal();
		
		reloadPrograms();
	}	
}

void ShaderEditorPane::reloadPrograms() {
	vertexProgramProp->comboEntry->clearItems();
	fragmentProgramProp->comboEntry->clearItems();
		
	vertexProgramProp->comboEntry->addComboItem("Custom...", NULL);
	fragmentProgramProp->comboEntry->addComboItem("Custom...", NULL);	
		
	std::vector<std::shared_ptr<Resource> > programResources = core->getResourceManager()->getResources(Resource::RESOURCE_PROGRAM);
	

	for(auto resource:programResources) {
		std::shared_ptr<ShaderProgram> program = std::static_pointer_cast<ShaderProgram>(resource);
		if(program->type == ShaderProgram::TYPE_VERT) {
			vertexProgramProp->comboEntry->addComboItemWithSharedData(program->getResourceName(), program);
			if(program == currentShader->vertexProgram) {
				vertexProgramProp->comboEntry->setSelectedIndex(vertexProgramProp->comboEntry->getNumItems()-1);
			}
		} else if(program->type == ShaderProgram::TYPE_FRAG) {
			fragmentProgramProp->comboEntry->addComboItemWithSharedData(program->getResourceName(), program);
			if(program == currentShader->fragmentProgram) {
				fragmentProgramProp->comboEntry->setSelectedIndex(fragmentProgramProp->comboEntry->getNumItems()-1);
			}
		}
	}

}

void ShaderEditorPane::setShader(std::shared_ptr<Shader> shader) {
	changingShader = true;
	
	currentShader = shader;
	
	reloadPrograms();
		
	nameProp->set(shader->getName());
	for(int i=0; i < vertexProgramProp->comboEntry->getNumItems(); i++) {
		std::shared_ptr<ShaderProgram> program = std::static_pointer_cast<ShaderProgram>(vertexProgramProp->comboEntry->getItemAtIndex(i)->sharedData);
		if(program == shader->vertexProgram) {
			vertexProgramProp->comboEntry->setSelectedIndex(i);
		}
	}

	for(int i=0; i < fragmentProgramProp->comboEntry->getNumItems(); i++) {
		std::shared_ptr<ShaderProgram> program = std::static_pointer_cast<ShaderProgram>(fragmentProgramProp->comboEntry->getItemAtIndex(i)->sharedData);
		if(program == shader->fragmentProgram) {
			fragmentProgramProp->comboEntry->setSelectedIndex(i);
		}
	}
	
	screenShaderProp->set(shader->screenShader);
	
	pointLightsProp->set(shader->numPointLights);
	spotLightsProp->set(shader->numSpotLights);
	
	enabled = true;
	
	changingShader = false; 
}

void ShaderEditorPane::Resize(Number width, Number height) {
	headerBg->Resize(width, 30);	
	propList->Resize(width, height);
	propList->updateProps();
}

PostPreviewBox::PostPreviewBox(Core *core, ResourcePool *pool) : UIElement(core) {

	headerBg = new UIRect(core, pool, 10,10);
	addChild(headerBg);
	headerBg->setAnchorPoint(-1.0, -1.0, 0.0);
	headerBg->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderBgColor")); 

	UILabel *label = new UILabel(core, pool, "POST PREVIEW", 18, "section", Label::ANTIALIAS_FULL);
	label->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderFontColor"));
	addChild(label);
	label->setPosition(10, 3);

	currentMaterial = NULL;
	previewScene = new Scene(core, Scene::SCENE_3D);
	
	renderTexture = new SceneRenderTexture(256, 256, true);

	previewScene->clearColor.setColor(0.0, 0.0, 0.0, 1.0);	
	previewScene->ambientColor.setColor(0.0, 0.0, 0.0, 1.0);

	mainLight = new SceneLight(SceneLight::POINT_LIGHT, 90.0);
	mainLight->setPosition(-6,6,6);
	previewScene->addChild(mainLight);

	secondLight = new SceneLight(SceneLight::POINT_LIGHT, 90.0);
	secondLight->setPosition(6,-6,6);
	previewScene->addChild(secondLight);

	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_PLANE, 10.0, 10.0);
	previewScene->addChild(previewPrimitive);
	previewPrimitive->setMaterial(pool->getMaterial("DefaultHDR"));
	previewPrimitive->setColorInt(198, 192, 166, 255);	
	
	if(previewPrimitive->getMaterial()) {
		previewPrimitive->getMaterial()->getShaderBinding(0)->getLocalParamByName("shininess")->setNumber(90);
	}
	
	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, 0.9, 0.45, 26, 16);
	previewScene->addChild(previewPrimitive);
	previewPrimitive->setMaterial(pool->getMaterial("DefaultHDR"));
	previewPrimitive->setColorInt(255, 0, 0, 255);	
	previewPrimitive->setPosition(1.5, 0.4, 1.0);

	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_SPHERE, 1.0, 16, 16);
	previewScene->addChild(previewPrimitive);
	previewPrimitive->setMaterial(pool->getMaterial("DefaultHDR"));
	previewPrimitive->setColorInt(0, 255, 0, 255);
	previewPrimitive->setPosition(-0.8, 1.0, 0.8);

	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 2,	2, 2);	
	previewScene->addChild(previewPrimitive);
	previewPrimitive->setMaterial(pool->getMaterial("DefaultHDR"));
	previewPrimitive->setColorInt(0, 0, 255, 255);
	previewPrimitive->setYaw(20);
	previewPrimitive->setPosition(1.0, 1.0, -2.5);
	
	previewScene->getDefaultCamera()->setPosition(0,3.0,5);
	previewScene->getDefaultCamera()->lookAt(Vector3(0.0, 0.5, 0.0));

	
	previewBase = new Entity();
	previewBase->processInputEvents = true;
	previewBase->setPosition(0, 30);
	addChild(previewBase);
	
	previewShape = new UIRect(core, pool, 256, 256);
	previewShape->setAnchorPoint(-1.0, -1.0, 0.0);	
	previewShape->setTexture(renderTexture->getTargetTexture());
//	previewShape->strokeEnabled = true;
//	previewShape->strokeColor = Color(1.0, 1.0, 1.0, 0.2);
//	previewShape->setStrokeWidth(1.0);
	previewBase->addChild(previewShape);

	spinValue = 0.0;
	
	rotateCheckBox = new UICheckBox(core, pool, "Auto-rotate", false);
	addChild(rotateCheckBox);
	rotateCheckBox->setPosition(150, 2);

	label = new UILabel(core, pool, "EXPOSURE", 18, "section", Label::ANTIALIAS_FULL);
	label->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderFontColor"));
	addChild(label);
	label->setPosition(270, 3);
	
	cameraExposureInput = new UITextInput(core, pool, false, 40, 12);
	addChild(cameraExposureInput);
	cameraExposureInput->setPosition(370, 2);
//	cameraExposureInput->setText(String::NumberToString(previewScene->getDefaultCamera()->getExposureLevel()));
	cameraExposureInput->addEventListener(this, UIEvent::CHANGE_EVENT);

	label = new UILabel(core, pool, "LIGHT INT.", 18, "section", Label::ANTIALIAS_FULL);
	label->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderFontColor"));
	addChild(label);
	label->setPosition(430, 3);

	lightStrength = new UITextInput(core, pool, false, 40, 12);
	addChild(lightStrength);
	lightStrength->setPosition(530, 2);
	lightStrength->setText("90.0");
	lightStrength->addEventListener(this, UIEvent::CHANGE_EVENT);
	
}

void PostPreviewBox::Update(Number elapsed) {
	spinValue += elapsed;
	
	if(rotateCheckBox->isChecked()) {
		previewScene->getDefaultCamera()->setPosition(sin(spinValue) * 5.0,3.0, cos(spinValue) * 5.0);
	} else {
		previewScene->getDefaultCamera()->setPosition(0,3.0,5);
	}	
	previewScene->getDefaultCamera()->lookAt(Vector3(0.0, 0.5, 0.0));		

}

void PostPreviewBox::Resize(Number width, Number height) {
	previewShape->Resize(width, height-30);
	headerBg->Resize(width, 30);
	
	int textureWidth = (int)width;
	int textureHeight = (int) (height-30);
	renderTexture->resizeRenderTexture(textureWidth, textureHeight);
	previewShape->setTexture(renderTexture->getTargetTexture());	
	UIElement::Resize(width, height);
	
	if(currentMaterial) {
		for(int i=0; i < currentMaterial->getNumShaderRenderTargets(); i++) {
			currentMaterial->getShaderRenderTarget(i)->normalizedWidth = textureWidth;
			currentMaterial->getShaderRenderTarget(i)->normalizedHeight = textureHeight;
			currentMaterial->recreateRenderTarget(currentMaterial->getShaderRenderTarget(i), Vector2(core->getXRes(), core->getYRes()));
		}
	}
}

PostPreviewBox::~PostPreviewBox() {
	previewScene->rootEntity.setOwnsChildrenRecursive(true);
	delete previewScene;
	delete renderTexture;
}

void PostPreviewBox::setMaterial(std::shared_ptr<Material> material) {
	previewScene->getDefaultCamera()->setPostFilter(material);
}

void PostPreviewBox::clearMaterial() {
	previewScene->getDefaultCamera()->removePostFilter();
}

void PostPreviewBox::handleEvent(Event *event) {
	if(event->getDispatcher() == cameraExposureInput && event->getEventCode() == UIEvent::CHANGE_EVENT) {
		Number newExposure = atof(cameraExposureInput->getText().c_str());
		//previewScene->getDefaultCamera()->setExposureLevel(newExposure);
	} else if(event->getDispatcher() == lightStrength && event->getEventCode() == UIEvent::CHANGE_EVENT) {
		Number newStrength = atof(lightStrength->getText().c_str());
		mainLight->setIntensity(newStrength);
		secondLight->setIntensity(newStrength);		
	}
}

MaterialPreviewBox::MaterialPreviewBox(Core *core, ResourcePool *pool) : UIElement(core) {
	currentMaterial = NULL;
	previewScene = new Scene(core,Scene::SCENE_3D);
	
	renderTexture = new SceneRenderTexture(512, 512, false);
	
	ScenePrimitive *previewBg = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 15.0, 15.0, 15.0);
	previewBg->Yaw(45.0);
	previewBg->backfaceCulled = false;
	
	previewBg->setMaterial(pool->getMaterial("Unlit"));
	std::shared_ptr<Texture> tex = pool->loadTexture("materialEditor/material_grid.png");
	
	if(previewBg->getNumShaderPasses() > 0) {
		previewBg->getShaderPass(0).shaderBinding->setTextureForParam("diffuse", tex);
	}
	previewScene->addChild(previewBg);
	
	previewScene->clearColor.setColor(0.1, 0.1, 0.1, 0.0);	
	previewScene->ambientColor.setColor(0.2, 0.2, 0.2, 1.0);
			
	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, 3.0, 1.2, 26, 16);
	previewScene->addChild(previewPrimitive);
	shapePrimitives.push_back(previewPrimitive);
	

	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_SPHERE, 3.0, 16, 16);
	previewScene->addChild(previewPrimitive);
	shapePrimitives.push_back(previewPrimitive);

	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 4.0, 4.0, 4.0);
	previewPrimitive->Yaw(45.0);
	previewScene->addChild(previewPrimitive);
	shapePrimitives.push_back(previewPrimitive);

	previewPrimitive = new ScenePrimitive(ScenePrimitive::TYPE_VPLANE, 6.0, 6.0);
	previewPrimitive->billboardMode = true;
	previewScene->addChild(previewPrimitive);
	shapePrimitives.push_back(previewPrimitive);	

	
	mainLight = new SceneLight(SceneLight::POINT_LIGHT, 290.0);
	mainLight->setPosition(-10,10,10);
	previewScene->addChild(mainLight);

	secondLight = new SceneLight(SceneLight::POINT_LIGHT,  250.0);
	secondLight->setPosition(10,-10,10);
	previewScene->addChild(secondLight);

	
	previewScene->getDefaultCamera()->setPosition(0,7,9);
	previewScene->getDefaultCamera()->lookAt(Vector3());
	
	previewBase = new Entity();
	previewBase->processInputEvents = true;
	previewBase->setPosition(0, 0);
	addChild(previewBase);
	
	previewShape = new UIRect(core, pool, 256, 256);
	previewShape->setAnchorPoint(-1.0, -1.0, 0.0);	
	previewShape->setTexture(renderTexture->getTargetTexture());
	previewBase->addChild(previewShape);
	
	shapeSelector = new UIImage(core, pool, "materialEditor/small_selector.png", 22, 22);
	previewBase->addChild(shapeSelector);
	shapeSelector->color.a = 1.0;
	
	shapeSwitches.push_back(new UIImageButton(core, pool, "materialEditor/torus_icon.png", 1.0, 18, 18));
	shapeSwitches.push_back(new UIImageButton(core, pool, "materialEditor/sphere_icon.png", 1.0, 18, 18));
	shapeSwitches.push_back(new UIImageButton(core, pool, "materialEditor/box_icon.png", 1.0, 18, 18));
	shapeSwitches.push_back(new UIImageButton(core, pool, "materialEditor/plane_icon.png", 1.0, 18, 18));

	for(int i=0; i < shapeSwitches.size(); i++) {
		previewBase->addChild(shapeSwitches[i]);
		shapeSwitches[i]->setPosition(270, 10+(25 * i));
		shapeSwitches[i]->addEventListener(this, UIEvent::CLICK_EVENT);
	}

	showPrimitive(0);
}

void MaterialPreviewBox::Render(GPUDrawBuffer *buffer) {
	renderTexture->Render(buffer->renderFrame, previewScene, previewScene->getDefaultCamera());
}

void MaterialPreviewBox::clearMaterial() {
	previewPrimitive->clearMaterial();
}

MaterialPreviewBox::~MaterialPreviewBox() {
	delete renderTexture;
	previewScene->rootEntity.setOwnsChildrenRecursive(true);
	delete previewScene;
}

void MaterialPreviewBox::setMaterial(std::shared_ptr<Material> material) {
	currentMaterial = material;
	previewPrimitive->setMaterial(material);	
	
}

void MaterialPreviewBox::showPrimitive(unsigned int index) {
	for(int i=0; i < shapePrimitives.size(); i++) {
		shapePrimitives[i]->visible = false;	
	}
	shapePrimitives[index]->visible = true; 
	
	shapeSelector->setPosition(270 - 2, 10 + ((25 * index) - 2));
	
	if(currentMaterial) {
		shapePrimitives[index]->setMaterial(currentMaterial);
	}
	previewPrimitive = shapePrimitives[index];

}

void MaterialPreviewBox::handleEvent(Event *event) {
	for(int i=0; i < shapeSwitches.size(); i++) {
		if(event->getDispatcher() == shapeSwitches[i]) {
			if(event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::CLICK_EVENT) {
				showPrimitive(i);
			}
		}
	}
}

MaterialPreviewProp::MaterialPreviewProp(Core *core, ResourcePool *pool) : PropProp(core, pool, "", "") {
	previewBox = new MaterialPreviewBox(core, pool);
	propContents->addChild(previewBox);		
	setHeight(300); 
}

void MaterialPreviewProp::setPropWidth(Number width) {
	previewBox->setPosition(((width-300.0)/2.0) - propContents->getPosition().x, 0.0);
}

MaterialEditorPane::MaterialEditorPane(Core *core, ResourcePool *pool, ResourcePool *localPool) : UIElement(core), localPool(localPool) {

	changingMaterial = false;
	
	headerBg = new UIRect(core, pool, 10,10);
	addChild(headerBg);
	headerBg->setAnchorPoint(-1.0, -1.0, 0.0);
	headerBg->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderBgColor")); 
	
	
	propList = new PropList(core, pool, "MATERIAL EDITOR");
	addChild(propList);
	propList->setPosition(0, 0);

	PropSheet *previewProps = new PropSheet(core, pool, "MATERIAL PREVIEW", "");
	propList->addPropSheet(previewProps);
	MaterialPreviewProp *materialPreviewProp = new MaterialPreviewProp(core, pool);
	previewProps->addProp(materialPreviewProp); 
	materialPreview = materialPreviewProp->previewBox;
	previewProps->propHeight = 300; 

	PropSheet *baseProps = new PropSheet(core, pool, "MATERIAL SETTINGS", "");
	propList->addPropSheet(baseProps);
	
	nameProp = new StringProp(core, pool, "Name", this);
	baseProps->addProp(nameProp);	
	nameProp->addEventListener(this, Event::CHANGE_EVENT);
		
	shaderProp = new ComboProp(core, pool, "Shader");
	baseProps->addProp(shaderProp);
	shaderProp->addEventListener(this, Event::CHANGE_EVENT);
	
	blendModeProp = new ComboProp(core, pool, "Blend mode");
	baseProps->addProp(blendModeProp);
	blendModeProp->addEventListener(this, Event::CHANGE_EVENT);

	blendModeProp->comboEntry->addComboItem("None");
	blendModeProp->comboEntry->addComboItem("Normal");
	blendModeProp->comboEntry->addComboItem("Lighten");
	blendModeProp->comboEntry->addComboItem("Color");
	blendModeProp->comboEntry->addComboItem("Premultiplied");
	blendModeProp->comboEntry->addComboItem("Multiply");	
	
	baseProps->propHeight = 130;	
	
	shaderOptionsSheet = new ShaderOptionsSheet(core, pool);
	propList->addPropSheet(shaderOptionsSheet);
	shaderOptionsSheet->addEventListener(this, Event::CHANGE_EVENT);
		
	propList->updateProps();
							
		
	ConfigRef conf = core->getConfig();
	String fontName = conf->getStringValue("Polycode", "uiDefaultFontName");	
				
	currentMaterial = NULL; 
	
	reloadShaders();
	
	enabled = false;
}

void MaterialEditorPane::reloadShaders() {
	shaderProp->comboEntry->clearItems();
	
	
	std::vector<std::shared_ptr<Resource> > shaderResources = localPool->getResources(Resource::RESOURCE_SHADER);
	
	for(auto resource:shaderResources) {
		std::shared_ptr<Shader> shader = std::static_pointer_cast<Shader>(resource);
		if(!shader->screenShader) {
			shaderProp->comboEntry->addComboItemWithSharedData(shader->getName(), shader);
		}
	}
}

void MaterialEditorPane::Resize(Number width, Number height) {
	headerBg->Resize(width, 30);	
	propList->Resize(width, height);
	propList->updateProps();
}

void MaterialEditorPane::handleEvent(Event *event) {

	if(event->getDispatcher() == &*currentMaterial) {
		shaderOptionsSheet->setShader(currentMaterial->getShader(0), currentMaterial, currentMaterial->getShaderBinding(0));
	}

	if(event->getDispatcher() == shaderOptionsSheet) {
		if(!changingMaterial) {
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}		
		
	} else if(event->getDispatcher() == blendModeProp) {
		currentMaterial->blendingMode = blendModeProp->get();
		if(!changingMaterial) {
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}
	} else if(event->getDispatcher() == nameProp) {
		currentMaterial->setName(nameProp->get());		  
		if(!changingMaterial) {
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}
	} else if(event->getDispatcher() == shaderProp) {
		
		std::shared_ptr<Shader> selectedShader = std::static_pointer_cast<Shader>(shaderProp->comboEntry->getSelectedItem()->sharedData);
		if(selectedShader) {			
			if(currentMaterial->getShader(0) != selectedShader) {
				currentMaterial->clearShaders();
				materialPreview->clearMaterial();
				
				std::shared_ptr<ShaderBinding> newShaderBinding = std::make_shared<ShaderBinding>();
				ShaderPass newPass;
				newPass.shader = selectedShader;
				newPass.shaderBinding = newShaderBinding;
				currentMaterial->addShaderPass(newPass);
				materialPreview->setMaterial(currentMaterial);					
			}
			
			shaderOptionsSheet->setShader(selectedShader, currentMaterial, currentMaterial->getShaderBinding(0));
		}
		
		if(!changingMaterial) {
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}		
	}
}

void MaterialEditorPane::Activate() {
	materialPreview->previewScene->enabled = true;
	materialPreview->renderTexture->enabled = true;
}

void MaterialEditorPane::Deactivate() {
	materialPreview->previewScene->enabled = false;
	materialPreview->renderTexture->enabled = false;
}

void MaterialEditorPane::setMaterial(std::shared_ptr<Material> material) {
	changingMaterial = true;
	
	if(currentMaterial) {
		currentMaterial->removeAllHandlersForListener(this);
	}
	
	currentMaterial = material;
	materialPreview->setMaterial(material);		
	
	material->addEventListener(this, Event::RESOURCE_RELOAD_EVENT);
	
	reloadShaders();
	
	if(!currentMaterial)
		return;
	
	blendModeProp->set(currentMaterial->blendingMode);
	
	if(currentMaterial->getShader(0)) { 
	for(int i=0; i < shaderProp->comboEntry->getNumItems(); i++) {
		std::shared_ptr<Shader> shader = std::static_pointer_cast<Shader>(shaderProp->comboEntry->getItemAtIndex(i)->sharedData);
		if(shader) {
			if(currentMaterial->getShader(0)->getName() == shader->getName()) {
				shaderProp->set(i);
				shaderProp->dispatchEvent(new Event(), Event::CHANGE_EVENT);
				break;
			}
		}
	}
	} else {
		shaderProp->set(0);
		shaderProp->dispatchEvent(new Event(), Event::CHANGE_EVENT);		
	}	
	
	nameProp->set(currentMaterial->getName());
	
	enabled = true;
	changingMaterial = false;
					
}

MaterialEditorPane::~MaterialEditorPane() {

}

MaterialMainWindow::MaterialMainWindow(Core *core, ResourcePool *resourcePool, ResourcePool *localResourcePool) : UIElement(core) {

	materialPane = new MaterialEditorPane(core, resourcePool, localResourcePool);
	shaderPane = new ShaderEditorPane(core, resourcePool, localResourcePool);
	cubemapPane = new CubemapEditorPane(core, resourcePool, localResourcePool);
	postPane = new PostEditorPane(core, resourcePool, localResourcePool);
	
	ownsChildren = true;
	
	addChild(materialPane);
	addChild(shaderPane);	
	addChild(cubemapPane);
	addChild(postPane);
}

void MaterialMainWindow::Activate() {
	materialPane->Activate();
	cubemapPane->Activate();
	cubemapPane->Activate();
}

void MaterialMainWindow::Deactivate() {
	materialPane->Deactivate();
	cubemapPane->Deactivate();
	cubemapPane->Deactivate();
}

MaterialMainWindow::~MaterialMainWindow() {
}
	
void MaterialMainWindow::Resize(Number width, Number height) {	
	Vector2 pos = getScreenPositionForMainCamera(); 
	scissorBox.setRect(pos.x,pos.y,width, height);
	materialPane->Resize(width, height);
	shaderPane->Resize(width, height);
	cubemapPane->Resize(width, height);
	postPane->Resize(width, height);
}

MaterialBrowser::MaterialBrowser(Core *core, ResourcePool *pool) : UIElement(core) {
	treeContainer = new UITreeContainer(core, pool, "boxIcon.png", L"Material Library", 200, 555);
	treeContainer->getRootNode()->toggleCollapsed();
	treeContainer->getRootNode()->addEventListener(this, UITreeEvent::SELECTED_EVENT);
	treeContainer->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
		
	shadersNode = treeContainer->getRootNode()->addTreeChild("materialEditor/shader_icon.png", "Shaders", NULL);
	materialsNode = treeContainer->getRootNode()->addTreeChild("materialEditor/material_icon.png", "Materials", NULL);
	cubemapsNode = treeContainer->getRootNode()->addTreeChild("materialEditor/cubemap_icon.png", "Cubemaps", NULL);
	postEffectsNode = treeContainer->getRootNode()->addTreeChild("materialEditor/screenshader_icon.png", "Post Effects", NULL);
				
	addChild(treeContainer);		
	selectedData = NULL;
	
	headerBg = new UIRect(core, pool, 10,10);
	addChild(headerBg);
	headerBg->setAnchorPoint(-1.0, -1.0, 0.0);
	headerBg->color.setColorHexFromString(core->getConfig()->getStringValue("Polycode", "uiHeaderBgColor")); 

	newShaderButton = new UIImageButton(core, pool, "materialEditor/new_shader.png", 1.0, 22, 22);
	addChild(newShaderButton);
	newShaderButton->setPosition(5,4);
	
	newMaterialButton = new UIImageButton(core, pool, "materialEditor/new_material.png", 1.0, 22, 22);
	addChild(newMaterialButton);
	newMaterialButton->setPosition(32,4);

	newCubemapButton = new UIImageButton(core, pool, "materialEditor/new_cubemap.png", 1.0, 22, 22);
	addChild(newCubemapButton);
	newCubemapButton->setPosition(59,4);
		
	newPostButton = new UIImageButton(core, pool, "materialEditor/new_screenshader.png", 1.0, 22, 22);
	addChild(newPostButton);
	newPostButton->setPosition(86,4);

	removeButton = new UIImageButton(core, pool, "main/remove_icon.png", 1.0, 12, 12);
	addChild(removeButton);
	removeButton->setPosition(0,4);
	
	selectedNode = NULL;

}

void MaterialBrowser::removeSelected() {
	selectedNode->getParent()->removeTreeChild(selectedNode);
}

void MaterialBrowser::handleEvent(Event *event) {

	if(event->getDispatcher() == treeContainer->getRootNode()) {
		if(event->getEventCode() == UITreeEvent::SELECTED_EVENT){ 
			selectedNode = treeContainer->getRootNode()->getSelectedNode();			
			MaterialBrowserData *data = (MaterialBrowserData *)selectedNode->getUserData();
			selectedData =	data;
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}
	}	
	Entity::handleEvent(event);
}


UITree *MaterialBrowser::addMaterial(std::shared_ptr<Material> material) {
	MaterialBrowserData *data = new MaterialBrowserData();
	data->material = material;
	return materialsNode->addTreeChild("materialEditor/material_icon.png", material->getName(), (void*)data);
}

UITree *MaterialBrowser::addShader(std::shared_ptr<Shader> shader) {
	MaterialBrowserData *data = new MaterialBrowserData();
	data->shader = shader;
	return shadersNode->addTreeChild("materialEditor/shader_icon.png", shader->getName(), (void*)data);
}

UITree *MaterialBrowser::addCubemap(std::shared_ptr<Cubemap> cubemap) {
	MaterialBrowserData *data = new MaterialBrowserData();
	data->cubemap = cubemap;
	return cubemapsNode->addTreeChild("materialEditor/cubemap_icon.png", cubemap->getResourceName(), (void*)data);
}

UITree *MaterialBrowser::addPostMaterial(std::shared_ptr<Material> material) {
	MaterialBrowserData *data = new MaterialBrowserData();
	data->postMaterial = material;
	return postEffectsNode->addTreeChild("materialEditor/screenshader_icon.png", material->getName(), (void*)data);
}

MaterialBrowser::~MaterialBrowser() {

}

void MaterialBrowser::Resize(Number width, Number height) {
	treeContainer->Resize(width, height-30);
	treeContainer->setPosition(0, 30);	
	headerBg->Resize(width, 30);	
	removeButton->setPosition(width - 24, 8);
}

PolycodeMaterialEditor::PolycodeMaterialEditor(Core *core, ResourcePool *pool) : PolycodeEditor(core, pool, true){

	selectedMaterialNode = NULL;
}

void PolycodeMaterialEditor::Activate() {
	mainWindow->Activate();
}

void PolycodeMaterialEditor::Deactivate() {
	mainWindow->Deactivate();
}

PolycodeMaterialEditor::~PolycodeMaterialEditor() {
	core->getResourceManager()->unsubscibeFromResourcePool(localResourcePool);
	mainWindow->setOwnsChildrenRecursive(true);
	delete mainWindow;
	delete mainSizer;
	delete materialBrowser;
}

bool PolycodeMaterialEditor::openFile(OSFileEntry filePath) {
	
	String resourceName = filePath.fullPath.replace(parentProject->getRootFolder()+"/", "");
	localResourcePool = core->getResourceManager()->getResourcePoolByName(resourceName);
	
	if(!localResourcePool) {
		localResourcePool = new ResourcePool(core, resourceName, core->getResourceManager()->getGlobalPool());
		resourcePool->reloadResourcesOnModify = true;
		resourcePool->deleteOnUnsubscribe = true;
		resourcePool->loadResourcesFromMaterialFile(filePath.fullPath);
		core->getResourceManager()->addResourcePool(localResourcePool);
	}
	core->getResourceManager()->subscribeToResourcePool(localResourcePool);
		
	mainSizer = new UIHSizer(core, resourcePool, 100,100,200,false);
	addChild(mainSizer);	
	
	materialBrowser = new MaterialBrowser(core, resourcePool);
	mainSizer->addRightChild(materialBrowser);
	
	materialBrowser->addEventListener(this, Event::CHANGE_EVENT);
	
	std::vector<std::shared_ptr<Resource> > res = localResourcePool->getResources(Resource::RESOURCE_SHADER);
	for(int i=0; i < res.size(); i++) {
		std::shared_ptr<Shader> shader = std::static_pointer_cast<Shader>(res[i]);
		materialBrowser->addShader(shader);
		shader->vertexProgram->reloadOnFileModify = true;
		shader->fragmentProgram->reloadOnFileModify = true;
		shaders.push_back(shader);
	}
	
	res = localResourcePool->getResources(Resource::RESOURCE_CUBEMAP);
	for(int i=0; i < res.size(); i++) {
		std::shared_ptr<Cubemap> cubemap = std::static_pointer_cast<Cubemap>(res[i]);
		materialBrowser->addCubemap(cubemap);
		cubemaps.push_back(cubemap);
	}
	
	res = localResourcePool->getResources(Resource::RESOURCE_MATERIAL);
	for(int i=0; i < res.size(); i++) {
		std::shared_ptr<Material> material = std::static_pointer_cast<Material>(res[i]);
		if(material->screenMaterial) {
			postMaterials.push_back(material);
			materialBrowser->addPostMaterial(material);
		} else {
			materials.push_back(material);
			materialBrowser->addMaterial(material);
		}
	}
	
	mainWindow = new MaterialMainWindow(core, resourcePool, localResourcePool);
	mainSizer->addLeftChild(mainWindow);
	
	mainWindow->materialPane->addEventListener(this, Event::CHANGE_EVENT);
	mainWindow->shaderPane->addEventListener(this, Event::CHANGE_EVENT);
	mainWindow->cubemapPane->addEventListener(this, Event::CHANGE_EVENT);
	mainWindow->postPane->addEventListener(this, Event::CHANGE_EVENT);
			
	mainWindow->shaderPane->parentProject = parentProject;
	
	materialBrowser->newMaterialButton->addEventListener(this, UIEvent::CLICK_EVENT);
	materialBrowser->newShaderButton->addEventListener(this, UIEvent::CLICK_EVENT);
	materialBrowser->newCubemapButton->addEventListener(this, UIEvent::CLICK_EVENT);
	materialBrowser->newPostButton->addEventListener(this, UIEvent::CLICK_EVENT);
	materialBrowser->removeButton->addEventListener(this, UIEvent::CLICK_EVENT);
					
	mainWindow->materialPane->addEventListener(this, Event::CHANGE_EVENT);
	
	PolycodeEditor::openFile(filePath);
	return true;
}

String PolycodeMaterialEditor::createStringValue(std::shared_ptr<LocalShaderParam> param) {
	String retString;
	
	switch(param->type) {
		case ProgramParam::PARAM_NUMBER:
			retString = String::NumberToString(*((Number*)param->data));
		break;
		case ProgramParam::PARAM_COLOR:
		{
			Color color = *((Color*)param->data);
			retString = String::NumberToString(color.r) + " " + String::NumberToString(color.g) + " " + String::NumberToString(color.b) + " " + String::NumberToString(color.a);
		}
		break;
		case ProgramParam::PARAM_VECTOR2:
		{
			Vector2 vec = *((Vector2*)param->data);
			retString = String::NumberToString(vec.x) + " " + String::NumberToString(vec.y);
		}
		break;
		case ProgramParam::PARAM_VECTOR3:
		{
			Vector3 vec = *((Vector3*)param->data);
			retString = String::NumberToString(vec.x) + " " + String::NumberToString(vec.y) + " " + String::NumberToString(vec.z);
		}
		break;
		case ProgramParam::PARAM_TEXTURE:
		{
            if(param->getTexture()) {
                retString = param->getTexture()->getResourcePath();
            }
		}
		break;
	}
	
	return retString;
}

void PolycodeMaterialEditor::saveMaterials(ObjectEntry *materialsEntry, std::vector<std::shared_ptr<Material> > materials) {
	for(int i=0; i < materials.size(); i++) {
		std::shared_ptr<Material> material = materials[i];				
		
		ObjectEntry *materialEntry = materialsEntry->addChild("material");
		materialEntry->addChild("name", material->getName());
		materialEntry->addChild("blendingMode", material->blendingMode);
		
		if(material->screenMaterial) {
			materialEntry->addChild("screen", true);
		}
		
		if(material->screenMaterial) {		
		if(material->getNumShaderRenderTargets() > 0) {		
			ObjectEntry *targetsEntry = materialEntry->addChild("rendertargets");
			if(material->fp16RenderTargets) {
				targetsEntry->addChild("type", String("rgba_fp16"));
			} else {
				targetsEntry->addChild("type", String("rgba"));			
			}
			
			for(int j=0; j < material->getNumShaderRenderTargets(); j++) {
				ObjectEntry *targetEntry = targetsEntry->addChild("rendertarget");
				
				ShaderRenderTarget *target = material->getShaderRenderTarget(j);
				
				targetEntry->addChild("id", target->id);
				if(target->sizeMode == ShaderRenderTarget::SIZE_MODE_NORMALIZED) {
					targetEntry->addChild("sizeMode", "normalized");
				} else {
					targetEntry->addChild("sizeMode", "pixels");
				}
				
				targetEntry->addChild("width", target->width);
				targetEntry->addChild("height", target->height);								
			}
			
			}
		}
		
		if(material->getNumShaderPasses() > 0) {
			for(int s=0; s < material->getNumShaderPasses(); s++) {
				std::shared_ptr<Shader> shader = material->getShader(s);
				
				ObjectEntry *shaderEntry = materialEntry->addChild("shader");
				shaderEntry->addChild("name", shader->getName());		
				ObjectEntry *texturesEntry = shaderEntry->addChild("textures");
				
				std::shared_ptr<ShaderBinding> shaderBinding = material->getShaderPass(s).shaderBinding;
				
				if(material->screenMaterial) {
					if(shaderBinding->getNumRenderTargetBindings() > 0) {
						ObjectEntry *targetsEntry = shaderEntry->addChild("targettextures");
						for(int j=0; j < shaderBinding->getNumRenderTargetBindings(); j++) {
							RenderTargetBinding *binding = shaderBinding->getRenderTargetBinding(j);
							ObjectEntry *targetEntry = targetsEntry->addChild("targettexture");
							switch(binding->mode) {
								case RenderTargetBinding::MODE_IN:
									targetEntry->addChild("mode", "in");								
									targetEntry->addChild("name", binding->name);
									targetEntry->addChild("id", binding->id);									
								break;
								case RenderTargetBinding::MODE_OUT:
									targetEntry->addChild("mode", "out");								
									targetEntry->addChild("name", binding->name);
									targetEntry->addChild("id", binding->id);								
								break;
								case RenderTargetBinding::MODE_COLOR:
									targetEntry->addChild("mode", "color");
									targetEntry->addChild("name", binding->name);		
								break;
								case RenderTargetBinding::MODE_DEPTH:
									targetEntry->addChild("mode", "depth");
									targetEntry->addChild("name", binding->name);
								break;																								
							}
						}
					}
				}
				
				if(shader->expectedParams.size() > 0 || shader->expectedParams.size() > 0) {
					ObjectEntry *paramsEntry = shaderEntry->addChild("params");
					
					for(int j=0; j < shader->expectedParams.size(); j++) {
						if(shaderBinding->getLocalParamByName(shader->expectedParams[j].name)) {
							ObjectEntry *paramEntry = paramsEntry->addChild("param");
							paramEntry->addChild("name", shader->expectedParams[j].name);
							paramEntry->addChild("value", PolycodeMaterialEditor::createStringValue(shaderBinding->getLocalParamByName(shader->expectedParams[j].name)));
						}
					}
				}
			}
		}
	}
}

void PolycodeMaterialEditor::saveFile() {

	Object fileData;
	fileData.root.name = "polycode_material_library";

	ObjectEntry *shadersEntry = fileData.root.addChild("shaders");
	for(int i=0; i < shaders.size(); i++) {
		ObjectEntry *shaderEntry = shadersEntry->addChild("shader");
		shaderEntry->addChild("type", String("glsl"));
		shaderEntry->addChild("name", shaders[i]->getName());
		shaderEntry->addChild("numPointLights", shaders[i]->numPointLights);
		shaderEntry->addChild("numSpotLights", shaders[i]->numSpotLights);
		shaderEntry->addChild("screen", shaders[i]->screenShader);

		ObjectEntry *vpEntry = shaderEntry->addChild("vp");
		String sourcePath = shaders[i]->vertexProgram->getResourcePath();
		sourcePath = sourcePath.replace(parentProject->getRootFolder()+"/", "");
		vpEntry->addChild("source", sourcePath);
				
		ObjectEntry *fpEntry = shaderEntry->addChild("fp");
		sourcePath = shaders[i]->fragmentProgram->getResourcePath();
		sourcePath = sourcePath.replace(parentProject->getRootFolder()+"/", "");		
		fpEntry->addChild("source", sourcePath);
	}

	ObjectEntry *cubemapsEntry = fileData.root.addChild("cubemaps");
	
	for(int i=0; i < cubemaps.size(); i++) {
		ObjectEntry *cubemapEntry = cubemapsEntry->addChild("cubemap");
		cubemapEntry->addChild("name", cubemaps[i]->getResourceName());
		
		cubemapEntry->addChild("xPos", cubemaps[i]->getTexture(Cubemap::CUBEMAP_XPOS)->getResourcePath());
		cubemapEntry->addChild("xNeg", cubemaps[i]->getTexture(Cubemap::CUBEMAP_XNEG)->getResourcePath());
		cubemapEntry->addChild("yPos", cubemaps[i]->getTexture(Cubemap::CUBEMAP_YPOS)->getResourcePath());
		cubemapEntry->addChild("yNeg", cubemaps[i]->getTexture(Cubemap::CUBEMAP_YNEG)->getResourcePath());
		cubemapEntry->addChild("zPos", cubemaps[i]->getTexture(Cubemap::CUBEMAP_ZPOS)->getResourcePath());
		cubemapEntry->addChild("zNeg", cubemaps[i]->getTexture(Cubemap::CUBEMAP_ZNEG)->getResourcePath());										
	}
	
	ObjectEntry *materialsEntry = fileData.root.addChild("materials");
	saveMaterials(materialsEntry, materials);
	saveMaterials(materialsEntry, postMaterials);
			
	fileData.saveToXML(core, filePath);
	setHasChanges(false);
}

void PolycodeMaterialEditor::handleEvent(Event *event) {

	if(event->getDispatcher() == mainWindow->materialPane && event->getEventType() == "" && event->getEventCode() == Event::CHANGE_EVENT) {
		if(selectedMaterialNode && mainWindow->materialPane->currentMaterial) {
			selectedMaterialNode->setLabelText(mainWindow->materialPane->currentMaterial->getName());
		}
		setHasChanges(true);
	}

	if(event->getDispatcher() == mainWindow->shaderPane && event->getEventType() == "" && event->getEventCode() == Event::CHANGE_EVENT) {
		if(selectedMaterialNode && mainWindow->shaderPane->currentShader) {
			selectedMaterialNode->setLabelText(mainWindow->shaderPane->currentShader->getName());
		}
		setHasChanges(true);
	}

	if(event->getDispatcher() == mainWindow->cubemapPane && event->getEventType() == "" && event->getEventCode() == Event::CHANGE_EVENT) {
		if(selectedMaterialNode && mainWindow->cubemapPane->currentCubemap) {
			selectedMaterialNode->setLabelText(mainWindow->cubemapPane->currentCubemap->getResourceName());
		}
		setHasChanges(true);
	}

	if(event->getDispatcher() == mainWindow->postPane && event->getEventType() == "" && event->getEventCode() == Event::CHANGE_EVENT) {
		if(selectedMaterialNode && mainWindow->postPane->currentMaterial) {
			selectedMaterialNode->setLabelText(mainWindow->postPane->currentMaterial->getName());
		}
		setHasChanges(true);
	}

	if(event->getDispatcher() == materialBrowser->newPostButton && event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::CLICK_EVENT) {
		
		std::shared_ptr<Material> newMaterial = std::make_shared<Material>("PostEffect"+String::IntToString(postMaterials.size()));
			resourcePool->addResource(newMaterial);
			newMaterial->screenMaterial = true;
			materialBrowser->addPostMaterial(newMaterial)->setSelected();
			postMaterials.push_back(newMaterial);
			setHasChanges(true);			
	}	

	if(event->getDispatcher() == materialBrowser->newMaterialButton && event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::CLICK_EVENT) {
			std::shared_ptr<Material> newMaterial = std::make_shared<Material>("Untitled");
			resourcePool->addResource(newMaterial);
			materialBrowser->addMaterial(newMaterial)->setSelected();
			resourcePool->addResource(newMaterial);
			materials.push_back(newMaterial);
			setHasChanges(true);			
	}	

	if(event->getDispatcher() == materialBrowser->newShaderButton && event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::CLICK_EVENT) {

		std::shared_ptr<ShaderProgram> vertexProgram = std::static_pointer_cast<ShaderProgram>(resourcePool->getResourceByPath("default/Unlit.vert"));
		std::shared_ptr<ShaderProgram> fragmentProgram = std::static_pointer_cast<ShaderProgram>(resourcePool->getResourceByPath("default/Unlit.frag"));
		
		
		std::shared_ptr<Shader> newShader = std::make_shared<Shader>(vertexProgram, fragmentProgram);
		newShader->setName("Untitled");
		
		resourcePool->addResource(newShader);
		if(newShader) {
			materialBrowser->addShader(newShader)->setSelected();
			shaders.push_back(newShader);
			setHasChanges(true);
		} else {
			printf("Error creating shader!\n");
		}
	}	

	if(event->getDispatcher() == materialBrowser->newCubemapButton && event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::CLICK_EVENT) {
		
		std::shared_ptr<Texture> texture = std::static_pointer_cast<Texture>(resourcePool->getResource(Resource::RESOURCE_SHADER, "default.png"));
		
		std::shared_ptr<Cubemap> cubemap = std::make_shared<Cubemap>(texture, texture, texture, texture, texture, texture);
		cubemap->setResourceName("Cubemap"+String::IntToString(cubemaps.size()));
		cubemaps.push_back(cubemap);
		materialBrowser->addCubemap(cubemap)->setSelected();
		setHasChanges(true);
		resourcePool->addResource(cubemap);
	}	

	if(event->getDispatcher() == materialBrowser->removeButton && event->getEventType() == "UIEvent" && event->getEventCode() == UIEvent::CLICK_EVENT) {

				mainWindow->materialPane->enabled = false;
				mainWindow->shaderPane->enabled = false;
				mainWindow->cubemapPane->enabled = false;
				mainWindow->postPane->enabled = false;
				
				if(materialBrowser->selectedData) {
				if(materialBrowser->selectedData->material) {
					for(int i=0; i < materials.size(); i++) {
						if(materials[i] == materialBrowser->selectedData->material) {
							materials.erase(materials.begin()+i);
							materialBrowser->removeSelected();
							break;							
						}
					}
				} else if(materialBrowser->selectedData->shader) {
					for(int i=0; i < shaders.size(); i++) {
						if(shaders[i] == materialBrowser->selectedData->shader) {
							shaders.erase(shaders.begin()+i);
							materialBrowser->removeSelected();							
							break;
						}
					}				
				} else if(materialBrowser->selectedData->cubemap) {
					for(int i=0; i < cubemaps.size(); i++) {
						if(cubemaps[i] == materialBrowser->selectedData->cubemap) {
							cubemaps.erase(cubemaps.begin()+i);
							materialBrowser->removeSelected();							
							break;
						}
					}								
				} else if(materialBrowser->selectedData->postMaterial) {
					for(int i=0; i < postMaterials.size(); i++) {
						if(postMaterials[i] == materialBrowser->selectedData->postMaterial) {
							postMaterials.erase(postMaterials.begin()+i);
							materialBrowser->removeSelected();							
							break;
						}
					}												
				}	
				}			
		
	}

	if(event->getDispatcher() == materialBrowser) {
		if(event->getEventType() == "" && event->getEventCode() == Event::CHANGE_EVENT) {
			if(materialBrowser->selectedData)  {
				mainWindow->materialPane->enabled = false;
				mainWindow->shaderPane->enabled = false;
				mainWindow->cubemapPane->enabled = false;
				mainWindow->postPane->enabled = false;
																
				if(materialBrowser->selectedData->material) {
					selectedMaterialNode = materialBrowser->selectedNode;				
					mainWindow->materialPane->setMaterial(materialBrowser->selectedData->material);					
				} else if(materialBrowser->selectedData->shader) {
					selectedMaterialNode = materialBrowser->selectedNode;				
					mainWindow->shaderPane->setShader(materialBrowser->selectedData->shader);
				} else if(materialBrowser->selectedData->cubemap) {
					selectedMaterialNode = materialBrowser->selectedNode;				
					mainWindow->cubemapPane->setCubemap(materialBrowser->selectedData->cubemap);
				} else if(materialBrowser->selectedData->postMaterial) {
					selectedMaterialNode = materialBrowser->selectedNode;				
					mainWindow->postPane->setMaterial(materialBrowser->selectedData->postMaterial);
				}

			}			
		}
	}
}

void PolycodeMaterialEditor::Resize(int x, int y) {
	mainSizer->Resize(((Number)x),((Number)y));
	PolycodeEditor::Resize(((Number)x),((Number)y));
}

