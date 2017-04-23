/*
 Copyright (C) 2015 by Ivan Safrin
 
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

#pragma once

#include "polycode/core/PolyMesh.h"
#include "polycode/core/PolyShader.h"
#include "polycode/core/PolyRectangle.h"
#include "polycode/core/PolyMaterial.h"
#include "polycode/core/PolyColor.h"
#include "polycode/core/PolyVector3.h"
#include <memory>

namespace Polycode {
	
	class RenderBuffer;
	class RenderFrame;
	
	class _PolyExport LightInfo {
		public:
			unsigned short importance;
			Vector3 position;
			Vector3 direction;
			unsigned short type;
			Color diffuseColor;
			Color specularColor;
			Number constantAttenuation;
			Number linearAttenuation;
			Number quadraticAttenuation;
			Number intensity;
			Number spotlightCutoff;
			Number spotlightExponent;
			bool shadowsEnabled;
			std::shared_ptr<Texture> shadowMapTexture;
			Matrix4 lightViewMatrix;
	};
	
	class _PolyExport GPUDrawOptions {
	public:
		bool depthTest;
		bool depthWrite;
		float linePointSize;
		bool backfaceCull;
		bool depthOnly;
		bool forceMaterial;
		unsigned int blendingMode;
		Polycode::Rectangle scissorBox;
		bool enableScissor;
		Color drawColor;
	};
	
	class _PolyExport GPUDrawCall {
	public:
		
		std::shared_ptr<MeshGeometry> submesh;
		GPUDrawOptions options;
		Matrix4 modelMatrix;
		std::shared_ptr<Material> material;
		std::vector<ShaderPass> shaderPasses;
	};
	
	class _PolyExport GPUDrawBuffer {
	public:
		GPUDrawBuffer();
		~GPUDrawBuffer();
		
		std::shared_ptr<RenderBuffer> targetFramebuffer;
		Matrix4 projectionMatrix;
		Matrix4 viewMatrix;
		Matrix4 cameraMatrix;
		Color clearColor;
		bool clearDepthBuffer;
		bool clearColorBuffer;
		Vector2 backingResolutionScale;
		std::shared_ptr<Material> globalMaterial;
		
		std::vector<LightInfo> lights;
		
		Polycode::Rectangle viewport;
		std::vector<GPUDrawCall> drawCalls;
		RenderFrame *renderFrame;
        bool shadowMapPass;
        void *userData;
	};
}