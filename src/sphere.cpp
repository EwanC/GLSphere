// Copyright (c) 2025-2026 Ewan Crawford

// clang-format off
#include <GL/glew.h>
#include "sphere.h"
#include <cmath>
#include <stdexcept>
#include <string>

#include <stb_image.h>
// clang-format on

// From https://www.songho.ca/opengl/gl_sphere.html#sphere
void sphere::buildVertices() {
  const float SectorStep = 2 * M_PI / MNumSectors;
  const float StackStep = M_PI / MNumStacks;
  const float InvRadius = 1.0f / MRadius;
  for (unsigned StackIdx = 0; StackIdx <= MNumStacks; ++StackIdx) {
    const float StackAngle = (M_PI / 2) - (StackIdx * StackStep);
    const float XY = MRadius * cosf(StackAngle);
    const float Z = MRadius * sinf(StackAngle);

    for (unsigned SectorIdx = 0; SectorIdx <= MNumSectors; ++SectorIdx) {
      const float SectorAngle = SectorIdx * SectorStep;

      // Vertex position
      const float X = XY * cosf(SectorAngle); // r * cos(u) * cos(v)
      const float Y = XY * sinf(SectorAngle); // r * cos(u) * sin(v)
      MVertices.push_back(X);
      MVertices.push_back(Y);
      MVertices.push_back(Z);

      // normals
      const float NormalX = X * InvRadius;
      const float NormalY = Y * InvRadius;
      const float NormalZ = Z * InvRadius;
      MNormals.push_back(NormalX);
      MNormals.push_back(NormalY);
      MNormals.push_back(NormalZ);

      // Texture coordinates
      float U = (float)SectorIdx / MNumSectors;
      float V = (float)StackIdx / MNumStacks;
      MTexCoords.push_back(U);
      MTexCoords.push_back(V);
    }
  }

  // EBO Indices, for square
  // K1--K1+1
  // |  / |
  // | /  |
  // K2--K2+1
  for (unsigned StackIdx = 0; StackIdx < MNumStacks; ++StackIdx) {
    unsigned K1 = StackIdx * (MNumSectors + 1); // current stack
    unsigned K2 = K1 + MNumSectors + 1;         // next stack
    for (unsigned SectorIdx = 0; SectorIdx < MNumSectors;
         ++SectorIdx, ++K1, ++K2) {
      // 2 triangles per sector, excluding 1st and last stacks
      if (StackIdx != 0) {
        // K1---K2---K1+1
        MIndices.push_back(K1);
        MIndices.push_back(K2);
        MIndices.push_back(K1 + 1);
      }

      if (StackIdx != (MNumStacks - 1)) {
        // K1+1---K2---K2+1
        MIndices.push_back(K1 + 1);
        MIndices.push_back(K2);
        MIndices.push_back(K2 + 1);
      }
    }
  }

  // Transform north pole from being +Z to +Y
  float TransX[] = {1.0f, 0.0f, 0.0f};
  float TransY[] = {0.0f, 0.0f, -1.0f};
  float TransZ[] = {0.0f, 1.0f, 0.0f};
  for (size_t i = 0; i < MVertices.size(); i += 3) {
    float VX = MVertices[i];
    float VY = MVertices[i + 1];
    float VZ = MVertices[i + 2];
    MVertices[i] = TransX[0] * VX + TransY[0] * VY + TransZ[0] * VZ;
    MVertices[i + 1] = TransX[1] * VX + TransY[1] * VY + TransZ[1] * VZ;
    MVertices[i + 2] = TransX[2] * VX + TransY[2] * VY + TransZ[2] * VZ;

    float NX = MNormals[i];
    float NY = MNormals[i + 1];
    float NZ = MNormals[i + 2];

    MNormals[i] = TransX[0] * NX + TransY[0] * NY + TransZ[0] * NZ;
    MNormals[i + 1] = TransX[1] * NX + TransY[1] * NY + TransZ[1] * NZ;
    MNormals[i + 2] = TransX[2] * NX + TransY[2] * NY + TransZ[2] * NZ;
  }
}

unsigned int sphere::loadTexture() {
  int Width, Height, NrComponents;
  const char *SphereTexturePath = "textures/football.jpg";
  unsigned char *Data =
      stbi_load(SphereTexturePath, &Width, &Height, &NrComponents, 0);
  if (!Data) {
    throw std::runtime_error(std::string("Texture failed to load at path: ") +
                             std::string(SphereTexturePath));
  }

  unsigned int TextureID;
  glGenTextures(1, &TextureID);
  glBindTexture(GL_TEXTURE_2D, TextureID);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, Data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(Data);
  return TextureID;
}
