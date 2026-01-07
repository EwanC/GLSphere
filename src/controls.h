// Copyright (c) 2025-2026 Ewan Crawford
#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

struct controls {
  controls(GLFWwindow *Window, int WindowWidth, int WindowHeight);

  void refreshMatrices();
  glm::mat4 getViewMatrix() const { return MViewMatrix; }
  glm::mat4 getProjectionMatrix() const { return MProjMatrix; }
  glm::mat4 getMVPMatrix(glm::mat4 ModelMatrix,
                         bool RotationOnly = false) const {
    glm::mat4 View =
        RotationOnly ? glm::mat4(glm::mat3(MViewMatrix)) : MViewMatrix;
    return MProjMatrix * View * ModelMatrix;
  }

  std::string getPositionStr() const;

private:
  GLFWwindow *MWindow; // Non owning
  int MWindowWidth;
  int MWindowHeight;

  glm::mat4 MViewMatrix;
  glm::mat4 MProjMatrix;

  glm::vec3 MPosition;
  float MHorizAngle;
  float MVertAngle;

  double MLastTime;
};
