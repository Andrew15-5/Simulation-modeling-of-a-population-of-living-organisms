#include "global_things.h"
#include "settingsWindow.h"
#include "drawFunctions.h"
#include <GL/freeglut.h>
#include <string.h>
#include "helpfulFunctions.h"
#include <stdio.h>

struct
{
  GLfloat Black, White;
} Color;

extern int screen_width,
           screen_height,
           settingsWindow;
const int UPDATE_TIME = 10; // 50 fps (60 to be real)
static const bool NORMAL_TICK = true;
static double normalizer = 0.25;
static bool default_beings_number = true, // for textbox logic
            default_width         = true,
            default_height        = true,
            GDon                  = true, // graphic drawing is on by default;
            BNinFocus             = true, // is being_number's textbox in focus
            WinFocus              = false, // is width's textbox in focus
            HinFocus              = false; // is height's textbox in focus
static char beings_number_str[10] = "10", // default number of beings
            width_str[10] = "800", // default width
            height_str[10] = "800"; // default height
bool nightMode  = false,
     CwinS      = false, // custom window sizes is off by default
     fullscreen = false,
     stopfuncs  = false;
int beings_number,
    WINDOW_WIDTH, // related to main window
    WINDOW_HEIGHT,
    mainWindow;

void SetColors()
{
  Color.Black = 0;
  Color.White = 0.7;
}

GLfloat bckgndclr(){
  return nightMode ? Color.Black : Color.White;}

GLfloat clr(){
  return nightMode ? Color.White : Color.Black;}

void SettingsInitRendering(){
  glEnable(GL_COLOR_MATERIAL);}

void SettingsReshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-2, 2, -1-normalizer, 1+normalizer);
}

void ExtRetWindow()
{
  CwinS = !CwinS;
  if (CwinS)
  {
    normalizer = 0.5;
    SettingsReshape(400, 300);
    glutReshapeWindow(400, 300);
  }
  else
  {
    normalizer = 0.25;
    glutReshapeWindow(400, 250);
    SettingsReshape(400, 250);
  }
}

void SettingsDisplay()
{ 
  glClearColor(bckgndclr(), bckgndclr(), bckgndclr(), 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor3f(clr(), clr(), clr());
  drawTextLine("Enable graphic drawing", 24, 0.15, -0.7 + normalizer);
  drawTextLine("(if unchecked simulation will run much faster)", 12, 0, -0.87 + normalizer);
  drawTextLine("Enter the number of beings", 24, 0, 0.5 + normalizer);
  drawTextLine("Custom window sizes", 24, 0.07, -1.2 + normalizer);
  if (CwinS)
  {
    drawTextLine("Fullscreen", 24, 0.07, -1.6 + normalizer);
    drawTextLine("x", 24, 0, -1.9 + normalizer);
  }

  glPushMatrix();
  glTranslatef(1.8, 0.8 + normalizer, 0);
  drawTextLine("Night", 13, 0, -0.25);
  drawTextLine("Mode", 13, 0, -0.35);
  drawNightModeSwitch(0.3);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, normalizer, 0);
  drawTextBox(0.7, 0.26, BNinFocus);
  glColor3f(0, 0, 0);
  drawTextLine(beings_number_str, 24, 0, -0.08);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-1.2, -0.61 + normalizer, 0);
  drawCheckBox(0.24);
  if (GDon) drawTick(0.18, NORMAL_TICK);
  glPopMatrix();
  
  glPushMatrix(); // Custom window sizes
  glTranslatef(-1.2, -1.1 + normalizer, 0);
  drawCheckBox(0.24);
  if (CwinS) drawTick(0.18, NORMAL_TICK);
  glPopMatrix();

  if (CwinS)
  {
    glPushMatrix(); // Fullscreen
    glTranslatef(-0.65, -1.5 + normalizer, 0);
    drawCheckBox(0.24);
    if (fullscreen) drawTick(0.18, NORMAL_TICK);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.45, -1.8 + normalizer, 0);
    drawTextBox(0.7, 0.26, WinFocus);
    glColor3f(0, 0, 0);
    drawTextLine(width_str, 24, 0, -0.08);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45, -1.8 + normalizer, 0);
    drawTextBox(0.7, 0.26, HinFocus);
    glColor3f(0, 0, 0);
    drawTextLine(height_str, 24, 0, -0.08);
    glPopMatrix();

  }

  glutSwapBuffers();
}

void Continue()
{
  beings_number = atoi(beings_number_str);
  WINDOW_WIDTH = atoi(width_str);
  WINDOW_HEIGHT = atoi(height_str);
  glutDestroyWindow(settingsWindow);
  stopfuncs = true;
  if (GDon) glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  else glutInitWindowSize(800, 800);
  mainWindow = glutCreateWindow("Simulation v2");
  InitMinMaxColor();
  InitRendering();
  glutReshapeFunc(Reshape);
  glutDisplayFunc(Display);
  glutKeyboardFunc(Keyboard);
  glutMouseFunc(Mouse);
  GDon ? glutTimerFunc(UPDATE_TIME, Update, 0) : NoDrawing();
}

void SettingsKeyboard(unsigned char key, int x, int y)
{
  if (stopfuncs) return;
  if (key == 9)
  {
    if (CwinS)
    {
      if (BNinFocus)
      {
        fullscreen = false;
        BNinFocus = false;
        WinFocus = true;
      }
      else if (WinFocus)
      {
        fullscreen = false;
        WinFocus = false;
        HinFocus = true;
      }
      else // if (HinFocus)
      {
        HinFocus = false;
        BNinFocus = true;
      }
        // printf("%d\n%d %d\n", BNinFocus, WinFocus, HinFocus);
    }
  }
  else if (key == 'F' || key == 'f')
  {
    fullscreen = !fullscreen;
    if (fullscreen)
    {
      CwinS = !fullscreen; // because of ExtRetWindow();
      ExtRetWindow();
    }
  }
  else if (key == 'N' || key == 'n') nightMode = !nightMode;
  else if (key == 'C' || key == 'c')
  {
    ExtRetWindow();
    if (!CwinS)
    {
      BNinFocus = true;
      WinFocus = false;
      HinFocus = false;
      fullscreen = false;
    }
  }
  else if (key >= 'A' && key <= 'Z' || key >= 'a' && key <= 'z') GDon = !GDon;
  else if (key >= '0' && key <= '9')
  {
    if (BNinFocus) ExpandNum(beings_number_str, &default_beings_number, key);
    else if (WinFocus) ExpandNum(width_str, &default_width, key);
    else if (HinFocus) ExpandNum(height_str, &default_height, key);
  }
  else if (key == 8) 
  {
    if (BNinFocus) ShrinkNum(beings_number_str);
    else if (WinFocus) ShrinkNum(width_str);
    else if (HinFocus) ShrinkNum(height_str);
  }
  else if (key == 13) Continue();
  else exit(0);
  // printf("%d %d\n", CwinS, fullscreen);
}

void SettingsMouse(int button, int state, int x, int y)
{
  if (stopfuncs) return;
  if (state == GLUT_UP)
  {
    // printf("%d %d\n", x, y);
    if (x >= 169  && x <= 234 && y >= 92 && y <= 113)
    {
      BNinFocus = true;
      WinFocus = false;
      HinFocus = false;
      // printf("%d\n%d %d\n", BNinFocus, WinFocus, HinFocus);
    }
    if (x >= 124  && x <= 189 && y >= 272 && y <= 293 && CwinS)
    {
      fullscreen = false;
      WinFocus = true;
      BNinFocus = false;
      HinFocus = false;
      // printf("%d\n%d %d\n", BNinFocus, WinFocus, HinFocus);
    }
    if (x >= 214  && x <= 279 && y >= 272 && y <= 293 && CwinS)
    {
      fullscreen = false;
      HinFocus = true;
      BNinFocus = false;
      WinFocus = false;
      // printf("%d\n%d %d\n", BNinFocus, WinFocus, HinFocus);
    }
    if (x >= 85 && x <= 258 && y >= 217 && y <= 263 && CwinS) fullscreen = !fullscreen;
    if (x >= 70  && x <= 330 && y >= 148 && y <= 174) GDon = !GDon;
    if (x >= 367  && x <= 396 && y >= 9 && y <= 37) nightMode = !nightMode;
    if (x >= 70  && x <= 314 && y >= 174 && y <= 222)
    {
      fullscreen = CwinS;
      ExtRetWindow();
    }
  }
}

void SettingsUpdate(int value)
{
  if (stopfuncs) return;
  glutPostRedisplay();
  glutTimerFunc(1, SettingsUpdate, 0);
}