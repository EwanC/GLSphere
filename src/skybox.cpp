// Copyright (c) 2025-2026 Ewan Crawford

#include "skybox.h"
#include <GL/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <string>

#include <stb_image.h>

skybox::skybox() {
  // clang-format off
  MVertices = std::array<float, MNumVertices> {
      -1.0f, 1.0f,  -1.0f, // v0
      -1.0f, -1.0f, -1.0f, // v1
      1.0f,  -1.0f, -1.0f, // v2
      1.0f,  1.0f,  -1.0f, // v3
      -1.0f, -1.0f, 1.0f,  // v4
      -1.0f, -1.0f, -1.0f, // v5
      -1.0f, 1.0f,  1.0f,  // v6
      1.0f,  -1.0f, 1.0f,  // v7
      1.0f,  1.0f,  1.0f,  // v8
  };

  MIndices = std::array<unsigned, MNumIndices> {
    0, 1, 2, // T1
    2, 3, 0, // T2
    4, 5, 0, // T3
    0, 6, 4, // T4
    2, 7, 8, // T5
    8, 3, 2, // T6
    4, 6, 8, // T7
    8, 7, 4, // T8
    0, 3, 8, // T9
    8, 6, 0, // T10
    1, 4, 2, // T11
    2, 4, 7, // T12
  };
  // clang-format on
}

unsigned int skybox::loadCubemap() {
  // Loads a cubemap texture from 6 individual texture images
  // +X (right)
  // -X (left)
  // +Y (top)
  // -Y (bottom)
  // +Z (front)
  // -Z (back)

  constexpr unsigned CubeFaces = 6;
  std::string Faces[CubeFaces] = {
      "textures/skybox_px.png", "textures/skybox_nx.png",
      "textures/skybox_py.png", "textures/skybox_ny.png",
      "textures/skybox_pz.png", "textures/skybox_nz.png"};

  unsigned int TextureID;
  glGenTextures(1, &TextureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

  for (unsigned int i = 0; i < CubeFaces; i++) {
    int Width, Height, NrComponents;
    unsigned char *Data =
        stbi_load(Faces[i].c_str(), &Width, &Height, &NrComponents, 0);
    if (!Data) {
      throw std::runtime_error(std::string("Texture failed to load at path: ") +
                               Faces[i]);
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, Width, Height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
    stbi_image_free(Data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return TextureID;
}

glm::mat4 skybox::getModelMatrix() const {
  // Scale cube by a factor of 5
  return glm::scale(glm::mat4(1.f), glm::vec3(5.f, 5.f, 5.f));
}
