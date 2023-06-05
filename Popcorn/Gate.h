#pragma once

#include "Config.h"

//-------------------------------------------------------------------------------------------------------------------------
enum class EGate_State : unsigned char
{
   Closed,
   Short_Open,
   Long_Open
};
//-------------------------------------------------------------------------------------------------------------------------
enum class EGate_Transformation: unsigned char
{
   Unknown,

   Init,
   Active,
   Finalize
};
//-------------------------------------------------------------------------------------------------------------------------
class AGate: public AGraphics_Object
{
public:
   AGate(int x_pos, int y_pos);

   virtual void Act();
   virtual void Clear(HDC hdc, RECT& paint_area);
   virtual void Draw(HDC hdc, RECT& paint_area);
   virtual bool Is_Finished();

   void Open_Gate(bool short_open);
   bool Is_Opened();
   void Get_Y_Size(int &gate_top_y, int &gate_low_y);
   void Get_Pos(int &gate_x_pos, int &gate_y_pos);

private:
   bool Act_For_Open(bool short_open, bool &correct_pos);
   void Draw_Cup(HDC hdc, bool top_cup);
   void Draw_Edge(HDC hdc);
   void Draw_Short_Opening_Edge(HDC hdc);
   void Draw_Long_Opening_Edge(HDC hdc);
   void Draw_One_Edge(HDC hdc, int edge_y_offset, bool long_edge);
   void Draw_Red_Edge(HDC hdc, int edge_y_offset, bool long_edge, bool has_highlight);
   void Draw_Charge(HDC hdc);
   void Redraw_Gate();

   EGate_State Gate_State;
   EGate_Transformation Gate_Transformation;

   int Edge_Count;
   int X_Pos;
   double Y_Pos, Origin_Y_Pos;
   int Gate_Close_Tick;
   double Gap_Height;

   RECT Gate_Rect;

   static const double Gap_Height_Short_Step;
   static const double Max_Gap_Short_Height;
   static const double Max_Gap_Long_Height;
   static const double Gap_Height_Long_Step;

   static const int Gate_Width = 6;
   static const int Gate_Height = 19;
   static const int Short_Opening_Timeout = AsConfig::FPS;  // 1 sec
   static const int Long_Opening_Timeout = AsConfig::FPS * 3; // 3 sec
};