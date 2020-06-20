#include <stdio.h>
#include <math.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "utils.h"
#include "gfx.h"

#define MAX_DOF 16

int fov = 100;
int previousFrameStartTime = 0;
float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle;

// Map
const int mapX = 8,
          mapY = 8*3,
          mapSize = 8*8;

int map[] = {
  1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,1,
  1,0,2,0,0,2,0,1,
  1,0,0,0,0,0,0,1,
  1,0,1,0,0,2,0,1,
  1,0,0,2,3,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,2,0,3,0,1,1,
  1,0,0,0,0,0,0,1,
  1,2,0,3,0,1,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,1,1,1,2,0,1,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1
};

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
  glBegin(GL_QUADS);
  int x, y, xOffset, yOffset;

  for (y = 0; y < mapY; y++) {
   for (x = 0; x < mapX; x++) {
     if (map[y*mapX+x] > 0) {
       glColor3f(1, 1, 1);
     } else {
       glColor3f(0, 0, 0);
     }

     xOffset = x * mapSize;
     yOffset = y * mapSize;

     glVertex2i(xOffset + 1, yOffset + 1);
     glVertex2i(xOffset + 1, yOffset + mapSize - 1);
     glVertex2i(xOffset + mapSize - 1, yOffset + mapSize - 1);
     glVertex2i(xOffset + mapSize - 1, yOffset + 1);
   }
  }
  glEnd();
}

void castRays() {
  int ray, mapXPos, mapYPos, mapArrPos, dof;
  float tileR = 0;
  float tileG = 0;
  float tileB = 0;

  float colourMod = 0.25;
  float rayX = 0, rayY = 0, rayAngle = 0, xOffset = 0, yOffset = 0, distance = 0;
  int horizontalWall = 0;
  int verticalWall = 0;

  rayAngle = playerAngle - DEGREE * (fov / 2);
  if (rayAngle < 0) { rayAngle += TAU; }
  if (rayAngle > TAU) { rayAngle -= TAU; }

  for (ray = 0; ray < fov; ray++) {
    // Horizontal Line Check
    dof = 0;
    float rayDistHor = 10000000;
    float horRayX = playerX;
    float horRayY = playerY;
    float aTan = -1/tan(rayAngle); // inverse tangent

    if (rayAngle > PI) { // ray looking up
      rayY = ROUND_TO_MULTIPLE_OF_64(playerY) - 0.0001; // round to nearest multiple of 64 (and subtract for accuracy)
                                                        // each tile is 64x64, and we subtract a small amount to ensure that
                                                        // we don't clip the boundary of the tile and get graphical errors
      rayX = (playerY - rayY) * aTan + playerX;
      yOffset = -64;
      xOffset = -yOffset * aTan;
    }

    if (rayAngle < PI) { // ray looking down
      rayY = ROUND_TO_MULTIPLE_OF_64(playerY) + 64;
      rayX = (playerY - rayY) * aTan + playerX;
      yOffset = 64;
      xOffset = -yOffset * aTan;
    }

    if (rayAngle == 0 || rayAngle == PI) { // ray directly left or right
      rayX = playerX;
      rayY = playerY;
      dof = MAX_DOF;
    }

    while (dof < MAX_DOF) {
      mapXPos = (int)(rayX) >> 6;
      mapYPos = (int)(rayY) >> 6;
      mapArrPos = mapYPos * mapX + mapXPos;

      if (mapArrPos >= 0 && mapArrPos < (mapX * mapY) && map[mapArrPos] > 0) { // hit a horizontal wall
        horRayX = rayX;
        horRayY = rayY;
        rayDistHor = length(playerX, playerY, horRayX, horRayY);
        dof = MAX_DOF;
        horizontalWall = mapArrPos;
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
      dof = MAX_DOF;
    }

    while (dof < MAX_DOF) {
      mapXPos = (int)(rayX) >> 6;
      mapYPos = (int)(rayY) >> 6;
      mapArrPos = mapYPos * mapX + mapXPos;

      if (mapArrPos >= 0 && mapArrPos < (mapX * mapY) && map[mapArrPos] > 0) { // hit a vertical wall
        vertRayX = rayX;
        vertRayY = rayY;
        rayDistVer = length(playerX, playerY, vertRayX, vertRayY);
        dof = MAX_DOF;
        verticalWall = mapArrPos;
      } else {
        rayX += xOffset;
        rayY += yOffset;
        dof += 1; // next line
      }
    }
    
    int tileType = 0;
    if (rayDistVer > rayDistHor) { tileType = map[horizontalWall]; } // if drawing horizontal wall
    if (rayDistVer < rayDistHor) { tileType = map[verticalWall]; } // if drawing horizontal wall
    
    switch (tileType) {
      case 1:
        tileR = 1.0;
        tileG = 0.0;
        tileB = 0.5;
        break;

      case 2:
        tileR = 0.69;
        tileG = 1.0;
        tileB = 0.2;
        break;

      case 3:
        tileR = 1.0;
        tileG = 1.0;
        tileB = 1.0;
        break;
    }

    if (rayDistVer < rayDistHor) {
      rayX = vertRayX;
      rayY = vertRayY;
      distance = rayDistVer;
      float rMod = tileR - colourMod;
      float gMod = tileG - colourMod;
      float bMod = tileB - colourMod;
      if (rMod < 0) {rMod = 0;}
      if (gMod < 0) {gMod = 0;}
      if (bMod < 0) {bMod = 0;}
      glColor3f(rMod, gMod, bMod);
    }

    if (rayDistHor < rayDistVer) {
      rayX = horRayX;
      rayY = horRayY;
      distance = rayDistHor;
      glColor3f(tileR, tileG, tileB);
    }

    // draw rays
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2i(playerX, playerY);
    glVertex2i(rayX, rayY);
    glEnd();

    // draw 3d walls
    float relativeAngle = rayAngle - playerAngle;
    float correctedDistance = distance * cos(relativeAngle);

    float lineHeight = (mapSize * VIEWPORT_HEIGHT) / correctedDistance;
    if (lineHeight > VIEWPORT_HEIGHT) { lineHeight = VIEWPORT_HEIGHT; }
    float lineOffset = (VIEWPORT_HEIGHT - lineHeight) / 2;

    float lineWidth = (float)VIEWPORT_WIDTH / fov;
    
    glBegin(GL_QUADS);
    glVertex2i(ray * lineWidth + 530, lineOffset);
    glVertex2i(ray * lineWidth + 530 + lineWidth + 1, lineOffset);
    glVertex2i(ray * lineWidth + 530 + lineWidth + 1, lineHeight + lineOffset);
    glVertex2i(ray * lineWidth + 530, lineHeight + lineOffset);
    glEnd();

    rayAngle += DEGREE;
    if (rayAngle < 0) { rayAngle += TAU; }
    if (rayAngle > TAU) { rayAngle -= TAU; }
  }
}

void display(GLFWwindow* window) {
  int frameStartTime = glfwGetTime();
  int deltaTime = frameStartTime - previousFrameStartTime;
  previousFrameStartTime = frameStartTime;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2D();
  castRays();
  drawPlayer();
  frameTime(deltaTime);
  glfwSwapBuffers(window); // swaps our buffers around
}