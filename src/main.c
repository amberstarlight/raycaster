// gcc -lglut -lGLU -lGL -lm -Iinclude src/*.c

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <time.h>

#include "utils.h"
#include "gfx.h"

void buttons(unsigned char key, int x, int y) {
  switch (key) {
  case 'd':
    playerAngle += 0.1;
    if (playerAngle > 2 * PI) { playerAngle -= 2 * PI; } // 2 * pi = 360Degrees 
    playerDeltaX = cos(playerAngle) * 5;
    playerDeltaY = sin(playerAngle) * 5;
    break;
  case 'a':
    playerAngle -= 0.1;
    if (playerAngle < 0) { playerAngle += 2 * PI; }
    playerDeltaX = cos(playerAngle) * 5;
    playerDeltaY = sin(playerAngle) * 5;
    
    break;
  case 'w':
    playerX += playerDeltaX;
    playerY += playerDeltaY;
    break;
  case 's':
    playerX -= playerDeltaX;
    playerY -= playerDeltaY;
    break;
  }
  glutPostRedisplay();
}

void init() {
  glClearColor(0.2, 0.2, 0.2, 0);
  gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  // set player pos
  playerAngle = 0;
  playerX = 400;
  playerY = 300;
  playerDeltaX = cos(playerAngle) * 5;
  playerDeltaY = sin(playerAngle) * 5;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Raycaster");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(buttons);
  glutMainLoop();
}