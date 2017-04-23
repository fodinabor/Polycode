/*
 Copyright (C) 2011 by Ivan Safrin
 
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

#include "polycode/core/PolySceneLabel.h"
#include "polycode/core/PolyLabel.h"
#include "polycode/core/PolyMesh.h"
#include "polycode/core/PolyTexture.h"
#include "polycode/core/PolyRenderer.h"
#include "polycode/core/PolyResourceManager.h"
#include <memory>

using namespace Polycode;

Vector3 SceneLabel::defaultAnchor = Vector3();
bool SceneLabel::defaultPositionAtBaseline = false;
bool SceneLabel::defaultSnapToPixels = false;
bool SceneLabel::createMipmapsForLabels = true;

SceneLabel::SceneLabel(std::shared_ptr<Material> material, const String& text, int size, std::shared_ptr<Font> font, int amode, Number actualHeight) : ScenePrimitive(ScenePrimitive::TYPE_VPLANE, 1, 1){
	setMaterial(material);
	label = new Label(font, text, size, amode, false, Color(0.0, 0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0, 1.0));
	positionAtBaseline = SceneLabel::defaultPositionAtBaseline;
	setAnchorPoint(SceneLabel::defaultAnchor);
	snapToPixels = SceneLabel::defaultSnapToPixels;
	setLabelActualHeight(actualHeight);
}

SceneLabel::SceneLabel(std::shared_ptr<Material> material, const String& text, int size, std::shared_ptr<Font> font, int amode, Number actualHeight, bool premultiplyAlpha, const Color &backgroundColor, const Color &foregroundColor) : ScenePrimitive(ScenePrimitive::TYPE_VPLANE, 1, 1){
	setMaterial(material);
	label = new Label(font, text, size, amode, premultiplyAlpha, backgroundColor, foregroundColor);
	
	positionAtBaseline = SceneLabel::defaultPositionAtBaseline;
	setAnchorPoint(SceneLabel::defaultAnchor);	
	snapToPixels = SceneLabel::defaultSnapToPixels; 
	setLabelActualHeight(actualHeight);
}

Entity *SceneLabel::Clone(bool deepClone, bool ignoreEditorOnly) const {
	SceneLabel *newLabel = new SceneLabel(material, label->getText(), label->getSize(), label->getFont(), label->getAntialiasMode(), actualHeight, label->getPremultiplyAlpha(), label->getBackgroundColor(), label->getForegroundColor());
	applyClone(newLabel, deepClone, ignoreEditorOnly);
	return newLabel;
}

void SceneLabel::applyClone(Entity *clone, bool deepClone, bool ignoreEditorOnly) const {
	
	SceneLabel* cloneLabel = (SceneLabel*) clone;
	

	cloneLabel->getLabel()->setSize(label->getSize());
	cloneLabel->getLabel()->setAntialiasMode(label->getAntialiasMode());
	cloneLabel->getLabel()->setFont(label->getFont());
	cloneLabel->getLabel()->setPremultiplyAlpha(label->getPremultiplyAlpha());
	cloneLabel->setLabelActualHeight(actualHeight);
	cloneLabel->getLabel()->setBackgroundColor(label->getBackgroundColor());
	cloneLabel->getLabel()->setForegroundColor(label->getForegroundColor());
	cloneLabel->positionAtBaseline = positionAtBaseline;
	cloneLabel->setText(label->getText());
	
	ScenePrimitive::applyClone(clone, deepClone, ignoreEditorOnly);
}


SceneLabel::~SceneLabel() {
	delete label;
}

Label *SceneLabel::getLabel() {
	return label;
}

String SceneLabel::getText() {
	return label->getText();
}

void SceneLabel::setLabelActualHeight(Number actualHeight) {
	this->actualHeight = actualHeight;
	if(actualHeight > 0.0) {
		labelScale = actualHeight/((Number)label->getSize());
	} else {
		labelScale = 1.0;
	}
	updateFromLabel();
}

Number SceneLabel::getLabelActualHeight() {
	return actualHeight;
}


void SceneLabel::updateFromLabel() {
	texture = nullptr;
	
	if(SceneLabel::createMipmapsForLabels) {
		texture = std::make_shared<Texture>(label, true, false);
	} else {
		texture = std::make_shared<Texture>(label, true, false);
	}

	setPrimitiveOptions(type, label->getWidth()*labelScale,label->getHeight()*labelScale);
	setLocalBoundingBox(label->getWidth()*labelScale, label->getHeight()*labelScale, 0.001);
    if(shaderPasses.size() > 0) {
        getShaderPass(0).shaderBinding->setTextureForParam("diffuse", texture);
    }
}

void SceneLabel::Render(GPUDrawBuffer *buffer) {
	ScenePrimitive::Render(buffer);
	if(positionAtBaseline) {
		if(buffer->drawCalls.size() > 0) {
			buffer->drawCalls[buffer->drawCalls.size()-1].modelMatrix.Translate(0.0, (((Number)label->getSize()*labelScale) * -1.0) + (((Number)label->getBaselineAdjust())*labelScale), 0.0);
		}
	}
}

int SceneLabel::getTextWidthForString(String text) {
	return label->getTextWidthForString(text);
}

void SceneLabel::setText(const String& newText) {
	
	if(newText == label->getText() && !label->optionsChanged()) {
		return;
	}		
	label->setText(newText);	
	updateFromLabel();
}
