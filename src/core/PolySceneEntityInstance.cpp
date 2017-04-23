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

#include "polycode/core/PolySceneEntityInstance.h"
#include "polycode/core/PolyLogger.h"
#include "polycode/core/PolyResourceManager.h"
#include "polycode/core/PolyMaterial.h"
#include "polycode/core/PolySceneLight.h"
#include "polycode/core/PolySceneMesh.h"
#include "polycode/core/PolySceneLabel.h"
#include "polycode/core/PolySceneSound.h"
#include "polycode/core/PolyCamera.h"

using namespace Polycode;

SceneEntityInstanceResourceEntry::SceneEntityInstanceResourceEntry()  : Resource(Resource::RESOURCE_ENTITY_INSTANCE), instance(NULL) {
}

SceneEntityInstanceResourceEntry::SceneEntityInstanceResourceEntry(SceneEntityInstance *instance)  : Resource(Resource::RESOURCE_ENTITY_INSTANCE) {
	this->instance = instance;
}

SceneEntityInstanceResourceEntry::~SceneEntityInstanceResourceEntry() {

}

SceneEntityInstance *SceneEntityInstanceResourceEntry::getInstance() {
	return instance;
}

SceneEntityInstance::SceneEntityInstance(Core *core) : core(core) {
	
}

void SceneEntityInstanceResourceEntry::reloadResource(Core *core) {
	instance->reloadEntityInstance();
	Resource::reloadResource(core);
}

SceneEntityInstance *SceneEntityInstance::BlankSceneEntityInstance() {
	return new SceneEntityInstance();
}

SceneEntityInstance::SceneEntityInstance(Core *core, const String& fileName) : core(core) {
	createNewLayer("default");
	resourceEntry = std::make_shared<SceneEntityInstanceResourceEntry>(this);
	topLevelResourcePool = core->getResourceManager()->getGlobalPool();
	loadFromFile(fileName);
	resourceEntry->setResourceName(fileName);
	resourceEntry->setResourcePath(fileName);
	cloneUsingReload = false;
	ownsChildren = true;
}

SceneEntityInstance::SceneEntityInstance() : Entity() {
	createNewLayer("default");
	cloneUsingReload = true;
	ownsChildren = true;
	topLevelResourcePool = core->getResourceManager()->getGlobalPool();	  
	resourceEntry = std::make_shared<SceneEntityInstanceResourceEntry>(this);
}

SceneEntityInstance::~SceneEntityInstance() {
	for(int i=0; i < layers.size(); i++) {
			delete layers[i];
	}
	core->getResourceManager()->removeResource(resourceEntry);
	for(int i=0; i < resourcePools.size(); i++) {
		core->getResourceManager()->unsubscibeFromResourcePool(resourcePools[i]);
	}
}

void SceneEntityInstance::reloadEntityInstance() {
	loadFromFile(fileName);
}

std::shared_ptr<SceneEntityInstanceResourceEntry> SceneEntityInstance::getResourceEntry() {
	return resourceEntry;
}

Entity *SceneEntityInstance::Clone(bool deepClone, bool ignoreEditorOnly) const {
	SceneEntityInstance *newEntity;
	if(cloneUsingReload) {
		newEntity = new SceneEntityInstance(core, fileName);
	} else {
		newEntity = new SceneEntityInstance(core);
	}
	applyClone(newEntity, deepClone, ignoreEditorOnly);
	return newEntity;
}

void SceneEntityInstance::applyClone(Entity *clone, bool deepClone, bool ignoreEditorOnly) const {
	if(cloneUsingReload) {
		Entity::applyClone(clone, false, ignoreEditorOnly);
	} else {
		Entity::applyClone(clone, deepClone, ignoreEditorOnly);
		SceneEntityInstance *_clone = (SceneEntityInstance*) clone;
		_clone->fileName = fileName;
	}
}


void SceneEntityInstance::linkResourcePool(ResourcePool *pool) {
	for(int i=0; i < resourcePools.size(); i++) {
		if(resourcePools[i] == pool) {
			return;
		}
	}
	pool->setFallbackPool(topLevelResourcePool);
	topLevelResourcePool = pool;
	core->getResourceManager()->subscribeToResourcePool(pool);
	resourcePools.push_back(pool);
}

unsigned int SceneEntityInstance::getNumLinkedResourePools() {
	return resourcePools.size();
}

ResourcePool *SceneEntityInstance::getLinkedResourcePoolAtIndex(unsigned int index) {
	return resourcePools[index];
}

void SceneEntityInstance::rebuildResourceLinks() {
	for(int i=0; i < resourcePools.size(); i++) {
		if(i == 0) {
			resourcePools[i]->setFallbackPool(core->getResourceManager()->getGlobalPool());
		} else {
			resourcePools[i]->setFallbackPool(resourcePools[i-1]);
		}
	}
	
	if(resourcePools.size() > 0) {
		topLevelResourcePool = resourcePools[resourcePools.size()-1];
	} else {
		topLevelResourcePool = core->getResourceManager()->getGlobalPool();
	}
}

void SceneEntityInstance::unlinkResourcePool(ResourcePool *pool) {
	for(int i=0; i < resourcePools.size(); i++) {
		if(resourcePools[i] == pool) {
			resourcePools.erase(resourcePools.begin() + i);
			rebuildResourceLinks();
			core->getResourceManager()->unsubscibeFromResourcePool(pool);
			return;
		}
	}
}

void SceneEntityInstance::applySceneMesh(ObjectEntry *entry, SceneMesh *sceneMesh) {
	if(!entry) {
		return;
	}
	
	if((*entry)["sendBoneMatricesToMaterial"]) {
		sceneMesh->sendBoneMatricesToMaterial = (*entry)["sendBoneMatricesToMaterial"]->boolVal;
	}
	
	if((*entry)["backfaceCulled"]) {
		sceneMesh->backfaceCulled = (*entry)["backfaceCulled"]->boolVal;
	}
	
	ObjectEntry *materialName =(*entry)["material"];
	if(materialName) {
        
        std::shared_ptr<Material> material = topLevelResourcePool->getMaterial(materialName->stringVal);
        sceneMesh->setMaterial(material);
		if(sceneMesh->getMaterial()) {
			ObjectEntry *optionsEntry =(*entry)["shader_options"];
			if(optionsEntry) {
				for(int i=0; i < optionsEntry->length; i++) {
					ObjectEntry *shaderEntry =(*optionsEntry)[i];
					if(shaderEntry) {
						
						// parse in texture bindings
						ObjectEntry *texturesEntry =(*shaderEntry)["textures"];
						if(texturesEntry) {
							for(int j=0; j < texturesEntry->length; j++) {
								ObjectEntry *textureEntry =(*texturesEntry)[j];
								if(textureEntry) {
									ObjectEntry *nameEntry = (*textureEntry)["name"];
									if(nameEntry && textureEntry->stringVal != "") {
										
										if(textureEntry->name == "cubemap") {
											Cubemap *cubemap;
											// RENDERER_TODO
											//cubemap = (Cubemap*)topLevelResourcePool->getResource(Resource::RESOURCE_CUBEMAP, textureEntry->stringVal);
											/*
											if(cubemap) {
												sceneMesh->getLocalShaderOptions()->addCubemap(nameEntry->stringVal, cubemap);
											}
											 */
										} else {
											std::shared_ptr<Texture> texture = core->getResourceManager()->getGlobalPool()->loadTexture(textureEntry->stringVal);
											sceneMesh->getShaderPass(0).shaderBinding->setTextureForParam(nameEntry->stringVal, texture);
										}
									}
								}
							}
						}
						
						ObjectEntry *paramsEntry =(*shaderEntry)["params"];
						if(paramsEntry) {
							for(int j=0; j < paramsEntry->length; j++) {
								ObjectEntry *paramEntry =(*paramsEntry)[j];
								if(paramEntry) {
									ObjectEntry *nameEntry = (*paramEntry)["name"];
									ObjectEntry *valueEntry = (*paramEntry)["value"];
									if(nameEntry && valueEntry) {
										std::shared_ptr<Shader> materialShader = sceneMesh->getMaterial()->getShader(i);
										if(materialShader) {
											int type = materialShader->getExpectedParamType(nameEntry->stringVal);
											
											// RENDERER_TODO
											std::shared_ptr<LocalShaderParam> param = sceneMesh->getShaderPass(0).shaderBinding->addParamFromData(core->getResourceManager()->getGlobalPool(), nameEntry->stringVal, valueEntry->stringVal);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void SceneEntityInstance::parseObjectIntoCurve(ObjectEntry *entry, BezierCurve *curve) {
	curve->clearControlPoints();
	ObjectEntry *controlPoints =(*entry)["controlPoints"];
	if(controlPoints) {
		for(int i=0; i < controlPoints->length; i++) {		
			ObjectEntry *controlPoint = ((*controlPoints))[i];		
			if(controlPoint) {
				Vector3 vpt1;
				Vector3 vpt2;
				Vector3 vpt3;
												
				ObjectEntry *pt1 = ((*controlPoint))["pt1"];
				if(pt1) {
					vpt1.x = ((*pt1))["x"]->NumberVal;
					vpt1.y = ((*pt1))["y"]->NumberVal;
					vpt1.z = ((*pt1))["z"]->NumberVal;
				}

				ObjectEntry *pt2 = ((*controlPoint))["pt2"];
				if(pt2) {
					vpt2.x = ((*pt2))["x"]->NumberVal;
					vpt2.y = ((*pt2))["y"]->NumberVal;
					vpt2.z = ((*pt2))["z"]->NumberVal;
					
				}

				ObjectEntry *pt3 = ((*controlPoint))["pt3"];
				if(pt3) {
					vpt3.x = ((*pt3))["x"]->NumberVal;
					vpt3.y = ((*pt3))["y"]->NumberVal;
					vpt3.z = ((*pt3))["z"]->NumberVal;
				}

				curve->addControlPoint(vpt1.x, vpt1.y, vpt1.z, vpt2.x, vpt2.y, vpt2.z, vpt3.x, vpt3.y, vpt3.z);
			}
		}
	}
	
}

Entity *SceneEntityInstance::loadObjectEntryIntoEntity(ObjectEntry *entry, Entity *targetEntity, int entityFileVersion) {

	Entity *entity = NULL;

	ObjectEntry *entityType = (*entry)["type"];
	if(entityType) {
		if(entityType->stringVal == "SceneEntityInstance") {
			ObjectEntry *instanceEntry = (*entry)["SceneEntityInstance"];
			String filePath = (*instanceEntry)["filePath"]->stringVal;
			SceneEntityInstance *instance = new SceneEntityInstance(core, filePath);
			entity = instance;
		 } else if(entityType->stringVal == "SceneCurve") {
			ObjectEntry *curveEntry = (*entry)["SceneCurve"];
			 
			 SceneCurve *curve = new SceneCurve();
			 
			if(curveEntry) {
				curve->renderCurve = (*curveEntry)["render"]->boolVal;
				curve->curveResolution = (*curveEntry)["resolution"]->intVal;				 
				parseObjectIntoCurve((*curveEntry)["curve"], curve->getCurve());
			}
			 applySceneMesh((*entry)["SceneMesh"], curve);
			 entity = curve;
			 
		 } else if(entityType->stringVal == "SceneSprite") {

			ObjectEntry *spriteEntry = (*entry)["SceneSprite"];
			String spriteSetName = (*spriteEntry)["sprite_set"]->stringVal;

			 SpriteSet *spriteSet = (SpriteSet*)core->getResourceManager()->getResourcePoolByName(spriteSetName);
			 
			 if(spriteSet) {
				 SceneSprite *sprite = new SceneSprite(spriteSet);
				 
				 String spriteName = (*spriteEntry)["sprite"]->stringVal;
				 sprite->setSpriteByName(spriteName);

				 
				 String stateName = (*spriteEntry)["state"]->stringVal;
				 
				 if(sprite->getCurrentSprite()) {
					 SpriteState *state = sprite->getCurrentSprite()->getStateByName(stateName);
					 if(state) {
						 sprite->setSpriteState(state, 0, false);
					 }
					 
					 ObjectEntry *randomFrameEntry = (*spriteEntry)["random_frame"];
					 if(randomFrameEntry) {
						sprite->setStartOnRandomFrame(randomFrameEntry->boolVal);
					 }
					 
				 }
				 
				 entity = sprite;
				 applySceneMesh((*entry)["SceneMesh"], sprite);
			 }

		} else	if(entityType->stringVal == "SceneLabel") {
			ObjectEntry *labelEntry = (*entry)["SceneLabel"];
			
			String text = (*labelEntry)["text"]->stringVal;
			String fontName = (*labelEntry)["font"]->stringVal;
			int size = (*labelEntry)["size"]->intVal;
			Number actualHeight = (*labelEntry)["actualHeight"]->intVal;
			int aaMode = (*labelEntry)["aaMode"]->intVal;
			
            std::shared_ptr<Font> font = core->getResourceManager()->getGlobalPool()->getFont(fontName);
			
			SceneLabel *label = new SceneLabel(topLevelResourcePool->getMaterial("Unlit"), text, size, font, aaMode, actualHeight);
			label->setAnchorPoint(0.0, 0.0, 0.0);
			label->snapToPixels = false;
			label->positionAtBaseline = false;
			applySceneMesh((*entry)["SceneMesh"], label);
			
			entity = label;
		} else if(entityType->stringVal == "SceneParticleEmitter") {
			
			ObjectEntry *emitterEntry = (*entry)["SceneParticleEmitter"];
			SceneParticleEmitter *emitter = new SceneParticleEmitter(1, 1, 1);

			emitter->setParticleType((*emitterEntry)["type"]->intVal);
			emitter->setParticleSpeed((*emitterEntry)["speed"]->NumberVal);
			emitter->setParticleCount((*emitterEntry)["count"]->intVal);
			emitter->setParticleLifetime((*emitterEntry)["lifetime"]->NumberVal);
			emitter->setParticleSize((*emitterEntry)["size"]->NumberVal);
			emitter->setParticlesInWorldSpace((*emitterEntry)["world"]->boolVal);
			emitter->setLoopParticles((*emitterEntry)["loop"]->boolVal);
			
			emitter->setParticleRotationSpeed(Vector3((*emitterEntry)["rX"]->NumberVal, (*emitterEntry)["rY"]->NumberVal, (*emitterEntry)["rZ"]->NumberVal));
			emitter->setGravity(Vector3((*emitterEntry)["gX"]->NumberVal, (*emitterEntry)["gY"]->NumberVal, (*emitterEntry)["gZ"]->NumberVal));
			emitter->setParticleDirection(Vector3((*emitterEntry)["dirX"]->NumberVal, (*emitterEntry)["dirY"]->NumberVal, (*emitterEntry)["dirZ"]->NumberVal));
			emitter->setEmitterSize(Vector3((*emitterEntry)["eX"]->NumberVal, (*emitterEntry)["eY"]->NumberVal, (*emitterEntry)["eZ"]->NumberVal));
			emitter->setDirectionDeviation(Vector3((*emitterEntry)["devX"]->NumberVal, (*emitterEntry)["devY"]->NumberVal, (*emitterEntry)["devZ"]->NumberVal));
			
			emitter->setPerlinEnabled((*emitterEntry)["perlin"]->boolVal);
			if(emitter->getPerlinEnabled()) {
				emitter->setPerlinValue(Vector3((*emitterEntry)["pX"]->NumberVal, (*emitterEntry)["pY"]->NumberVal, (*emitterEntry)["pZ"]->NumberVal));
			}
			
			emitter->useColorCurves = (*emitterEntry)["useColorCurves"]->boolVal;
			emitter->useScaleCurve = (*emitterEntry)["useScaleCurve"]->boolVal;
			
			parseObjectIntoCurve((*emitterEntry)["colorCurveR"], &emitter->colorCurveR);
			parseObjectIntoCurve((*emitterEntry)["colorCurveG"], &emitter->colorCurveG);
			parseObjectIntoCurve((*emitterEntry)["colorCurveB"], &emitter->colorCurveB);
			parseObjectIntoCurve((*emitterEntry)["colorCurveA"], &emitter->colorCurveA);
			parseObjectIntoCurve((*emitterEntry)["scaleCurve"], &emitter->scaleCurve);
			
			applySceneMesh((*entry)["SceneMesh"], emitter);
			entity = emitter;
			
		} else if(entityType->stringVal == "SceneLight") {
			
			ObjectEntry *lightEntry = (*entry)["SceneLight"];
			if(lightEntry) {
				int lightType = (*lightEntry)["type"]->intVal;
				SceneLight *newLight  = new SceneLight(lightType, 0, 1, 1, 1, core->getResourceManager()->getGlobalPool()->getMaterial("Unlit"));
				
				newLight->setIntensity((*lightEntry)["intensity"]->NumberVal);
				
				ObjectEntry *importanceEntry = (*lightEntry)["importance"];
				if(importanceEntry) {
					newLight->setLightImportance(importanceEntry->intVal);
				}
				
				newLight->setDiffuseLightColor((*lightEntry)["cR"]->NumberVal, (*lightEntry)["cG"]->NumberVal, (*lightEntry)["cB"]->NumberVal, (*lightEntry)["cA"]->NumberVal);
				newLight->setSpecularLightColor((*lightEntry)["scR"]->NumberVal, (*lightEntry)["scG"]->NumberVal, (*lightEntry)["scB"]->NumberVal, (*lightEntry)["scA"]->NumberVal);

				newLight->setAttenuation((*lightEntry)["cAtt"]->NumberVal, (*lightEntry)["lAtt"]->NumberVal, (*lightEntry)["qAtt"]->NumberVal);
				
				if(newLight->getType() == SceneLight::SPOT_LIGHT) {
					newLight->setSpotlightProperties((*lightEntry)["spotCutoff"]->NumberVal, (*lightEntry)["spotExponent"]->NumberVal);
					
					if((*lightEntry)["shadows"]->boolVal) {
						newLight->enableShadows(true, (*lightEntry)["shadowmapRes"]->intVal);
						newLight->setShadowMapFOV((*lightEntry)["shadowmapFOV"]->NumberVal);
					}
				}
				
				//parentScene->addLight(newLight);
				entity = newLight;
			}
		} else if(entityType->stringVal == "ScenePrimitive") {
			ObjectEntry *scenePrimitiveEntry = (*entry)["ScenePrimitive"];
			int pType = (*scenePrimitiveEntry)["type"]->intVal;
			Number p1 = (*scenePrimitiveEntry)["p1"]->NumberVal;
			Number p2 = (*scenePrimitiveEntry)["p2"]->NumberVal;
			Number p3 = (*scenePrimitiveEntry)["p3"]->NumberVal;
			Number p4 = (*scenePrimitiveEntry)["p4"]->NumberVal;
			Number p5 = (*scenePrimitiveEntry)["p5"]->NumberVal;
			
			ScenePrimitive *primitive = new ScenePrimitive(pType, p1, p2, p3, p4, p5);
			entity = primitive;
			applySceneMesh((*entry)["SceneMesh"], primitive);
		} else if(entityType->stringVal == "SceneMesh") {
			ObjectEntry *meshEntry = (*entry)["SceneMesh"];
			if(meshEntry) {
				ObjectEntry *fileName = (*meshEntry)["file"];
				if(fileName) {
					SceneMesh *newMesh = new SceneMesh(topLevelResourcePool, fileName->stringVal);
					applySceneMesh(meshEntry, newMesh);
					entity = newMesh;
				}
			}
		} else if(entityType->stringVal == "SceneSound") {
			ObjectEntry *soundEntry = (*entry)["SceneSound"];
			
			String filePath = (*soundEntry)["filePath"]->stringVal;
			Number refDistance = (*soundEntry)["refDistance"]->NumberVal;
			Number maxDistance = (*soundEntry)["maxDistance"]->NumberVal;
			Number volume = (*soundEntry)["volume"]->NumberVal;
			Number pitch = (*soundEntry)["pitch"]->NumberVal;
			
			SceneSound *sound = new SceneSound(core, filePath, refDistance, maxDistance);
			sound->getSound()->setVolume(volume);
			sound->getSound()->setPitch(pitch);
			
			if((*soundEntry)["loopOnLoad"]) {
				bool loopOnLoad = (*soundEntry)["loopOnLoad"]->boolVal;
				sound->setLoopOnLoad(loopOnLoad);
				if(loopOnLoad) {
					sound->getSound()->Play(true);
				}
			}
			
			
			entity = sound;
		} else if(entityType->stringVal == "Camera") {
			ObjectEntry *cameraEntry = (*entry)["Camera"];
			
			Camera *camera = new Camera();
			
			camera->setClippingPlanes((*cameraEntry)["nearClip"]->NumberVal, (*cameraEntry)["farClip"]->NumberVal);
			camera->setOrthoMode((*cameraEntry)["ortho"]->boolVal);
			
			if(camera->getOrthoMode()) {
				camera->setOrthoSizeMode((*cameraEntry)["sizeMode"]->intVal);
				camera->setOrthoSize((*cameraEntry)["orthoWidth"]->NumberVal, (*cameraEntry)["orthoHeight"]->NumberVal);
			} else {
				camera->setFOV((*cameraEntry)["fov"]->NumberVal);
			}
			
			entity = camera;
		}

		
	}

	if(!entity) {
		if(targetEntity) {
			entity = targetEntity;
		} else {
			entity = new Entity();
		}
	}
	
	entity->ownsChildren = true;

	Vector3 bBox;
	entry->readNumber("bbX", &bBox.x);
	entry->readNumber("bbY", &bBox.y);
	entry->readNumber("bbZ", &bBox.z);
	entity->setLocalBoundingBox(bBox);
	
	entity->color.r = (*entry)["cR"]->NumberVal;
	entity->color.g = (*entry)["cG"]->NumberVal;
	entity->color.b = (*entry)["cB"]->NumberVal;
	entity->color.a = (*entry)["cA"]->NumberVal;


	if(!targetEntity) { 
		entity->setBlendingMode((*entry)["blendMode"]->intVal);

		entity->setScale((*entry)["sX"]->NumberVal, (*entry)["sY"]->NumberVal, (*entry)["sZ"]->NumberVal);
		entity->setPosition((*entry)["pX"]->NumberVal, (*entry)["pY"]->NumberVal, (*entry)["pZ"]->NumberVal);
		
		if(entityFileVersion > 1) {
			entity->setRotationQuat((*entry)["rW"]->NumberVal, (*entry)["rX"]->NumberVal, (*entry)["rY"]->NumberVal, (*entry)["rZ"]->NumberVal);
		} else {
			entity->setRotationEuler(Vector3((*entry)["rX"]->NumberVal, (*entry)["rY"]->NumberVal, (*entry)["rZ"]->NumberVal));
		}
	}
	
	if((*entry)["id"]->stringVal != "") {
		entity->id = (*entry)["id"]->stringVal;
	}
	
	if((*entry)["layerID"]) {
		entity->layerID = (unsigned int)((*entry)["layerID"]->intVal);
	}
	
	String tagString = (*entry)["tags"]->stringVal; 
	
	if(tagString != "") {
		std::vector<String> tags = tagString.split(",");
		for(int i=0; i < tags.size(); i++) {
			entity->addTag(tags[i]);
		}
	}
	
	ObjectEntry *props = (*entry)["props"];
	if(props) {
		for(int i=0; i < props->length; i++) {		
			ObjectEntry *prop = ((*props))[i];		
			if(prop) {
				entity->setEntityProp((*prop)["name"]->stringVal, (*prop)["value"]->stringVal);
			}
		}
	}
														
	ObjectEntry *children = (*entry)["children"];
	
	if(children) {
		for(int i=0; i < children->length; i++) {
			ObjectEntry *childEntry = ((*children))[i];
			ScreenEntity *childEntity = loadObjectEntryIntoEntity(childEntry, NULL, entityFileVersion);
			entity->addChild(childEntity);				
		}
	}

	return entity;
}

String SceneEntityInstance::getFileName() const {
	return fileName;
}

ResourcePool *SceneEntityInstance::getTopLevelResourcePool() {
	return topLevelResourcePool;
}

void SceneEntityInstance::clearInstance() {
	
	resourcePools.clear();
	topLevelResourcePool = core->getResourceManager()->getGlobalPool();
	for(int i=0; i < children.size(); i++) {
		children[i]->setOwnsChildrenRecursive(true);
		delete children[i];
	}
	children.clear();
}

bool SceneEntityInstance::hasLayerID(unsigned char layerID) const {
	for(int i=0; i < layers.size(); i++) {
		if(layers[i]->layerID == layerID) {
			return true;
		}
	}
	return false;
}

SceneEntityInstanceLayer::SceneEntityInstanceLayer() : instance(NULL), visible(true) {
	
}

SceneEntityInstanceLayer::SceneEntityInstanceLayer(SceneEntityInstance *instance, String name) {
	this->instance = instance;
	this->name = name;
	visible = true;
}

void SceneEntityInstanceLayer::setLayerVisibility(bool val) {
	visible = val;
	
	std::vector<Entity*> entities = instance->getEntitiesByLayerID(layerID, true);
	for(int i=0; i < entities.size(); i++) {
		entities[i]->visible = val;
	}
}

SceneEntityInstanceLayer *SceneEntityInstance::createNewLayer(String name) {
	SceneEntityInstanceLayer *newLayer = new SceneEntityInstanceLayer(this, name);
	
	unsigned char layerID;
	for(layerID=0; layerID <= 255; layerID++) {
		if(!hasLayerID(layerID)) {
			break;
		}
	}
	newLayer->layerID = layerID;
	layers.push_back(newLayer);
	return newLayer;
}

unsigned int SceneEntityInstance::getNumLayers() const {
	return layers.size();
}

void SceneEntityInstance::removeLayer(SceneEntityInstanceLayer *layer) {
	for(int i=0; i < layers.size(); i++) {
		if(layers[i] == layer) {
			delete layers[i];
			layers.erase(layers.begin()+i);
		}
	}
}

SceneEntityInstanceLayer *SceneEntityInstance::getLayerAtIndex(unsigned int index) const {
	if(index < layers.size()) {
		return layers[index];
	} else {
		return NULL;
	}
}

bool SceneEntityInstance::loadFromFile(const String& fileName) {

	clearInstance();
	
	// NOCMAKE_TODO: fix this!
	resourceEntry->resourceFileTime = 0; //OSBasics::getFileTime(fileName);

	this->ownsChildren = true;
	this->fileName = fileName;
	Object loadObject;
	if(!loadObject.loadFromBinary(core, fileName)) {
		if(!loadObject.loadFromXML(core, fileName)) {
			Logger::log("Error loading entity instance.\n");
		}
	}
	
	int entityFileVersion = 1;
	
	ObjectEntry *versionObject = loadObject.root["version"];
	if(versionObject) {
		entityFileVersion = versionObject->intVal;
	}
	
	ObjectEntry *settings = loadObject.root["settings"];
	if(settings) {
		
		ObjectEntry *layersEntry = (*settings)["layers"];
		if(layersEntry) {
			for(int i=0; i < layersEntry->length; i++) {
				ObjectEntry *layer = (*layersEntry)[i];
				if(layer) {
					ObjectEntry *name = (*layer)["name"];
					ObjectEntry *layerID = (*layer)["id"];
					ObjectEntry *visible = (*layer)["visible"];
					if(name && layerID && visible) {
						SceneEntityInstanceLayer *newLayer = new SceneEntityInstanceLayer(this, name->stringVal);
						newLayer->visible = visible->boolVal;
						newLayer->layerID = (unsigned char) layerID->intVal;
						layers.push_back(newLayer);
					}
				}
			}
		}
		ObjectEntry *resPools = (*settings)["linkedResourcePools"];
		if(resPools) {
			for(int i=0; i < resPools->length; i++) {
				ObjectEntry *resPool = (*resPools)[i];
				if(resPool) {
					ObjectEntry *path = (*resPool)["path"];
					if(path) {
						ResourcePool *newPool = core->getResourceManager()->getResourcePoolByName(path->stringVal);
						
						if(!newPool) {
							
							String extension = path->stringVal.substr(path->stringVal.find_last_of(".")+1, path->stringVal.length());
							
							if(extension == "mat") {
								newPool = new ResourcePool(core, path->stringVal, core->getResourceManager()->getGlobalPool());
								newPool->deleteOnUnsubscribe = true;
								newPool->loadResourcesFromMaterialFile(path->stringVal);
							} else if( extension == "sprites") {
								SpriteSet *spriteSet = new SpriteSet(core, path->stringVal,  core->getResourceManager()->getGlobalPool());
								spriteSet->deleteOnUnsubscribe = true;
								newPool = spriteSet;
								
							}
							
							core->getResourceManager()->addResourcePool(newPool);
						}
						
						linkResourcePool(newPool);
					}
				}
			}
		}
	}
	
	ObjectEntry *root = loadObject.root["root"];
	if(root) {
		loadObjectEntryIntoEntity(root, this, entityFileVersion);
	}
	
	for(int i=0; i < layers.size(); i++ ) {
		SceneEntityInstanceLayer *layer = layers[i];
		if(layer->layerID != 0) {
			if(layer->visible == false) {
				std::vector<Entity*> layerEntities = getEntitiesByLayerID(layer->layerID, true);
				for(int j=0; j < layerEntities.size(); j++) {
					layerEntities[j]->visible = false;
				}
			}
		}
	}
	
	return true;
}
