#pragma once

#include "Laser_Beam.h"

//-------------------------------------------------------------------------------------------------------------------------
class ALaser_Beam_Set : public AsGame_Objects_Set
{
public:
   void Fire(double left_gun_x_pos, double right_gun_x_pos);

private:
   virtual bool Get_Next_Game_Object(int &index, AGame_Object **game_object);
   static const int Max_Laser_Beam_Count = 10;

   ALaser_Beam Laser_Beams[Max_Laser_Beam_Count];
};
//-------------------------------------------------------------------------------------------------------------------------