// Copyright (C) 2019-2023  Andrew Voynov
#ifndef BEING_STRUCT_H
#define BEING_STRUCT_H

typedef struct Being Being;

enum STATUS {Idle,Hunting,Running,Fighting,Dead};

struct Being
{
  double Audacity,
         DOV,// Distance of view (starts where body ends)
         Direction,
         FullDOV,// Distance of view (starts at the center of the body)
         FOV,// Field of view
         RunCooldown,
         Speed,
         Weight,
         x,
         y;
  int FocusedOn,
      me,
      Status;
};

struct MinStruct
{
  int x, y;
  int Weight;
  double Audacity, FOV, DOV;//dov without weight diameter
};

struct MaxStruct
{
  int x, y;
  int Weight;
  double Audacity, FOV, DOV;//fov max 179; dov without weight diameter
};

double LeftFOVAngle(Being b);
double RightFOVAngle(Being b);
double RunSpeed(Being b);
double WalkSpeed(Being b);
double CalculateWeight(Being b);
double CalculateDOV(Being b);
double FOVCenterLineX(Being b);
double FOVCenterLineY(Being b);
double FOVLeftLineX(Being b);
double FOVLeftLineY(Being b);
double FOVRightLineX(Being b);
double FOVRightLineY(Being b);
bool BeingIsInMyFOV(Being b, Being other);
int WhichBeingIsInMyFOV(int me, Being *beings);
bool VictimIsInMyFOV(Being b, Being *beings);
void SetDirection(Being *b, double direction);
void RunAwayFromBeing(Being *b, Being other);
void HuntAfterVictim(Being *b, Being *beings);
bool NobodyIsFocusedOnMe(int me, Being *beings);
void ForgetVictim(Being *b);
Being Clone(Being original);

// Other
bool FightStarted(Being first, Being second);
void FightConsequences(Being *winner, Being *loser);
double AudacityPercent(Being b);
double WeightPercent(Being b);
void Fight(Being *first, Being *second);

// Initialize struct fields with random Value in interval [Min.Value; Max.Value]
void InitStruct(Being *b, int index);

#endif /* BEING_STRUCT_H */
