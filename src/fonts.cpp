#include "fonts.h"

#include <gl_errors.h>
#include <iostream>
#include <vector>

void FontAtlas::destroy() const {
    GL_CHECK(glDeleteBuffers(1, &glyphBuffer));
    GL_CHECK(glDeleteTextures(1, &glyphTexture));
}

FontAtlas *createFontTextureAtlas(const unsigned char *source, const FontInfo *fontInfo) {
    // Create a texture atlas
    GLuint glyphTexture, glyphBuffer;
    GL_CHECK(glGenBuffers(1, &glyphBuffer));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, glyphBuffer));
    GL_CHECK(
        glBufferData(GL_UNIFORM_BUFFER, fontInfo->characterCount * sizeof(CharacterInfo), fontInfo->characterInfoList,
            GL_STATIC_DRAW));

    GL_CHECK(glGenTextures(1, &glyphTexture));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, glyphTexture));

    // Set texture parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Upload the atlas data to the texture
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        fontInfo->width,
        fontInfo->height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        source
    ));

    // Unbind the texture
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));


    return new FontAtlas{glyphTexture, glyphBuffer, static_cast<float>(fontInfo->width), static_cast<float>(fontInfo->height)};
}
