/*
 Copyright (C) 2013 by Ivan Safrin
 
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
 
#include "polycode/ide/TransformGizmo.h"
#include <cmath>

extern UIGlobalMenu *globalMenu;

TransformGrips::TransformGrips(Core *core, ResourcePool *pool) : UIElement(core) {
	
	mainRect = new UIRect(core, pool, 1.0, 1.0);
	mainRect->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	mainRect->color.setColor(0.0, 0.5, 1.0, 0.2);
	grips.push_back(mainRect);
	
	transformTL = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformTL);
	transformT = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformT);
	transformTR = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformTR);
	transformL = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformL);
	transformR = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformR);
	transformBL = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformBL);
	transformB = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformB);
	transformBR = new UIImage(core, pool, "spriteEditor/transform_corner.png", 8, 8);
	grips.push_back(transformBR);
	transformOffset = new UIImage(core, pool, "spriteEditor/transform_offset.png", 12, 12);
	grips.push_back(transformOffset);
	
	for(int i=0; i < grips.size(); i++) {
		addChild(grips[i]);
		if(grips[i] != mainRect) {
			grips[i]->setAnchorPoint(Vector3());
		}
		grips[i]->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
		grips[i]->addEventListener(this, InputEvent::EVENT_MOUSEUP);
		grips[i]->addEventListener(this, InputEvent::EVENT_MOUSEUP_OUTSIDE);
		grips[i]->blockMouseInput = true;
	}
	
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEMOVE);	
	transforming = false;
	movingTransform = NULL;
}

Polycode::Rectangle TransformGrips::getGripRectangle() {
	return gripRectangle;
}

void TransformGrips::handleEvent(Event *event) {
	if(event->getDispatcher() == core->getInput()) {
		if(transforming) {
			
			Vector2 newMouse = core->getInput()->getMousePosition();
			
			if(movingTransform == mainRect) {
				gripRectangle.x += newMouse.x - mouseBase.x;
				gripRectangle.y += newMouse.y - mouseBase.y;
			} else if(movingTransform == transformTL) {
				gripRectangle.x += newMouse.x - mouseBase.x;
				gripRectangle.y += newMouse.y - mouseBase.y;
				gripRectangle.w -= newMouse.x - mouseBase.x;
				gripRectangle.h -= newMouse.y - mouseBase.y;
			} else if(movingTransform == transformTR) {
				gripRectangle.y += newMouse.y - mouseBase.y;
				gripRectangle.w += newMouse.x - mouseBase.x;
				gripRectangle.h -= newMouse.y - mouseBase.y;
			} else if(movingTransform == transformT) {
				gripRectangle.y += newMouse.y - mouseBase.y;
				gripRectangle.h -= newMouse.y - mouseBase.y;
			} else if(movingTransform == transformL) {
				gripRectangle.x += newMouse.x - mouseBase.x;
				gripRectangle.w -= newMouse.x - mouseBase.x;
			} else if(movingTransform == transformR) {
				gripRectangle.w += newMouse.x - mouseBase.x;
			} else if(movingTransform == transformBL) {
				gripRectangle.x += newMouse.x - mouseBase.x;
				gripRectangle.w -= newMouse.x - mouseBase.x;
				gripRectangle.h += newMouse.y - mouseBase.y;
			} else if(movingTransform == transformBR) {
				gripRectangle.w += newMouse.x - mouseBase.x;
				gripRectangle.h += newMouse.y - mouseBase.y;
			} else if(movingTransform == transformB) {
				gripRectangle.h += newMouse.y - mouseBase.y;
			} else if(movingTransform == transformOffset) {
				anchorPoint.x += (newMouse.x - mouseBase.x) / gripRectangle.w;
				anchorPoint.y += (newMouse.y - mouseBase.y) / gripRectangle.h;
			}
			
			mouseBase = newMouse;
			dispatchEvent(new Event(), Event::CHANGE_EVENT);
		}
	} else {
		if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
			movingTransform = (UIImage*) event->getDispatcher();
			transforming = true;
			
			mouseBase = core->getInput()->getMousePosition();
			
		} else {
			transforming = false;
		}
	}
}

Vector2 TransformGrips::getAnchorPoint() {
	return anchorPoint;
}

TransformGrips::~TransformGrips() {
	core->getInput()->removeAllHandlersForListener(this);
}

void TransformGrips::setGripRectangle(Polycode::Rectangle rectangle, Vector2 offset) {
	
	mainRect->setPosition(rectangle.x, rectangle.y);
	mainRect->Resize(rectangle.w, rectangle.h);
	
	transformTL->setPosition(rectangle.x, rectangle.y);
	transformT->setPosition(rectangle.x + (rectangle.w * 0.5), rectangle.y);
	transformTR->setPosition(rectangle.x + (rectangle.w), rectangle.y);
	
	transformL->setPosition(rectangle.x, rectangle.y +(rectangle.h * 0.5));
	transformR->setPosition(rectangle.x + (rectangle.w), rectangle.y+(rectangle.h * 0.5));
	
	transformBL->setPosition(rectangle.x, rectangle.y+rectangle.h);
	transformB->setPosition(rectangle.x + (rectangle.w * 0.5), rectangle.y+rectangle.h);
	transformBR->setPosition(rectangle.x+rectangle.w, rectangle.y+rectangle.h);
	
	transformOffset->setPosition(rectangle.x + (rectangle.w * 0.5) + (offset.x * rectangle.w), rectangle.y + (rectangle.h * 0.5) + (offset.y * rectangle.h));
	
	gripRectangle = rectangle;
	anchorPoint = offset;
	
}

TrasnformGizmoEvent::TrasnformGizmoEvent(int mode) : Event() {
	this->mode =  mode;
	this->eventCode = eventCode;
	eventType = "TrasnformGizmoEvent";
}

TransformGizmoMenu::TransformGizmoMenu(Core *core, ResourcePool *pool, TransformGizmo *gizmo) : UIElement(core) {
	processInputEvents = true;
	
	this->gizmo = gizmo;
	
	transformSelector = new UIIconSelector(core, pool);
	addChild(transformSelector);
	transformSelector->addIcon("entityEditor/move_gizmo.png");
	transformSelector->addIcon("entityEditor/scale_gizmo.png");
	transformSelector->addIcon("entityEditor/rotate_gizmo.png");
	transformSelector->setPosition(4, 3.0);
	transformSelector->addEventListener(this, UIEvent::SELECT_EVENT);
	
	orientationCombo = new UIComboBox(core, pool, globalMenu, 100);
	orientationCombo->addComboItem("Global");
	orientationCombo->addComboItem("Local");
	orientationCombo->setSelectedIndex(0);
	addChild(orientationCombo);
	orientationCombo->setPosition(100, 2);
	orientationCombo->addEventListener(this, UIEvent::CHANGE_EVENT);
	
	centerSelector = new UIIconSelector(core, pool);
	addChild(centerSelector);
	centerSelector->addIcon("entityEditor/median_center.png");
	centerSelector->addIcon("entityEditor/individual_centers.png");
	centerSelector->setPosition(210, 3.0);
	centerSelector->addEventListener(this, UIEvent::SELECT_EVENT);
	
}

void TransformGizmoMenu::handleEvent(Event *event) {
	if(event->getDispatcher() == transformSelector) {
		switch(transformSelector->getSelectedIndex()) {
			case 0:
				gizmo->setTransformMode(TransformGizmo::TRANSFORM_MOVE);
			break;
			case 1:
				gizmo->setTransformMode(TransformGizmo::TRANSFORM_SCALE);
			break;
			case 2:
				gizmo->setTransformMode(TransformGizmo::TRANSFORM_ROTATE);
			break;
		}
	} else if(event->getDispatcher() == orientationCombo) {
		gizmo->setTransformOrientation(orientationCombo->getSelectedIndex());
	} else if(event->getDispatcher() == centerSelector) {
		if(centerSelector->getSelectedIndex() == 0) {
			gizmo->setCenterMode(TransformGizmo::CENTER_MODE_MEDIAN);
		} else {
			gizmo->setCenterMode(TransformGizmo::CENTER_MODE_INDIVIDUAL);
		}
	}
}

TransformGizmoMenu::~TransformGizmoMenu() {

}

void TransformGizmo::setCenterMode(int centerMode) {
	this->centerMode = centerMode;
}

void TransformGizmo::toggleOrientation() {
	if(startingOrientation == -1)
		startingOrientation = orientation; 
	
	if(orientation == ORIENTATION_GLOBAL)
		orientation = ORIENTATION_LOCAL;
	else if(orientation == ORIENTATION_LOCAL)
		orientation = ORIENTATION_GLOBAL;
}

TransformGizmo::TransformGizmo(Core *core, ResourcePool *pool, Scene *targetScene, Camera *targetCamera) : core(core) {
	processInputEvents = true;
	orientation = ORIENTATION_GLOBAL;
	startingOrientation = -1;
	centerMode = CENTER_MODE_MEDIAN;
	enableGizmo = true;
	firstMove = true;
	
	snapEnabled = false;
	snapSize = 1.0;
	
	this->targetScene = targetScene;
	this->targetCamera = targetCamera;
	
	ScenePrimitive *centerCircle = new ScenePrimitive(ScenePrimitive::TYPE_LINE_CIRCLE, 0.3, 0.3, 16);
	centerCircle->setColor(0.7, 0.7, 0.7, 1.0);
	centerCircle->depthTest = false;
	centerCircle->billboardMode = true;
	addChild(centerCircle);
	centerCircle->setLineWidth(core->getRenderer()->getBackingResolutionScaleX());
	centerCircle->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
		  
	trasnformDecorators = new Entity();
	addChild(trasnformDecorators);
	
	scaleDecorators = new Entity();
	addChild(scaleDecorators);
		
	transformAndScaleLines = new Entity();
	addChild(transformAndScaleLines);	 
		
	rotateDectorators = new Entity();
	addChild(rotateDectorators);
	
	yLine = new SceneMesh();
	
	MeshGeometry yLineGeometry;
	yLineGeometry.meshType = MeshGeometry::LINE_MESH;
	yLineGeometry.addVertex(0.0, 0.0, 0.0);
	yLineGeometry.addVertex(0.0, 0.75, 0.0);
	yLine->getMesh()->addSubmesh(yLineGeometry);
	yLine->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	yLine->depthTest = false;
	yLine->color.setColorHexFromString(TRANSGIZMO_Y_COLOR);
	yLine->setLocalBoundingBox(yLine->getMesh()->calculateBBox());
	yLine->setLineWidth(core->getRenderer()->getBackingResolutionScaleX() * 2.0);
	transformAndScaleLines->addChild(yLine);
	yLine->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	yLine->setForceMaterial(true);
	
	xLine = new SceneMesh();
	MeshGeometry xLineGeometry;
	xLineGeometry.meshType = MeshGeometry::LINE_MESH;
	xLineGeometry.addVertex(0.0, 0.0, 0.0);
	xLineGeometry.addVertex(0.75, 0.0, 0.0);
	xLine->getMesh()->addSubmesh(xLineGeometry);
	xLine->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	xLine->depthTest = false;
	xLine->color.setColorHexFromString(TRANSGIZMO_X_COLOR);
	xLine->setLocalBoundingBox(xLine->getMesh()->calculateBBox());
	xLine->setLineWidth(core->getRenderer()->getBackingResolutionScaleX() * 2.0);
	transformAndScaleLines->addChild(xLine);
	xLine->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	xLine->setForceMaterial(true);
	
	zLine = new SceneMesh();
	MeshGeometry zLineGeometry;
	zLineGeometry.meshType = MeshGeometry::LINE_MESH;
	zLineGeometry.addVertex(0.0, 0.0, 0.0);
	zLineGeometry.addVertex(0.00, 0.0, 0.75);
	zLine->getMesh()->addSubmesh(zLineGeometry);
	
	zLine->depthTest = false;
	zLine->color.setColorHexFromString(TRANSGIZMO_Z_COLOR);
	zLine->setLocalBoundingBox(zLine->getMesh()->calculateBBox());
	zLine->setLineWidth(core->getRenderer()->getBackingResolutionScaleX() * 2.0);
	transformAndScaleLines->addChild(zLine);
	zLine->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	zLine->setForceMaterial(true);
	zLine->setMaterial(pool->getMaterial("UnlitUntextured"));
	// MOVE
	
	yArrow = new ScenePrimitive(ScenePrimitive::TYPE_CONE, 0.2, 0.05, 12);
	yArrow->color.setColorHexFromString(TRANSGIZMO_Y_COLOR);
	yArrow->setPosition(0.0, 0.75, 0.0);
	yArrow->depthTest = false;
	trasnformDecorators->addChild(yArrow);
	yArrow->setForceMaterial(true);
	yArrow->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	xArrow = new ScenePrimitive(ScenePrimitive::TYPE_CONE, 0.2, 0.05, 12);
	xArrow->color.setColorHexFromString(TRANSGIZMO_X_COLOR);
	xArrow->setPosition(0.75, 0.0, 0.0);
	xArrow->Roll(-90);
	xArrow->depthTest = false;
	trasnformDecorators->addChild(xArrow);
	xArrow->setForceMaterial(true);
	xArrow->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	zArrow = new ScenePrimitive(ScenePrimitive::TYPE_CONE, 0.2, 0.05, 12);
	zArrow->color.setColorHexFromString(TRANSGIZMO_Z_COLOR);
	zArrow->setPosition(0.0, 0.0, 0.75);
	zArrow->Pitch(90);
	zArrow->depthTest = false;	
	trasnformDecorators->addChild(zArrow);
	zArrow->setForceMaterial(true);
	zArrow->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	// SCALE

	yBox = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 0.08, 0.08, 0.08);
	yBox->color.setColorHexFromString(TRANSGIZMO_Y_COLOR);
	yBox->setPosition(0.0, 0.75, 0.0);
	yBox->depthTest = false;
	scaleDecorators->addChild(yBox);
	yBox->setForceMaterial(true);
	yBox->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	xBox = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 0.08, 0.08, 0.08);
	xBox->color.setColorHexFromString(TRANSGIZMO_X_COLOR);
	xBox->setPosition(0.75, 0.0, 0.0);
	xBox->Roll(-90);
	xBox->depthTest = false;
	scaleDecorators->addChild(xBox);
	xBox->setForceMaterial(true);
	xBox->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	zBox = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 0.08, 0.08, 0.08);
	zBox->color.setColorHexFromString(TRANSGIZMO_Z_COLOR);
	zBox->setPosition(0.0, 0.0, 0.75);
	zBox->Pitch(90);
	zBox->depthTest = false;	
	scaleDecorators->addChild(zBox);
	zBox->setForceMaterial(true);
	zBox->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	// ROTATE

	bgCircle = new ScenePrimitive(ScenePrimitive::TYPE_LINE_CIRCLE, 2.6, 2.6, 32);
	bgCircle->setColor(0.0, 0.0, 0.0, 1.0);
	bgCircle->depthTest = false;
//	bgCircle->billboardMode = true;
	//rotateDectorators->addChild(bgCircle);
	bgCircle->setLineWidth(core->getRenderer()->getBackingResolutionScaleX());
	bgCircle->setForceMaterial(true);
	bgCircle->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	bgCircle->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	outerCircle = new ScenePrimitive(ScenePrimitive::TYPE_LINE_CIRCLE, 3.0, 3.0, 32);
	outerCircle->setColor(1.0, 1.0, 1.0, 1.0);
	outerCircle->depthTest = false;
	outerCircle->billboardMode = true;
	rotateDectorators->addChild(outerCircle);
	outerCircle->setLineWidth(core->getRenderer()->getBackingResolutionScaleX() * 2.0);
	outerCircle->setForceMaterial(true);
	outerCircle->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	outerCircle->setMaterial(pool->getMaterial("UnlitUntextured"));
	
	pitchCircle = new ScenePrimitive(ScenePrimitive::TYPE_LINE_CIRCLE, 1.55, 1.55, 64);
	pitchCircle->color.setColorHexFromString(TRANSGIZMO_X_COLOR);
	pitchCircle->depthTest = false;
	pitchCircle->Yaw(90);	
	rotateDectorators->addChild(pitchCircle);
	pitchCircle->setMaterial(pool->getMaterial("OneSidedLine"));
	pitchCircle->setLineWidth(core->getRenderer()->getBackingResolutionScaleX() * 2.0);
	pitchCircle->setForceMaterial(true);
	pitchCircle->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	
	yawCircle = new ScenePrimitive(ScenePrimitive::TYPE_LINE_CIRCLE, 1.65, 1.65, 64);
	yawCircle->color.setColorHexFromString(TRANSGIZMO_Y_COLOR);
	yawCircle->depthTest = false;
	yawCircle->Pitch(90);
	rotateDectorators->addChild(yawCircle);
	yawCircle->setMaterial(pool->getMaterial("OneSidedLine"));
	yawCircle->setLineWidth(core->getRenderer()->getBackingResolutionScaleX()* 2.0);
	yawCircle->setForceMaterial(true);
	yawCircle->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	
	rollCircle = new ScenePrimitive(ScenePrimitive::TYPE_LINE_CIRCLE, 1.6, 1.6, 64);
	rollCircle->color.setColorHexFromString(TRANSGIZMO_Z_COLOR);
	rollCircle->depthTest = false;
	rotateDectorators->addChild(rollCircle);
	rollCircle->setMaterial(pool->getMaterial("OneSidedLine"));
	rollCircle->setLineWidth(core->getRenderer()->getBackingResolutionScaleX()* 2.0);
	rollCircle->setForceMaterial(true);
	rollCircle->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	
	rotateDectorators->processInputEvents = true;
	
	//pitchGrip = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, 1.55 * 0.5, 0.05, 16, 3);
	pitchGrip = new ScenePrimitive(ScenePrimitive::TYPE_UNCAPPED_CYLINDER, 0.15, 1.55 * 0.5, 16);
	pitchGrip->color.setColorHexFromString(TRANSGIZMO_X_COLOR);
	pitchGrip->depthTest = false;
	pitchGrip->Pitch(90);	
	pitchGrip->Yaw(90);		
	rotateDectorators->addChild(pitchGrip);
	pitchGrip->processInputEvents = true;
	pitchGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	pitchGrip->useGeometryHitDetection = true;
	pitchGrip->blockMouseInput = true;

	rollGrip = new ScenePrimitive(ScenePrimitive::TYPE_UNCAPPED_CYLINDER, 0.15, 1.6 * 0.5, 16);
	rollGrip->color.setColorHexFromString(TRANSGIZMO_Z_COLOR);
	rollGrip->depthTest = false;
	rollGrip->Pitch(90);		
	rotateDectorators->addChild(rollGrip);
	rollGrip->processInputEvents = true;
	rollGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	rollGrip->useGeometryHitDetection = true;
	rollGrip->blockMouseInput = true;
	
	yawGrip= new ScenePrimitive(ScenePrimitive::TYPE_UNCAPPED_CYLINDER, 0.15, 1.65 * 0.5, 16);
	yawGrip->color.setColorHexFromString(TRANSGIZMO_Y_COLOR);
	yawGrip->depthTest = false;
	yawGrip->Yaw(90);		
	rotateDectorators->addChild(yawGrip);
	yawGrip->processInputEvents = true;
	yawGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	yawGrip->useGeometryHitDetection = true;
	yawGrip->blockMouseInput = true;
	
	viewportRotateGripBase = new Entity();
	viewportRotateGripBase->processInputEvents = true;
	rotateDectorators->addChild(viewportRotateGripBase);
	
	viewportRotateGrip = new ScenePrimitive(ScenePrimitive::TYPE_TORUS, 3.0 * 0.5, 0.08, 16, 3);
	viewportRotateGrip->Pitch(90);
	viewportRotateGrip->setColor(0.0, 1.0, 0.0, 0.2);
	viewportRotateGrip->depthTest = false;
	viewportRotateGripBase->addChild(viewportRotateGrip);
	viewportRotateGrip->processInputEvents = true;
	viewportRotateGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	viewportRotateGrip->useGeometryHitDetection = true;
	viewportRotateGrip->blockMouseInput = true;

	
	pitchGrip->visible = false;
	yawGrip->visible = false;
	rollGrip->visible = false;
	viewportRotateGrip->visible = false;

	xTransformGrip = new Entity();
	xTransformGrip->setLocalBoundingBox(1.3, 0.1, 0.1);
	xTransformGrip->depthTest = false;
	xTransformGrip->setColor(1.0, 0.0, 0.0, 1.0);
	addChild(xTransformGrip);
	xTransformGrip->setAnchorPoint(Vector3(-1.0, 0.0, 0.0));
	xTransformGrip->processInputEvents = true;
	xTransformGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);

	yTransformGrip = new Entity();
	yTransformGrip->setLocalBoundingBox(0.1, 1.3, 0.1);
	addChild(yTransformGrip);
	yTransformGrip->setAnchorPoint(Vector3(0.0, -1.0, 0.0));
	yTransformGrip->processInputEvents = true;
	yTransformGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);

	zTransformGrip = new Entity();
	zTransformGrip->setLocalBoundingBox(0.1, 0.1, 1.3);
	addChild(zTransformGrip);
	zTransformGrip->setAnchorPoint(Vector3(0.0, 0.0, -1.0));
	zTransformGrip->processInputEvents = true;
	zTransformGrip->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
		
	transforming = false;
	mode = TRANSFORM_MOVE;
		
	visible = false;
	enabled = false;
	
	coreInput = core->getInput();
	coreInput->addEventListener(this, InputEvent::EVENT_MOUSEMOVE);
	coreInput->addEventListener(this, InputEvent::EVENT_MOUSEUP);
	coreInput->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	coreInput->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	
	gizmoMode = GIZMO_MODE_3D;
	setTransformMode(TRANSFORM_MOVE);
}

void TransformGizmo::enableSnap(bool val) {
	snapEnabled = val;
}

void TransformGizmo::setSnapSize(Number snapSize) {
	this->snapSize = snapSize;
}

void TransformGizmo::setTransformOrientation(int orientation) {
	this->orientation = orientation;
}

void TransformGizmo::setTransformPlaneFromView() {
	
	switch(gizmoMode) {
		case GIZMO_MODE_2D_X:
			setTransformPlane(1.0, 0.0, 0.0, true);
			transformConstraint = Vector3(0.0, 1.0, 1.0);
		break;
		case GIZMO_MODE_2D_Y:
			setTransformPlane(0.0, 1.0, 0.0, true);
			transformConstraint = Vector3(1.0, 0.0, 1.0);
		break;
		case GIZMO_MODE_2D_Z:
			setTransformPlane(0.0, 0.0, 1.0, true);
			transformConstraint = Vector3(1.0, 1.0, 0.0);
		break;
		default:
			Vector3 camVec =  targetCamera->getConcatenatedMatrix().getPosition() - getConcatenatedMatrix().getPosition();
			camVec.Normalize();
			setTransformPlane(camVec.x, camVec.y, camVec.z, true);
		break;
	}
}

void TransformGizmo::setTransformPlane(Number x, Number y, Number z, bool forceGlobal) {
	planeMatrix = getConcatenatedMatrix();
	
	Vector3 localPlane = Vector3(x,y,z);
	localTransformPlane = localPlane;
	Number planeDistance = 0;

	if(forceGlobal) {
		transformPlane = localPlane;
		transformPlaneDistance = planeDistance;
		planeMatrix.identity();
		return;
	}
	
	transformPlane = planeMatrix.rotateVector(localPlane);
	Vector3 planePoint = planeMatrix.getPosition();
	transformPlaneDistance = planePoint.dot(transformPlane);
	
	Ray gizmoRay;
	gizmoRay.origin = 0.0;
	gizmoRay.direction = transformPlane * -1;
	
	gizmoPoint = gizmoRay.planeIntersectPoint(transformPlane, transformPlaneDistance);
	gizmoPoint = planeMatrix.Inverse() * gizmoPoint;
	
}

void TransformGizmo::setTransformPlane(bool useX, bool useY, bool useZ) {
	Vector3 xDir = getAnchorAdjustedMatrix().rotateVector(Vector3(1.0, 0.0, 0.0));
	Vector3 yDir = getAnchorAdjustedMatrix().rotateVector(Vector3(0.0, 1.0, 0.0));
	Vector3 zDir = getAnchorAdjustedMatrix().rotateVector(Vector3(0.0, 0.0, 1.0));
	
	Number xDot = abs(targetCamera->getRotationQuat().applyTo(Vector3(0.0, 0.0, -1.0)).dot(xDir));
	Number yDot = abs(targetCamera->getRotationQuat().applyTo(Vector3(0.0, 0.0, -1.0)).dot(yDir));
	Number zDot = abs(targetCamera->getRotationQuat().applyTo(Vector3(0.0, 0.0, -1.0)).dot(zDir));

	if(useX && !useY && !useZ) {
		if(yDot > zDot)
			setTransformPlane(0.0, 1.0, 0.0);
		else
			setTransformPlane(0.0, 0.0, 1.0);
	} else if(!useX && useY && !useZ) {
		if(xDot > zDot)
			setTransformPlane(1.0, 0.0, 0.0);
		else
			setTransformPlane(0.0, 0.0, 1.0);
	} else if(!useX && !useY && useZ) {
		if(xDot > yDot)
			setTransformPlane(1.0, 0.0, 0.0);
		else
			setTransformPlane(0.0, 1.0, 0.0);
	} else if(!useX) {
		setTransformPlane(1.0, 0.0, 0.0);
	} else if(!useY) {
		setTransformPlane(0.0, 1.0, 0.0);
	} else if(!useZ) {
		setTransformPlane(0.0, 0.0, 1.0);
	}
}

Vector3 TransformGizmo::getTransformPlanePosition() {

	Ray ray = targetScene->projectRayFromCameraAndViewportCoordinate(targetCamera, coreInput->getMousePosition());
	
	// hack to fix NaN's in the plane intersect math (sorry)
	ray.direction += Vector3(0.00001, 0.00001, 0.00001);

	Vector3 ret = ray.planeIntersectPoint(transformPlane, transformPlaneDistance);
	return ret;
}

Vector3 TransformGizmo::getPositionAlongAxis() {
	Ray ray = Ray(position, rotationQuat.applyTo(transformConstraint));
	Ray ray2 = targetScene->projectRayFromCameraAndViewportCoordinate(targetCamera, coreInput->getMousePosition());
	Vector3 ret;
	ray.closestPointsBetween(ray2, &ret, NULL);

	return ret;
}

bool TransformGizmo::isConstrainedToSingleAxis() {
	if(transformConstraint.x == 1.0) {
		if(transformConstraint.y == 0.0 && transformConstraint.z == 0.0)
			return true;
	}
	if(transformConstraint.y == 1.0) {
		if(transformConstraint.x == 0.0 && transformConstraint.z == 0.0)
			return true;
	}
	if(transformConstraint.z == 1.0) {
		if(transformConstraint.x == 0.0 && transformConstraint.y == 0.0)
			return true;
	}
	return false;
}

void TransformGizmo::setTransformMode(int newMode) {
	
	transformMode = newMode;
	trasnformDecorators->visible = false;
	scaleDecorators->visible = false;
	transformAndScaleLines->visible = false;
	rotateDectorators->visible = false;
	xTransformGrip->enabled = false;
	yTransformGrip->enabled = false;
	zTransformGrip->enabled = false;
	
	pitchGrip->enabled = false;
	rollGrip->enabled = false;
	yawGrip->enabled = false;
	viewportRotateGrip->enabled = false;
	
	xTransformGrip->enabled = false;
	yTransformGrip->enabled = false;
	zTransformGrip->enabled = false;
	
			
	mode = newMode;
	switch (mode) {
		case TRANSFORM_MOVE:
			trasnformDecorators->visible = true;
			transformAndScaleLines->visible = true;
			
			switch(gizmoMode) {
				case GIZMO_MODE_3D:
					xTransformGrip->enabled = true;
					yTransformGrip->enabled = true;
					zTransformGrip->enabled = true;
					xArrow->visible = true;
					yArrow->visible = true;
					zArrow->visible = true;
					xLine->visible = true;
					yLine->visible = true;
					zLine->visible = true;
				break;
				case GIZMO_MODE_2D_X:
					xTransformGrip->enabled = false;
					yTransformGrip->enabled = true;
					zTransformGrip->enabled = true;
					xArrow->visible = false;
					yArrow->visible = true;
					zArrow->visible = true;
					xLine->visible = false;
					yLine->visible = true;
					zLine->visible = true;
				break;
				case GIZMO_MODE_2D_Y:
					xTransformGrip->enabled = true;
					yTransformGrip->enabled = false;
					zTransformGrip->enabled = true;
					xArrow->visible = true;
					yArrow->visible = false;
					zArrow->visible = true;
					xLine->visible = true;
					yLine->visible = false;
					zLine->visible = true;
				break;
				case GIZMO_MODE_2D_Z:
					xTransformGrip->enabled = true;
					yTransformGrip->enabled = true;
					zTransformGrip->enabled = false;
					xArrow->visible = true;
					yArrow->visible = true;
					zArrow->visible = false;
					xLine->visible = true;
					yLine->visible = true;
					zLine->visible = false;

				break;
			}
		break;
		case TRANSFORM_SCALE:
			scaleDecorators->visible = true;
			transformAndScaleLines->visible = true;
			switch(gizmoMode) {
				case GIZMO_MODE_3D:
					xTransformGrip->enabled = true;
					yTransformGrip->enabled = true;
					zTransformGrip->enabled = true;
					xBox->visible = true;
					yBox->visible = true;
					zBox->visible = true;
					xLine->visible = true;
					yLine->visible = true;
					zLine->visible = true;
					break;
				case GIZMO_MODE_2D_X:
					xTransformGrip->enabled = false;
					yTransformGrip->enabled = true;
					zTransformGrip->enabled = true;
					xBox->visible = false;
					yBox->visible = true;
					zBox->visible = true;
					xLine->visible = false;
					yLine->visible = true;
					zLine->visible = true;
					break;
				case GIZMO_MODE_2D_Y:
					xTransformGrip->enabled = true;
					yTransformGrip->enabled = false;
					zTransformGrip->enabled = true;
					xBox->visible = true;
					yBox->visible = false;
					zBox->visible = true;
					xLine->visible = true;
					yLine->visible = false;
					zLine->visible = true;
					break;
				case GIZMO_MODE_2D_Z:
					xTransformGrip->enabled = true;
					yTransformGrip->enabled = true;
					zTransformGrip->enabled = false;
					xBox->visible = true;
					yBox->visible = true;
					zBox->visible = false;
					xLine->visible = true;
					yLine->visible = true;
					zLine->visible = false;
					break;
			}
			

		break;	
		case TRANSFORM_ROTATE:
			rotateDectorators->visible = true;
			
			switch(gizmoMode) {
				case GIZMO_MODE_3D:
					rollGrip->enabled = true;
					rollCircle->visible = true;
					pitchGrip->enabled = true;
					pitchCircle->visible = true;
					yawGrip->enabled = true;
					yawCircle->visible = true;
					viewportRotateGrip->enabled = false;
					outerCircle->visible = false;
					outerCircle->setColor(1.0, 1.0, 1.0, 1.0);
				break;
				case GIZMO_MODE_2D_X:
					rollGrip->enabled = false;
					rollCircle->visible = false;
					pitchGrip->enabled = false;
					pitchCircle->visible = false;
					yawGrip->enabled = false;
					yawCircle->visible = false;
					viewportRotateGrip->enabled = true;
					outerCircle->visible = true;
					outerCircle->setColor(1.0, 0.0, 0.0, 1.0);
					viewportRotateGrip->setYaw(90);
					viewportRotateGrip->setPitch(90);
				break;
				case GIZMO_MODE_2D_Y:
					rollGrip->enabled = false;
					rollCircle->visible = false;
					pitchGrip->enabled = false;
					pitchCircle->visible = false;
					yawGrip->enabled = false;
					yawCircle->visible = false;
					viewportRotateGrip->enabled = true;
					outerCircle->visible = true;
					outerCircle->setColor(0.0, 1.0, 0.0, 1.0);
					viewportRotateGrip->setPitch(0);
					viewportRotateGrip->setYaw(0);
				break;
				case GIZMO_MODE_2D_Z:
					rollGrip->enabled = false;
					rollCircle->visible = false;
					pitchGrip->enabled = false;
					pitchCircle->visible = false;
					yawGrip->enabled = false;
					yawCircle->visible = false;
					viewportRotateGrip->enabled = true;
					outerCircle->visible = true;
					outerCircle->setColor(0.0, 0.0, 1.0, 1.0);
					viewportRotateGrip->setPitch(90);
					viewportRotateGrip->setYaw(0);
				break;
			}
		break;
		default:
			assert(false); // invalid mode
		break;
	}
}

void TransformGizmo::setTransformSelection(std::vector<Entity*> selectedEntities) {
	entityPositions.clear();
	this->selectedEntities = selectedEntities;
	if(selectedEntities.size() > 0) {
		visible = true;
		enabled = true;
		
		Vector3 centerPoint;
		for(int i=0; i < selectedEntities.size(); i++) {
			centerPoint += selectedEntities[i]->getConcatenatedMatrix().getPosition();
			entityPositions.push_back(selectedEntities[i]->getPosition());
		}
		centerPoint = centerPoint / selectedEntities.size();
		setPosition(centerPoint);
	} else {
		visible = false;
		enabled = false;
	}
	
}

void TransformGizmo::resetTransform() {
	if(firstMove) return;
	
	for(int i=0; i < selectedEntities.size(); i++) {
		selectedEntities[i]->setPosition(oldPosition[i]);
		entityPositions[i] = oldPosition[i];
		selectedEntities[i]->setScale(oldScale[i]);
		selectedEntities[i]->setRotationByQuaternion(oldRotation[i]);
	}
	if(isConstrainedToSingleAxis())
		startingPoint = getPositionAlongAxis();
	else
		startingPoint = getTransformPlanePosition();
}

void TransformGizmo::transformSelectedEntities(const Vector3 &move, const Vector3 &scale, Number rotate) {
	
	if(firstMove) {
		firstMove = false;
		dispatchEvent(new TrasnformGizmoEvent(mode), Event::SELECT_EVENT);
		
		oldPosition.clear();
		oldScale.clear();
		oldRotation.clear();
		for(int i=0; i < selectedEntities.size(); i++) {
			oldPosition.push_back(selectedEntities[i]->getPosition());
			oldScale.push_back(selectedEntities[i]->getScale());
			oldRotation.push_back(selectedEntities[i]->getRotationQuat());
		}
	}
	
	Vector3 globalCenter = getConcatenatedMatrix().getPosition();
	for(int i=0; i < selectedEntities.size(); i++) {
		
		
		if((orientation == ORIENTATION_GLOBAL && mode != TRANSFORM_SCALE_VIEW)) {//|| (ORIENTATION_LOCAL && mode == TRANSFORM_MOVE_VIEW)) {
			entityPositions[i] += move;
			
			Quaternion q;
			Quaternion currentRotation = selectedEntities[i]->getRotationQuat();
			Vector3 axisVector = transformConstraint;
			axisVector = currentRotation.Inverse().applyTo(axisVector);
			axisVector.Normalize();
			q.fromAngleAxis(rotate, axisVector);
			
			
			Vector3 newScale = selectedEntities[i]->getRotationQuat().applyTo(scale);
			newScale.x = fabs(newScale.x);
			newScale.y = fabs(newScale.y);
			newScale.z = fabs(newScale.z);
			if(scale.x < 0 || scale.y < 0 || scale.z < 0) {
				newScale = newScale * -1.0;
			}
			
			if(centerMode == CENTER_MODE_MEDIAN) {
				Vector3 globalPosition = selectedEntities[i]->getConcatenatedMatrix().getPosition();

				Quaternion tQ;
				tQ.fromAngleAxis(rotate, transformConstraint);
				Vector3 trans = globalCenter + tQ.applyTo(globalPosition-globalCenter) - globalPosition;
				globalPosition += trans;
				
				selectedEntities[i]->setPosition(globalPosition - selectedEntities[i]->getParentEntity()->getConcatenatedMatrix().getPosition());
				selectedEntities[i]->setRotationByQuaternion(currentRotation * q);
				if(move.length() == 0.0) {
					entityPositions[i] = selectedEntities[i]->getPosition();
				}

				selectedEntities[i]->setScale(selectedEntities[i]->getScale() * (Vector3(1.0, 1.0, 1.0)+newScale));
				
				if(newScale.length() > 0.0) {
					Vector3 scalePosition;
					scalePosition.x = globalPosition.x + ((globalPosition.x - globalCenter.x) * newScale.x);
					scalePosition.y = globalPosition.y + ((globalPosition.y - globalCenter.y) * newScale.y);
					scalePosition.z = globalPosition.z + ((globalPosition.z - globalCenter.z) * newScale.z);
					scalePosition = selectedEntities[i]->getParentEntity()->getConcatenatedMatrix().Inverse().transpose() * scalePosition;
					selectedEntities[i]->setPosition(scalePosition);
					if(move.length() == 0.0) {
						entityPositions[i] = selectedEntities[i]->getPosition();
					}
				}
				
			} else {
				selectedEntities[i]->setRotationByQuaternion(currentRotation * q);
				if(mode == TRANSFORM_SCALE_VIEW)
					selectedEntities[i]->setScale(oldScale[i] * scale);
				else
					selectedEntities[i]->setScale(selectedEntities[i]->getScale() * (Vector3(1.0, 1.0, 1.0)+newScale));
			}
		} else {
			
			entityPositions[i] += getRotationQuat().applyTo(move);
			
			Quaternion q;
			Quaternion currentRotation = selectedEntities[i]->getRotationQuat();
			Vector3 axisVector = transformConstraint;
			
			// always global in the 2d view
			if(gizmoMode != GIZMO_MODE_3D) {
				axisVector = currentRotation.Inverse().applyTo(axisVector);
			}
			axisVector.Normalize();
			q.fromAngleAxis(rotate, axisVector);
			
			
			if(centerMode == CENTER_MODE_MEDIAN) {
				Vector3 globalPosition = selectedEntities[i]->getConcatenatedMatrix().getPosition();
				
				Quaternion tQ;
				tQ.fromAngleAxis(rotate, getRotationQuat().applyTo(axisVector));
				Vector3 trans = globalCenter + tQ.applyTo(globalPosition-globalCenter) - globalPosition;
				globalPosition += trans;
				
				selectedEntities[i]->setPosition(globalPosition - selectedEntities[i]->getParentEntity()->getConcatenatedMatrix().getPosition());
				selectedEntities[i]->setRotationByQuaternion(currentRotation * q);
				if(move.length() == 0.0) {
					entityPositions[i] = selectedEntities[i]->getPosition();
				}
								
				if(mode == TRANSFORM_SCALE_VIEW)
					selectedEntities[i]->setScale(oldScale[i] * scale);
				else
					selectedEntities[i]->setScale(selectedEntities[i]->getScale() * (Vector3(1.0, 1.0, 1.0)+scale));
				
				if(scale.length() > 0.0) {
					Vector3 scalePosition;
					scalePosition.x = globalPosition.x + ((globalPosition.x - globalCenter.x) * scale.x);
					scalePosition.y = globalPosition.y + ((globalPosition.y - globalCenter.y) * scale.y);
					scalePosition.z = globalPosition.z + ((globalPosition.z - globalCenter.z) * scale.z);
					scalePosition = selectedEntities[i]->getParentEntity()->getConcatenatedMatrix().Inverse().transpose() * scalePosition;
					selectedEntities[i]->setPosition(scalePosition);
					if(move.length() == 0.0) {
						entityPositions[i] = selectedEntities[i]->getPosition();
					}
				}
				
			} else {
				selectedEntities[i]->setRotationByQuaternion(currentRotation * q);
				if(mode == TRANSFORM_SCALE_VIEW)
					selectedEntities[i]->setScale(oldScale[i] * scale);
				else
					selectedEntities[i]->setScale(selectedEntities[i]->getScale() * (Vector3(1.0, 1.0, 1.0)+scale));
			}

			
		}

		// snap if moving and snap is on
		if(scale.length() == 0.0 && rotate == 0.0) {
			if(snapEnabled) {
				Vector3 snappedPositon = entityPositions[i];
				snappedPositon.x = round(((Number)snappedPositon.x)/(snapSize)) * snapSize;
				snappedPositon.y = round(((Number)snappedPositon.y)/(snapSize)) * snapSize;
				snappedPositon.z = round(((Number)snappedPositon.z)/(snapSize)) * snapSize;
				selectedEntities[i]->setPosition(snappedPositon);
			} else {
				selectedEntities[i]->setPosition(entityPositions[i]);
			}
		}
		
	}
}

Number TransformGizmo::getTransformPlaneAngle() {
	
	Ray gizmoRay;
	gizmoRay.origin = getConcatenatedMatrix().getPosition();
	gizmoRay.direction = localTransformPlane * -1;
				
	Vector3 gizmoIntersect = gizmoRay.planeIntersectPoint(transformPlane, transformPlaneDistance);
	gizmoIntersect = planeMatrix.Inverse() * gizmoIntersect;
	   
	
	Ray ray = targetScene->projectRayFromCameraAndViewportCoordinate(targetCamera, coreInput->getMousePosition());	
	Vector3 mouseIntersect = ray.planeIntersectPoint(transformPlane, transformPlaneDistance);
	mouseIntersect = planeMatrix.Inverse() * mouseIntersect;
	
	Vector2 planePosition;
	
	if(localTransformPlane.x > 0) {
		planePosition.x = mouseIntersect.z - gizmoIntersect.z;
		planePosition.y = mouseIntersect.y - gizmoIntersect.y;
	} else if(localTransformPlane.y > 0.0) {
		planePosition.x = mouseIntersect.x - gizmoIntersect.x;
		planePosition.y = mouseIntersect.z - gizmoIntersect.z;
	} else if(localTransformPlane.z > 0.0) {
		planePosition.x = mouseIntersect.x - gizmoIntersect.x;
		planePosition.y = mouseIntersect.y - gizmoIntersect.y;
	}
	
	planePosition.Normalize();
	
	return atan2(planePosition.x, planePosition.y);
}

Vector2 TransformGizmo::getCorrectedMousePosition() {
	Vector2 localMouse = core->getInput()->getMousePosition();
	localMouse.x -= targetScene->sceneMouseRect.x;
	localMouse.y -= targetScene->sceneMouseRect.y;
	return localMouse;
}

Number TransformGizmo::get2dAngle() {

	Polycode::Rectangle view = targetScene->sceneMouseRect;
	Polycode::Rectangle camView = targetCamera->getViewport();
	Vector2 origin = getScreenPosition(targetCamera->getProjectionMatrix(), targetCamera->getConcatenatedMatrix().Inverse(), camView);
	origin.y = core->getYRes() - origin.y;
	
	
	Vector2 localStart = mouseStart2d - origin;
	Vector2 localMouse = core->getInput()->getMousePosition();

	
	localMouse.x -= view.x;
	localMouse.y -= view.y;
	
	localMouse -= origin;

	
	Number ang = atan2(localStart.crossProduct(localMouse), localStart.dot(localMouse));
	Number dot = targetCamera->getRotationQuat().applyTo(Vector3(0,0,-1)).dot(rotationQuat.applyTo(transformConstraint));
	if(dot < 0.0)
		ang *= -1.0;
	else {
		if(gizmoMode == GIZMO_MODE_2D_X || gizmoMode == GIZMO_MODE_2D_Y)
			ang *= -1.0;
	}
	return ang;
}

void TransformGizmo::setGizmoMode(int newMode) {
	gizmoMode = newMode;
	setTransformMode(transformMode);
}

void TransformGizmo::handleEvent(Event *event) {
	if(!enableGizmo) {
		return;
	}
	
	if(!coreInput->getKeyState(KEY_LALT) && !coreInput->getKeyState(KEY_RALT)) {
		if(event->getDispatcher() == pitchGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					transforming = true;
					transformConstraint = Vector3(1.0, 0.0, 0.0);
					setTransformPlane(1.0, 0.0, 0.0);
					mouseStart2d = getCorrectedMousePosition();
					startingAngle = get2dAngle();
				}
			}
		} else	if(event->getDispatcher() == yawGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					transforming = true;
					transformConstraint = Vector3(0.0, 1.0, 0.0);
					setTransformPlane(0.0, 1.0, 0.0);
					mouseStart2d = getCorrectedMousePosition();
					startingAngle = get2dAngle();
				}
			}
		} else	if(event->getDispatcher() == rollGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					transforming = true;
					transformConstraint = Vector3(0.0, 0.0, 1.0);
					setTransformPlane(0.0, 0.0, -1.0);
					mouseStart2d = getCorrectedMousePosition();
					startingAngle = get2dAngle();
				}
			}
		} else	if(event->getDispatcher() == viewportRotateGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					if(gizmoMode != GIZMO_MODE_3D) {
						transforming = true;
						setTransformPlaneFromView();
						transformConstraint = transformPlane;
						mouseStart2d = getCorrectedMousePosition();
						startingAngle = get2dAngle();
					}
				}
			}
		}

		if(event->getDispatcher() == xTransformGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					transforming = true;
					transformConstraint = Vector3(1.0, 0.0, 0.0);
					setTransformPlane(true, false, false);
					startingPoint = getTransformPlanePosition();
				}
			}
		} else	if(event->getDispatcher() == yTransformGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					transforming = true;
					transformConstraint = Vector3(0.0, 1.0, 0.0);
					setTransformPlane(false, true, false);
					startingPoint = getTransformPlanePosition();
				}
			}
		} else	if(event->getDispatcher() == zTransformGrip) {
			if(event->getEventCode() == InputEvent::EVENT_MOUSEDOWN) {
				if(((InputEvent*)event)->getMouseButton() != CoreInput::MOUSE_BUTTON3) {
					transforming = true;
					transformConstraint = Vector3(0.0, 0.0, 1.0);
					setTransformPlane(false, false, true);
					startingPoint = getTransformPlanePosition();
				}
			}		
		}
	}
	
	if(event->getDispatcher() == coreInput) {
		
		if (!coreInput->getKeyState(KEY_LCTRL) && !coreInput->getKeyState(KEY_RCTRL) && !coreInput->getKeyState(KEY_LALT) && !coreInput->getKeyState(KEY_RALT)) {
			if (event->getEventCode() == InputEvent::EVENT_KEYDOWN) {
				InputEvent *inputEvent = (InputEvent*)event;
				switch (inputEvent->key) {
				case KEY_s:
				{
					transforming = true;
					transformConstraint = Vector3(1.0, 1.0, 1.0);
					previousMode = mode;
					mode = TRANSFORM_SCALE_VIEW;
					setTransformPlaneFromView();
					startingPoint = getTransformPlanePosition();
					mouseStart2d = getCorrectedMousePosition();
					scaleAmount = 0.0;
					resetTransform();
				}
					break;
				case KEY_r:
				{
					previousMode = mode;
					mode = TRANSFORM_ROTATE_VIEW;
					transforming = true;
					transformConstraint = Vector3(1.0, 1.0, 1.0);
					setTransformPlaneFromView();
					transformConstraint = transformPlane;
					mouseStart2d = getCorrectedMousePosition();
					startingAngle = get2dAngle();
					resetTransform();
				}
					break;
				case KEY_g:
				{
					previousMode = mode;
					mode = TRANSFORM_MOVE_VIEW;
					transforming = true;
					transformConstraint = Vector3(1.0, 1.0, 1.0);
					setTransformPlaneFromView();
					startingPoint = getTransformPlanePosition();
					resetTransform();
				}
					break;
				case KEY_x:
				{
					if(transforming && (mode == TRANSFORM_MOVE_VIEW || mode == TRANSFORM_SCALE_VIEW || ((mode == TRANSFORM_ROTATE_VIEW) && (gizmoMode == GIZMO_MODE_3D)))) {
						if((coreInput->getKeyState(KEY_LSHIFT) || coreInput->getKeyState(KEY_RSHIFT)) && (mode != TRANSFORM_ROTATE_VIEW) && ((gizmoMode == GIZMO_MODE_2D_X) || (gizmoMode == GIZMO_MODE_3D))) {
							if(transformConstraint != Vector3(0.0, 1.0, 1.0)) {
								transformConstraint = Vector3(0.0, 1.0, 1.0);
								setTransformPlane(false, true, true);
								resetTransform();
							} else {
								toggleOrientation();
								resetTransform();
							}
						} else if(gizmoMode != GIZMO_MODE_2D_X){
							if(transformConstraint != Vector3(1.0, 0.0, 0.0)) {
								transformConstraint = Vector3(1.0, 0.0, 0.0);
								setTransformPlane(true, false, false);
								resetTransform();
							} else {
								switch(gizmoMode) {
									case GIZMO_MODE_3D:
										toggleOrientation();
										break;
									case GIZMO_MODE_2D_Y:
										transformConstraint = Vector3(1.0, 0.0, 1.0);
										break;
									case GIZMO_MODE_2D_Z:
										transformConstraint = Vector3(1.0, 1.0, 0.0);
										break;
								}
								resetTransform();
							}
						}
					}	
				}
					break;
				case KEY_y:
				{
					if(transforming && (mode == TRANSFORM_MOVE_VIEW || mode == TRANSFORM_SCALE_VIEW || ((mode == TRANSFORM_ROTATE_VIEW) && (gizmoMode == GIZMO_MODE_3D)))) {
						if((coreInput->getKeyState(KEY_LSHIFT) || coreInput->getKeyState(KEY_RSHIFT)) && (mode != TRANSFORM_ROTATE_VIEW) && ((gizmoMode == GIZMO_MODE_2D_Y) || (gizmoMode == GIZMO_MODE_3D))) {
							if(transformConstraint != Vector3(1.0, 0.0, 1.0)) {
								transformConstraint = Vector3(1.0, 0.0, 1.0);
								setTransformPlane(true, false, true);
								resetTransform();
							} else {
								toggleOrientation();
								resetTransform();
							}
						} else if(gizmoMode != GIZMO_MODE_2D_Y){
							if(transformConstraint != Vector3(0.0, 1.0, 0.0)) {
								transformConstraint = Vector3(0.0, 1.0, 0.0);
								setTransformPlane(false, true, false);
								resetTransform();
							} else {
								switch(gizmoMode) {
									case GIZMO_MODE_3D:
										toggleOrientation();
										break;
									case GIZMO_MODE_2D_X:
										transformConstraint = Vector3(0.0, 1.0, 1.0);
										break;
									case GIZMO_MODE_2D_Z:
										transformConstraint = Vector3(1.0, 1.0, 0.0);
										break;
								}
								resetTransform();
							}
						}
					}	
				}
					break;
				case KEY_z:
				{
					if(transforming && (mode == TRANSFORM_MOVE_VIEW || mode == TRANSFORM_SCALE_VIEW || ((mode == TRANSFORM_ROTATE_VIEW) && (gizmoMode == GIZMO_MODE_3D)))) {
						if((coreInput->getKeyState(KEY_LSHIFT) || coreInput->getKeyState(KEY_RSHIFT)) && (mode != TRANSFORM_ROTATE_VIEW) && ((gizmoMode == GIZMO_MODE_2D_Z) || (gizmoMode == GIZMO_MODE_3D))) {
							if(transformConstraint != Vector3(1.0, 1.0, 0.0)) {
								transformConstraint = Vector3(1.0, 1.0, 0.0);
								setTransformPlane(true, true, false);
								resetTransform();
							} else {
								toggleOrientation();
								resetTransform();
							}
						} else if(gizmoMode != GIZMO_MODE_2D_Z){
							if(transformConstraint != Vector3(0.0, 0.0, 1.0)) {
								transformConstraint = Vector3(0.0, 0.0, 1.0);
								setTransformPlane(false, false, true);
								resetTransform();
							} else {
								switch(gizmoMode) {
									case GIZMO_MODE_3D:
										toggleOrientation();
										break;
									case GIZMO_MODE_2D_X:
										transformConstraint = Vector3(0.0, 1.0, 1.0);
										break;
									case GIZMO_MODE_2D_Y:
										transformConstraint = Vector3(1.0, 0.0, 1.0);
										break;
								}
								resetTransform();
							}
						}
					}	
				}
					break;
				case KEY_ESCAPE:
				{
								   if (mode == TRANSFORM_SCALE_VIEW || mode == TRANSFORM_ROTATE_VIEW || mode == TRANSFORM_MOVE_VIEW) {
									   dispatchEndEvent();
									   mode = previousMode;
								   }
								   resetTransform();
								   transforming = false;
								   firstMove = true;
				}
					break;
				}
			}
		}
		
		if(transforming) {
			switch(event->getEventCode()) {
				case InputEvent::EVENT_MOUSEMOVE:
				{
					switch(mode) {
						case TRANSFORM_MOVE:
						{
							Vector3 newPoint;
							if(isConstrainedToSingleAxis())
								newPoint = getPositionAlongAxis();
							else
								newPoint = getTransformPlanePosition();

							Vector3 diff = (planeMatrix.Inverse() * newPoint) -(planeMatrix.Inverse() * startingPoint);
							diff = diff * getCompoundScale();														 
							transformSelectedEntities(transformConstraint * diff, Vector3(0.0, 0.0, 0.0), 0.0);
							startingPoint = newPoint;
						}
						break;
						case TRANSFORM_SCALE_VIEW:
						{
							Vector2 pos2d = getScreenPosition(targetCamera->getProjectionMatrix(), targetCamera->getAnchorAdjustedMatrix(), targetCamera->getViewport());
							Vector2 currentMouse = getCorrectedMousePosition();

							Number scaleNew = currentMouse.distance(pos2d) / mouseStart2d.distance(pos2d);
							
							Vector3 scaleVec = transformConstraint * scaleNew;
							scaleAmount = scaleNew;
							if((mouseStart2d - pos2d).dot(currentMouse - pos2d) < 0.0)
								scaleVec = -scaleVec;

							scaleVec.x =  (transformConstraint.x == 0.0) ? 1.0 : scaleVec.x;
							scaleVec.y =  (transformConstraint.y == 0.0) ? 1.0 : scaleVec.y;
							scaleVec.z =  (transformConstraint.z == 0.0) ? 1.0 : scaleVec.z;

							transformSelectedEntities(Vector3(0.0, 0.0, 0.0), scaleVec, 0.0);
						}
						break;
						case TRANSFORM_ROTATE_VIEW:
						{
							Number newAngle = get2dAngle();
							
							Vector3 newPoint = planeMatrix.Inverse() *getTransformPlanePosition();
							
							transformSelectedEntities(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 0.0), newAngle - startingAngle);
							startingAngle = newAngle;
						}
						break;
						case TRANSFORM_MOVE_VIEW:
						{
							Vector3 newPoint;
							if(isConstrainedToSingleAxis())
								newPoint = getPositionAlongAxis();
							else
								newPoint = getTransformPlanePosition();

							Vector3 diff = newPoint - startingPoint;
							transformSelectedEntities(transformConstraint * diff, Vector3(0.0, 0.0, 0.0), 0.0);
							startingPoint = newPoint;
						}
						break;
						case TRANSFORM_SCALE:
						{
							Vector3 newPoint = getTransformPlanePosition();
							
							Vector3 diff = (planeMatrix.Inverse() * newPoint) -(planeMatrix.Inverse() * startingPoint);
							diff = diff * getCompoundScale();
							
							
							transformSelectedEntities(Vector3(0.0, 0.0, 0.0), (diff * transformConstraint) / getCompoundScale().x, 0.0);
							startingPoint = newPoint;					
						}
						break;
						case TRANSFORM_ROTATE:
						{
							Number newAngle = get2dAngle();
							transformSelectedEntities(Vector3(0.0, 0.0, 0.0), Vector3(0.0, 0.0, 0.0), newAngle - startingAngle);
							startingAngle = newAngle;						
						}
						break;
					}
				}
				break;
				case InputEvent::EVENT_MOUSEUP:
				{
					dispatchEndEvent();
					
					if(mode == TRANSFORM_SCALE_VIEW || mode == TRANSFORM_ROTATE_VIEW || mode == TRANSFORM_MOVE_VIEW) {
						mode = previousMode;
					}
					if( orientation != startingOrientation && startingOrientation != -1) {
						orientation = startingOrientation;
						startingOrientation = -1;
					}
					transforming = false;
					firstMove = true;
				}
				break;
			}
		}
	}
}

void TransformGizmo::dispatchEndEvent() {
	switch(mode) {
		case TRANSFORM_MOVE:
		case TRANSFORM_MOVE_VIEW:
			dispatchEvent(new TrasnformGizmoEvent(mode), Event::CHANGE_EVENT);
			break;
		case TRANSFORM_SCALE:
		case TRANSFORM_SCALE_VIEW:
			dispatchEvent(new TrasnformGizmoEvent(mode), Event::CHANGE_EVENT);
			break;
		case TRANSFORM_ROTATE:
		case TRANSFORM_ROTATE_VIEW:
			dispatchEvent(new TrasnformGizmoEvent(mode), Event::CHANGE_EVENT);
			break;
	}
}

TransformGizmo::~TransformGizmo() {
	coreInput->removeAllHandlersForListener(this);
}

void TransformGizmo::updateOrientationForEntity(Entity *entity) {
	Quaternion q;
	switch(orientation) {
		case ORIENTATION_GLOBAL:
			setRotationByQuaternion(q);
		break;
		case ORIENTATION_LOCAL:
			if(gizmoMode == GIZMO_MODE_3D || mode == TRANSFORM_SCALE)
				q = entity->getRotationQuat();
			setRotationByQuaternion(q);				
		break;
	}
}

void TransformGizmo::fixedUpdate() {
	
	if(selectedEntities.size() > 0) {
		Vector3 centerPoint;
		for(int i=0; i < selectedEntities.size(); i++) {
			centerPoint += selectedEntities[i]->getConcatenatedMatrix().getPosition();
		}
		centerPoint = centerPoint / selectedEntities.size();
		setPosition(centerPoint);
		
		updateOrientationForEntity(selectedEntities[0]);
	}
	
	viewportRotateGripBase->setRotationByQuaternion(getRotationQuat().Inverse());
	
	Number scale;
	if(gizmoMode != GIZMO_MODE_3D) {
		scale = targetCamera->getOrthoSizeX() * 0.1;

	} else {
		scale = getPosition().distance(targetCamera->getPosition()) * 0.1;
	}

	setScale(scale, scale, scale);
}

