#include "Monster_Set.h"

// AExplosive_Ball
AColor AExplosive_Ball::Fading_Red_Colors[Max_Fade_Step];
AColor AExplosive_Ball::Fading_Blue_Colors[Max_Fade_Step];
//------------------------------------------------------------------------------------------------------------
AExplosive_Ball::AExplosive_Ball()
: Explosive_Ball_State(EExplosive_Ball_State::Idle), Is_Red(false), X_Pos(0), Y_Pos(0), Max_Size(0), Step_Count(0),
  Start_Expanding_Tick(0), Start_Fading_Tick(0), Time_Offset(0), Size(0.0), Size_Step(0.0), Ball_Rect{}
{
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Act()
{
	switch (Explosive_Ball_State)
	{
	case EExplosive_Ball_State::Idle:
		break;


	case EExplosive_Ball_State::Charging:
		if (AsConfig::Current_Timer_Tick >= Start_Expanding_Tick)
			Explosive_Ball_State = EExplosive_Ball_State::Expanding;
		break;


	case EExplosive_Ball_State::Expanding:
		Size += Size_Step;

		if (Size >= Max_Size)
		{
			Explosive_Ball_State = EExplosive_Ball_State::Fading;
			Start_Fading_Tick = AsConfig::Current_Timer_Tick;
		}
		else
			Update_Ball_Rect();

		break;


	case EExplosive_Ball_State::Fading:
		if (AsConfig::Current_Timer_Tick > Start_Fading_Tick + Fading_Timeout)
			Explosive_Ball_State = EExplosive_Ball_State::Idle;
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Clear(HDC hdc, RECT &paint_area)
{
	// ��������, �� ������������
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Draw(HDC hdc, RECT &paint_area)
{
	int curr_timeout;
	int color_index;
	double ratio;
	const AColor *color;

	switch (Explosive_Ball_State)
	{
	case EExplosive_Ball_State::Idle:
	case EExplosive_Ball_State::Charging:
		break;


	case EExplosive_Ball_State::Expanding:
		if (Is_Red)
			color = &AsConfig::Explosion_Red_Color;
		else
			color = &AsConfig::Explosion_Blue_Color;

		AsTools::Ellipse(hdc, Ball_Rect, *color);
		break;


	case EExplosive_Ball_State::Fading:
		curr_timeout = AsConfig::Current_Timer_Tick - Start_Fading_Tick;

		if (curr_timeout > Fading_Timeout)
			curr_timeout = Fading_Timeout;

		if (curr_timeout == Fading_Timeout)
			AsTools::Ellipse(hdc, Ball_Rect, AsConfig::BG_Color);
		else
		{
			ratio = (double)curr_timeout / (double)Fading_Timeout;
			color_index = (int)round(ratio * (double)(Max_Fade_Step - 1) );

			if (Is_Red)
				color = &Fading_Red_Colors[color_index];
			else
				color = &Fading_Blue_Colors[color_index];

			AsTools::Ellipse(hdc, Ball_Rect, *color);
		}
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
bool AExplosive_Ball::Is_Finished()
{
	return false;  // ��������, �� ������������
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Explode(int x_pos, int y_pos, int size, bool is_red, int time_offset, int step_count)
{
	X_Pos = x_pos;
	Y_Pos = y_pos;
	Max_Size = size;
	Size = 0.0;
	Time_Offset = time_offset;
	Step_Count = step_count;
	Is_Red = is_red;

	Start_Expanding_Tick = AsConfig::Current_Timer_Tick + Time_Offset;
	Explosive_Ball_State = EExplosive_Ball_State::Charging;

	Size_Step = (double)Max_Size / (double)Step_Count;

	Update_Ball_Rect();
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Setup_Colors()
{
	int i;

	for (i = 0; i < Max_Fade_Step; i++)
	{
		AsTools::Get_Fading_Color(AsConfig::Red_Color, i, Fading_Red_Colors[i], Max_Fade_Step);
		AsTools::Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Colors[i], Max_Fade_Step);
	}
}
//------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Update_Ball_Rect()
{
	Ball_Rect.left = X_Pos - (int)(Size / 2.0);
	Ball_Rect.top = Y_Pos - (int)(Size / 2.0);
	Ball_Rect.right = Ball_Rect.left + (int)Size;
	Ball_Rect.bottom = Ball_Rect.top + (int)Size;
}
//------------------------------------------------------------------------------------------------------------




// AMonster
const double AMonster::Max_Cornea_Height = 11.0;
const double AMonster::Blink_Timeouts[Blink_Stages_Count] = { 0.4, 0.2, 0.8, 0.4, 0.4, 0.4, 0.8 };
const EEye_State AMonster::Blink_States[Blink_Stages_Count] = 
{
	EEye_State::Closed, EEye_State::Opening, EEye_State::Staring, EEye_State::Closing,
	EEye_State::Opening, EEye_State::Staring, EEye_State::Closing
};
//------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
: Monster_State(EMonster_State::Missing), Eye_State(EEye_State::Closed), X_Pos(0), Y_Pos(0), Cornea_Height(Max_Cornea_Height), Start_Blink_Timeout(0),
  Total_Animation_Timeout(0), Monster_Rect{}, Blink_Ticks{}
{
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
	//!!! ���� �������!
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
	//!!! ���� �������!
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
	//!!! ���� �������!
}
//------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	return 0.0;  //!!! ���� �������!
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{
	switch (Monster_State)
	{
	case EMonster_State::Missing:
		return;

	case EMonster_State::Alive:
		Act_Alive();
		break;

	case EMonster_State::Destroing:
		Act_Destroing();
		break;
	}

	AsTools::Invalidate_Rect(Monster_Rect);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::BG_Color);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	switch (Monster_State)
	{
	case EMonster_State::Missing:
		break;

	case EMonster_State::Alive:
		Draw_Alive(hdc);
		break;

	case EMonster_State::Destroing:
		Draw_Destroing(hdc, paint_area);
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	return false;  //!!! ���� �������!
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Activate(int x_pos, int y_pos)
{
	int i;
	int tick_offset;
	double curr_timeout = 0.0;
	const int scale = AsConfig::Global_Scale;

	Monster_State = EMonster_State::Alive;

	X_Pos = x_pos + 10;
	Y_Pos = y_pos;

	Monster_Rect.left = X_Pos * scale;
	Monster_Rect.top = Y_Pos * scale;
	Monster_Rect.right = Monster_Rect.left + Width * scale;
	Monster_Rect.bottom = Monster_Rect.top + Height * scale;

	// ������������ ���� ��������
	curr_timeout;
	Start_Blink_Timeout = AsConfig::Current_Timer_Tick;

	for (i = 0; i < Blink_Stages_Count; i++)
	{
		curr_timeout += Blink_Timeouts[i];
		tick_offset = (int)( (double)AsConfig::FPS * curr_timeout);
		Blink_Ticks[i] = tick_offset;
	}

	Total_Animation_Timeout = tick_offset;
}
//------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Active()
{
	if (Monster_State == EMonster_State::Missing)
		return false;
	else
		return true;
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Destroy()
{
	int i;
	int half_width = Width * AsConfig::Global_Scale / 2;
	int half_height = Height * AsConfig::Global_Scale / 2;
	int x_pos = X_Pos * AsConfig::Global_Scale + half_width;
	int y_pos = Y_Pos * AsConfig::Global_Scale + half_height;
	int x_offset, y_offset;
	int size, half_size, rest_size;
	int time_offset;
	bool is_red;

	half_size = half_width;

	if (half_height < half_size)
		half_size = half_height;

	for (i = 0; i < Explosive_Balls_Count; i++)
	{
		x_offset = AsTools::Rand(half_width) - half_width / 2;
		y_offset = AsTools::Rand(half_height) - half_height / 2;

		rest_size = half_size - (int)sqrt(x_offset * x_offset + y_offset * y_offset);

		size = AsTools::Rand(rest_size / 2) + rest_size / 2;

		if (size < AsConfig::Global_Scale)
			size = AsConfig::Global_Scale;

		time_offset = AsTools::Rand(AsConfig::FPS * 3 / 2);

		if (AsTools::Rand(2) == 0)
			is_red = true;
		else
			is_red = false;

		Explosive_Balls[i].Explode(x_pos + x_offset, y_pos + y_offset, size * 2, is_red, time_offset, 10);
	}


	Monster_State = EMonster_State::Destroing;

	//Explosive_Balls[0].Explode(Monster_Rect.left + 20, Monster_Rect.top + 20, 30, 0, 10);
	//Explosive_Balls[1].Explode(Monster_Rect.left + 30, Monster_Rect.top + 30, 25, 5, 10);
	//Explosive_Balls[2].Explode(Monster_Rect.left + 20, Monster_Rect.top + 30, 20, 10, 10);
	//Explosive_Balls[3].Explode(Monster_Rect.left + 30, Monster_Rect.top + 20, 15, 15, 10);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Alive(HDC hdc)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	const int half_scale = scale / 2;
	HRGN region;
	RECT rect, cornea_rect;

	if (Monster_State == EMonster_State::Missing)
		return;


	// 1. ������ ���
	// 1.1. ������������ ����� ����
	rect = Monster_Rect;

	++rect.right;
	++rect.bottom;

	region = CreateEllipticRgnIndirect(&rect);
	SelectClipRgn(hdc, region);

	// 1.2. Ҹ���� ���
	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::Monster_Dark_Red_Color);

	// 1.3. ������� ���
	rect = Monster_Rect;

	rect.left -= 2 * scale;
	rect.top -= 3 * scale;
	rect.right -= 2 * scale;
	rect.bottom -= 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Red_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// 2. ������ ��� ����
	if (Eye_State == EEye_State::Closed)
		return;

	// 2.1. ��������
	cornea_rect = Monster_Rect;

	cornea_rect.left += scale + half_scale;
	cornea_rect.top += 2 * scale + (int)( (Max_Cornea_Height / 2.0 - Cornea_Height / 2.0) * d_scale);
	cornea_rect.right -= scale + half_scale;
	cornea_rect.bottom = cornea_rect.top + (int)(Cornea_Height * d_scale);

	// 2.2. ������������ ����� ���������� ����� �����
	region = CreateEllipticRgnIndirect(&cornea_rect);
	SelectClipRgn(hdc, region);

	AsTools::Ellipse(hdc, cornea_rect, AsConfig::Monster_Cornea_Color);

	// 2.3. �������
	rect = Monster_Rect;

	rect.left += 4 * scale + half_scale;
	rect.top += 4 * scale;
	rect.right -= 4 * scale + half_scale;
	rect.bottom -= 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Monster_Iris_Color);

	// 2.4. ������
	rect = Monster_Rect;

	rect.left += 7 * scale;
	rect.top += 6 * scale;
	rect.right = rect.left + 2 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsTools::Rect(hdc, rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// 2.5. ������� ��������
	AsConfig::BG_Outline_Color.Select_Pen(hdc);

	Arc(hdc, cornea_rect.left, cornea_rect.top, cornea_rect.right - 1, cornea_rect.bottom - 1, 0, 0, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Destroing(HDC hdc, RECT &paint_area)
{
	int i;

	for (i = 0; i < Explosive_Balls_Count; i++)
		Explosive_Balls[i].Draw(hdc, paint_area);
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act_Alive()
{
	int i;
	int curr_tick_offset;
	int prev_tick;
	double ratio;

	if (Monster_State == EMonster_State::Missing)
		return;

	curr_tick_offset = (AsConfig::Current_Timer_Tick - Start_Blink_Timeout) % Total_Animation_Timeout;

	for (i = 0; i < Blink_Stages_Count; i++)
	{
		if (curr_tick_offset < Blink_Ticks[i])
		{
			Eye_State = Blink_States[i];
			break;
		}
	}

	if (i == 0)
		prev_tick = 0;
	else
		prev_tick = Blink_Ticks[i - 1];

	ratio = (double)(curr_tick_offset - prev_tick) / (double)(Blink_Ticks[i] - prev_tick);

	switch (Eye_State)
	{
	case EEye_State::Closed:
		Cornea_Height = 0.0;
		break;

	case EEye_State::Opening:
		Cornea_Height = Max_Cornea_Height * ratio;
		break;

	case EEye_State::Staring:
		Cornea_Height = Max_Cornea_Height;
		break;

	case EEye_State::Closing:
		Cornea_Height = Max_Cornea_Height * (1.0 - ratio);
		break;

	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void AMonster::Act_Destroing()
{
	int i;

	for (i = 0; i < Explosive_Balls_Count; i++)
		Explosive_Balls[i].Act();
}
//------------------------------------------------------------------------------------------------------------




// AsMonster_Set
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
: Border(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder *border)
{
	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
	int i;
	int gate_x_pos, gate_y_pos;
	AMonster *monster = 0;

	if (gate_index < 0 || gate_index >= AsConfig::Gates_Count)
		AsConfig::Throw();

	for (i = 0; i < Max_Monsters_Count; i++)
	{
		if (! Monsters[i].Is_Active() )
		{
			monster = &Monsters[i];
			break;
		}
	}

	if (monster == 0)
		return;  // ��� ������� - ������ (��� �� ����)

	Border->Get_Gate_Pos(gate_index, gate_x_pos, gate_y_pos);

	monster->Activate(gate_x_pos, gate_y_pos);

	monster->Destroy();
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_Game_Object(int &index, AGame_Object **game_obj)
{
	if (index < 0 || index >= AsConfig::Max_Balls_Count)
		return false;

	*game_obj = &Monsters[index++];

	return true;
}
//------------------------------------------------------------------------------------------------------------