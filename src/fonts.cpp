#include "fonts.h"

#include <gl_errors.h>
#include <iostream>
#include <utf8cpp/utf8.h>
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

void setFontSize(const FT_Face face, const int width, const int height) {
    if (const FT_Error error = FT_Set_Pixel_Sizes(face, width, height)) {
        std::cerr << "Error setting font size: " << error << std::endl;
        exit(1);
    }
}

FontAtlas createFontTextureAtlas(const FT_Face face, const std::string &characters) {
    uint atlasWidth = 0;
    uint atlasHeight = 0;
    std::vector<FT_Bitmap> bitmaps;
    std::vector<CharacterInfo> characterInfoList;

    // Render each character to a bitmap and calculate the atlas size
    for (auto it = utf8::iterator<std::string::const_iterator>(characters.begin(), characters.begin(), characters.end());
         it != utf8::iterator<std::string::const_iterator>(characters.end(), characters.begin(), characters.end()); ++it) {
        char32_t character = *it;
        if (const FT_Error error = FT_Load_Char(face, character, FT_LOAD_RENDER)) {
            std::cerr << "Error loading character '" << static_cast<uint32_t>(character) << "': " << error << std::endl;
            continue;
        }

        FT_Bitmap bitmap = face->glyph->bitmap;
        bitmaps.push_back(bitmap);
        characterInfoList.push_back({
            atlasWidth, 0,
            bitmap.width, bitmap.rows
        });
        std::cout << "Character: " << static_cast<uint32_t>(character) << " X: " << atlasWidth << " Y: " << 0 <<
                " Width: " << bitmap.width << " Height: " << bitmap.rows << std::endl;
        atlasWidth += bitmap.width;
        atlasHeight = std::max(atlasHeight, bitmap.rows);
    }

    // Create a texture atlas
    GLuint texture, glyphBuffer;
    GL_CHECK(glGenBuffers(1, &glyphBuffer));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, glyphBuffer));
    GL_CHECK(
        glBufferData(GL_UNIFORM_BUFFER, characterInfoList.size() * sizeof(CharacterInfo), characterInfoList.data(),
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
    for (size_t i = 0; i < bitmaps.size(); ++i) {
        const FT_Bitmap &bitmap = bitmaps[i];
        uint yOffset = atlasHeight - bitmap.rows; // Align to bottom

        for (int y = 0; y < bitmap.rows; ++y) {
            for (int x = 0; x < bitmap.width; ++x) {
                // Calculate source and destination indices
                int srcIndex = y * bitmap.pitch + x;
                int dstIndex = ((y + yOffset) * atlasWidth) + xOffset + x;

                // Debugging: Print values to verify alignment
                if (srcIndex >= bitmap.pitch * bitmap.rows) {
                    std::cerr << "Source index out of bounds for char: " << i
                              << " srcIndex=" << srcIndex << std::endl;
                    continue;
                }
                if (dstIndex >= static_cast<int>(atlasData.size())) {
                    std::cerr << "Destination index out of bounds for char: " << i
                              << " dstIndex=" << dstIndex << std::endl;
                    continue;
                }

                // Copy the pixel
                atlasData[dstIndex] = bitmap.buffer[srcIndex];
            }
        }

        std::cout << "Char: " << i << " packed at xOffset=" << xOffset
                  << ", yOffset=" << yOffset << ", width=" << bitmap.width
                  << ", height=" << bitmap.rows << std::endl;

        xOffset += bitmap.width; // Move to the next character's position
    }

    // Upload the atlas data to the texture
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
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
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));


    return {texture, glyphBuffer, static_cast<float>(atlasWidth), static_cast<float>(atlasHeight)};
}
