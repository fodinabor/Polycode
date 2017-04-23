class "Image"

Image.IMAGE_RGB = 0
Image.IMAGE_RGBA = 1
Image.IMAGE_FP16 = 2

function Image:createEmpty(width, height, fillColor)
	local retVal = Polycode.Image_createEmpty(self.__ptr, width, height, fillColor.__ptr)
end

function Image:fill(color)
	local retVal = Polycode.Image_fill(self.__ptr, color.__ptr)
end

function Image:setPixel(x, y, r, g, b, a)
	local retVal = Polycode.Image_setPixel(self.__ptr, x, y, r, g, b, a)
end

function Image:getPixel(x, y)
	local retVal = Polycode.Image_getPixel(self.__ptr, x, y)
	if retVal == nil then return nil end
	local __c = _G["Color"]("__skip_ptr__")
	__c.__ptr = retVal
	return __c
end

function Image:drawLine(x0, y0, x1, y1, col)
	local retVal = Polycode.Image_drawLine(self.__ptr, x0, y0, x1, y1, col.__ptr)
end

function Image:moveBrushTo(x, y)
	local retVal = Polycode.Image_moveBrushTo(self.__ptr, x, y)
end

function Image:moveBrush(x, y)
	local retVal = Polycode.Image_moveBrush(self.__ptr, x, y)
end

function Image:drawLineTo(x, y, col)
	local retVal = Polycode.Image_drawLineTo(self.__ptr, x, y, col.__ptr)
end

function Image:fillRect(x, y, w, h, col)
	local retVal = Polycode.Image_fillRect(self.__ptr, x, y, w, h, col.__ptr)
end

function Image:perlinNoise(seed, alpha)
	local retVal = Polycode.Image_perlinNoise(self.__ptr, seed, alpha)
end

function Image:fastBlur(blurSize)
	local retVal = Polycode.Image_fastBlur(self.__ptr, blurSize)
end

function Image:fastBlurVert(blurSize)
	local retVal = Polycode.Image_fastBlurVert(self.__ptr, blurSize)
end

function Image:fastBlurHor(blurSize)
	local retVal = Polycode.Image_fastBlurHor(self.__ptr, blurSize)
end

function Image:getBrushX()
	local retVal =  Polycode.Image_getBrushX(self.__ptr)
	return retVal
end

function Image:getBrushY()
	local retVal =  Polycode.Image_getBrushY(self.__ptr)
	return retVal
end

function Image:isLoaded()
	local retVal =  Polycode.Image_isLoaded(self.__ptr)
	return retVal
end

function Image:getType()
	local retVal =  Polycode.Image_getType(self.__ptr)
	return retVal
end

function Image:getWidth()
	local retVal =  Polycode.Image_getWidth(self.__ptr)
	return retVal
end

function Image:getHeight()
	local retVal =  Polycode.Image_getHeight(self.__ptr)
	return retVal
end

function Image:premultiplyAlpha()
	local retVal =  Polycode.Image_premultiplyAlpha(self.__ptr)
end

function Image:__delete()
	if self then Polycode.delete_Image(self.__ptr) end
end
