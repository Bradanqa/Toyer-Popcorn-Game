﻿#include "Border.h"

//AsBorder
//-------------------------------------------------------------------------------------------------------------------------
AsBorder::~AsBorder()
{
   for (auto &gate : Gates)
      delete gate;

   Gates.erase(Gates.begin(), Gates.end());
}
//-------------------------------------------------------------------------------------------------------------------------
AsBorder::AsBorder()
   : Floor_Rect{}
{
   Floor_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
   Floor_Rect.top = AsConfig::Floor_Y_Pos * AsConfig::Global_Scale;
   Floor_Rect.right = (AsConfig::Max_X_Pos - 1) * AsConfig::Global_Scale;
   Floor_Rect.bottom = AsConfig::Max_Y_Pos * AsConfig::Global_Scale;

   // Gates
   Gates.push_back(new AGate(1, 29, 0, 3));
   Gates.push_back(new AGate(AsConfig::Max_X_Pos, 29, AsConfig::Level_Width - 1, 3));

   Gates.push_back(new AGate(1, 77, 0, 9));
   Gates.push_back(new AGate(AsConfig::Max_X_Pos, 77, AsConfig::Level_Width - 1, 9));

   Gates.push_back(new AGate(1, 129));
   Gates.push_back(new AGate(AsConfig::Max_X_Pos, 129));

   Gates.push_back(new AGate(1, 178));
   Gates.push_back(new AGate(AsConfig::Max_X_Pos, 178));

   if (Gates.size() != AsConfig::Gate_Count)
      AsConfig::Throw();
}
//-------------------------------------------------------------------------------------------------------------------------
bool AsBorder::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{//Корректируем движения отражения от рамки

   bool got_hit = false;

   // Отрижение от левого края
   if (next_x_pos - AsConfig::Ball_Radius < AsConfig::Boeder_X_Offset)
   {
      got_hit = true;
      ball->Reflect(false);
   }

   // Отрижение от верхнего края
   if (next_y_pos - AsConfig::Ball_Radius < AsConfig::Boeder_Y_Offset)
   {
      got_hit = true;
      ball->Reflect(true);
   }

   // Отрижение от правого края
   if (next_x_pos + AsConfig::Ball_Radius > AsConfig::Max_X_Pos + 1)
   {
      got_hit = true;
      ball->Reflect(false);
   }

   // Отрижение от пола
   if (AsConfig::Level_Has_Floor && next_y_pos + AsConfig::Ball_Radius > AsConfig::Floor_Y_Pos)
   {
      got_hit = true;
      ball->Reflect(true);
   }

   // чтобы щарик смог улететь нижу пола, поланрим его макс У поз
   if (next_y_pos + AsConfig::Ball_Radius > AsConfig::Max_Y_Pos + AsConfig::Ball_Radius * 4.0)
      ball->Set_State(EBall_State::Lost);

   return got_hit;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Begin_Movement()
{
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Finish_Movement()
{
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Advance(double max_speed)
{
}
//-------------------------------------------------------------------------------------------------------------------------
double AsBorder::Get_Speed()
{
   return 0.0;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Act()
{
   for (auto *gate : Gates)
      gate->Act();
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Clear(HDC hdc, RECT& paint_area)
{
   RECT intersection_rect;

   for (auto *gate : Gates)
      gate->Clear(hdc, paint_area);

   if (!AsConfig::Level_Has_Floor)
      return;

   if (!IntersectRect(&intersection_rect, &paint_area, &Floor_Rect))
      return;

   AsTools::Rect(hdc, Floor_Rect, AsConfig::BG_Color);
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
   for (auto *gate : Gates)
      gate->Draw(hdc, paint_area);
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
   if (gate_index != Gates.size() - 1 && short_open)
      AsConfig::Throw();

   if (gate_index >= 0 && gate_index < (int)Gates.size())
      Gates[gate_index]->Open_Gate(short_open);
   else
      AsConfig::Throw();
}
//-------------------------------------------------------------------------------------------------------------------------
int AsBorder::Long_Open_Gate()
{
   int i;
   int gate_index;
   bool got_gate = false;
   AGate *gate;

   gate_index = AsTools::Rand(Gates.size());

   for (i = 0; i < (int)Gates.size(); i++)
   {
      gate = Gates[gate_index];

      if (gate_index != Gates.size() - 1) // Гейт из которого выкатывается платформа не выпускает монстра
         if (gate->Is_Closed())
         {
            if (gate->Level_X_Pos == -1)
            {
               got_gate = true;
               break;
            }

            if(! AsLevel::Has_Brick_At(gate->Level_X_Pos, gate->Level_Y_Pos) && AsLevel::Has_Brick_At(gate->Level_X_Pos, gate->Level_Y_Pos + 1) )
            {
               got_gate = true;
               break;
            }
         }
      
      ++gate_index;

      if (gate_index >= (int)Gates.size())
         gate_index = 0;
   }

   if (!got_gate)
      AsConfig::Throw();

   Open_Gate(gate_index, false);

   return gate_index;
}
//-------------------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Opened(int gate_index)
{
   if (gate_index >= 0 && gate_index < (int)Gates.size())
      return Gates[gate_index]->Is_Opened();
   else
   {
      AsConfig::Throw();
      return false;
   }
}
//-------------------------------------------------------------------------------------------------------------------------
bool AsBorder::Is_Gate_Closed(int gate_index)
{
   if (gate_index >= 0 && gate_index < (int)Gates.size())
      return Gates[gate_index]->Is_Closed();
   else
   {
      AsConfig::Throw();
      return false;
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Get_Gate_Pos(int gate_index, int &gate_x_pos, int &gate_y_pos)
{
   if (gate_index >= 0 && gate_index < (int)Gates.size())
      Gates[gate_index]->Get_Pos(gate_x_pos, gate_y_pos);
   else
      AsConfig::Throw();
}
//-------------------------------------------------------------------------------------------------------------------------
void AsBorder::Draw_Border(HDC hdc, RECT &paint_area, int x, int y, bool top_border)
{ // Элементы рамки

   int gate_top_y, gate_low_y;
   RECT intersection_rect, rect;

   rect.left = x * AsConfig::Global_Scale;
   rect.top = y * AsConfig::Global_Scale;
   rect.right = (x + 4) * AsConfig::Global_Scale;
   rect.bottom = (y + 4) * AsConfig::Global_Scale;

   if (!top_border)
   {
      for (auto *gate : Gates)
      {
         gate->Get_Y_Size(gate_top_y, gate_low_y);

         if (rect.top >= gate_top_y && rect.bottom <= gate_low_y)
            return; // Выходим из метода. Гейт целиком перекроет элемент рамки
      }
   }

   if (! IntersectRect(&intersection_rect, &paint_area, &rect))
      return;

   //Основаная линия
   AsConfig::Blue_Color.Select(hdc);

   if (top_border)
      AsTools::Rect(hdc, x, y + 1, 4, 3, AsConfig::Blue_Color);
   else
      AsTools::Rect(hdc, x + 1, y + 1, 3, 4, AsConfig::Blue_Color);

   //White line
   if (top_border)
      AsTools::Rect(hdc, x, y, 4, 1, AsConfig::White_Color);
   else
      AsTools::Rect(hdc, x, y, 1, 4, AsConfig::White_Color);

   //Перфорация
   AsConfig::BG_Color.Select(hdc);

   if (top_border)
      AsTools::Rect(hdc, x + 2, y + 2, 1, 1, AsConfig::BG_Color);
   else
      AsTools::Rect(hdc, x + 2, y + 1, 1, 1, AsConfig::BG_Color);
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
