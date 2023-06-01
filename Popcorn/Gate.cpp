#include "Gate.h"

// AGate
const double AGate::Max_Gap_Short_Height = 9.0;
const double AGate::Gap_Height_Short_Step = Max_Gap_Short_Height / ((double)AsConfig::FPS / 2.0); // ��� �������� �� 0.5 ���
const double AGate::Max_Gap_Long_Height = 18.0;
const double AGate::Gap_Height_Long_Step = Max_Gap_Long_Height / ((double)AsConfig::FPS * 3.0); // ��� �������� �� 3 ���
//-------------------------------------------------------------------------------------------------------------------------
AGate::AGate(int x_pos, int y_pos)
   : Gate_State(EGate_State::Closed), Gate_Transformation(EGate_Transformation::Unknown), Edge_Count(5), X_Pos(x_pos), Y_Pos(y_pos), Origin_Y_Pos(y_pos), Gate_Close_Tick(0),
   Gap_Height(0.0)

{
   const int scale = AsConfig::Global_Scale;
   Gate_Rect.left = X_Pos * scale;
   Gate_Rect.top = (int)Y_Pos * scale;
   Gate_Rect.right = Gate_Rect.left + Gate_Width * scale;
   Gate_Rect.bottom = Gate_Rect.top + Gate_Height * scale;
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Act()
{
   bool correct_pos;

   switch (Gate_State)
   {
   case EGate_State::Closed:
      break;

   case EGate_State::Short_Open:
      if (Act_For_Open(true, correct_pos))
         Redraw_Gate();
      break;

   case EGate_State::Long_Open:
      if (Act_For_Open(false, correct_pos))
      {
         if (correct_pos)
         {
            Y_Pos = Origin_Y_Pos - Gap_Height / 2.0;

            Gate_Rect.top = (int)round(Y_Pos * AsConfig::D_Global_Scale);
            Gate_Rect.bottom = (int)round((Y_Pos + (double)Gate_Height + Gap_Height) * AsConfig::D_Global_Scale);
         }
         Redraw_Gate();
      }
      break;

   default:
      break;
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Clear(HDC hdc, RECT& paint_area)
{
   RECT intersection_rect;

   if (!IntersectRect(&intersection_rect, &paint_area, &Gate_Rect))
      return;

   AsTools::Rect(hdc, Gate_Rect, AsConfig::BG_Color);
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw(HDC hdc, RECT& paint_area)
{
   RECT intersection_rect;

   if (!IntersectRect(&intersection_rect, &paint_area, &Gate_Rect))
      return;

   Clear(hdc, paint_area);

   Draw_Cup(hdc, true);
   Draw_Cup(hdc, false);

   if (Gate_State == EGate_State::Long_Open && (Gate_Transformation == EGate_Transformation::Init || Gate_Transformation == EGate_Transformation::Finalize))
      Draw_Charge(hdc);
}
//-------------------------------------------------------------------------------------------------------------------------
bool AGate::Is_Finished()
{
   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Open_Gate(bool short_open)
{
   if (Gate_State != EGate_State::Closed)
      return;

   if (short_open)
      Gate_State = EGate_State::Short_Open;
   else
      Gate_State = EGate_State::Long_Open;

   Gate_Transformation = EGate_Transformation::Init;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Is_Opened()
{
   if (Gate_State == EGate_State::Short_Open || Gate_State == EGate_State::Long_Open)
   {
      if (Gate_Transformation == EGate_Transformation::Active)
         return true;
   }

   return false;
}//------------------------------------------------------------------------------------------------------------
void AGate::Get_Y_Size(int& gate_top_y, int& gate_low_y)
{
   gate_top_y = Gate_Rect.top;
   gate_low_y = Gate_Rect.bottom;
}
//------------------------------------------------------------------------------------------------------------
bool AGate::Act_For_Open(bool short_open, bool &correct_pos)
{
   double max_gate_height, gap_height_step;
   int opening_timeout;
   correct_pos = false;

   if (short_open)
   {
      max_gate_height = Max_Gap_Short_Height;
      gap_height_step = Gap_Height_Short_Step;
      opening_timeout = Short_Opening_Timeout;
   }
   else
   {
      max_gate_height = Max_Gap_Long_Height;
      gap_height_step = Gap_Height_Long_Step;
      opening_timeout = Long_Opening_Timeout;
   }

   switch (Gate_Transformation)
   {
   case EGate_Transformation::Unknown:
      break;

   case EGate_Transformation::Init:
      if (Gap_Height < max_gate_height)
      {
         Gap_Height += gap_height_step;
         correct_pos = true;
      }
      else
      {
         Gate_Close_Tick = AsConfig::Current_Timer_Tick + opening_timeout;
         Gate_Transformation = EGate_Transformation::Active;
      }

      return true;

   case EGate_Transformation::Active:
      if (AsConfig::Current_Timer_Tick >= Gate_Close_Tick)
         Gate_Transformation = EGate_Transformation::Finalize;
      break;

   case EGate_Transformation::Finalize:
      if (Gap_Height > 0.0)
      {
         Gap_Height -= gap_height_step;
         correct_pos = true;
      }
      else
      {
         Gate_Transformation = EGate_Transformation::Unknown;
         Gate_State = EGate_State::Closed;
      }

      return true;
   }

   return false;
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_Cup(HDC hdc, bool top_cup)
{
   int x = 0, y = 0;
   double cup_y_offset;
   HRGN region;
   RECT rect;
   const int scale = AsConfig::Global_Scale;
   const int half_scale = scale / 2;

   XFORM xform, old_xform;

   xform.eM11 = 1.0f;
   xform.eM12 = 0.0f;
   xform.eM21 = 0.0f;
   xform.eDx = (float)(X_Pos * scale);

   if (top_cup)
   {
      xform.eM22 = 1.0f;
      cup_y_offset = 0.0;
   }
   else
   {
      xform.eM22 = -1.0f;

      if (Gate_State == EGate_State::Long_Open)
         cup_y_offset = ((double)Gate_Height + Gap_Height) * AsConfig::D_Global_Scale - 1.0;
      else
         cup_y_offset = (double)Gate_Height * AsConfig::D_Global_Scale - 1.0;
   }

   xform.eDy = (float)round(Y_Pos * AsConfig::D_Global_Scale + cup_y_offset);

   GetWorldTransform(hdc, &old_xform);
   SetWorldTransform(hdc, &xform);

   // 1.1 ����������� ����� ����
   rect.left = x * scale;
   rect.top = (y + 1) * scale;
   rect.right = rect.left + 6 * scale;
   rect.bottom = rect.top + 4 * scale;

   // 1.2 ������
   AsConfig::Blue_Color.Select(hdc);
   AsTools::Round_Rect(hdc, rect, 3);

   // 1.3 ���� �����
   rect.left = X_Pos * scale;
   rect.right = rect.left + 3 * scale;

   if (top_cup)
   {
      rect.top = (int)round((Y_Pos + 1.0) * AsConfig::D_Global_Scale);
      rect.bottom = rect.top + 4 * scale;
   }
   else
   {
      rect.top = (int)round((Y_Pos - 1.0) * AsConfig::D_Global_Scale + cup_y_offset + 1.0);
      rect.bottom = rect.top - 4 * scale;
   }


   region = CreateRectRgnIndirect(&rect);
   SelectClipRgn(hdc, region);

   AsConfig::Gate_Color.Select_Pen(hdc);

   rect.left = x * scale + half_scale;
   rect.top = (y + 1) * scale + half_scale;
   rect.right = rect.left + 5 * scale + half_scale;
   rect.bottom = rect.top + 5 * scale + half_scale;
   AsTools::Round_Rect(hdc, rect, 3);

   SelectClipRgn(hdc, 0);
   DeleteObject(region);

   AsTools::Rect(hdc, x, y + 4, 4, 1, AsConfig::White_Color);   //  ���� �����

   AsTools::Rect(hdc, x + 4, y + 3, 2, 2, AsConfig::Blue_Color);  //  "��������" � ����

   AsTools::Rect(hdc, x + 4, y + 3, 1, 1, AsConfig::BG_Color);  //  ����������

   AsTools::Rect(hdc, x + 2, y, 2, 1, AsConfig::Blue_Color);  //  ����� ������

   Draw_Edge(hdc);

   SetWorldTransform(hdc, &old_xform);
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_Edge(HDC hdc)
{
   if (Gate_State == EGate_State::Long_Open)
      Draw_Long_Opening_Edge(hdc);
   else
      Draw_Short_Opening_Edge(hdc);
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_Short_Opening_Edge(HDC hdc)
{
   int i;
   int count;
   double ratio = 1.0 - Gap_Height / Max_Gap_Short_Height;
   bool is_long;

   is_long = false;

   count = (int)((double)Edge_Count * ratio);

   for (i = 0; i < count; i++)
   {
      Draw_One_Edge(hdc, 5 + i, is_long);
      is_long = !is_long;
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_Long_Opening_Edge(HDC hdc)
{
   int i;
   bool is_long = false;
   double ratio = Gap_Height / Max_Gap_Long_Height;

   if (ratio < 0.3)
   {
      for (i = 0; i < 4; i++)
      {
         Draw_One_Edge(hdc, 5 + i, is_long);
         is_long = !is_long;
      }

      if (ratio > 0.1)
         Draw_Red_Edge(hdc, 9, false, false);
   }
   else if (ratio < 0.5)
   {
      Draw_One_Edge(hdc, 5, false);
      Draw_One_Edge(hdc, 6, true);
      Draw_One_Edge(hdc, 7, true);

      Draw_Red_Edge(hdc, 8, true, true);
      Draw_Red_Edge(hdc, 9, false, false);
   }
   else
   {
      for (i = 0; i < 2; i++)
      {
         Draw_One_Edge(hdc, 5 + i, true);
      }

      Draw_Red_Edge(hdc, 7, true, false);
      Draw_Red_Edge(hdc, 8, true, true);
      Draw_Red_Edge(hdc, 9, false, false);
   }

}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_One_Edge(HDC hdc, int edge_y_offset, bool long_edge)
{
   if (long_edge)
   {
      AsTools::Rect(hdc, 0, 0 + edge_y_offset, 4, 1, AsConfig::White_Color);
      AsTools::Rect(hdc, 0 + 4, 0 + edge_y_offset, 2, 1, AsConfig::Blue_Color);
   }
   else
   {
      AsTools::Rect(hdc, 0 + 1, 0 + edge_y_offset, 2, 1, AsConfig::Blue_Color);
      AsTools::Rect(hdc, 0 + 4, 0 + edge_y_offset, 1, 1, AsConfig::Blue_Color);
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_Red_Edge(HDC hdc, int edge_y_offset, bool long_edge, bool has_highlight)
{
   if (long_edge)
   {
      AsTools::Rect(hdc, 0, 0 + edge_y_offset, 6, 1, AsConfig::Orange_Color);

      if (has_highlight)
         AsTools::Rect(hdc, 1, edge_y_offset, 1, 1, AsConfig::White_Color);
   }
   else
   {
      AsTools::Rect(hdc, 0 + 1, 0 + edge_y_offset, 4, 1, AsConfig::Orange_Color);
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Draw_Charge(HDC hdc)
{
   int i;
   int dots_count = 4;
   int dot_x, dot_y;
   int field_y;
   double ratio = Gap_Height / Max_Gap_Long_Height;

   if (ratio < 0.2 || ratio > 0.9)
      return;

   field_y = (int)(Origin_Y_Pos + (double)Gate_Height / 2.0 - Gap_Height / 2.0) + 1;

   for (i = 0; i < dots_count; i++)
   {
      dot_x = 1 + AsTools::Rand(4);
      dot_y = AsTools::Rand((int)Gap_Height - 1);

      AsTools::Rect(hdc, X_Pos + dot_x, field_y + dot_y, 1, 1, AsConfig::White_Color);
   }
}
//-------------------------------------------------------------------------------------------------------------------------
void AGate::Redraw_Gate()
{
   --Gate_Rect.top;
   ++Gate_Rect.bottom;

   AsTools::Invalidate_Rect(Gate_Rect);

   ++Gate_Rect.top;
   --Gate_Rect.bottom;
}
//-------------------------------------------------------------------------------------------------------------------------
