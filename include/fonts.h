#ifndef FONTS_H
#define FONTS_H
#include <glad.h>
#include <vector>
#include <boost/mpl/int.hpp>

struct CharacterInfo {
    unsigned int xOffset;
    unsigned int yOffset;
    unsigned int width;
    unsigned int height;
};

struct FontInfo {
    const int width, height, size, characterCount;
    const CharacterInfo* characterInfoList;
};

struct FontAtlas {
    GLuint glyphTexture, glyphBuffer;
    float atlasWidth;
    float atlasHeight;

    void destroy() const;
};

FontAtlas *createFontTextureAtlas(const unsigned char *source, const FontInfo *fontInfo);

#endif //FONTS_H
