#include <stdio.h>
#include <math.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "utils.h"
#include "gfx.h"

int previousFrameStartTime = 0;
float playerX, playerY, playerDirX, playerDirY, planeX, planeY;

// Map
const int mapWidth = 8,
          mapHeight = 8*3;

int map[] = {
  1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
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

void drawMap2D() {
  int scale = 10;
  float playerSize = 0.25;

  glBegin(GL_QUADS);
  for (int y = 0; y < mapHeight; y++) {
    for (int x = 0; x < mapWidth; x++) {
      if (map[(y * mapWidth) + x] > 0) {
        glColor3f(1, 1, 1);
      } else {
        glColor3f(0, 0, 0);
      }

      int scaledX = x * scale;
      int scaledY =  y * scale;

      glVertex2i(scaledX, scaledY);
      glVertex2i(scaledX + scale, scaledY);
      glVertex2i(scaledX + scale, scaledY + scale);
      glVertex2i(scaledX, scaledY + scale);
    }
  }
  glEnd();

  glColor3f(0, 1, 0);
  glPointSize(playerSize * scale);
  glBegin(GL_POINTS);
  glVertex2i(playerX*scale, playerY*scale);
  glEnd();

  glBegin(GL_LINES);
  glLineWidth(4);
  glVertex2i(playerX * scale,playerY * scale);
  glVertex2i((playerX + playerDirX) * scale, (playerY + playerDirY) * scale);
  glEnd();
}

void castRays() {
  for (int x = 0; x < VIEWPORT_WIDTH; x++) {
    // calculate ray position and direction
    float cameraX = (2 * (x / (float) mapWidth)) - 1; // x-coordinate in camera space mapped from -1 to 1

    float rayDirX = playerDirX + (planeX * cameraX), // ray vector
          rayDirY = playerDirY + (planeY * cameraX);
    
    int currentTileX = (int) playerX, // the tile the player is standing in (x-coordinate)
        currentTileY = (int) playerY; // the tile the player is standing in (y-coordinate)

    float sideDistX, // distance from player to next vertical edge
          sideDistY; // distance from player to next horizontal edge

    float deltaDistX = fabsf(1.f / rayDirX), // delta between horizontal edges
          deltaDistY = fabsf(1.f / rayDirY), // delta between vertical edges
          perpWallDist; // length between camera plane and wall
    
    // what direction to step in x or y-direction (either +1 or -1)
    int stepX;
    int stepY;

    int hit = 0;  // was there a wall hit?
    int side;     // was a Vertical or a Horizontal wall hit?

    if (rayDirX < 0) {
      stepX = -1;
      sideDistX = (playerX - currentTileX) * deltaDistX;
    } else {
      stepX = 1;
      sideDistX = (currentTileX + 1.0 - playerX) * deltaDistX;
    }

    if (rayDirY < 0) {
      stepY = -1;
      sideDistY = (playerY - currentTileY) * deltaDistY;
    } else {
      stepY = 1;
      sideDistY = (currentTileY + 1.0 - playerY) * deltaDistY;
    }

    // DDA algorithm
    while (!hit) {
      if (sideDistX < sideDistY) {
        sideDistX += deltaDistX;
        currentTileX += stepX;
        side = 0;
      } else {
        sideDistY += deltaDistY;
        currentTileY += stepY;
        side = 1;
      }

      if (map[(currentTileY * mapWidth) + currentTileX] > 0) hit = 1;
    }
    
    // Calculate distance projected on camera direction
    if (!side) {
      perpWallDist = (currentTileX - playerX  + (1 - stepX) / 2) / rayDirX;
    } else {
      perpWallDist = (currentTileY - playerY  + (1 - stepY) / 2) / rayDirY;
    }
    
    // Calculate line height to draw
    int lineHeight = (int)(VIEWPORT_HEIGHT / perpWallDist);

    // Calculate start and end points for line drawing
    int lineStart = -(lineHeight / 2) + (VIEWPORT_HEIGHT / 2);
    if (lineStart < 0) lineStart = 0;
    int lineEnd = (lineHeight / 2) + (VIEWPORT_HEIGHT / 2);
    if (lineEnd >= VIEWPORT_HEIGHT) lineEnd = VIEWPORT_HEIGHT - 1;


    glBegin(GL_LINES);
    switch (map[(currentTileY * mapWidth) + currentTileX]) {
    case 1:
      glColor3f(1.0, 1.0, 1.0);
      break;
    case 2:
      glColor3f(0.5, 0.5, 0.5);
      break;
    default:
      glColor3f(0.7, 0.0, 0.65);
      break;
    }
    glVertex2i(x,lineStart);
    glVertex2i(x,lineEnd);
    glEnd();
  }
}

void display(GLFWwindow* window) {
  int frameStartTime = glfwGetTime();
  int deltaTime = frameStartTime - previousFrameStartTime;
  previousFrameStartTime = frameStartTime;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  castRays();
  
  drawMap2D();
  frameTime(deltaTime);
  glfwSwapBuffers(window); // swaps our buffers around
}
