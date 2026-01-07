// Copyright (c) 2025-2026 Ewan Crawford

// clang-format off
#include "shaders.h"
#include "sphere.h"
#include "controls.h"
#include "skybox.h"
#include "text.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
// clang-format on

void printUsage(std::string Name) {
  std::cout << "Usage: " << Name << std::endl
            << "OpenGL implementation of a sphere in a skybox." << std::endl
            << "Use arrow keys to move camera and mouse to look around. "
            << "Press escape key to exit." << std::endl
            << "Options:" << std::endl
            << "\t-h, --help\t\tShow this help message" << std::endl
            << "\t--stacks N \t\tNumber of stacks in sphere, defaults to 18"
            << std::endl
            << "\t--sectors N \t\tNumber of sectors in sphere, defaults to 36"
            << std::endl;
}

int parseCLI(int argc, char *argv[], unsigned &Sectors, unsigned &Stacks) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      printUsage(argv[0]);
      return 0;
    } else if (arg == "--stacks") {
      if (i + 1 < argc) {
        i++;
        Stacks = std::atoi(argv[i]);
      } else {
        std::cout << "Error: --stacks CLI requires an argument" << std::endl;
        return -1;
      }
    } else if (arg == "--sectors") {
      if (i + 1 < argc) {
        i++;
        Sectors = std::atoi(argv[i]);
      } else {
        std::cout << "Error: --sectors CLI requires an argument" << std::endl;
        printUsage(argv[0]);
        return -1;
      }
    } else {
      std::cout << "Error: Unknown CLI argument \"" << arg << "\"" << std::endl;
      printUsage(argv[0]);
      return -1;
    }
  }
  return 1;
}

void APIENTRY glDebugOutput(GLenum Source, GLenum Type, unsigned int ID,
                            GLenum Severity, GLsizei Length,
                            const char *Message, const void *UserParam) {
  (void)UserParam;
  std::cerr << "---------------" << std::endl;
  std::cerr << "GL Debug Message (" << ID << "): " << Message << std::endl;

  switch (Source) {
  case GL_DEBUG_SOURCE_API:
    std::cerr << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cerr << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cerr << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cerr << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cerr << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cerr << "Source: Other";
    break;
  }
  std::cerr << std::endl;

  switch (Type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cerr << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cerr << "Type: Deprecated Behavior";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cerr << "Type: Undefined Behavior";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cerr << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cerr << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cerr << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cerr << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cerr << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cerr << "Type: Other";
    break;
  }
  std::cerr << std::endl;

  switch (Severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cerr << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cerr << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cerr << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cerr << "Severity: notification";
    break;
  }
  std::cerr << std::endl;
  std::cerr << std::endl;
}

int main(int argc, char *argv[]) {
  // Default number of Stacks and Sectors
  unsigned Sectors = 36;
  unsigned Stacks = 18;

  if (int Ret = parseCLI(argc, argv, Sectors, Stacks); Ret < 1) {
    return Ret;
  }

  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  // 4.3 so we can use debug context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

  // Open a window and create its OpenGL context
  const int WindowWidth = 1024;
  const int WindowHeight = 768;
  GLFWwindow *Window =
      glfwCreateWindow(WindowWidth, WindowHeight, "GLSphere", NULL, NULL);
  if (!Window) {
    std::cerr << "Failed to open GLFW window." << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(Window);

  // Initialize GLEW
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    glfwTerminate();
    return -1;
  }

#ifndef NDEBUG
  std::cout << "GL version " << glGetString(GL_VERSION) << std::endl;
#endif

  int ContextFlags = 0;
  glGetIntegerv(GL_CONTEXT_FLAGS, &ContextFlags);
  if (ContextFlags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    /// Enable debug output and invoke callback to moment an
    /// error occurs
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // Enable callback for all messages
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }

  // Ensure we can capture the escape key being pressed to exit
  glfwSetInputMode(Window, GLFW_STICKY_KEYS, GL_TRUE);
  // Hide the mouse and enable unlimited movement
  glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set the mouse at the center of the screen
  glfwPollEvents();
  glfwSetCursorPos(Window, WindowWidth / 2, WindowHeight / 2);

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // OpenGL state
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*
    Text GL objects
  */
  text Text; // Init glyph textures

  // Create and bind text Vertex Array Object (VA0)
  GLuint TextVAO;
  glGenVertexArrays(1, &TextVAO);
  glBindVertexArray(TextVAO);

  // Create and bind text Vertex Buffer Object (VBO)
  GLuint TextVBO;
  glGenBuffers(1, &TextVBO);
  glBindBuffer(GL_ARRAY_BUFFER, TextVBO);

  // 2D quad requires 6 vertices of 4 floats each
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GLuint TextProgram;
  try {
    TextProgram = loadTextShaders();
  } catch (std::exception &E) {
    std::cerr << "Error " << E.what() << std::endl;
    glfwTerminate();
    return -1;
  }
  glm::mat4 TextProjMatrix = glm::ortho(0.0f, static_cast<float>(WindowWidth),
                                        0.0f, static_cast<float>(WindowHeight));
  GLuint TextMVPUniform = glGetUniformLocation(TextProgram, "MVP");
  GLuint TextColorUniform = glGetUniformLocation(TextProgram, "TextColor");
  glUseProgram(TextProgram);
  glUniformMatrix4fv(TextMVPUniform, 1, GL_FALSE,
                     glm::value_ptr(TextProjMatrix));

  /*
    Skybox GL objects
  */
  skybox Skybox;
  unsigned int SkyboxTexture;
  try {
    SkyboxTexture = Skybox.loadCubemap();
  } catch (std::exception &E) {
    std::cerr << "Error " << E.what() << std::endl;
    glfwTerminate();
    return -1;
  }

  // Create and bind skybox Vertex Array Object (VA0)
  GLuint SkyboxVAO;
  glGenVertexArrays(1, &SkyboxVAO);
  glBindVertexArray(SkyboxVAO);

  // Create and bind skybox Vertex Buffer Object (VBO)
  GLuint SkyboxVBO;
  glGenBuffers(1, &SkyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, SkyboxVBO);

  // Tie skybox vertex data to VBO
  glBufferData(GL_ARRAY_BUFFER, Skybox.getVerticesSize(), Skybox.getVertices(),
               GL_STATIC_DRAW);

  // Create and bind Element Buffer Object (EBO)
  GLuint SkyboxEBO;
  glGenBuffers(1, &SkyboxEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SkyboxEBO);

  // Tie index data to EBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Skybox.getIndexSize(),
               Skybox.getIndexData(), GL_STATIC_DRAW);

  GLuint SkyboxProgram;
  try {
    SkyboxProgram = loadSkyboxShaders();
  } catch (std::exception &E) {
    std::cerr << "Error " << E.what() << std::endl;
    glfwTerminate();
    return -1;
  }
  GLuint SkyboxMVPUniform = glGetUniformLocation(SkyboxProgram, "MVP");

  /*
    Sphere GL objects
  */
  sphere Sphere(1.0f /* radius */, Sectors, Stacks);
  unsigned int SphereTexture;
  try {
    SphereTexture = Sphere.loadTexture();
  } catch (std::exception &E) {
    std::cerr << "Error " << E.what() << std::endl;
    glfwTerminate();
    return -1;
  }

  // Create and bind sphere Vertex Array Object (VA0)
  GLuint SphereVAO;
  glGenVertexArrays(1, &SphereVAO);
  glBindVertexArray(SphereVAO);

  // Create and bind sphere Vertex Buffer Object (VBO)
  GLuint SphereVertexVBO;
  glGenBuffers(1, &SphereVertexVBO);
  glBindBuffer(GL_ARRAY_BUFFER, SphereVertexVBO);

  // Tie sphere vertex data to VBO
  glBufferData(GL_ARRAY_BUFFER, Sphere.getVertexSize(), Sphere.getVertexData(),
               GL_STATIC_DRAW);

  // Create and bind Element Buffer Object (EBO)
  GLuint SphereEBO;
  glGenBuffers(1, &SphereEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);

  // Tie index data to EBO
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, Sphere.getIndexSize(),
               Sphere.getIndexData(), GL_STATIC_DRAW);

  GLuint SphereNormalVBO;
  glGenBuffers(1, &SphereNormalVBO);
  glBindBuffer(GL_ARRAY_BUFFER, SphereNormalVBO);
  glBufferData(GL_ARRAY_BUFFER, Sphere.getNormalSize(), Sphere.getNormalData(),
               GL_STATIC_DRAW);

  GLuint SphereTexCoordsVBO;
  glGenBuffers(1, &SphereTexCoordsVBO);
  glBindBuffer(GL_ARRAY_BUFFER, SphereTexCoordsVBO);
  glBufferData(GL_ARRAY_BUFFER, Sphere.getTexCoordSize(),
               Sphere.getTexCoordData(), GL_STATIC_DRAW);

  GLuint SphereProgram;
  try {
    SphereProgram = loadSphereShaders();
  } catch (std::exception &E) {
    std::cerr << "Error " << E.what() << std::endl;
    glfwTerminate();
    return -1;
  }
  GLuint SphereMVPUniform = glGetUniformLocation(SphereProgram, "MVP");
  GLuint SphereVUniform = glGetUniformLocation(SphereProgram, "V");
  GLuint SphereLightUniform =
      glGetUniformLocation(SphereProgram, "LightPosition");
  GLuint SphereTexUniform = glGetUniformLocation(SphereProgram, "TexSampler");

  // Matches sun on skybox texture
  glm::vec3 SphereLightPos = glm::vec3(4, 4, 4);

  controls Controls(Window, WindowWidth, WindowHeight);
  double Timestamp = glfwGetTime(); // seconds
  unsigned ElapsedFrames = 0;
  unsigned FPS = 0;
  do {
    // Measure FPS
    ElapsedFrames++;
    if (double CurrentTime = glfwGetTime(); (CurrentTime - Timestamp) >= 1.0) {
      Timestamp = CurrentTime;
      FPS = ElapsedFrames;
      ElapsedFrames = 0;
    }

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Refresh user inputs
    Controls.refreshMatrices();

    // Sphere
    glm::mat4 SphereMVP = Controls.getMVPMatrix(Sphere.getModelMatrix(), false);
    glm::mat4 SphereView = Controls.getViewMatrix();
    glUseProgram(SphereProgram);
    glUniformMatrix4fv(SphereMVPUniform, 1, GL_FALSE, &SphereMVP[0][0]);
    glUniformMatrix4fv(SphereVUniform, 1, GL_FALSE, &SphereView[0][0]);
    glUniform3f(SphereLightUniform, SphereLightPos.x, SphereLightPos.y,
                SphereLightPos.z);

    // Vertex
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, SphereVertexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(0,        // Matches shader layer
                          3,        // matches vec3
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride, 0 lets GL decide
                          (void *)0 // array buffer offset
    );

    // Normal
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, SphereNormalVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(1,        // attribute
                          3,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );

    // Texture Coordinate
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, SphereTexCoordsVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SphereEBO);
    glVertexAttribPointer(2,        // attribute
                          2,        // size
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, SphereTexture);

    glDrawElements(GL_TRIANGLES,          // primitive type
                   Sphere.getIndexSize(), // # of indices
                   GL_UNSIGNED_INT,       // data type
                   (void *)0);            // ptr to indices
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Skybox
    glm::mat4 SkyboxMVP = Controls.getMVPMatrix(Skybox.getModelMatrix(), true);
    glUseProgram(SkyboxProgram);
    glBindBuffer(GL_ARRAY_BUFFER, SkyboxVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SkyboxEBO);
    glUniformMatrix4fv(SkyboxMVPUniform, 1, GL_FALSE, &SkyboxMVP[0][0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,        // Matches shader layer
                          3,        // matches vec3
                          GL_FLOAT, // type
                          GL_FALSE, // normalized?
                          0,        // stride, 0 lets GL decide
                          (void *)0 // array buffer offset
    );
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTexture);
    glDrawElements(GL_TRIANGLES,          // primitive type
                   Skybox.getIndexSize(), // # of indices
                   GL_UNSIGNED_INT,       // data type
                   (void *)0);            // ptr to indices
    glDisableVertexAttribArray(0);

    // Text
    glUseProgram(TextProgram);
    glEnableVertexAttribArray(0);
    glm::vec3 TextColor(1., 1.f, 1.f);
    glUniform3f(TextColorUniform, TextColor.x, TextColor.y, TextColor.z);
    glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glActiveTexture(GL_TEXTURE0);

    std::string StrFPS("FPS: ");
    if (FPS) {
      StrFPS.append(std::to_string(FPS));
    }
    Text.render(TextVBO, StrFPS, 5.0f, 5.0f, .5f);
    Text.render(TextVBO, Controls.getPositionStr(), 5.0f, 40.0f, .5f);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(Window);
    glfwPollEvents();
  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(Window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(Window) == 0);

  // Cleanup
  glDeleteProgram(SphereProgram);
  glDeleteProgram(SkyboxProgram);
  glDeleteVertexArrays(1, &SphereVAO);
  glDeleteBuffers(1, &SphereVertexVBO);
  glDeleteBuffers(1, &SphereNormalVBO);
  glDeleteBuffers(1, &SphereTexCoordsVBO);
  glDeleteBuffers(1, &SphereEBO);
  glDeleteTextures(1, &SphereTexture);
  glDeleteVertexArrays(1, &SkyboxVAO);
  glDeleteBuffers(1, &SkyboxVBO);
  glDeleteBuffers(1, &SkyboxEBO);
  glDeleteTextures(1, &SkyboxTexture);
  glDeleteVertexArrays(1, &TextVAO);
  glDeleteBuffers(1, &TextVBO);
  Text.freeTextures();
  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}
