// gcc -lglfw -lGLU -lGL -lm -Iinclude src/*.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "utils.h"
#include "gfx.h"

#define PLAYER_SPEED 0.01
#define ROTATION_ANGLE 0.025
#define SECOND *1000
#define PI 3.14159265358979323846

void errorCallback(int error, const char* description) {
  fputs(description, stderr);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mousePosCallback(GLFWwindow* window, double xPos, double yPos) {

}

void movement(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    playerX += (playerDirX * PLAYER_SPEED);
    playerY += (playerDirY * PLAYER_SPEED);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    playerX -= (playerDirX * PLAYER_SPEED);
    playerY -= (playerDirY * PLAYER_SPEED);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    playerX += (playerDirY * PLAYER_SPEED);
    playerY -= (playerDirX * PLAYER_SPEED);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    playerX -= (playerDirY * PLAYER_SPEED);
    playerY += (playerDirX * PLAYER_SPEED);
  }
}

void mouseLook(GLFWwindow* window) {
  double mouseX, mouseY;
  static double prevMouseX, prevMouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  if (!(mouseX == prevMouseX)) {
    if(prevMouseX < mouseX) {
      // left
      float prevPlayerDirX = playerDirX;
      playerDirX = playerDirX * cos(ROTATION_ANGLE) - playerDirY * sin(ROTATION_ANGLE);
      playerDirY = prevPlayerDirX * sin(ROTATION_ANGLE) + playerDirY * cos(ROTATION_ANGLE);

      float prevPlaneX = planeX;
      planeX = planeX * cos(ROTATION_ANGLE) - planeY * sin(ROTATION_ANGLE);
      planeY = prevPlaneX * sin(ROTATION_ANGLE) + planeY * cos(ROTATION_ANGLE);
    } else {
      // right
      float prevPlayerDirX = playerDirX;
      playerDirX = playerDirX * cos(-ROTATION_ANGLE) - playerDirY * sin(-ROTATION_ANGLE);
      playerDirY = prevPlayerDirX * sin(-ROTATION_ANGLE) + playerDirY * cos(-ROTATION_ANGLE);

      float prevPlaneX = planeX;
      planeX = planeX * cos(-ROTATION_ANGLE) - planeY * sin(-ROTATION_ANGLE);
      planeY = prevPlaneX * sin(-ROTATION_ANGLE) + planeY * cos(-ROTATION_ANGLE);
    }
  }

  prevMouseX = mouseX;
  prevMouseY = mouseY;
}

void init() {
  glClearColor(0.2, 0.2, 0.2, 0);
  gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  
  int fov = 120; // FOV in degrees
  float fovRadians = (fov * (PI / 180));

  // set player pos
  playerX = 4;
  playerY = 4;
  playerDirX = 1;
  playerDirY = 0;
  planeX = 0;
  planeY = tan(fovRadians/2);
}

int main(int argc, char** argv) {
  glfwInit();
  
  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwSetErrorCallback(errorCallback);

  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raycaster", NULL, NULL);

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mousePosCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  init();

  int previousFrameStartTime = 0;
  float physicsInterval = (1 SECOND/100); // physics runs at this speed, in frames per second (1 second/100 frames)
  float accumulator = 0;

  while (!glfwWindowShouldClose(window)) {
    int frameStartTime = glfwGetTime();
    int deltaTime = frameStartTime - previousFrameStartTime;
    previousFrameStartTime = frameStartTime;

    accumulator += deltaTime;

    while (accumulator >= physicsInterval) {
      // physics function here
      accumulator -= physicsInterval;
    }

    mouseLook(window);
    movement(window);
    display(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
