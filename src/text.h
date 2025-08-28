// Copyright (c) 2025 Ewan Crawford
#pragma once

// clang-format off
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
// clang-format on

#include <ft2build.h>
#include FT_FREETYPE_H

struct charInfo {
  unsigned int TextureID; // ID handle of the glyph texture
  glm::ivec2 Size;        // Size of glyph
  glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Horizontal offset to advance to next glyph
};

struct text {
  text();
  ~text();

  void render(GLuint VBO, std::string Text, float X, float Y, float Scale);
  void freeTextures();

private:
  std::unordered_map<GLchar, charInfo> MCharMap;
  FT_Library MFreeType;
  FT_Face MFontFace;
};
