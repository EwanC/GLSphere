// Copyright (c) 2025 Ewan Crawford
#include "controls.h"
#include <glm/gtc/matrix_transform.hpp>

#include <sstream>

controls::controls(GLFWwindow *Window, int WindowWidth, int WindowHeight)
    : MWindow(Window), MWindowWidth(WindowWidth), MWindowHeight(WindowHeight),
      MLastTime(0.0) {
  // Initial position +Z
  MPosition = glm::vec3(0, 0, 3);

  // Initial horizontal angle, towards -z
  MHorizAngle = 3.14f;

  // Initial vertical angle : none
  MVertAngle = 0.0f;
}

void controls::refreshMatrices() {
  const float FoV = 45.0f;  // Field of view
  const float Speed = 3.0f; // 3 units / second
  const float MouseSpeed = 0.005f;

  if (MLastTime == 0.0) {
    MLastTime = glfwGetTime();
    return;
  }

  // Compute time difference between current and last frame
  double CurrentTime = glfwGetTime();
  float DeltaTime = float(CurrentTime - MLastTime);

  // Get mouse position
  double XPos, YPos;
  glfwGetCursorPos(MWindow, &XPos, &YPos);

  // Reset mouse position for next frame
  glfwSetCursorPos(MWindow, MWindowWidth / 2, MWindowHeight / 2);

  // Compute new orientation
  if (XPos != 0) {
    MHorizAngle += MouseSpeed * float(MWindowWidth / 2 - XPos);
  }
  if (YPos != 0) {
    MVertAngle += MouseSpeed * float(MWindowHeight / 2 - YPos);
  }

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 Direction(cos(MVertAngle) * sin(MHorizAngle), sin(MVertAngle),
                      cos(MVertAngle) * cos(MHorizAngle));

  // Right vector
  glm::vec3 Right = glm::vec3(sin(MHorizAngle - 3.14f / 2.0f), 0,
                              cos(MHorizAngle - 3.14f / 2.0f));

  // Up vector
  glm::vec3 Up = glm::cross(Right, Direction);

  // Move forward
  if (glfwGetKey(MWindow, GLFW_KEY_UP) == GLFW_PRESS) {
    MPosition += Direction * DeltaTime * Speed;
  }
  // Move backward
  if (glfwGetKey(MWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
    MPosition -= Direction * DeltaTime * Speed;
  }
  // Strafe right
  if (glfwGetKey(MWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    MPosition += Right * DeltaTime * Speed;
  }
  // Strafe left
  if (glfwGetKey(MWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
    MPosition -= Right * DeltaTime * Speed;
  }

  // Limit camera from moving outside of the skybox
  MPosition = glm::clamp(MPosition, glm::vec3(-4.f, -4.f, -4.f),
                         glm::vec3(4.f, 4.f, 4.f));

  // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit
  // <-> 100 units
  MProjMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
  // Camera matrix
  MViewMatrix = glm::lookAt(
      MPosition, // Camera is here
      MPosition +
          Direction, // and looks here : at the same position, plus "direction"
      Up             // Head is up (set to 0,-1,0 to look upside-down)
  );

  // For the next frame, the "last time" will be "now"
  MLastTime = CurrentTime;
}

std::string controls::getPositionStr() const {
  std::stringstream sstr;
  sstr << "Camera Position (" << MPosition.x << ", " << MPosition.y << ", "
       << MPosition.z << ")";
  return sstr.str();
}
