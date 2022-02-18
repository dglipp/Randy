#include <shared/internal/Bitmap.h>
#include <shared/internal/UtilsMath.h>

Bitmap::Bitmap(int w, int h, int comp, eBitmapFormat fmt)
    : w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w * h * comp * getBytesPerComponents(fmt))
{
    initGetSetFuncs();
}

Bitmap::Bitmap(int w, int h, int d, int comp, eBitmapFormat fmt)
    : w_(w), h_(h), d_(d), comp_(comp), fmt_(fmt), data_(w * h * d * comp * getBytesPerComponents(fmt))
{
    initGetSetFuncs();
}

Bitmap::Bitmap(int w, int h, int comp, eBitmapFormat fmt, const void *ptr)
    : w_(w), h_(h), comp_(comp), fmt_(fmt), data_(w * h * comp * getBytesPerComponents(fmt))
{
    initGetSetFuncs();
    std::memcpy(data_.data(), ptr, data_.size());
}

void Bitmap::setPixel(int x, int y, const vec4 &c)
{
    (*this.*setPixelFunc)(x, y, c);
}

vec4 Bitmap::getPixel(int x, int y) const
{
    return (*this.*getPixelFunc)(x, y);
}

void Bitmap::initGetSetFuncs()
{
    switch (fmt_)
    {
    case eBitmapFormat_UnsignedByte:
        setPixelFunc = &Bitmap::setPixelUnsignedByte;
        getPixelFunc = &Bitmap::getPixelUnsignedByte;
        break;

    case eBitmapFormat_Float:
        setPixelFunc = &Bitmap::setPixelFloat;
        getPixelFunc = &Bitmap::getPixelFloat;
        break;
    }
}

void Bitmap::setPixelFloat(int x, int y, const vec4 &c)
{
    const int offset = comp_ * (y * w_ + x);
    float *data = reinterpret_cast<float *>(data_.data());

    if (comp_ > 0)
        data[offset + 0] = c.x;
    if (comp_ > 1)
        data[offset + 1] = c.y;
    if (comp_ > 2)
        data[offset + 2] = c.z;
    if (comp_ > 3)
        data[offset + 3] = c.w;
}

vec4 Bitmap::getPixelFloat(int x, int y) const
{
    const int offset = comp_ * (y * w_ + x);
    const float *data = reinterpret_cast<const float *>(data_.data());
    return vec4(
        comp_ > 0 ? data[offset + 0] : 0.0f,
        comp_ > 1 ? data[offset + 1] : 0.0f,
        comp_ > 2 ? data[offset + 2] : 0.0f,
        comp_ > 3 ? data[offset + 3] : 0.0f);
}

void Bitmap::setPixelUnsignedByte(int x, int y, const vec4 &c)
{
    const int offset = comp_ * (y * w_ + x);
    if (comp_ > 0)
        data_[offset + 0] = uint8_t(c.x * 255.0f);
    if (comp_ > 1)
        data_[offset + 1] = uint8_t(c.y * 255.0f);
    if (comp_ > 2)
        data_[offset + 2] = uint8_t(c.z * 255.0f);
    if (comp_ > 3)
        data_[offset + 3] = uint8_t(c.w * 255.0f);
}

vec4 Bitmap::getPixelUnsignedByte(int x, int y) const
{
    const int offset = comp_ * (y * w_ + x);
    return vec4(
        comp_ > 0 ? float(data_[offset + 0]) / 255.0f : 0.0f,
        comp_ > 1 ? float(data_[offset + 1]) / 255.0f : 0.0f,
        comp_ > 2 ? float(data_[offset + 2]) / 255.0f : 0.0f,
        comp_ > 3 ? float(data_[offset + 3]) / 255.0f : 0.0f);
}

vec3 faceCoordsToXYZ(int i, int j, int faceID, int faceSize)
{
    const float A = 2.0f * float(i) / faceSize;
    const float B = 2.0f * float(j) / faceSize;

    if (faceID == 0)
        return vec3(-1.0f, A - 1.0f, B - 1.0f);
    if (faceID == 1)
        return vec3(A - 1.0f, -1.0f, 1.0f - B);
    if (faceID == 2)
        return vec3(1.0f, A - 1.0f, 1.0f - B);
    if (faceID == 3)
        return vec3(1.0f - A, 1.0f, 1.0f - B);
    if (faceID == 4)
        return vec3(B - 1.0f, A - 1.0f, 1.0f);
    if (faceID == 5)
        return vec3(1.0f - B, A - 1.0f, -1.0f);

    return vec3();
}

Bitmap convertEquirectangularMapToVerticalCross(const Bitmap &b)
{
    if (b.type_ != eBitmapType_2D)
        return Bitmap();

    const int faceSize = b.w_ / 4;

    const int w = faceSize * 3;
    const int h = faceSize * 4;

    Bitmap result(w, h, b.comp_, b.fmt_);

    const ivec2 kFaceOffsets[] =
        {
            ivec2(faceSize, faceSize * 3),
            ivec2(0, faceSize),
            ivec2(faceSize, faceSize),
            ivec2(faceSize * 2, faceSize),
            ivec2(faceSize, 0),
            ivec2(faceSize, faceSize * 2)};

    const int clampW = b.w_ - 1;
    const int clampH = b.h_ - 1;

    for (int face = 0; face != 6; face++)
    {
        for (int i = 0; i != faceSize; i++)
        {
            for (int j = 0; j != faceSize; j++)
            {
                const vec3 P = faceCoordsToXYZ(i, j, face, faceSize);
                const float R = hypot(P.x, P.y);
                const float theta = atan2(P.y, P.x);
                const float phi = atan2(P.z, R);
                //	float point source coordinates
                const float Uf = float(2.0f * faceSize * (theta + M_PI) / M_PI);
                const float Vf = float(2.0f * faceSize * (M_PI / 2.0f - phi) / M_PI);
                // 4-samples for bilinear interpolation
                const int U1 = clamp(int(floor(Uf)), 0, clampW);
                const int V1 = clamp(int(floor(Vf)), 0, clampH);
                const int U2 = clamp(U1 + 1, 0, clampW);
                const int V2 = clamp(V1 + 1, 0, clampH);
                // fractional part
                const float s = Uf - U1;
                const float t = Vf - V1;
                // fetch 4-samples
                const vec4 A = b.getPixel(U1, V1);
                const vec4 B = b.getPixel(U2, V1);
                const vec4 C = b.getPixel(U1, V2);
                const vec4 D = b.getPixel(U2, V2);
                // bilinear interpolation
                const vec4 color = A * (1 - s) * (1 - t) + B * (s) * (1 - t) + C * (1 - s) * t + D * (s) * (t);
                result.setPixel(i + kFaceOffsets[face].x, j + kFaceOffsets[face].y, color);
            }
        };
    }

    return result;
}

Bitmap convertVerticalCrossToCubeMapFaces(const Bitmap &b)
{
    /*
            ------
            | +Y |
     ----------------
     | -X | -Z | +X |
     ----------------
            | -Y |
            ------
            | +Z |
            ------
    */

    const int faceWidth = b.w_ / 3;
    const int faceHeight = b.h_ / 4;
    Bitmap cubemap(faceWidth, faceHeight, 6, b.comp_, b.fmt_);

    cubemap.type_ = eBitmapType_Cube;

    const uint8_t *src = b.data_.data();
    uint8_t *dst = cubemap.data_.data();

    const int pixelSize = cubemap.comp_ * Bitmap::getBytesPerComponents(cubemap.fmt_);

    for (int face = 0; face != 6; ++face)
    {
        for (int j = 0; j != faceHeight; ++j)
        {
            for (int i = 0; i != faceWidth; ++i)
            {
                int x = 0;
                int y = 0;

                switch (face)
                {
                // GL_TEXTURE_CUBEMAP_POSITIVE_X
                case 0:
                    x = i;
                    y = faceHeight + j;
                    break;

                // GL_TEXTURE_CUBEMAP_NEGATIVE_X
                case 1:
                    x = 2 * faceWidth + i;
                    y = faceHeight + j;
                    break;

                // GL_TEXTURE_CUBEMAP_POSITIVE_Y
                case 2:
                    x = 2 * faceWidth - (i + 1);
                    y = faceHeight - (j + 1);
                    break;

                // GL_TEXTURE_CUBEMAP_NEGATIVE_Y
                case 3:
                    x = 2 * faceWidth - (i + 1);
                    y = 3 * faceHeight - (j + 1);
                    break;

                // GL_TEXTURE_CUBEMAP_POSITIVE_Z
                case 4:
                    x = 2 * faceWidth - (i + 1);
                    y = b.h_ - (j + 1);
                    break;

                // GL_TEXTURE_CUBEMAP_NEGATIVE_Z
                case 5:
                    x = faceWidth + i;
                    y = faceHeight + j;
                    break;
                }

                std::memcpy(dst, src + (y * b.w_ + x) * pixelSize, pixelSize);
                dst += pixelSize;
            }
        }
    }
    return cubemap;
}
