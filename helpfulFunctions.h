#ifndef HELPFULFUNCTIONS_H
#define HELPFULFUNCTIONS_H

#define PI 3.1415926535

// For struct
double getSpeed(double weight);
double randf();
double randF(double min, double max);
double xMove(double angle, double speed);
double yMove(double angle, double speed);
double distanceSquared(double x1, double y1, double x2, double y2);
bool PointInTriangle(double Ax, double Ay, double Bx, double By,
                     double Cx, double Cy, double Px, double Py);
double ChangeDirection1(double minAngle, double maxAngle);
double ChangeDirection2(double minAngle1, double maxAngle1, double minAngle2, double maxAngle2);
double RandomPercent();

// For settings window
void ExpandNum(char *b, bool *d, char k);
void ShrinkNum(char *b);

// For main window
int len(int x);

#endif /* HELPFULFUNCTIONS_H */
