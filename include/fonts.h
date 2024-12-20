#ifndef FONTS_H
#define FONTS_H
#include <glad.h>
#include <string>
#include <vector>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

inline FT_Library library = nullptr;

struct CharacterInfo {
    uint xOffset;
    uint yOffset;
    uint width;
    uint height;
};

struct FontAtlas {
    GLuint texture;
    GLuint glyphBuffer;
    float atlasWidth;
    float atlasHeight;
};

void initFonts();
void destroyFonts();
FT_Face loadFont(const unsigned char *source, int length);
void setFontSize(FT_Face face, int width, int height);
FontAtlas createFontTextureAtlas(const FT_Face face, const std::string &characters);

#endif //FONTS_H
