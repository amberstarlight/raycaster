#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "utils.h"
#include "gfx.h"

int fov = 360;
int previousFrameStartTime = 0;
float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle;

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

void castRays() {
  int ray, mapXPos, mapYPos, mapArrPos, dof;
  float rayX = 0, rayY = 0, rayAngle = 0, xOffset = 0, yOffset = 0, distance = 0;

  rayAngle = playerAngle - DEGREE * (fov / 2);
  if (rayAngle < 0) { rayAngle += (2 * PI); }
  if (rayAngle > (2 * PI)) { rayAngle -= (2 * PI); }

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
      dof = 8;
    }

    while (dof < 8) {
      mapXPos = (int)(rayX) >> 6;
      mapYPos = (int)(rayY) >> 6;
      mapArrPos = mapYPos * mapX + mapXPos;

      if (mapArrPos >= 0 && mapArrPos < (mapX * mapY) && map[mapArrPos] == 1) { // hit a horizontal wall
        horRayX = rayX;
        horRayY = rayY;
        rayDistHor = length(playerX, playerY, horRayX, horRayY);
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
        rayDistVer = length(playerX, playerY, vertRayX, vertRayY);
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
    float relativeAngle = rayAngle - playerAngle;
    if (relativeAngle < 0) { relativeAngle += (2 * PI); }
    if (relativeAngle > (2 * PI)) { relativeAngle -= (2 * PI); }
    float correctedDistance = distance * cos(relativeAngle);

    float lineHeight = (mapSize * VIEWPORT_HEIGHT) / correctedDistance;
    if (lineHeight > VIEWPORT_HEIGHT) { lineHeight = VIEWPORT_HEIGHT; }
    float lineOffset = (VIEWPORT_HEIGHT - lineHeight) / 2;

    float lineWidth = (float)VIEWPORT_WIDTH / fov;
    glBegin(GL_QUADS);
    glVertex2i(ray * lineWidth + 530, lineOffset);
    glVertex2i(ray * lineWidth + 530 + lineWidth, lineOffset);
    glVertex2i(ray * lineWidth + 530 + lineWidth, lineHeight + lineOffset);
    glVertex2i(ray * lineWidth + 530, lineHeight + lineOffset);
    glEnd();

    // rayAngle = math.deg(math.atan2(screenWidth/2 - (ray - 0.5), distToPlane)) + self.player.angle
    rayAngle += DEGREE;
    if (rayAngle < 0) { rayAngle += 2 * PI; }
    if (rayAngle > 2 * PI) { rayAngle -= 2 * PI; }
  }
}

void display() {
  int frameStartTime = glutGet(GLUT_ELAPSED_TIME);
  int deltaTime = frameStartTime - previousFrameStartTime;
  previousFrameStartTime = frameStartTime;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawMap2D();
  castRays();
  drawPlayer();
  frameTime(deltaTime);
  glutSwapBuffers(); // swaps our buffers around
 
}