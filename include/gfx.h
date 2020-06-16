#pragma once

#define PI 3.14159265358979323846
#define DEGREE ((2 * PI)/360) // one degree in radians
#define ROUND_TO_MULTIPLE_OF_64(x) (((int)x >> 6) << 6)
#define VIEWPORT_WIDTH 1024
#define VIEWPORT_HEIGHT 512

// Player position, deltas, and angle
extern float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle;

void drawPlayer();
void drawMap2D();
void castRays();
void display();