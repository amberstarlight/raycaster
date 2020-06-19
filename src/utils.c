#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "utils.h"

// pythagorean theorem
//     /|
//  c /_| b
//     a
float length(float ax, float ay, float bx, float by) {
  return ( sqrt((bx - ax) * (bx - ax) + (by-ay) * (by - ay)) );
}

void frameTime(int frameTime) {
  // Draw blue text at screen coordinates (100, 120), where (0, 0) is the top-left of the
  // screen in an 18-point Helvetica font
  char stringToPrint[256];
  sprintf(stringToPrint, "FPS: %i, Last frame: %ims", 1000/frameTime,frameTime);
  glColor3f(1, 1, 1);
  glRasterPos2i(520, 500);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)stringToPrint);
}