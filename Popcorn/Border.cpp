﻿#include "Border.h"

//AsBorder
//-------------------------------------------------------------------------------------------------------------------------
AsBorder::~AsBorder()
{
   int i;

   for (i = 0; i < AsConfig::Gate_Count; i++)
      delete Gates[i];
}
//-------------------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
   : Floor_Rect{}, Gates{}
{
   Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
   Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
   Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * AsConfig::Global_Scale;
   Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;

   // Gates
   Gates[0] = new AGate(1, 29);
   Gates[1] = new AGate(AsConfig::Max_X_Pos, 29);

   Gates[2] = new AGate(1, 77);
   Gates[3] = new AGate(AsConfig::Max_X_Pos, 77);

   Gates[4] = new AGate(1, 129);
   Gates[5] = new AGate(AsConfig::Max_X_Pos, 129);

   Gates[6] = new AGate(1, 178);
   Gates[7] = new AGate(AsConfig::Max_X_Pos, 178);
}
//-------------------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall* ball)
{//Корректируем движения отражения от рамки

   bool got_hit = false;

   // Отрижение от левого края
   if (next_x_pos - ball->Radius < AsConfig::Boeder_X_Offset)
   {
      got_hit = true;
      ball->Reflect(false);
   }

   // Отрижение от верхнего края
   if (next_y_pos - ball->Radius < AsConfig::Boeder_Y_Offset)
   {
      got_hit = true;
      ball->Reflect(true);
   }

   // Отрижение от правого края
   if (next_x_pos + ball->Radius > AsConfig::Max_X_Pos + 1)
   {
      got_hit = true;
      ball->Reflect(false);
   }

   // Отрижение от пола
   if (AsConfig::Level_Has_Floor && next_y_pos + ball->Radius > AsConfig::Floor_Y_Pos)
   {
      got_hit = true;
      ball->Reflect(true);
   }

   // чтобы щарик смог улететь нижу пола, поланрим его макс У поз
   if (next_y_pos + ball->Radius > AsConfig::Max_Y_Pos + ball->Radius * 4.0)
      ball->Set_State(EBall_State::Lost);

   return got_hit;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
   int i;

   for (i = 0; i < AsConfig::Gate_Count; i++)
      Gates[i]->Act();
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT& paint_area)
{
   RECT intersection_rect;
   int i;

   for (i = 0; i < AsConfig::Gate_Count; i++)
      Gates[i]->Draw(hdc, paint_area);

   if (!AsConfig::Level_Has_Floor)
      return;

   if (!IntersectRect(&intersection_rect, &paint_area, &Floor_Rect))
      return;

   AsConfig::BG_Color.Select(hdc);

   Rectangle(hdc, Floor_Rect.left, Floor_Rect.top, Floor_Rect.right - 1, Floor_Rect.bottom - 1);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Draw(HDC hdc, RECT& paint_area)
{
   //1 рамка слева
   int i;

   for (i = 0; i < 50; i++)
      Draw_Border(hdc, paint_area, 2, 1 + i * 4, false);

   //2 Рамка справа
   for (i = 0; i < 50; i++)
      Draw_Border(hdc, paint_area, AsConfig::Max_X_Pos + 1, 1 + i * 4, false);

   //3 Рамка сверху
   for (i = 0; i < 50; i++)
      Draw_Border(hdc, paint_area, 3 + i * 4, 0, true);

   //4 Floor
   if (AsConfig::Level_Has_Floor)
      Draw_Floor(hdc, paint_area);

   //5 Gates
   for( i = 0; i < AsConfig::Gate_Count; i++)
      Gates[i]->Draw(hdc, paint_area);
}
//-------------------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Finished()
{
   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Redraw_Floor()
{
   AsTools::Invalidate_Rect(Floor_Rect);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Open_Gate(int gate_index, bool short_open)
{
   if (gate_index != AsConfig::Gate_Count - 1 && short_open)
      AsConfig::Throw();

   if (gate_index >= 0 && gate_index < AsConfig::Gate_Count)
      Gates[gate_index]->Open_Gate(short_open);
   else
      AsConfig::Throw();
}
//-------------------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Opened(int gate_index)
{
   if (gate_index >= 0 && gate_index < AsConfig::Gate_Count)
      return Gates[gate_index]->Is_Opened();
   else
   {
      AsConfig::Throw();
      return false;
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Border(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{ // Элементы рамки

   int i;
   int gate_top_y, gate_low_y;
   RECT intersection_rect, rect;

   rect.left = x * AsConfig::Global_Scale;
   rect.top = y * AsConfig::Global_Scale;
   rect.right = (x + 4) * AsConfig::Global_Scale;
   rect.bottom = (y + 4) * AsConfig::Global_Scale;

   if (!top_border)
   {
      for (i = 0; i < AsConfig::Gate_Count; i++)
      {
         Gates[i]->Get_Y_Size(gate_top_y, gate_low_y);

         if (rect.top >= gate_top_y && rect.bottom <= gate_low_y)
            return; // Выходим из метода. Гейт целиком перекроет элемент рамки
      }
   }

   if (! IntersectRect(&intersection_rect, &paint_area, &rect))
      return;

   //Основаная линия
   AsConfig::Blue_Color.Select(hdc);

   if (top_border)
      Rectangle(hdc, x * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);
   else
      Rectangle(hdc, (x + 1) * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

   //White line
   AsConfig::White_Color.Select(hdc);

   if (top_border)
      Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 4) * AsConfig::Global_Scale - 1, (y + 1) * AsConfig::Global_Scale - 1);
   else
      Rectangle(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + 1) * AsConfig::Global_Scale - 1, (y + 4) * AsConfig::Global_Scale - 1);

   //Перфорация
   AsConfig::BG_Color.Select(hdc);

   if (top_border)
      Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 2) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 3) * AsConfig::Global_Scale - 1);
   else
      Rectangle(hdc, (x + 2) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 3) * AsConfig::Global_Scale - 1, (y + 2) * AsConfig::Global_Scale - 1);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Floor(HDC hdc, RECT& paint_area)
{
   int i, stroke_count;
   int x_pos, y_pos;
   int line_len = 4 * AsConfig::Global_Scale;
   int gap_len = 2 * AsConfig::Global_Scale;
   int stroke_len = line_len + gap_len;
   RECT intersection_rect;

   // Floor (если он есть)
   if (! IntersectRect(&intersection_rect, &paint_area, &Floor_Rect))
      return;

   stroke_count = (Floor_Rect.right - Floor_Rect.left + AsConfig::Global_Scale) / stroke_len;
   x_pos = Floor_Rect.left + 1;
   y_pos = Floor_Rect.top + (Floor_Rect.bottom - Floor_Rect.top) / 2;

   AsConfig::Letter_Color.Select(hdc);

   for (i = 0; i < stroke_count; i++)
   {
      MoveToEx(hdc, x_pos, y_pos, 0);
      LineTo(hdc, x_pos + line_len, y_pos);

      x_pos += stroke_len;
   }
}
//-------------------------------------------------------------------------------------------------------------------------
