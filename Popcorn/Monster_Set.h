#pragma once

#include "Border.h"

//-------------------------------------------------------------------------------------------------------------------------
enum class EEye_State : unsigned char
{
   Closed,
   Openning,
   Staring,
   Closing,
};
//-------------------------------------------------------------------------------------------------------------------------
enum class EMonster_State : unsigned char
{
   Missing,
   Emitting,
   Alive,
   Destroing
};
//-------------------------------------------------------------------------------------------------------------------------
enum class EExplosive_Ball_State : unsigned char
{
   Idle,
   Charging,
   Expanding,
   Fading
};
//-------------------------------------------------------------------------------------------------------------------------
class AExplosive_Ball: public AGraphics_Object
{
public:
   AExplosive_Ball();

   virtual void Act();
   virtual void Clear(HDC hdc, RECT& paint_area);
   virtual void Draw(HDC hdc, RECT& paint_area);
   virtual bool Is_Finished();

   void Explode(int x_pos, int y_pos, int size, bool is_red, int time_offset, int step_count);

   static void Setup_Color();

private:
   void Update_Ball_Rect();

   EExplosive_Ball_State Explosive_Ball_State;

   int X_Pos, Y_Pos;
   int Step_Count;
   double Size, Max_Size, Size_Step;
   int Start_Expanding_Tick, Start_Fading_Tick;
   int Time_Offset;
   bool Is_Red;
   RECT Ball_Rect;

   static const int Fading_Timeout = AsConfig::FPS;
   static const int Max_Fade_Step = AsConfig::FPS;
   static AColor Fading_Orange_Colors[Max_Fade_Step];
   static AColor Fading_Blue_Colors[Max_Fade_Step];
};
//-------------------------------------------------------------------------------------------------------------------------
class AMonster : public AGame_Object
{
public:
   AMonster();

   virtual void Begin_Movement();
   virtual void Finish_Movement();
   virtual void Advance(double max_speed);
   virtual double Get_Speed();

   virtual void Act();
   virtual void Clear(HDC hdc, RECT& paint_area);
   virtual void Draw(HDC hdc, RECT& paint_area);
   virtual bool Is_Finished();

   void Activate(int x_pos, int y_pos, bool moving_right);
   bool Is_Active();
   void Destroy();

   static const int Width = 16;
   static const int Height = 16;

private:
   void Draw_Alive(HDC hdc);
   void Draw_Destroing(HDC hdc, RECT &paint_area);
   void Act_Alive();
   void Act_Destroing();
   void Redraw_Monster();

   EEye_State Eye_State;
   EMonster_State Monster_State;
   double X_Pos, Y_Pos;
   double Cornea_Height;
   int Start_Blink_Timeout, Total_Animation_Timeout;
   double Speed, Direction;
   int Next_Direction_Switch_Tick;
   int Alive_Timer_Tick;
   RECT Monster_Rect, Prev_Monster_Rect;

   static const int Blink_Stages_Count = 7;
   static const int Explosive_Balls_Count = 20;

   int Blink_Ticks[Blink_Stages_Count];
   AExplosive_Ball Explosive_Balls[Explosive_Balls_Count];

   static const double Max_Cornea_Height;
   static const double Blinks_Timeouts[Blink_Stages_Count];
   static const EEye_State Blink_States[Blink_Stages_Count];
};
//-------------------------------------------------------------------------------------------------------------------------
enum class EMonster_Set_State : unsigned char
{
   Idle,
   Select_Next_Gate,
   Waiting_Gate_Opening,
   Waiting_Gate_Closing
};
//-------------------------------------------------------------------------------------------------------------------------
class AsMonster_Set : public AGame_Objects_Set
{
public:
   AsMonster_Set();

   virtual void Act();

   void Init(AsBorder *border);
   void Emit_At_Gate(int gate_index);
   void Activate(int max_alive_monsters_count);

private:
   EMonster_Set_State Monster_Set_State;
   AsBorder *Border; //UNO
   int Current_Gate_Index;
   int Max_Alive_Monsters_Count;

   virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_object);

   static const int Max_Monsters_Count = 10;

   AMonster Monsters[Max_Monsters_Count];
};
//-------------------------------------------------------------------------------------------------------------------------
