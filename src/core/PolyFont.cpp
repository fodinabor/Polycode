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

#include "polycode/core/PolyFont.h"
#include "polycode/core/PolyLogger.h"
#include "polycode/core/PolyCore.h"

using namespace Polycode;

Font::Font() : Resource(Resource::RESOURCE_FONT) {
    
}

Font::Font(Core *core, const String& fileName, FT_Library FTLibrary) : Resource(Resource::RESOURCE_FONT) {
	this->fileName = fileName;
	
	loaded = false;
	buffer = NULL;
	CoreFile *file = core->openFile(fileName, "rb");
	if(file) {
		file->seek(0, SEEK_END);
		long progsize = file->tell();
		file->seek(0, SEEK_SET);
		buffer = (unsigned char*)malloc(progsize);
		memset(buffer, 0, progsize);
		file->read(buffer, progsize, 1);
		
		core->closeFile(file);
		valid = true;
		if(FT_New_Memory_Face(FTLibrary, buffer, progsize, 0, &ftFace) != 0) {
			Logger::log("Error loading font %s\n", fileName.c_str());
			valid = false;
		}
	
		if(FT_Select_Charmap(ftFace, ft_encoding_unicode) != 0) {
			printf("Warning font does not support unicode (%s)\n", fileName.c_str());
		}
		loaded = true;
	} else {
		Logger::log("Invalid font file specified (%s)\n", fileName.c_str());	
	}
	
	setResourcePath(fileName);
}

String Font::getFontPath() {
	return	fileName;
}

void Font::setFontName(String fontName) {
	setResourceName(fontName);
}

String Font::getFontName() {
	return getResourceName();
}

bool Font::isValid() const {
	return valid;
}

Font::~Font() {
	if(buffer) {
		free(buffer);
	}
}

FT_Face Font::getFace() {
	return ftFace;
}
