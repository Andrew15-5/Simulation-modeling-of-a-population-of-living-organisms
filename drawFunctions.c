// Copyright (C) 2019-2023  Andrew Voynov
#define PI 3.1415926535
#include "global_things.h"
#include "drawFunctions.h"
#include <GL/freeglut.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// For settings window
inline void drawTextLine(char *str, char size, double x, double y)
{
  double SCALE = 0.00497;
  double Xoffset = 0;
  void *font;
  switch(size)
  {
    case 10:
      font = GLUT_BITMAP_TIMES_ROMAN_10;
      break;
    case 11:
      font = GLUT_BITMAP_HELVETICA_10;
      break;
    case 12:
      font = GLUT_BITMAP_HELVETICA_12;
      break;
    case 13:
      font = GLUT_BITMAP_8_BY_13;
      break;
    case 15:
      font = GLUT_BITMAP_9_BY_15;
      break;
    case 18:
      font = GLUT_BITMAP_HELVETICA_18;
      break;
    case 24:
      font = GLUT_BITMAP_TIMES_ROMAN_24;
      break;
  }
  for (int i = 0; i < strlen(str); i++)
    Xoffset -= glutBitmapWidth(font, str[i])*SCALE;
  glRasterPos2f(x+Xoffset, y);
  for (int i = 0; i < strlen(str); i++)
    glutBitmapCharacter(font, str[i]);
}
inline void drawSquare(double size)
{
  glBegin(GL_QUADS);
  glVertex2f(-size/2, -size/2);
  glVertex2f(size/2, -size/2);
  glVertex2f(size/2, size/2);
  glVertex2f(-size/2, size/2);
  glEnd();
}
inline void drawCheckBox(double size)
{
  glColor3f(0.5, 0.5, 0.5);
  drawSquare(size);
  glColor3f(1, 1, 1);
  drawSquare(size - 0.04);
}

inline void drawCrescent(double size)
{
  glPushMatrix();
  drawCircle(0, 0, size/2);
  glTranslatef(0.02, 0, 0);
  glColor3f(0, 0, 0);
  drawCircle(0, 0, size*2/5);
  glPopMatrix();
}
inline void drawNightModeSwitch(double size)
{
  glColor3f(0.5, 0.5, 0.5);
  drawSquare(size);
  glColor3f(0, 0, 0);
  drawSquare(size - 0.02);
  glColor3f(0.5, 0.5, 0.5);
  drawCrescent(size - 0.1);
}
inline void drawRectangle(double w, double h)
{
  glBegin(GL_QUADS);
  glVertex2f(-w/2, -h/2);
  glVertex2f(w/2, -h/2);
  glVertex2f(w/2, h/2);
  glVertex2f(-w/2, h/2);
  glEnd();
}
inline void drawTextBox(double w, double h, bool inFocus)
{
  glColor3f(0.5, 0.5, 0.5);
  drawRectangle(w, h);
  if (inFocus) glColor3f(1, 1, 1); else glColor3f(0.8, 0.8, 0.8);
  drawRectangle(w - 0.04, h - 0.04);
}
inline void drawTick(double size, bool normal)
{
  glColor3f(0, 0.8, 0);
  if (normal)
  {
    glBegin(GL_POLYGON);
    glVertex2f(-size/6, -size/2);
    glVertex2f(size/2, size/6);
    glVertex2f(size/3, size/3);
    glVertex2f(-size/6, -size/5);
    glVertex2f(-size/2.8, -size/40);
    glVertex2f(-size/2, -size/6);
    glEnd();
  } else drawSquare(size);
}

// For main window
inline void drawFOV(double x, double y, double r, double Lang,
             double Rang, float *color)
{
  glColor3fv(color);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x, y);
  for (double a = Rang; a <= Lang; a += (Lang - Rang) / 100)
    glVertex2f(x + cos(a) * r, y + sin(a) * r);
  glVertex2f(x, y);
  glEnd();
}
inline void drawCircle(double x, double y, double r)
{
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(x, y);
  for (double a = 0; a <= 2 * PI; a += 2 * PI / 32)
    glVertex2f(x + cos(a) * r, y + sin(a) * r);
  glEnd();
}
