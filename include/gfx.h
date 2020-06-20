#pragma once

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define VIEWPORT_WIDTH 1280
#define VIEWPORT_HEIGHT 720

// Player position, deltas, and angle
extern float playerX,    playerY,
             playerDirX, playerDirY,
             planeX,     planeY;

void drawMap2D();
void castRays();
void display();