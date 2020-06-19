#pragma once

#define PI 3.14159265358979323846
#define TAU 6.28318530718 // 2 pi
#define DEGREE 0.0174532925199 // one degree in radians
#define ROUND_TO_MULTIPLE_OF_64(x) (((int)x >> 6) << 6)
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 512

#define VIEWPORT_WIDTH 680
#define VIEWPORT_HEIGHT 480

// Player position, deltas, and angle
extern float playerX, playerY, playerDeltaX, playerDeltaY, playerAngle;

void drawPlayer();
void drawMap2D();
void castRays();
void display();