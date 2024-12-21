#ifndef FONTS_H
#define FONTS_H
#include <glad.h>
#include <vector>
#include <boost/mpl/int.hpp>

struct CharacterInfo {
    uint xOffset;
    uint yOffset;
    uint width;
    uint height;
};

struct FontInfo {
    int width, height, size, characterCount;
    CharacterInfo characterInfoList[];
};

struct FontAtlas {
    GLuint glyphTexture, glyphBuffer;
    float atlasWidth;
    float atlasHeight;

    void destroy() const;
};

FontAtlas *createFontTextureAtlas(const unsigned char *source, const FontInfo *fontInfo);

#endif //FONTS_H
