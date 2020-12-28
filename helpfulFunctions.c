#include "global_things.h"
#include "helpfulFunctions.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dSFMT.h"

extern dsfmt_t dsfmt;
static const int DECIMALS_AFTER_POINT = 15;
static const double RANDOM_WEIGHT_COEFFICIENT = 0.1;
static double m, l, r;

// For struct
inline double getSpeed(double weight)
{
  r = 7.7 * log(weight / 2 + 1) * exp(-weight / 10 - 1);
  return r < 0 ? 0 : r;
}
inline double randf()
{
  return dsfmt_genrand_close_open(&dsfmt);
}
inline double randF(double min, double max)
{
  return randf() * (max - min) + min;
}
inline double xMove(double angle, double speed)
{
  return cos(angle) * speed;
}
inline double yMove(double angle, double speed)
{
  return sin(angle) * speed;
}
inline double distanceSquared(double x1, double y1, double x2, double y2){
  return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1- y2);}
inline bool PointInTriangle(double Ax, double Ay, double Bx, double By,
                     double Cx, double Cy, double Px, double Py)
{
  Bx -= Ax;
  By -= Ay;
  Cx -= Ax;
  Cy -= Ay;
  Px -= Ax;
  Py -= Ay;
  m = (Px * By - Bx * Py) / (Cx * By - Bx * Cy);
  if (m >= 0 && m <= 1)
  {
    l = (Px - m * Cx) / Bx;
    if (l >= 0 && m + l <= 1) return true;
  }
  return false;
}
inline double ChangeDirection1(double minAngle, double maxAngle)
{
  return randF(minAngle, maxAngle);
}
inline double ChangeDirection2(double minAngle1, double maxAngle1, double minAngle2, double maxAngle2)
{
  return randf() < 0.5 ? randF(minAngle1, maxAngle1) : randF(minAngle2, maxAngle2);
}
inline double RandomPercent()
{
  return randf() * RANDOM_WEIGHT_COEFFICIENT;
}

// For settings window
// b = beings_number_str
// d = default_beings_number
inline void ExpandNum(char *b, bool *d, char k)
{
  if (strlen(b) == 5) return;
  char key = k;
  if (*d)
  {
    key == '0' ? strcat(b, &key) : strcpy(b, &key);
    *d = false;
  }
  else
  {
    strcmp(b, "0") == 0 ?
    strcpy(b, &key) :
    strcat(b, &key);
  }
}
// b = beings_number_str
inline void ShrinkNum(char *b)
{
  b[strlen(b)-1] = 0;
  if (b[0] == 0) strcpy(b, "0");
}

// For main window
inline int len(int x) { 
    if (x >= 10)         return 2;
    if (x >= 100)        return 3;
    if (x >= 1000)       return 4;
    if (x >= 10000)      return 5;
    if (x >= 100000)     return 6;
    if (x >= 1000000)    return 7;
    if (x >= 10000000)   return 8;
    if (x >= 100000000)  return 9;
    if (x >= 1000000000) return 10;
    return 1;
}