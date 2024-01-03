// Copyright (C) 2019-2023  Andrew Voynov
#include "global_things.h"
#include "settingsWindow.h"
#include "drawFunctions.h"
#include "being_struct.h"
#include "helpfulFunctions.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <GL/freeglut.h>
#include <string.h>
#include "dSFMT.h"

struct MinStruct Min;
struct MaxStruct Max;

static struct
{
  GLfloat Idle[3], Hunting[3], Running[3], Black[3], White[3];
} Color;

extern int WINDOW_WIDTH,
           WINDOW_HEIGHT;
extern const int UPDATE_TIME;// set in settings window
extern int beings_number,
           mainWindow;
extern bool nightMode,
            fullscreen;
static const int RUN_COOLDOWN_TIME = 2000;// in miliseconds
static const int AUDACITY_HIGH_THRESHOLD = 75;
static const double AUDACITY_INCREASING = 1.01,
                    CRITICAL_WEIGHT     = 2.8,
                    SUFFERING_POINT     = 5,
                    LOW_WEIGHT          = 10;
static Being *beings, *being, *stranger, *beings_to_draw;
static bool FOVisVisible = true,
            new_fps      = false;
static int beings_alive,
           loop_counter = 0,
           slen         = 0,
           frame_counter = 0,
           firstT,
           secondT,
           execT,
           fps = 0;
static FILE *f;
static char **strings;
static double xoffset, yoffset;
static pthread_mutex_t mutex;
int RUN_COOLDOWN,
    screen_width,
    screen_height,
    settingsWindow;
dsfmt_t dsfmt;
extern GLfloat bckgndclr();
extern GLfloat clr();

void CloseApplication()
{
  for (int i = 0; i < slen; i++)
    free(strings[i]);
  free(strings);
  free(beings);
  free(beings_to_draw);
  exit(0);
}

char * str(int t, int a)
{
  char *sta = malloc(sizeof(char*)*(len(t)+len(a)+1));
  sprintf(sta, "%d %d", t, a);
  return sta;
}

void AddString(char * a)
{
  char **newS = realloc(strings, sizeof(char*)*(slen+1));
  if (!newS)
  {
    printf("Error");
    CloseApplication();
  }
  strings = newS;
  slen++;
  strings[slen - 1] = malloc(strlen(a));
  strings[slen - 1] = a;
}

void InitMinMaxColor()
{
  Min.Weight = 15;
  Min.Audacity = 70;
  Min.FOV = 80 / 180.0 * PI;
  Min.DOV = 40;
  Min.x  = 0;
  Min.y = 0;
  Max.Weight = 30;
  Max.Audacity = 100;
  Max.FOV = 130 / 180.0 * PI;
  Max.DOV = 60;
  Max.x = WINDOW_WIDTH;
  Max.y = WINDOW_HEIGHT;
  for (int i = 0; i < 3; i++)
  {
    Color.Idle[i]    = i == 1 ? 0.7 : 0;
    Color.Hunting[i] = i == 0 ? 0.7 : 0;
    Color.Running[i] = i == 2 ? 0.7 : 0;
    Color.Black[i] = 0;
    Color.White[i] = 1;
  }
}

void InitBeings()
{
  // printf("%d %d\n", Max.x, Max.y);
  execT = time(NULL);
  beings_alive = beings_number;
  RUN_COOLDOWN = RUN_COOLDOWN_TIME / UPDATE_TIME;
  beings = malloc(beings_number * sizeof(Being));
  beings_to_draw = malloc(beings_number * sizeof(Being));
  for (int i = 0; i < beings_number; i++)
  {
    InitStruct(&beings[i], i);
    beings_to_draw[i] = Clone(beings[i]);
  }
}

void *DrawThread()
{
  while (true)
  {
    glutPostRedisplay();
  }
}

void InitRendering()
{
  if (fullscreen) glutFullScreen();
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  pthread_t thrID;
  pthread_create(&thrID, NULL, DrawThread, NULL);
  // pthread_join(thrID, NULL);
}

void Reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  WINDOW_WIDTH = glutGet(GLUT_WINDOW_WIDTH);
  WINDOW_HEIGHT = glutGet(GLUT_WINDOW_HEIGHT); 
  Max.x = WINDOW_WIDTH;
  Max.y = WINDOW_HEIGHT; 
  gluOrtho2D(Min.x, Max.x, Min.y, Max.y);
  InitBeings();
}

float* st(int status)
{
  switch (status)
  {
    case Idle:
      return Color.Idle;
      break;
    case Hunting:
      return Color.Hunting;
      break;
    case Running:
      return Color.Running;
      break;
  }
}

void Display()
{ 
  Color.Running[1] = nightMode ? 0.4 : 0;
  glClearColor(bckgndclr(), bckgndclr(), bckgndclr(), 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLineWidth(2);
  glLoadIdentity();
  if (FOVisVisible)
    for (int i = 0; i < beings_number; i++)
    {
      Being b = beings_to_draw[i];
      if (b.Status != Dead)
        drawFOV(b.x, b.y, b.FullDOV, LeftFOVAngle(b), RightFOVAngle(b), st(b.Status));
    }
  glColor3f(clr(), clr(), clr());
  for (int i = 0; i < beings_number; i++)
  {
    Being b = beings_to_draw[i];
    if (b.Status != Dead)
      drawCircle(b.x, b.y, b.Weight);
  }
  char s[8];

  if (new_fps)
  {
    fps = frame_counter;
    frame_counter = 0;
    new_fps = false;
  }
  sprintf(s, "FPS: %d", fps);
  // drawTextLine(s, 24, WINDOW_WIDTH - 80, WINDOW_HEIGHT - 20);
  
  glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
  if (key == 'N' || key == 'n') nightMode = !nightMode;
  else CloseApplication();
}

void Mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    FOVisVisible = !FOVisVisible;
}

void Logic()
{
  loop_counter++;
  AddString(str(loop_counter, beings_alive));
  for (int i = 0; i < beings_number; i++)
  {
    being = &beings[i];
    if (being->Status == Dead) continue;
    if (being->Status != Fighting)
    {
      if (being->Weight < SUFFERING_POINT) being->DOV *= 1 - 1 / (100 * being->Weight);
      being->Weight = CalculateWeight(*being);
      being->FullDOV = being->DOV + being->Weight / 2;
    }//If moving (not fighting): edit DOV, decrease weight
    if (being->Weight < CRITICAL_WEIGHT)
    {
      being->Status = Dead;
      beings_alive--;
    }// If too small: dead
    if (being->Weight < LOW_WEIGHT && being->Audacity * AUDACITY_INCREASING <= 100){
      being->Audacity *= AUDACITY_INCREASING;
    }//If weight is low: increase audacity
    int someone = WhichBeingIsInMyFOV(i, beings);
    if (someone != -1)
    {
      stranger = &beings[someone];
      if (being->Status == Idle)
      {
        if (being->Audacity >= AUDACITY_HIGH_THRESHOLD && being->Weight >= stranger->Weight && NobodyIsFocusedOnMe(stranger->me, beings))
        {
          being->FocusedOn = stranger->me;
          being->Status = Hunting;
        } else being->Status = Running;
      }// If not(running or hunting): run or hunt
      if (being->Status == Running)
      {
        being->RunCooldown = RUN_COOLDOWN;
        RunAwayFromBeing(being, *stranger);
      }// If running: run from stranger, reset run cooldown
      if (being->Status == Hunting && VictimIsInMyFOV(*being, beings))      {
        HuntAfterVictim(being, beings);
      }// if status hunting and victim is in my fov: hunt after victim
      if (being->Status == Hunting || being->Status == Running)
      {
        if (FightStarted(*being, *stranger))
        {
          beings_alive--;
          Fight(being, stranger);
        } else being->Speed = RunSpeed(*being);
      }// If running or hunting: if fightstarted: fight else speed = run
    }// If see someone
    if (being->Status == Running){//If running: if RunCooldown <= 0: idle
      if (being->RunCooldown > 0) being->RunCooldown--; else being->Status = Idle;}
    if (being->Status == Hunting && !VictimIsInMyFOV(*being, beings))
      ForgetVictim(being);//if hunter loses victim: he forgets victim
    if (being->Status == Idle)
    {
      if (randf() < 0.01) being->Direction = randf() * 2 * PI;
      being->Speed = WalkSpeed(*being);
    }// If idle: speed = walk (maybe in the other direction)
    if (being->Status != Fighting && being->Status != Dead)
    {
      xoffset = xMove(being->Direction, being->Speed),
      yoffset = yMove(being->Direction, being->Speed);

      if (FOVLeftLineX(*being)   + xoffset < Min.x ||
          FOVCenterLineX(*being) + xoffset < Min.x ||
          FOVRightLineX(*being)  + xoffset < Min.x)
        being->Direction = ChangeDirection2(0, 80 / 180.0 * PI, 280 / 180.0 * PI, 2 * PI);
      else
      if (FOVLeftLineX(*being)   + xoffset > Max.x ||
          FOVCenterLineX(*being) + xoffset > Max.x ||
          FOVRightLineX(*being)  + xoffset > Max.x)
        being->Direction = ChangeDirection1(100 / 180.0 * PI, 260 / 180.0 * PI);
      else being->x += xoffset;
      if (FOVLeftLineY(*being)   + xoffset < Min.y ||
          FOVCenterLineY(*being) + xoffset < Min.y ||
          FOVRightLineY(*being)  + xoffset < Min.y)
        being->Direction = ChangeDirection1(10 / 180.0 * PI, 170 / 180.0 * PI);
      else
      if (FOVLeftLineY(*being)   + xoffset > Max.y ||
          FOVCenterLineY(*being) + xoffset > Max.y ||
          FOVRightLineY(*being)  + xoffset > Max.y)
        being->Direction = ChangeDirection1(190 / 180.0 * PI, 350 / 180.0 * PI);
      else being->y += yoffset;
    }// If moving (not(fighting or dead)): find out where to move
  }
  pthread_mutex_lock(&mutex);
  for (int i; i < beings_number; i++)
    beings_to_draw[i] = Clone(beings[i]);
  pthread_mutex_unlock(&mutex);
}

void SaveData()
{
  if (beings_number)
  {
    AddString(str(loop_counter, beings_alive));
    f = fopen("text.txt", "w+");
    for (int i = 0; i < slen; i++)
      fprintf(f, "%s\n", strings[i]);
    fclose(f);
  }
  int sec = time(NULL) - execT;
  printf("%d - %d'%d\"\n", beings_number, sec/60, sec%60);
  CloseApplication();
}

void NoDrawing()
{
  InitBeings();
  while (beings_alive > 0)
    Logic();
  SaveData();
  if ((time(NULL) - execT) % 60 == 0) printf("%d\n", (int)(time(NULL) - execT) / 60);
}

void Update(int value)
{
  Logic();
  if (!beings_alive) SaveData();
  frame_counter++;
  secondT = time(NULL);
  if (secondT - firstT >= 1)
  {
    firstT = secondT;
    new_fps = true;
  }
  glutTimerFunc(UPDATE_TIME, Update, 0);
}

void setScreenWH(char **argv)
{
  switch (*argv[1])
  {
    case '1':
       screen_width= 1920, screen_height = 1080;// Laptop monitor
      break;
    case '2':
      screen_width = 1600, screen_height = 900;// External monitor
      break;
    case '3':// 3-4 if resolutions are maxed out
      screen_width = 1920, screen_height = 870;// 2 monitors show on external
      break;
    case '4':
      screen_width = 1920, screen_height = 2830;// 2 monitors show on laptop
      break;
    case '5':// 5-6 if resolutions are both equal min(laptop, external)
      screen_width = 1600, screen_height = 870;// 2 monitors show on external
      break;
    case '6':
      screen_width = 1600, screen_height = 2670;// 2 monitors show on laptop
      break;
  }
}

int main(int argc, char **argv)
{
  dsfmt_init_gen_rand(&dsfmt, 12345);
  srand(time(NULL));
  strings = malloc(sizeof(**strings));
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  int w = 400, h = 250;// Settings window size
  glutInitWindowSize(w, h);
  settingsWindow = glutCreateWindow("Simulation v2 settings");
  SetColors();
  SettingsInitRendering();
  glutReshapeFunc(SettingsReshape);
  glutDisplayFunc(SettingsDisplay);
  glutMouseFunc(SettingsMouse);
  glutKeyboardFunc(SettingsKeyboard);
  firstT = time(NULL);
  glutTimerFunc(1, SettingsUpdate, 0);
  glutMainLoop();
  return 0;
}
