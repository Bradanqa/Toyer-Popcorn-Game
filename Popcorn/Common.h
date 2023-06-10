#pragma once

#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

//-------------------------------------------------------------------------------------------------------------------------
class AGraphics_Object
{
public:
   virtual ~AGraphics_Object();

   virtual void Act() = 0;
   virtual void Clear(HDC hdc, RECT& paint_area) = 0;
   virtual void Draw(HDC hdc, RECT& paint_area) = 0;
   virtual bool Is_Finished() = 0;

};
//-------------------------------------------------------------------------------------------------------------------------
class ABall;
class AHit_Checker
{
public:
   virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball) = 0;
   virtual bool Check_Hit(double next_x_pos, double next_y_pos);

   bool Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double& x);
};
//-------------------------------------------------------------------------------------------------------------------------
class AHit_Checker_List
{
public:
   AHit_Checker_List();

   void Add_Hit_Checker(AHit_Checker *hit_checker);
   bool Check_Hit(double x_pos, double y_pos, ABall *ball);
   bool Check_Hit(double x_pos, double y_pos);

private:
   int Hit_Checkers_Count;
   AHit_Checker *Hit_Checkers[3];
};
//-------------------------------------------------------------------------------------------------------------------------
class AMover
{
public:
   virtual ~AMover();

   virtual void Begin_Movement() = 0;
   virtual void Finish_Movement() = 0;
   virtual void Advance(double max_speed) = 0;
   virtual double Get_Speed() = 0;
};
//-------------------------------------------------------------------------------------------------------------------------
class AColor
{
public:
   AColor();
   AColor(unsigned char r, unsigned char g, unsigned char b);
   AColor(unsigned char r, unsigned char g, unsigned char b, int pen_size);
   AColor(const AColor &color, int pen_size);
   AColor(const AColor &pen_color, const AColor &brush_color, int pen_size);

   int Get_RGB() const;
   void Select(HDC hdc) const;
   void Select_Pen(HDC hdc) const;
   HBRUSH Get_Brush() const;

   unsigned char R, G, B;

private:
   HPEN Pen;
   HBRUSH Brush;
};
//-------------------------------------------------------------------------------------------------------------------------
class AGame_Object : public AMover, public AGraphics_Object
{
};
//-------------------------------------------------------------------------------------------------------------------------
class AGame_Objects_Set : public AGame_Object
{
public:
   virtual void Begin_Movement();
   virtual void Finish_Movement();
   virtual void Advance(double max_speed);
   virtual double Get_Speed();

   virtual void Act();
   virtual void Clear(HDC hdc, RECT& paint_area);
   virtual void Draw(HDC hdc, RECT& paint_area);
   virtual bool Is_Finished();

protected:
   virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_object) = 0;
};
//-------------------------------------------------------------------------------------------------------------------------
