﻿#include "Config.h"

//AsConfig CLASS
//-------------------------------------------------------------------------------------------------------------------------
bool AsConfig::Level_Has_Floor = false;
int AsConfig::Current_Timer_Tick = 0;

const AColor AsConfig::BG_Color(0, 0, 0);
const AColor AsConfig::Orange_Color(255, 118, 14);
const AColor AsConfig::Blue_Color(106, 90, 235);
const AColor AsConfig::White_Color(255, 255, 255);
const AColor AsConfig::Letter_Color(White_Color, Global_Scale);
const AColor AsConfig::Gate_Color(White_Color, Global_Scale);
const AColor AsConfig::Laser_Color(White_Color, Global_Scale);
const AColor AsConfig::Unbreakable_Blue_Highlight(Blue_Color, Global_Scale);
const AColor AsConfig::Unbreakable_Orange_Highlight(Orange_Color, 3 * Global_Scale); 
const AColor AsConfig::Advertisement_Orange_Table(Orange_Color, 2 * Global_Scale);
const AColor AsConfig::Advertisement_Blue_Table(0, 159, 159, Global_Scale);
const AColor AsConfig::Teleport_Portal_Color(Blue_Color, BG_Color, Global_Scale);
const AColor AsConfig::Monster_Dark_Orange(170, 80, 15);
const AColor AsConfig::Monster_Comet_Tail(Monster_Dark_Orange, Global_Scale);
const AColor AsConfig::Monster_Cornea_Color(BG_Color, White_Color, Global_Scale * 2 / 3);
const AColor AsConfig::Monster_Iris_Color(BG_Color, Blue_Color, Global_Scale * 2 / 3);
const AColor AsConfig::BG_Outline_Color(BG_Color, Global_Scale * 2 / 3);
const AColor AsConfig::Explosion_Dark_Orange(White_Color, Orange_Color, 0);
const AColor AsConfig::Explosion_Dark_Blue(White_Color, Blue_Color, 0);

HWND AsConfig::Hwnd;

const double AsConfig::D_Global_Scale = (double)Global_Scale;
const double AsConfig::Start_Ball_Y_Pos = 184.0;
const double AsConfig::Moving_Step_Size = 1.0 / Global_Scale;
const double AsConfig::Ball_Acceleration = 1.001;
const double AsConfig::Normal_Ball_Speed = 3.0;
const double AsConfig::Min_Ball_Angle = M_PI / 8.0;
const double AsConfig::Ball_Radius = 2.0 - 0.5 / Global_Scale;
//-------------------------------------------------------------------------------------------------------------------------
void AsConfig::Throw()
{
   throw 13;
}
//-------------------------------------------------------------------------------------------------------------------------



// AsTools
//-------------------------------------------------------------------------------------------------------------------------
int AsTools::Rand(int range)
{
   return rand() * range / RAND_MAX;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsTools::Round_Rect(HDC hdc, RECT &rect, int corner_radius)
{
   int radius;

   radius = corner_radius * AsConfig::Global_Scale;
   RoundRect(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1, radius, radius);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsTools::Rect(HDC hdc, RECT &rect, const AColor &color)
{
   color.Select(hdc);
   Rectangle(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsTools::Rect(HDC hdc, int x, int y, int width, int height , const AColor& color)
{
   const int scale = AsConfig::Global_Scale;

   color.Select(hdc);
   Rectangle(hdc, x * scale, y * scale, (x + width) * scale - 1, (y + height) * scale - 1);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsTools::Ellipse(HDC hdc, RECT& rect, const AColor& color)
{
   color.Select(hdc);
   ::Ellipse(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 1);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsTools::Invalidate_Rect(RECT &rect)
{
   InvalidateRect(AsConfig::Hwnd, &rect, FALSE);
}
//-------------------------------------------------------------------------------------------------------------------------
unsigned char AsTools::Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step, int max_step)
{
   return color - step * (color - bg_color) / (max_step - 1);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsTools::Get_Fading_Color(const AColor &origin_color, int step, AColor &result_color, int max_step)
{
   unsigned char r, g, b;

   r = Get_Fading_Channel(origin_color.R, AsConfig::BG_Color.R, step, max_step);
   g = Get_Fading_Channel(origin_color.G, AsConfig::BG_Color.G, step, max_step);
   b = Get_Fading_Channel(origin_color.B, AsConfig::BG_Color.B, step, max_step);

   result_color = AColor(r, g, b);
}
//------------------------------------------------------------------------------------------------------------
bool AsTools::Reflect_On_Circle(double next_x_pos, double next_y_pos, double circle_x, double circle_y, double circle_radius, ABall_Object *ball)
{
	double dx, dy;
	double distance, two_radiuses;
	double alpha, beta, gamma;
	double related_ball_direction;
	const double pi_2 = 2.0 * M_PI;

	dx = next_x_pos - circle_x;
	dy = next_y_pos - circle_y;

	distance = sqrt(dx * dx + dy * dy);
	two_radiuses = circle_radius + AsConfig::Ball_Radius;

	//if (fabs(distance - two_radiuses) < AsConfig::Moving_Step_Size)
	if (distance + AsConfig::Moving_Step_Size < two_radiuses)
	{// Мячик коснулся бокового шарика

		beta = atan2(-dy, dx);

		related_ball_direction = ball->Get_Direction();
		related_ball_direction -= beta;

		if (related_ball_direction > pi_2)
			related_ball_direction -= pi_2;

		if (related_ball_direction < 0.0)
			related_ball_direction += pi_2;

		if (related_ball_direction > M_PI_2 && related_ball_direction < M_PI + M_PI_2)
		{
			alpha = beta + M_PI - ball->Get_Direction();
			gamma = beta + alpha;

			ball->Set_Direction(gamma);

			return true;
		}
	}
	return false;
}
//-------------------------------------------------------------------------------------------------------------------------




// AHit_Checker
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Check_Hit(double next_x_pos, double next_y_pos)
{
   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Check_Hit(RECT& rect)
{
   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double& x)
{ // Проверяет пересечание горизонтального отрезка (проходящего от left_x до right_x через у) с окружностью радиусом radius

   double min_x, max_x;

   // x*x + y*y = R*R
   // x = sqrt(R * R - y * y)
   // y = sqrt(R * R - x * x)

   if (y > radius)
      return false;

   x = sqrt(radius * radius - y * y);

   max_x = next_x_pos + x;
   min_x = next_x_pos - x;

   if (max_x >= left_x && max_x <= right_x || min_x >= left_x && min_x <= right_x)
      return true;
   else
      return false;
}
//-------------------------------------------------------------------------------------------------------------------------




// AHit_Checker_List
//-------------------------------------------------------------------------------------------------------------------------
AHit_Checker_List::AHit_Checker_List()
   : Hit_Checkers_Count(0), Hit_Checkers{}
{
}
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker_List::Add_Hit_Checker(AHit_Checker* hit_checker)
{
   if (Hit_Checkers_Count >= sizeof(Hit_Checkers) / sizeof(Hit_Checkers[0]))
      AsConfig::Throw();

   Hit_Checkers[Hit_Checkers_Count++] = hit_checker;

   return true;
}
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker_List::Check_Hit(double x_pos, double y_pos, ABall_Object *ball)
{
   int i;

   for (i = 0; i < Hit_Checkers_Count; i++)
      if (Hit_Checkers[i]->Check_Hit(x_pos, y_pos, ball))
         return true;

   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker_List::Check_Hit(double x_pos, double y_pos)
{
   int i;

   for (i = 0; i < Hit_Checkers_Count; i++)
      if (Hit_Checkers[i]->Check_Hit(x_pos, y_pos))
         return true;

   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
bool AHit_Checker_List::Check_Hit(RECT &rect)
{
   int i;

   for (i = 0; i < Hit_Checkers_Count; i++)
      if (Hit_Checkers[i]->Check_Hit(rect))
         return true;

   return false;
}
//-------------------------------------------------------------------------------------------------------------------------


