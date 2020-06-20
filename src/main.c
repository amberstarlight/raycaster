// gcc -lglfw -lGLU -lGL -lm -Iinclude src/*.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "utils.h"
#include "gfx.h"

#define PLAYER_SPEED 0.75
#define ANGLE_STEP 0.001

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
    playerX += (playerDeltaX * PLAYER_SPEED);
    playerY += (playerDeltaY * PLAYER_SPEED);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    playerX -= (playerDeltaX * PLAYER_SPEED);
    playerY -= (playerDeltaY * PLAYER_SPEED);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    playerX += (playerDeltaY * PLAYER_SPEED);
    playerY -= (playerDeltaX * PLAYER_SPEED);
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    playerX -= (playerDeltaY * PLAYER_SPEED);
    playerY += (playerDeltaX * PLAYER_SPEED);
  }
}

void mouseLook(GLFWwindow* window) {
  double mouseX, mouseY;
  static double prevMouseX, prevMouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  if (!(mouseX == prevMouseX)) {
    playerAngle -= ((prevMouseX - mouseX) * ANGLE_STEP);
    if (playerAngle > TAU) { playerAngle -= TAU; }
    playerDeltaX = cos(playerAngle) * PLAYER_SPEED;
    playerDeltaY = sin(playerAngle) * PLAYER_SPEED;
  }

  prevMouseX = mouseX;
  prevMouseY = mouseY;
}


void init() {
  glClearColor(0.2, 0.2, 0.2, 0);
  gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  // set player pos
  playerAngle = 0;
  playerX = 400;
  playerY = 300;
  playerDeltaX = cos(playerAngle) * PLAYER_SPEED;
  playerDeltaY = sin(playerAngle) * PLAYER_SPEED;
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

  while (!glfwWindowShouldClose(window)) {
    mouseLook(window);
    movement(window);
    display(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
