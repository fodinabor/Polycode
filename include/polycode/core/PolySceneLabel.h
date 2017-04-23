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
 
#pragma once
#include "polycode/core/PolyGlobals.h"
#include "polycode/core/PolyScenePrimitive.h"
#include <memory>

namespace Polycode {
	
	class Label;
	class Mesh;
	class Texture;
	class Material;
	class ShaderBinding;

	/**
	* 3D text label. Creates a 3D text label.
	*/
	class _PolyExport SceneLabel : public ScenePrimitive {
		public:
		
			SceneLabel(std::shared_ptr<Material> material, const String& text, int size, std::shared_ptr<Font> font, int amode = 0, Number actualHeight = 0.0);
			
			SceneLabel(std::shared_ptr<Material> material, const String& text, int size, std::shared_ptr<Font> font, int amode, Number actualHeight, bool premultiplyAlpha, const Color &backgroundColor, const Color &foregroundColor);
			
			String getText();
		
			void setLabelActualHeight(Number actualHeight);
			Number getLabelActualHeight();
			
			void Render(GPUDrawBuffer *buffer);

			int getTextWidthForString(String text);
		
			virtual ~SceneLabel();
		
			/**
			* Sets new text for the labe.
			* @param newText New text to display.
			*/ 
			void setText(const String& newText);
		
			virtual Entity *Clone(bool deepClone, bool ignoreEditorOnly) const;
			virtual void applyClone(Entity *clone, bool deepClone, bool ignoreEditorOnly) const;
			
			void updateFromLabel();
		
			Label *getLabel();
			
			bool positionAtBaseline;
			
			static Vector3 defaultAnchor;
			static bool defaultPositionAtBaseline;
			static bool defaultSnapToPixels;
			static bool createMipmapsForLabels;
						
		protected:
			
			std::shared_ptr<Texture> texture;
			Number actualHeight;
			Number labelScale;
			Label *label;
	};
}
