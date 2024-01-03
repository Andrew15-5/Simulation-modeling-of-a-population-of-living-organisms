// Copyright (C) 2019-2023  Andrew Voynov
#include "global_things.h"
#include "being_struct.h"
#include "helpfulFunctions.h"
#include <math.h>
#include <stdio.h>

extern int RUN_COOLDOWN,
           beings_number;
// extern inline double distanceSquared(double x1, double y1, double x2, double y2);
const double AUDACITY_WEIGHT_COEFFICIENT = 0.45,
             EATABLE_PART                = 0.8,
             WEIGHT_LOSE_SPEED           = 0.999,
             WEIGHT_WEIGHT_COEFFICIENT   = 0.45;
const int AUDACITY_LOW_THRESHOLD = 25;
static int distSqr;

extern struct MinStruct Min;
extern struct MaxStruct Max;

inline double LeftFOVAngle(Being b){
  return b.Direction + b.FOV / 2;}
inline double RightFOVAngle(Being b){
  return b.Direction - b.FOV / 2;}
inline double RunSpeed(Being b){
  return getSpeed(b.Weight);}
inline double WalkSpeed(Being b){
  return  randF(0.4, 0.6) *  getSpeed(b.Weight);}
inline double CalculateWeight(Being b){
  return b.Weight * (WEIGHT_LOSE_SPEED - b.Speed / 3000);}
inline double FOVCenterLineX(Being b){
  return b.x + xMove(b.Direction, b.FullDOV);}
inline double FOVCenterLineY(Being b){
  return b.y + yMove(b.Direction, b.FullDOV);}
inline double FOVLeftLineX(Being b){
  return b.x + xMove(LeftFOVAngle(b), b.FullDOV);}
inline double FOVLeftLineY(Being b){
  return b.y + yMove(LeftFOVAngle(b), b.FullDOV);}
inline double FOVRightLineX(Being b){
  return b.x + xMove(RightFOVAngle(b), b.FullDOV);}
inline double FOVRightLineY(Being b){
  return b.y + yMove(RightFOVAngle(b), b.FullDOV);}
inline double FOVLeftLineXext(Being b){
  return b.x + xMove(LeftFOVAngle(b), 8000);}
inline double FOVLeftLineYext(Being b){
  return b.y + yMove(LeftFOVAngle(b), 8000);}
inline double FOVRightLineXext(Being b){
  return b.x + xMove(RightFOVAngle(b), 8000);}
inline double FOVRightLineYext(Being b){
  return b.y + yMove(RightFOVAngle(b), 8000);}
inline bool BeingIsInMyFOV(Being b, Being other)
{
  if (other.x < b.x - b.FullDOV || other.x > b.x + b.FullDOV ||
      other.y < b.y - b.FullDOV || other.y > b.y + b.FullDOV) return false;
  distSqr = distanceSquared(b.x, b.y, other.x, other.y);
  return distSqr <= (b.FullDOV + other.Weight / 2) * (b.FullDOV + other.Weight / 2) && // Far
         PointInTriangle(FOVLeftLineXext(b),  FOVLeftLineYext(b),
                         FOVRightLineXext(b), FOVRightLineYext(b),
                         b.x, b.y, other.x, other.y) ||
         distSqr * 4 <= (b.Weight + other.Weight) * (b.Weight + other.Weight); // Close
}
inline int WhichBeingIsInMyFOV(int me, Being *beings)
{
  for (int i = 0; i < beings_number; i++)
    if (beings[i].Status != Dead && BeingIsInMyFOV(beings[me],beings[i]) && i != me) return i;
  return -1;
}
inline bool VictimIsInMyFOV(Being b, Being *beings){
  return beings[b.FocusedOn].Status != Dead && BeingIsInMyFOV(b, beings[b.FocusedOn]);}
inline void SetDirection(Being *b, double direction){
  b->Direction = b->Direction < 0 ? direction + 2 * PI : direction;}
inline void RunAwayFromBeing(Being *b, Being other){
  SetDirection(b, atan2(b->y - other.y, b->x - other.x));}
inline void HuntAfterVictim(Being *b, Being *beings){
  SetDirection(b, atan2(beings[b->FocusedOn].y - b->y, beings[b->FocusedOn].x - b->x));}
inline bool NobodyIsFocusedOnMe(int me, Being *beings)
{
  for (int i = 0; i < beings_number; i++)
    if (beings[i].FocusedOn == me && i != me) return false;
  return true;
}
inline void ForgetVictim(Being *b)
{
    b->FocusedOn = -1;
    b->Status = Idle;
}
inline Being Clone(Being original)
{
  Being b;
  b.Audacity = original.Audacity;
  b.DOV = original.DOV;
  b.Direction = original.Direction;
  b.FOV = original.FOV;
  b.FullDOV = original.FullDOV;
  b.FocusedOn = original.FocusedOn;
  b.me = original.me;
  b.RunCooldown = original.RunCooldown;
  b.Speed = original.Speed;
  b.Status = original.Status;
  b.Weight = original.Weight;
  b.x = original.x;
  b.y = original.y;
  return b;
}

//Other
inline bool FightStarted(Being first, Being second)
{
  if (distanceSquared(first.x, first.y, second.x, second.y) >
      fmax(first.Weight, second.Weight) *
      fmax(first.Weight, second.Weight) / 4) return false;
  return first.Status == Running ? second.FocusedOn == first.me : first.FocusedOn == second.me;
}
inline void FightConsequences(Being *winner, Being *loser)
{
  winner->Weight += loser->Weight * EATABLE_PART;
  winner->FullDOV = winner->DOV + winner->Weight / 2;
  loser->Status = Dead;
  winner->FocusedOn = -1;
  winner->Status = Idle;
}
inline double AudacityPercent(Being b)
{
  return (b.Audacity - Min.Audacity) / (Max.Audacity - Min.Audacity) * AUDACITY_WEIGHT_COEFFICIENT;
}
inline double WeightPercent(Being b)
{
    return (b.Weight - Min.Weight) / (Max.Weight - Min.Weight) * WEIGHT_WEIGHT_COEFFICIENT;
}
inline void Fight(Being *first, Being *second)
{
  if (first->Audacity > AUDACITY_LOW_THRESHOLD && AudacityPercent(*first) + WeightPercent(*first) + RandomPercent() > 0.5)
      FightConsequences(first, second);//first win
  else FightConsequences(second, first);//second win
}


inline void InitStruct(Being *b, int index)
{
  b->Audacity = randF(Min.Audacity, Max.Audacity);
  b->Direction = randf() * 2 * PI;
  b->FOV = randF(Min.FOV, Max.FOV);
  b->FocusedOn = -1;
  b->me = index;
  b->RunCooldown = RUN_COOLDOWN;
  b->Weight = randF(Min.Weight, Max.Weight);
  b->DOV = randF(Min.DOV, Max.DOV);
  b->FullDOV = b->DOV + b->Weight / 2;
  b->Speed = WalkSpeed(*b);
  b->Status = Idle;
  b->x = randf() * (Max.x - b->FullDOV * 2) + b->FullDOV;
  b->y = randf() * (Max.y - b->FullDOV * 2) + b->FullDOV;
}
