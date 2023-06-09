﻿#include "Engine.h"

//AsEngine
//-------------------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
   :  Timer_ID(WM_USER + 1), Game_State(EGame_State::Lost_Ball), Rest_Distance(0), Life_Count(AsConfig::Initial_Life_Count), Modules{}
{
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::Init_Engine(HWND hwnd)
{//Настройка игры при старте

   int index;
   SYSTEMTIME sis_time;
   FILETIME file_time;

   GetSystemTime(&sis_time);
   SystemTimeToFileTime(&sis_time, &file_time);

   srand(file_time.dwLowDateTime);

   AsConfig::Hwnd = hwnd;

   AActive_Brick_Blue_Orange::Setup_Color();
   AExplosive_Ball::Setup_Color();

   Level.Init();
   Platform.Init(&Ball_Set, &Laser_Beam_Set);
   Monster_Set.Init(&Border);
   Info_Panel.Init();

   AFalling_Letter::Init();

   ABall::Hit_Checker_List.Add_Hit_Checker(&Border);
   ABall::Hit_Checker_List.Add_Hit_Checker(&Level);
   ABall::Hit_Checker_List.Add_Hit_Checker(&Platform);
   ABall::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

   ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Level);
   ALaser_Beam::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

   AsPlatform::Hit_Checker_List.Add_Hit_Checker(&Monster_Set);

   Level.Set_Current_Level(AsLevel::Level_01);

   Platform.Redraw_Platform();

   SetTimer(AsConfig::Hwnd, Timer_ID, 1000 / AsConfig::FPS, 0);

   // Modules
   index = 0;

   Modules.push_back(&Level);
   Modules.push_back(&Border);
   Modules.push_back(&Platform);
   Modules.push_back(&Ball_Set);
   Modules.push_back(&Laser_Beam_Set);
   Modules.push_back(&Monster_Set);
   Modules.push_back(&Info_Panel);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HDC hdc, RECT &paint_area)
{// Отрисовка экрана игры

   SetGraphicsMode(hdc, GM_ADVANCED);

   for (auto *curr_module : Modules)
      curr_module->Clear(hdc, paint_area);

   for (auto *curr_module : Modules)
      curr_module->Draw(hdc, paint_area);
}
//-------------------------------------------------------------------------------------------------------------------------
int AsEngine::On_Key(EKey_Type key_type, bool key_down)
{
   switch (key_type)
   {
   case EKey_Type::Left:
      Platform.Move(true, key_down);
      break;

   case EKey_Type::Right:
      Platform.Move(false, key_down);
      break;

   case EKey_Type::Space:
      Platform.On_Space_Key(key_down);
      break;
   }
   return 0;
}
//-------------------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{
   ++AsConfig::Current_Timer_Tick;

   switch (Game_State)
   {
   case EGame_State::Test_Ball:
      Ball_Set.Set_For_Test();
      Game_State = EGame_State::Play_Level;
      break;

   case EGame_State::Play_Level:
      Play_Level();
      break;

   case EGame_State::Lost_Ball:
      if (Platform.Has_State(EPlatform_Substate_Regular::Missing))
         Restart_Level();
      break;

   case EGame_State::Restart_Level:
      if (Platform.Has_State(EPlatform_Substate_Regular::Ready))
      {
         Game_State = EGame_State::Play_Level;
         Ball_Set.Set_On_Platform(Platform.Get_Middle_Pos());
         Monster_Set.Activate(3);
         Info_Panel.Floor_Indicator.Restart();

      }
      break;

   default:
      break;
   }

   Act();

   return 0;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::Restart_Level()
{
   Game_State = EGame_State::Restart_Level;
   Border.Open_Gate(7, true);
   //Border.Open_Gate(5, false);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::Play_Level()
{
   Advance_Movers();

   if (Ball_Set.All_Balls_Are_Lost())
   {// Потеряли все мячики
      Game_State = EGame_State::Lost_Ball;
      Level.Stop();
      Monster_Set.Destroy_All();
      Laser_Beam_Set.Disable_All();
      Platform.Set_State(EPlatform_State::Meltdown);
   }
   else
      Ball_Set.Accelerate();

   if (Ball_Set.Is_Test_Finished())
      Game_State = EGame_State::Test_Ball;
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::Advance_Movers()
{
   double current_speed, max_speed = 0.0;

   // Получаем максимальную скорость движущихся объектов
   for (auto *curr_module : Modules)
   {
      curr_module->Begin_Movement();

      current_speed = fabs(curr_module->Get_Speed());

      if (current_speed > max_speed)
         max_speed = current_speed;
   }

   // 1. Смещаем все объекты
   Rest_Distance += max_speed;

   while (Rest_Distance > 0.0)
   {
      for (auto *curr_module : Modules)
         curr_module->Advance(max_speed);

      Rest_Distance -= AsConfig::Moving_Step_Size;
   }

   // Заканчиваем все движения на этом кадре
   for (auto *curr_module : Modules)
      curr_module->Finish_Movement();
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::Act()
{
   int index = 0;
   AFalling_Letter *falling_letter;

   // 1. Выполняем все действия
   for (auto *curr_module : Modules)
      curr_module->Act();

   // 2. Ловим пдаюзие буквы
   while (Level.Get_Next_Falling_Letter(index, &falling_letter) )
   {
      if (Platform.Hit_By(falling_letter))
         On_Falling_Letter(falling_letter);
   }

   // 3. Выпускаем мячик если гейт открыт
   if (Game_State == EGame_State::Restart_Level)
      if (Border.Is_Gate_Opened(AsConfig::Gate_Count - 1))
         Platform.Set_State(EPlatform_State::Rolling);
}
//-------------------------------------------------------------------------------------------------------------------------
void AsEngine::On_Falling_Letter(AFalling_Letter *falling_letter)
{
   switch (falling_letter->Letter_Type)
   {
   case ELetter_Type::O:
      Platform.Set_State(EPlatform_Substate_Regular::Normal);
      break;

   case ELetter_Type::I:
      Ball_Set.Inverse_Balls();
      Platform.Set_State(EPlatform_Substate_Regular::Normal);
      break;

   case ELetter_Type::C:
      Ball_Set.Reset_Speed();
      Platform.Set_State(EPlatform_Substate_Regular::Normal);
      break;

   //case ELetter_Type::M:
   //   break;

   case ELetter_Type::G:
      if (Life_Count < AsConfig::Max_Life_Count)
         ++Life_Count;
      Platform.Set_State(EPlatform_Substate_Regular::Normal);
      break;

   case ELetter_Type::K:
      Platform.Set_State(EPlatform_State::Glue);
      break;

   case ELetter_Type::W:
      Platform.Set_State(EPlatform_State::Expanding);
      break;

   case ELetter_Type::T:
      Platform.Set_State(EPlatform_Substate_Regular::Normal);
      Ball_Set.Triple_Balls();
      break;

   case ELetter_Type::L:
      Platform.Set_State(EPlatform_State::Laser);
      break;

   case ELetter_Type::P:
      AsConfig::Level_Has_Floor = true;
      Border.Redraw_Floor();
      Info_Panel.Floor_Indicator.Restart();
      Platform.Set_State(EPlatform_Substate_Regular::Normal);
      break;

   //case ELetter_Type::Plus:
   //   break;

   default:
      break;
   }

   falling_letter->Finalize();

   AsInfo_Panel::Update_Score(EScore_Event_Type::Catch_Letter);
}
//-------------------------------------------------------------------------------------------------------------------------

