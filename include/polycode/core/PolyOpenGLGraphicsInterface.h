	
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

#include "polycode/core/PolyGlobals.h"
#include "polycode/core/PolyRenderer.h"
#include "polycode/core/PolyTexture.h"
#include "polycode/core/PolyMesh.h"
#include "polycode/core/PolyCore.h"

#if PLATFORM == PLATFORM_MAC
	
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>

#elif PLATFORM == PLATFORM_IOS

	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>

#elif PLATFORM == PLATFORM_WINDOWS

	#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
		#define STRICT_OPENGLES2
		#include <angle/GLES2/gl2.h>
		#include <angle/GLES2/gl2ext.h>
		#include <angle/EGL/egl.h>
		#include <angle/EGL/eglext.h>
		#include <angle/EGL/eglplatform.h>
		#include <angle/angle_windowsstore.h>
	#else
		#include <glew/GL/glew.h>
	#endif
#else	
	#if defined(USE_EGL)
		#include <EGL/egl.h>
		#include <EGL/eglext.h>
		#include <GLES2/gl2.h>
		#include <GLES2/gl2ext.h>
	#else
		#ifdef EMSCRIPTEN
			#include "SDL/SDL_opengl.h"
		#else
			#include <glew/GL/glew.h>
		#endif
		/*#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glext.h>*/
	#endif
#endif

namespace Polycode {
	
	class ShaderPlatformData {
		public:
			GLuint shaderID;
			GLuint vertexProgramID;
			GLuint fragmentProgramID;
			std::vector<GLuint*> expectedData;
	};
	
	class _PolyExport OpenGLGraphicsInterface : public GraphicsInterface {
	public:
		
		OpenGLGraphicsInterface(Core *core);
		~OpenGLGraphicsInterface();

		// implementation
		
		void createTexture(Texture *texture);
		
		void setViewport(unsigned int x,unsigned  int y,unsigned  int width, unsigned height);
		void clearBuffers(const Color &clearColor, bool colorBuffer, bool depthBuffer, bool stencilBuffer);
		void setParamInShader(Shader *shader, ProgramParam *param, LocalShaderParam *localParam);
		void setAttributeInShader(Shader *shader, ProgramAttribute *attribute, AttributeBinding *attributeBinding);
		void disableAttribute(Shader *shader, const ProgramAttribute &attribute);
		void useShader(Shader *shader);
		
		void createProgram(ShaderProgram *program);
		void createShader(Shader *shader);
		
		void beginDrawCall();
		void endDrawCall();
		void setBlendingMode(unsigned int blendingMode);
		
		void createRenderBuffer(RenderBuffer *renderBuffer);
		void bindRenderBuffer(RenderBuffer *renderBuffer);
		
		void createSubmeshBuffers(MeshGeometry *submesh);
		void drawSubmeshBuffers(MeshGeometry *submesh, Shader *shader);
		
		void destroySubmeshBufferData(void *platformData);
		void destroyProgramData(void *platformData);
		void destroyShaderData(void *platformData);
		void destroyTextureData(void *platformData);
		void destroyRenderBufferData(void *platformData);
		
		void drawIndices(int type, IndexDataArray *indexArray);
		void drawArrays(int type, unsigned int vertexCount);
		
		void enableDepthTest(bool val);
		void enableDepthWrite(bool val);
		void enableBackfaceCulling(bool val);
		void setLineSize(Number lineSize);
		
		void setWireframeMode(bool val);
		
		void enableScissor(bool val);
		void setScissorBox(const Polycode::Rectangle &box);
		
		bool lineSmooth;
		
	protected:
		
        Core *core;
		GLuint currentShaderID;
		int textureIndex;
		
		static int mapNameToArrayType(const String &name);
		static GLenum getGLDrawMode(int polycodeMode);
		static int getPolycodeParamType(int glType);
		static int getAttributeSize(int glType);
		void setUniformMatrix(GLint paramLocation, const Polycode::Matrix4& matrix);
		void createVertexBuffer(VertexDataArray *array);
		void createIndexBuffer(IndexDataArray *array);
		void destroyBuffer(RenderDataArray *array);
	};
}

