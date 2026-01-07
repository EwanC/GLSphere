// Copyright (c) 2025-2026 Ewan Crawford

#include "text.h"
#include <stdexcept>

text::text() {
  if (FT_Init_FreeType(&MFreeType)) {
    throw std::runtime_error("Error: Could not init FreeType Library");
  }

  const char *FontName = "fonts/FreeSans.ttf";
  if (FT_New_Face(MFreeType, FontName, 0, &MFontFace)) {
    throw std::runtime_error(std::string("Error: Failed to load font ") +
                             FontName);
  }

  FT_Set_Pixel_Sizes(MFontFace, 0 /* width- default */, 48 /* height */);

  // disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load first 128 characters of ASCII set
  for (unsigned char Char = 0; Char < 128; Char++) {
    if (FT_Load_Char(MFontFace, Char, FT_LOAD_RENDER)) {
      throw std::runtime_error(std::string("Error: Failed to glyph ") += Char);
    }

    unsigned int CharTexture;
    glGenTextures(1, &CharTexture);
    glBindTexture(GL_TEXTURE_2D, CharTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, MFontFace->glyph->bitmap.width,
                 MFontFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 MFontFace->glyph->bitmap.buffer);

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // now store character for later use
    charInfo CharInfo = {
        CharTexture,
        glm::ivec2(MFontFace->glyph->bitmap.width,
                   MFontFace->glyph->bitmap.rows),
        glm::ivec2(MFontFace->glyph->bitmap_left, MFontFace->glyph->bitmap_top),
        static_cast<unsigned int>(MFontFace->glyph->advance.x)};
    MCharMap.insert(std::pair<char, charInfo>(Char, CharInfo));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

text::~text() {
  freeTextures();

  FT_Done_Face(MFontFace);
  FT_Done_FreeType(MFreeType);
}

void text::freeTextures() {
  for (auto &Info : MCharMap) {
    charInfo &I = Info.second;
    if (I.TextureID != 0) {
      glDeleteTextures(1, &I.TextureID);
      I.TextureID = 0;
    }
  }
}

void text::render(GLuint VBO, std::string Text, float X, float Y, float Scale) {
  for (auto Char : Text) {
    charInfo I = MCharMap[Char];

    float XPos = X + I.Bearing.x * Scale;
    float YPos = Y - (I.Size.y - I.Bearing.y) * Scale;
    float Width = I.Size.x * Scale;
    float Height = I.Size.y * Scale;

    float Vertices[6][4] = {{XPos, YPos + Height, 0.0f, 0.0f},
                            {XPos, YPos, 0.0f, 1.0f},
                            {XPos + Width, YPos, 1.0f, 1.0f},

                            {XPos, YPos + Height, 0.0f, 0.0f},
                            {XPos + Width, YPos, 1.0f, 1.0f},
                            {XPos + Width, YPos + Height, 1.0f, 0.0f}};

    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, I.TextureID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64
    // pixels) bitshift by 6 to get value in pixels 2^6 = 64 (divide amount of
    // 1/64th pixels by 64 to get amount of pixels)
    X += (I.Advance >> 6) * Scale;
  }
}
