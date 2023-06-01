﻿#pragma once
#include "Config.h"

//-------------------------------------------------------------------------------------------------------------------------
enum class EBall_State: unsigned char
{
   Disabled, // Отключен, не рисуется и тд

   Normal,
   Lost,
   On_Platform,
   On_Parachute,
   Off_Parachute,
   Teleporting
};
//-------------------------------------------------------------------------------------------------------------------------
class ABall: public AGame_Object
{
public:
   ABall();

   virtual void Act();
   virtual void Clear(HDC hdc, RECT& paint_area);
   virtual void Draw(HDC hdc, RECT& paint_area);
   virtual bool Is_Finished();

   virtual void Begin_Movement();
   virtual void Finish_Movement();
   virtual void Advance(double max_speed);
   virtual double Get_Speed();

   void Set_Speed(double new_speed);
   void Draw_Teleporting(HDC hdc, int step);
   void Set_For_Test();
   bool Is_Test_Finished();
   EBall_State Get_State();
   void Set_State(EBall_State new_state, double x_pos = 0, double y_pos = 0);
   void Get_Center(double& x_pos, double& y_pos);
   double Get_Direction();
   void Set_Direction(double new_derection);
   void Reflect(bool from_horizontal);
   bool Is_Moving_Up();
   bool Is_Moving_Left();
   void Set_On_Parachute(int brick_x, int brick_y);
   void Forced_Advance(double direction, double speed, double max_speed);
   void Release();

   int Release_Timer_Tick;

   static AHit_Checker_List Hit_Checker_List;

   static const double Radius;

private:
   void Redraw_Ball();
   void Draw_Parachute(HDC hdc, RECT &paint_area);
   void Redraw_Parachute();
   void Clear_Parachute(HDC hdc);

   EBall_State Ball_State, Prev_Ball_State;

   double Ball_Speed, Prev_Ball_Speed;
   double Center_X_Pos;
   double Center_Y_Pos;
   //double Rest_Distance;
   double Ball_Direction, Prev_Ball_Direction;

   bool Testing_Is_Active;
   int Test_Iteration;
   double Rest_Test_Distance;

   RECT Ball_Rect, Prev_Ball_Rect;
   RECT Parachute_Rect, Prev_Parachute_Rect;

   static const int Parachute_Size = 15;
   static const int On_Platform_Timeout = 10 * AsConfig::FPS;
};
//-------------------------------------------------------------------------------------------------------------------------