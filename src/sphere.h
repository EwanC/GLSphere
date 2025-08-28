// Copyright (c) 2025 Ewan Crawford
#pragma once

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct sphere {
  sphere(GLfloat Radius, unsigned NumSectors, unsigned NumStacks)
      : MRadius(Radius) {
    // Ensure at least 2 stacks and sectors
    MNumSectors = (2 > NumSectors) ? 2 : NumSectors;
    MNumStacks = (2 > NumStacks) ? 2 : NumStacks;

    buildVertices();
  }

  size_t getVertexSize() const { return sizeof(GLfloat) * MVertices.size(); }
  GLfloat *getVertexData() { return MVertices.data(); }

  size_t getIndexSize() const { return sizeof(unsigned) * MIndices.size(); }
  unsigned *getIndexData() { return MIndices.data(); }

  size_t getNormalSize() const { return sizeof(GLfloat) * MNormals.size(); }
  GLfloat *getNormalData() { return MNormals.data(); }

  size_t getTexCoordSize() const { return sizeof(GLfloat) * MTexCoords.size(); }
  GLfloat *getTexCoordData() { return MTexCoords.data(); }

  glm::mat4 getModelMatrix() const { return glm::mat4(1.f); }

  unsigned int loadTexture();

private:
  void buildVertices();

  GLfloat MRadius;
  unsigned MNumSectors;
  unsigned MNumStacks;

  std::vector<unsigned> MIndices; // EBO
  std::vector<GLfloat> MVertices;
  std::vector<GLfloat> MNormals;
  std::vector<GLfloat> MTexCoords;
};
