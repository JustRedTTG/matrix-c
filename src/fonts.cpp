#include "fonts.h"

#include <gl_errors.h>
#include <iostream>
#include <vector>

void initFonts() {
    if (library) {
        return;
    }
    if (const FT_Error error = FT_Init_FreeType(&library)) {
        std::cerr << "Error initializing FreeType library: " << error << std::endl;
        exit(1);
    }
}

void destroyFonts() {
    if (library) {
        FT_Done_FreeType(library);
        library = nullptr;
    }
}

FT_Face loadFont(const unsigned char *source, const int length) {
    FT_Face face;
    if (const FT_Error error =
            FT_New_Memory_Face(
                library,
                source, length,
                0,
                &face)
    ) {
        std::cerr << "Error initializing FreeType library: " << error << std::endl;
        exit(1);
    }
    return face;
}

std::pair<GLuint, GLuint> createFontTextureAtlas(const FT_Face face, const std::string &characters) {
    uint atlasWidth = 0;
    uint atlasHeight = 0;
    std::vector<FT_Bitmap> bitmaps;
    std::vector<CharacterInfo> characterInfoList;

    // Render each character to a bitmap and calculate the atlas size
    for (const char character: characters) {
        if (const FT_Error error = FT_Load_Char(face, character, FT_LOAD_RENDER)) {
            std::cerr << "Error loading character '" << character << "': " << error << std::endl;
            continue;
        }

        FT_Bitmap bitmap = face->glyph->bitmap;
        bitmaps.push_back(bitmap);
        characterInfoList.push_back({
            atlasWidth, static_cast<uint>(face->glyph->bitmap_top),
            bitmap.width, bitmap.rows
        });
        atlasWidth += bitmap.width;
        atlasHeight = std::max(atlasHeight, bitmap.rows);
    }

    // Create a texture atlas
    GLuint texture, glyphBuffer;
    GL_CHECK(glGenBuffers(1, &glyphBuffer));
    GL_CHECK(glBindBuffer(GL_TEXTURE_BUFFER, glyphBuffer));
    GL_CHECK(
        glBufferData(GL_TEXTURE_BUFFER, characterInfoList.size() * sizeof(CharacterInfo), characterInfoList.data(),
            GL_STATIC_DRAW));

    GL_CHECK(glGenTextures(1, &texture));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture));

    // Set texture parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Allocate memory for the texture atlas
    std::vector<unsigned char> atlasData(atlasWidth * atlasHeight, 0);

    // Copy the bitmaps into the texture atlas
    uint xOffset = 0;
    for (const FT_Bitmap &bitmap: bitmaps) {
        for (int y = 0; y < bitmap.rows; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                atlasData[(y * atlasWidth) + xOffset + x] = bitmap.buffer[y * bitmap.pitch + x];
            }
        }
        xOffset += bitmap.width;
    }

    // Upload the atlas data to the texture
    GL_CHECK(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        atlasWidth,
        atlasHeight,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        atlasData.data()
    ));

    // Unbind the texture
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    GLuint glyphMatrixBuffer;
    GL_CHECK(glGenBuffers(1, &glyphMatrixBuffer));
    GL_CHECK(glBindBuffer(GL_TEXTURE_BUFFER, glyphMatrixBuffer));
    GL_CHECK(glBufferData(GL_TEXTURE_BUFFER, characterInfoList.size() * sizeof(CharacterInfo), characterInfoList.data(),
        GL_STATIC_DRAW));

    // Now bind it as a texture
    GLuint glyphMatrixTexture;
    GL_CHECK(glGenTextures(1, &glyphMatrixTexture));
    GL_CHECK(glBindTexture(GL_TEXTURE_BUFFER, glyphMatrixTexture));
    GL_CHECK(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, glyphMatrixBuffer));  // Choose an appropriate internal format

    // Unbind the texture buffer
    GL_CHECK(glBindTexture(GL_TEXTURE_BUFFER, 0));

    return {texture, glyphMatrixTexture};
}
