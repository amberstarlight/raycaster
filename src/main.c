// gcc -lglfw -lGLU -lGL -lm -Iinclude src/*.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "utils.h"
#include "gfx.h"

#define PLAYER_SPEED 1
#define ANGLE_STEP 0.02

void errorCallback(int error, const char* description) {
  fputs(description, stderr);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void movement(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    playerX += playerDeltaX;
    playerY += playerDeltaY;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    playerX -= playerDeltaX;
    playerY -= playerDeltaY;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    playerAngle -= ANGLE_STEP;
    if (playerAngle < 0) { playerAngle += TAU; }
    playerDeltaX = cos(playerAngle) * PLAYER_SPEED;
    playerDeltaY = sin(playerAngle) * PLAYER_SPEED;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    playerAngle += ANGLE_STEP;
    if (playerAngle > TAU) { playerAngle -= TAU; }
    playerDeltaX = cos(playerAngle) * PLAYER_SPEED;
    playerDeltaY = sin(playerAngle) * PLAYER_SPEED;
  }
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

  init();

  while (!glfwWindowShouldClose(window)) {
    movement(window);
    display(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
