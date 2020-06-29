#pragma once

#define WINDOW_WIDTH 1920 
#define WINDOW_HEIGHT 1080

#define VIEWPORT_WIDTH 1920
#define VIEWPORT_HEIGHT 1080

// Player position, deltas, and angle
extern float playerX,    playerY,
             playerDirX, playerDirY,
             planeX,     planeY;

void drawMap2D();
void castRays();
void display();