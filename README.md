# GLSphere

OpenGL application that renders a sphere in a skybox using football related
textures. The sphere is generated using quads split vertical stacks and
horizontal sectors, the number of which can be controlled by command-line
arguments.

Keyboard arrows can be used to move the camera, and the mouse can be used
to look around. The position of the camera, along with the FPS, is displayed
at the bottom left of the screen.

Some basic phong shading is done, but there are no more advanced techniques
like shadows.

![GLSphere](images/glsphere.gif)

## Usage

```sh
Usage: ./glsphere
OpenGL implementation of a sphere in a skybox.
Use arrow keys to move camera and mouse to look around. Press escape key to exit.
Options:
	-h, --help		Show this help message
	--stacks N 		Number of stacks in sphere, defaults to 18
	--sectors N 	Number of sectors in sphere, defaults to 36
```

## Building

The project has only been tested building on Ubuntu 24.04

The `external` directory contains the following dependencies as git submodules
which are built as part of the project:
* freetype
* stb
* glew
* glfw
* glm

```sh
$ cd <path/to/source>
$ cmake -S . B build
$ cmake --build build
$ ./build/glsphere
```

### Debug

A Debug build of CMake enables OpenGL callback error reporting, which is
directed to standard error.

## References

Resources I used to help me develop the application

* [Learn OpenGL](https://learnopengl.com/)
* [opengl-tutorial](https://www.opengl-tutorial.org)
* [OpenGL Sphere](ww.songho.ca/opengl/gl_sphere.html)
