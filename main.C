#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <GL/glxext.h>

#include "config.h"
#include "Cube.h"
#include "Stopwatch.h"

void GLFWCALL windowResize(int width, int height)
{
  glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {

  if (!glfwInit())
    exit(EXIT_FAILURE);

  // Open a window.
  if (!glfwOpenWindow(500, 500, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSwapInterval(1);
  glfwSetWindowSizeCallback(windowResize);

  // Initialize glew.
  glewInit();

  Cube cube;
  Stopwatch stopwatch;
  float secondsElapsed = 0.0f;
  float targetSPF = 1.0f / 30.0f;
  int running = GL_TRUE;
  while (running) {
    stopwatch.start();

    // Draw the scene.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.render(secondsElapsed);
    glfwSwapBuffers();
    
    // Sleep to achieve target framerate.
    secondsElapsed = stopwatch.secondsElapsed();
    float sleepTimeSec = targetSPF - secondsElapsed;
    usleep(sleepTimeSec > 0 ? sleepTimeSec * 1000000.0f : 0);

    // Check if ESC key was pressed or window was closed.
    running = !glfwGetKey(GLFW_KEY_ESC) &&
      glfwGetWindowParam(GLFW_OPENED);

    // Get the time spent this frame.
    secondsElapsed = stopwatch.secondsElapsed();
  }

  glfwTerminate();
  exit(EXIT_SUCCESS);
}


