// gcc -lglut -lGLU -lGL -lm main.c

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.14159265358979323846
#define DEGREE ((2 * PI)/360) // one degree in radians

#define VIEWPORT_WIDTH 1024
#define VIEWPORT_HEIGHT 512

// Map
const int mapX = 8,
          mapY = 8,
          mapSize = mapX * mapY;

int map[] = {
  1,1,1,1,1,1,1,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,1,0,1,
  1,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,
};

// Player position, deltas, and angle
float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle;

void drawPlayer() {
  glColor3f(0, 1, 0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2i(playerX, playerY);
  glEnd();

  glLineWidth(3);
  glBegin(GL_LINES);
  glVertex2i(playerX, playerY);
  glVertex2i(playerX + playerDeltaX * 5, playerY + playerDeltaY * 5);
  glEnd();
}

void drawMap2D() {
  int x, y, xOffset, yOffset;

  for (y = 0; y < mapY; y++) {
   for (x = 0; x < mapX; x++) {
     if (map[y*mapX+x] == 1) {
       glColor3f(1, 1, 1);
     } else {
       glColor3f(0, 0, 0);
     }

     xOffset = x * mapSize;
     yOffset = y * mapSize;

     glBegin(GL_QUADS);
     glVertex2i(xOffset + 1, yOffset + 1);
     glVertex2i(xOffset + 1, yOffset + mapSize - 1);
     glVertex2i(xOffset + mapSize - 1, yOffset + mapSize - 1);
     glVertex2i(xOffset + mapSize - 1, yOffset + 1);
     glEnd();
   }
  }  
}

// pythagorean theorem
//     /|
//  c /_| b
//     a

float dist(float ax, float ay, float bx, float by) {
  return ( sqrt((bx - ax) * (bx - ax) + (by-ay) * (by - ay)) );
}

void castRays() {
  int ray, mapXPos, mapYPos, mapArrPos, dof;
  float rayX = 0, rayY = 0, rayAngle = 0, xOffset = 0, yOffset = 0, distance = 0;

  rayAngle = playerAngle - DEGREE * 30;
  if (rayAngle < 0) { rayAngle += (2 * PI); }
  if (rayAngle > (2 * PI)) { rayAngle -= (2 * PI); }

  for (ray = 0; ray < 60; ray++) {
    // Horizontal Line Check
    dof = 0;
    float rayDistHor = 10000000;
    float horRayX = playerX;
    float horRayY = playerY;
    float aTan = -1/tan(rayAngle); // inverse tangent

    if (rayAngle > PI) { // ray looking up
      rayY = (((int)playerY >> 6) << 6) - 0.0001; // round to nearest multiple of 64 (and subtract for accuracy)
      rayX = (playerY - rayY) * aTan + playerX;
      yOffset = -64;
      xOffset = -yOffset * aTan;
    }

    if (rayAngle < PI) { // ray looking down
      rayY = (((int)playerY >> 6) << 6) + 64;
      rayX = (playerY - rayY) * aTan + playerX;
      yOffset = 64;
      xOffset = -yOffset * aTan;
    }

    if (rayAngle == 0 || rayAngle == PI) { // ray directly left or right
      rayX = playerX;
      rayY = playerY;
      dof = 8;
    }

    while (dof < 8) {
      mapXPos = (int)(rayX) >> 6;
      mapYPos = (int)(rayY) >> 6;
      mapArrPos = mapYPos * mapX + mapXPos;

      if (mapArrPos >= 0 && mapArrPos < (mapX * mapY) && map[mapArrPos] == 1) { // hit a horizontal wall
        horRayX = rayX;
        horRayY = rayY;
        rayDistHor = dist(playerX, playerY, horRayX, horRayY);
        dof = 8;
      } else {
        rayX += xOffset;
        rayY += yOffset;
        dof += 1; // next line
      }
    }

    // Vertical Line Check
    dof = 0;
    float rayDistVer = 1000000;
    float vertRayX = playerX;
    float vertRayY = playerY;
    float nTan = -tan(rayAngle);

    if ((rayAngle > (PI / 2)) && (rayAngle < (3*PI/2))) { // ray looking left
      rayX = (((int)playerX >> 6) << 6) - 0.0001;
      rayY = (playerX - rayX) * nTan + playerY;
      xOffset = -64;
      yOffset = -xOffset * nTan;
    }

    if ((rayAngle < (PI / 2)) || (rayAngle > (3*PI/2))) { // ray looking right
      rayX = (((int)playerX >> 6) << 6) + 64;
      rayY = (playerX - rayX) * nTan + playerY;
      xOffset = 64;
      yOffset = -xOffset * nTan;
    }

    if (rayAngle == 0 || rayAngle == PI) { // ray directly up or down
      rayX = playerX;
      rayY = playerY;
      dof = 8;
    }

    while (dof < 8) {
      mapXPos = (int)(rayX)>>6;
      mapYPos = (int)(rayY)>>6;
      mapArrPos = mapYPos * mapX + mapXPos;

      if (mapArrPos >= 0 && mapArrPos < (mapX * mapY) && map[mapArrPos] == 1) { // hit a vertical wall
        vertRayX = rayX;
        vertRayY = rayY;
        rayDistVer = dist(playerX, playerY, vertRayX, vertRayY);
        dof = 8;
      } else {
        rayX += xOffset;
        rayY += yOffset;
        dof += 1; // next line
      }
    }

    if (rayDistVer < rayDistHor) {
      rayX = vertRayX;
      rayY = vertRayY;
      distance = rayDistVer;
      glColor3f(0.55, 0.45, 0.40);
    }

    if (rayDistHor < rayDistVer) {
      rayX = horRayX;
      rayY = horRayY;
      distance = rayDistHor;
      glColor3f(0.65, 0.6, 0.55);
    }

    // draw rays
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(playerX, playerY);
    glVertex2i(rayX, rayY);
    glEnd();

    // draw 3d walls
    float correctedAngle = rayAngle - playerAngle;
    if (correctedAngle < 0) { correctedAngle += (2 * PI); }
    if (correctedAngle > (2 * PI)) { correctedAngle -= (2 * PI); }
    float correctedDistance = distance * cos(correctedAngle);

    float lineHeight = (mapSize * 320) / correctedDistance;
    if (lineHeight > 320) { lineHeight = 320; }
    float lineOffset = 160 - lineHeight / 2;

    glLineWidth(8);
    glBegin(GL_LINES);
    glVertex2i(ray * 8 + 530, lineOffset);
    glVertex2i(ray * 8 + 530, lineHeight + lineOffset);
    glEnd();

    rayAngle += DEGREE;
    if (rayAngle < 0) { rayAngle += 2 * PI; }
    if (rayAngle > 2 * PI) { rayAngle -= 2 * PI; }
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2D();
  castRays();
  drawPlayer();
  glutSwapBuffers(); // swaps our buffers around
}

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
  gluOrtho2D(0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 0);
  // set player pos
  playerX = 400;
  playerY = 300;
  playerDeltaX = cos(playerAngle) * 5;
  playerDeltaY = sin(playerAngle) * 5;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
  glutCreateWindow("Raycaster");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(buttons);
  glutMainLoop();
}