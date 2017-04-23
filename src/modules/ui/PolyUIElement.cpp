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

#include "polycode/modules/ui/PolyUIElement.h"
#include "polycode/core/PolyRenderer.h"
#include "polycode/core/PolyResourceManager.h"
#include "polycode/core/PolyTexture.h"
#include "polycode/core/PolyConfig.h"
#include "polycode/core/PolyCoreInput.h"
#include "polycode/core/PolyInputEvent.h"
#include "polycode/core/PolyScene.h"
#include "polycode/core/PolyCamera.h"
#include "polycode/core/PolyLabel.h"

using namespace Polycode;

UIElement *UIElement::globalFocusedChild = NULL;

UIMultilineLabel::UIMultilineLabel(Core *core, ResourcePool *resourcePool, const String& text, int size, int spacing, const String& fontName, int amode) : resourcePool(resourcePool), UIElement(core)
{
	labelSize = size;
	labelFontName = fontName;
	labelAAMode = amode;
	this->spacing = spacing;
	setText(text);
}

void UIMultilineLabel::setText(const String& text) {
	clearLabels();
	
	std::vector<String> lines = text.split("\n");
	linesCount = lines.size();

	Number lineSize = spacing;
	Number yPos = 0.0;
	for(int i=0; i < lines.size(); i++) {
		if(lines[i] == "") {
			yPos += lineSize + spacing;
		} else {
			UILabel *label = new UILabel(core, resourcePool, lines[i], labelSize, labelFontName, labelAAMode);
			lineSize = label->getHeight();
			label->setPositionY(yPos);
			yPos += label->getHeight() + spacing;
			addChild(label);
			labels.push_back(label);
		}
	}
}

String UIMultilineLabel::getText() {
	String text;
	for(int i=0; i < labels.size(); i++) {
		if(i != 0) {
			text += "\n";
		}
		text += labels[i]->getText();
	}
	return text;
}

void UIMultilineLabel::setColor(Color color) {
	for (int i = 0; i < labels.size(); i++) {
		labels[i]->color.setColor(&color);
	}
}

void UIMultilineLabel::setColorInt(int r, int g, int b, int a) {
	for (int i = 0; i < labels.size(); i++) {
		labels[i]->color.setColorRGBA(r, g, b, a);
	}
}

void UIMultilineLabel::setColor(Number r, Number g, Number b, Number a) {
	for (int i = 0; i < labels.size(); i++) {
		labels[i]->color.setColor(r, g, b, a);
	}
}

void UIMultilineLabel::clearLabels() {
	for(int i=0; i < labels.size(); i++) {
		removeChild(labels[i]);
		delete labels[i];
	}
	labels.clear();
}

Number UIMultilineLabel::getWidth(){
	Number maxWidth = 0;
	for (int i = 0; i < labels.size(); i++) {
		if (labels[i]->getWidth() > maxWidth){
			maxWidth = labels[i]->getWidth();
		}
	}
	return maxWidth;
}

Number UIMultilineLabel::getHeight(){
	Number retHeight = 0;
	for (int i = 0; i < labels.size(); i++) {
		retHeight += labels[i]->getHeight() + spacing;
	}
	retHeight += (linesCount - labels.size()) * (labelSize + spacing);
	return retHeight;
}

UIMultilineLabel::~UIMultilineLabel() {
	if(!ownsChildren) {
		clearLabels();
	}
}

UILabel::UILabel(Core *core, ResourcePool *pool, const String& text, int size, const String& fontName, int amode) : UIElement(core) {
	
	ConfigRef conf = core->getConfig();
	if(size == -1) {
		size = conf->getNumericValue("Polycode", "uiDefaultFontSize");
	}
	Color fontColor;
	fontColor.setColorHexFromString(conf->getStringValue("Polycode", "uiDefaultFontColor"));

	label = new SceneLabel(pool->getMaterial("Unlit"), text, size, pool->getFont(fontName), amode);
	label->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	label->positionAtBaseline = true;
	label->setAnchorPoint(-1.0, -1.0, 0.0);
	label->snapToPixels = true;
	label->depthTest = false;
	label->depthWrite = false;
	label->color = fontColor;
	addChild(label);
	setLocalBoundingBox(label->getLocalBoundingBox());
}


void UILabel::setLabelColor(const Color &color) {
	label->color = color;
}

void UILabel::setText(const String& text) {
	label->setText(text);
	setLocalBoundingBox(label->getLocalBoundingBox());
}

String UILabel::getText() {
	return label->getText();
}

void UILabel::setColor(Color color) {
	label->color.setColor(&color);
}

void UILabel::setColorInt(int r, int g, int b, int a) {
	label->color.setColorRGBA(r, g, b, a);
}

void UILabel::setColor(Number r, Number g, Number b, Number a) {
	label->color.setColor(r, g, b, a);
}

UILabel::~UILabel() {
	if(!ownsChildren) {
		delete label;
	}
}

Label *UILabel::getLabel() {
	return label->getLabel();
}

UIRect::UIRect(Core *core, ResourcePool *pool, String fileName, Number width, Number height) : resourcePool(pool), UIElement(core)
{
	texture = NULL;
	loadTexture(fileName);
	initRect(width, height);
	imageWidth = width;
	imageHeight = height;
}

UIRect::UIRect(Core *core, ResourcePool *pool, String fileName) : resourcePool(pool), UIElement(core)
{
	texture = NULL;
	loadTexture(fileName);
	if(texture) {	
		initRect(texture->getWidth(), texture->getHeight());
		imageWidth = texture->getWidth();
		imageHeight = texture->getHeight(); 
	} else {
		initRect(1,	 1);
		imageWidth = 0;
		imageHeight = 0;
	}
	setWidth(imageWidth);
	setHeight(imageHeight);
}

UIRect::UIRect(Core *core, ResourcePool *pool, Number width, Number height) : resourcePool(pool), UIElement(core)
{
	texture = NULL;
	initRect(width, height);
	imageWidth = 0;
	imageHeight = 0;
}

void UIRect::setImageCoordinates(Number x, Number y, Number width, Number height, Number imageScale) {

	Number pixelSizeX = 1/imageWidth;
	Number pixelSizeY = 1/imageHeight;

	setWidth(width);
	setHeight(height);

	Number whalf = width/2.0f;
	Number hhalf = height/2.0f;
		
	Number xFloat = x * pixelSizeX * imageScale;
	Number yFloat = (y * pixelSizeY) * imageScale;
	Number wFloat = width * pixelSizeX * imageScale;
	Number hFloat = height * pixelSizeY * imageScale;

	rectMesh->clearMesh();
	
	rectMeshGeometry.vertexPositionArray.data.clear();
	rectMeshGeometry.vertexTexCoordArray.data.clear();
	rectMeshGeometry.indexArray.data.clear();
	
	rectMeshGeometry.addVertex(-whalf,-hhalf,0);
	rectMeshGeometry.addTexCoord(xFloat, (1.0-yFloat) - hFloat);

	rectMeshGeometry.addVertex(-whalf+width,-hhalf,0);
	rectMeshGeometry.addTexCoord(xFloat + wFloat, (1.0-yFloat) - hFloat);

	rectMeshGeometry.addVertex(-whalf+width,-hhalf+height,0);
	rectMeshGeometry.addTexCoord(xFloat + wFloat, 1.0-yFloat);

	rectMeshGeometry.addVertex(-whalf,-hhalf+height,0);
	rectMeshGeometry.addTexCoord(xFloat, 1.0-yFloat);

	rectMeshGeometry.addIndexedFace(0, 1, 2);
	rectMeshGeometry.addIndexedFace(0, 2, 3);
	
	rectMesh->addSubmesh(rectMeshGeometry);
	
	
	rebuildTransformMatrix();
	matrixDirty = true;
}

Number UIRect::getImageWidth() const {
	return imageWidth;
}

Number UIRect::getImageHeight() const {
	return imageHeight; 
}

void UIRect::initRect(Number width, Number height) {
	rectMesh = new Mesh();
	processInputEvents = true;

	setAnchorPoint(-1.0, -1.0, 0.0);
	setWidth(width);	
	setHeight(height);
	
	Number whalf = width/2.0f;
	Number hhalf = height/2.0f;
	
	rectMeshGeometry.addVertexWithUV(-whalf,-hhalf,0,0,0);
	rectMeshGeometry.addVertexWithUV(-whalf+width,-hhalf,0, 1, 0);
	rectMeshGeometry.addVertexWithUV(-whalf+width,-hhalf+height,0, 1, 1);
	rectMeshGeometry.addVertexWithUV(-whalf,-hhalf+height,0,0,1);
	
	rectMeshGeometry.indexedMesh = true;
	rectMeshGeometry.addIndexedFace(0, 1, 2);
	rectMeshGeometry.addIndexedFace(0, 2, 3);
	
	rectMesh->addSubmesh(rectMeshGeometry);
	
	setMaterial(resourcePool->getMaterial("UnlitUntextured"));
	
	if(texture) {
		setMaterial(resourcePool->getMaterial("Unlit"));
		shaderPasses[0].shaderBinding->setTextureForParam("diffuse", texture);
	}
}

UIRect::~UIRect() {
	delete rectMesh;
}

void UIRect::loadTexture(String fileName) {

	
	material =	resourcePool->getMaterial("Unlit");
	texture = resourcePool->loadTexture(fileName);
	
	if(!texture) {
		setMaterial(resourcePool->getMaterial("UnlitUntextured"));
	} else {
		setMaterial(resourcePool->getMaterial("Unlit"));

	}
	
	if(shaderPasses.size() > 0) {
		shaderPasses[0].shaderBinding->setTextureForParam("diffuse", texture);
	}
}

void UIRect::setTexture(std::shared_ptr<Texture> texture) {
	
	if(!texture) {
		setMaterial(resourcePool->getMaterial("UnlitUntextured"));
	} else {
		setMaterial(resourcePool->getMaterial("Unlit"));
	}
	
	this->texture = texture;
	if(shaderPasses[0].shaderBinding) {
		shaderPasses[0].shaderBinding->setTextureForParam("diffuse", texture);
	}
}

void UIRect::setMaterial(std::shared_ptr<Material> material) {
	shaderPasses.clear();
	
	this->material = material;
	
	ShaderPass pass;
	pass.shaderBinding = std::make_shared<ShaderBinding>();
	pass.shaderBinding->targetShader = pass.shader;
	pass.shader = material->getShaderPass(0).shader;
	shaderPasses.push_back(pass);
	
	shaderPasses[0].shaderBinding->addParamPointer(ProgramParam::PARAM_COLOR, "entityColor", &color);
}

std::shared_ptr<Texture> UIRect::getTexture() {
	return texture;
}

void UIRect::Render(GPUDrawBuffer *buffer) {
	
	drawCall.options.depthTest = depthTest;
	drawCall.options.depthWrite = depthWrite;
	
	if(rectMesh->getNumSubmeshes() == 0) {
		assert(false);
	}
	drawCall.submesh = rectMesh->getSubmeshPointer(0);
	drawCall.material = material;
	drawCall.shaderPasses = shaderPasses;
	buffer->drawCalls.push_back(drawCall);
}

void UIRect::Resize(Number width, Number height) {

	setWidth(width);	
	setHeight(height);

	Number whalf = width/2.0f;
	Number hhalf = height/2.0f;

	rectMesh->clearMesh();
	
	rectMeshGeometry.vertexPositionArray.data.clear();
	rectMeshGeometry.addVertex(-whalf,-hhalf,0);
	rectMeshGeometry.addVertex(-whalf+width,-hhalf,0);
	rectMeshGeometry.addVertex(-whalf+width,-hhalf+height,0);
	rectMeshGeometry.addVertex(-whalf,-hhalf+height,0);
	
	rectMeshGeometry.indexArray.data.clear();
	rectMeshGeometry.addIndexedFace(0, 1, 2);
	rectMeshGeometry.addIndexedFace(0, 2, 3);
	
	rectMesh->addSubmesh(rectMeshGeometry);
}

UIImage::UIImage(Core *core, ResourcePool *pool, String imagePath, int width, int height) : UIRect(core, pool, imagePath, width, height) {
	setBlendingMode(Renderer::BLEND_MODE_NORMAL);
}

UIImage::UIImage(Core *core, ResourcePool *pool, String imagePath) : UIRect(core, pool, imagePath) {
	setBlendingMode(Renderer::BLEND_MODE_NORMAL);
}

UIElement::UIElement(Core *core) : core(core) {
	snapToPixels = true;
	setAnchorPoint(-1.0, -1.0, 0.0);
	processInputEvents = true;
	depthTest = false;	
	hasFocus = false;
	focusable = false;
	focusParent = NULL;
	hasDragLimits = false;
	dragged = false;
	depthTest = false;
	depthWrite = false;
	core->getInput()->addEventListenerUnique(this, InputEvent::EVENT_KEYDOWN);
}

UIElement::UIElement(Core *core, Number width, Number height) : core(core) {
	setAnchorPoint(-1.0, -1.0, 0.0);
	processInputEvents = true;
	focusParent = NULL;
	hasFocus = false;
	hasDragLimits = false;
	focusable = false;
	dragged = false;	
	depthTest = false;
	depthWrite = false;		
	setWidth(width);
	setHeight(height);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
}

void UIElement::handleEvent(Event *event) {
	if(event->getDispatcher() == core->getInput() && event->getEventCode() == InputEvent::EVENT_KEYDOWN) {
		if(hasFocus && focusParent) {
			InputEvent *inputEvent = (InputEvent*) event;
			if(inputEvent->key == KEY_TAB) {
				if(core->getInput()->getKeyState(KEY_RSHIFT) || core->getInput()->getKeyState(KEY_LSHIFT)) {
					focusParent->focusPreviousChild();
				} else {
					focusParent->focusNextChild();
				}
				inputEvent->cancelEvent();
			}
		}
	}
}

void UIElement::setDragLimits(Rectangle rect) {
	dragLimits.x = rect.x;
	dragLimits.y = rect.y;
	dragLimits.w = rect.w;
	dragLimits.h = rect.h;
	hasDragLimits = true;
}

void UIElement::clearDragLimits() {
	hasDragLimits = false;
}

bool UIElement::isDragged() {
	return dragged;
}

void UIElement::startDrag(Number xOffset, Number yOffset) {
	dragged = true;
	dragOffsetX = xOffset;
	dragOffsetY = yOffset;
}

void UIElement::stopDrag() {
	dragged = false;
}

bool UIElement::isFocusable() {
	return focusable;
}

MouseEventResult UIElement::onMouseMove(const Ray &ray, int timestamp) {
	if(dragged) {	
		Vector3 localCoordinate = Vector3(ray.origin.x, ray.origin.y,0);

		if(parentEntity) {
			Matrix4 inverse = parentEntity->getConcatenatedMatrix().Inverse();
			localCoordinate = inverse * localCoordinate;		
		}
		setPosition(localCoordinate.x-dragOffsetX,-localCoordinate.y-dragOffsetY);
		if(hasDragLimits) {
			if(position.x < dragLimits.x)
				position.x = dragLimits.x;
			if(position.x > dragLimits.x + dragLimits.w)
				position.x = dragLimits.x + dragLimits.w;
			if(position.y < dragLimits.y)
				position.y = dragLimits.y;
			if(position.y > dragLimits.y + dragLimits.h)
				position.y = dragLimits.y + dragLimits.h;
		}		
		
	}
	return Entity::onMouseMove(ray, timestamp);
}

UIElement::~UIElement() {
	core->getInput()->removeAllHandlersForListener(this);
	if(focusParent) {
		focusParent->unregisterFocusChild(this);
	}
	
	if(UIElement::globalFocusedChild == this) {
		UIElement::globalFocusedChild = NULL;
	}
}

Vector2 UIElement::getScreenPositionForMainCamera() {
	Scene *containerScene = getContainerScene();
	Vector2 screenPos;
	
	if(containerScene) {
		Camera *camera = containerScene->getActiveCamera();
		if(camera) {
			screenPos = getScreenPosition(camera->getProjectionMatrix(), camera->getConcatenatedMatrix().Inverse(), camera->getViewport());
		}
	}
	screenPos.y = core->getYRes() - screenPos.y;
	
	return screenPos;
}

void UIElement::focusPreviousChild() {
	
	int j = 0;
	bool hasFocusedChild = false;
	if(UIElement::globalFocusedChild) {
		for(int i=0; i < focusChildren.size(); i++) {
			if(focusChildren[i] == UIElement::globalFocusedChild) {
				j = i;
				hasFocusedChild = true;
			}
		}
	}
	
	if(!hasFocusedChild) {
		return;
	}
	
	for(int i=0; i < focusChildren.size(); i++) {
		if(focusChildren[j]->isFocusable() && focusChildren[j] != UIElement::globalFocusedChild && focusChildren[j]->enabled && focusChildren[j]->visible) {
			focusChild(focusChildren[j]);
			return;
		}
		
		j--;
		if(j == -1) {
			j = focusChildren.size()-1;
		}
	}
}


void UIElement::focusNextChild() {

	int j = 0;
	bool hasFocusedChild = false;
	if(UIElement::globalFocusedChild) {
		for(int i=0; i < focusChildren.size(); i++) {
			if(focusChildren[i] == UIElement::globalFocusedChild) {
				j = i;
				hasFocusedChild = true;
			}
		}
	}

	if(!hasFocusedChild) {
		return;
	}

	for(int i=0; i < focusChildren.size(); i++) {
		if(focusChildren[j]->isFocusable() && focusChildren[j] != UIElement::globalFocusedChild && focusChildren[j]->enabled && focusChildren[j]->visible) {
			focusChild(focusChildren[j]);
			return;
		}

		j++;
		if(j == focusChildren.size()) {
			j = 0;
		}
	}
}

void UIElement::focusSelf() {
	if(UIElement::globalFocusedChild) {
		UIElement::globalFocusedChild->onLoseFocus();
		UIElement::globalFocusedChild->hasFocus = false;
	}

	UIElement::globalFocusedChild = this;
	
	onGainFocus();
	hasFocus = true;
}

void UIElement::focusChild(UIElement *child) {

	if(UIElement::globalFocusedChild) {
		UIElement::globalFocusedChild->onLoseFocus();
		UIElement::globalFocusedChild->hasFocus = false;
	}

	UIElement::globalFocusedChild = child;
	
	if(child) {
		UIElement::globalFocusedChild->onGainFocus();
		UIElement::globalFocusedChild->hasFocus = true;
	}	
}

void UIElement::registerFocusChild(UIElement *element) {
	if(element->isFocusable()) {
		element->setFocusParent(this);
		focusChildren.push_back(element);
	}
}

void UIElement::unregisterFocusChild(UIElement *element) {
	for(int i=0; i < focusChildren.size(); i++) {
		if(focusChildren[i] == element) {
			focusChildren.erase(focusChildren.begin()+i);
			return;
		}
	}
}

void UIElement::addFocusChild(UIElement *element) {
	registerFocusChild(element);
	addChild(element);
}

void UIElement::setFocusParent(UIElement *element) {
	focusParent = element;	
}

void UIElement::Resize(Number width, Number height) {
	setWidth(width);
	setHeight(height);
	dirtyMatrix(true);	
}
