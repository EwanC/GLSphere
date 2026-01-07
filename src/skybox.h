// Copyright (c) 2025-2026 Ewan Crawford
#pragma once

#include <array>
#include <glm/glm.hpp>

struct skybox {
  skybox();

  float *getVertices() { return MVertices.data(); }
  unsigned getVerticesSize() const { return MNumVertices * sizeof(float); }
  unsigned *getIndexData() { return MIndices.data(); }
  unsigned getIndexSize() const { return MNumIndices * sizeof(unsigned); }
  glm::mat4 getModelMatrix() const;

  static unsigned int loadCubemap();

  static constexpr unsigned MNumIndices = 12 * 3;
  static constexpr unsigned MNumVertices = 9 * 3;

private:
  std::array<float, MNumVertices> MVertices;
  std::array<unsigned, MNumIndices> MIndices;
};
