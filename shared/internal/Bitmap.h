#pragma once

#include <vector>
#include <cstring>

#include <glm/glm.hpp>


enum eBitmapType
{
	eBitmapType_2D,
	eBitmapType_Cube
};

enum eBitmapFormat
{
	eBitmapFormat_UnsignedByte,
	eBitmapFormat_Float,
};

using glm::ivec2;
using glm::vec3;
using glm::vec4;

class Bitmap
{
    public:
        Bitmap() = default;
        Bitmap(int w, int h, int comp, eBitmapFormat fmt);
        Bitmap(int w, int h, int d, int comp, eBitmapFormat fmt);
        Bitmap(int w, int h, int comp, eBitmapFormat fmt, const void * ptr);

        static int getBytesPerComponents(eBitmapFormat fmt)
        {
            if(fmt == eBitmapFormat_UnsignedByte)
                return 1;
            if(fmt == eBitmapFormat_Float)
                return 4;
            return 0;
        }

        void setPixel(int x, int y, const vec4 &c);
        vec4 getPixel(int x, int y) const;

        int w_ = 0;
        int h_ = 0;
        int d_ = 1;
        int comp_ = 3;

        eBitmapFormat fmt_ = eBitmapFormat_UnsignedByte;
        eBitmapType type_ = eBitmapType_2D;
        std::vector<uint8_t> data_;

    private:
        using setPixel_t = void(Bitmap::*)(int, int, const vec4&);
	    using getPixel_t = vec4(Bitmap::*)(int, int) const;
	    setPixel_t setPixelFunc = &Bitmap::setPixelUnsignedByte;
	    getPixel_t getPixelFunc = &Bitmap::getPixelUnsignedByte;

        void initGetSetFuncs();

        void setPixelUnsignedByte(int x, int y, const vec4 &c);
        vec4 getPixelUnsignedByte(int x, int y) const;

        void setPixelFloat(int x, int y, const vec4 &c);
        vec4 getPixelFloat(int x, int y) const;
};

vec3 faceCoordsToXYZ(int i, int j, int faceID, int faceSize);
Bitmap convertEquirectangularMapToVerticalCross(const Bitmap &b);
Bitmap convertVerticalCrossToCubeMapFaces(const Bitmap &b);