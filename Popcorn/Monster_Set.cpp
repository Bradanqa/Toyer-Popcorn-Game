#include "Monster_Set.h"



// AExplosive_Ball
AColor AExplosive_Ball::Fading_Orange_Colors[Max_Fade_Step];
AColor AExplosive_Ball::Fading_Blue_Colors[Max_Fade_Step];
//-------------------------------------------------------------------------------------------------------------------------
AExplosive_Ball::AExplosive_Ball()
	: Explosive_Ball_State(EExplosive_Ball_State::Idle), X_Pos(0), Y_Pos(0), Step_Count(0), Size(0.0), Max_Size(0.0), Size_Step(0.0), Start_Expanding_Tick(0),
	Start_Fading_Tick(0), Time_Offset(0), Is_Red(false), Ball_Rect{}
{
}
//-------------------------------------------------------------------------------------------------------------------------
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
		{
			Update_Ball_Rect();
		}
		break;

	case EExplosive_Ball_State::Fading:
		if (AsConfig::Current_Timer_Tick >= Start_Fading_Tick + Fading_Timeout)
			Explosive_Ball_State = EExplosive_Ball_State::Idle;
		break;

	default:
		break;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Clear(HDC hdc, RECT& paint_area)
{
}
//-------------------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Draw(HDC hdc, RECT& paint_area)
{
	int curr_timeout;
	double ratio;
	int color_index;
	const AColor *color;

	switch (Explosive_Ball_State)
	{
	case EExplosive_Ball_State::Idle:
	case EExplosive_Ball_State::Charging:
		break;

	case EExplosive_Ball_State::Expanding:
		if (Is_Red)
			color = &AsConfig::Explosion_Dark_Orange;
		else
			color = &AsConfig::Explosion_Dark_Blue;

		AsTools::Ellipse(hdc, Ball_Rect, AsConfig::Explosion_Dark_Orange);
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
			color_index = (int)round(ratio * (double)(Max_Fade_Step - 1));

			if (Is_Red)
				color = &Fading_Orange_Colors[color_index];
			else
				color = &Fading_Blue_Colors[color_index];

			AsTools::Ellipse(hdc, Ball_Rect, *color);
		}
		break;

	default:
		break;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
bool AExplosive_Ball::Is_Finished()
{
	return false;
}
//-------------------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Explode(int x_pos, int y_pos, int size, bool is_red, int time_offset, int step_count)
{
	Explosive_Ball_State = EExplosive_Ball_State::Expanding;

	X_Pos = x_pos;
	Y_Pos = y_pos;
	Max_Size = size;
	Step_Count = step_count;
	Size = 0.0;
	Time_Offset = time_offset;
	Is_Red = is_red;

	Start_Expanding_Tick = AsConfig::Current_Timer_Tick + Time_Offset;
	Explosive_Ball_State = EExplosive_Ball_State::Charging;

	Size_Step = (double)Max_Size / (double)Step_Count;

	Update_Ball_Rect();
}
//-------------------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Setup_Color()
{
	int i;

	for (i = 0; i < Max_Fade_Step; i++)
	{
		AsTools::Get_Fading_Color(AsConfig::Orange_Color, i, Fading_Orange_Colors[i], Max_Fade_Step);
		AsTools::Get_Fading_Color(AsConfig::Blue_Color, i, Fading_Blue_Colors[i], Max_Fade_Step);
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void AExplosive_Ball::Update_Ball_Rect()
{
	Ball_Rect.left = X_Pos - (int)(Size / 2.0);
	Ball_Rect.top = Y_Pos - (int)(Size / 2.0);
	Ball_Rect.right = Ball_Rect.left + (int)Size;
	Ball_Rect.bottom = Ball_Rect.top + (int)Size;

	//AsTools::Invalidate_Rect(Ball_Rect);
}
//-------------------------------------------------------------------------------------------------------------------------




// AMonster
const double AMonster::Max_Cornea_Height = 11.0;
const double AMonster::Blinks_Timeouts[Blink_Stages_Count] = {0.4, 0.2, 0.8, 0.4, 0.4, 0.4, 0.8};
const EEye_State AMonster::Blink_States[Blink_Stages_Count] =
{
	EEye_State::Closed, EEye_State::Openning, EEye_State::Staring, EEye_State::Closing,
	EEye_State::Openning, EEye_State::Staring, EEye_State::Closing
};
//-------------------------------------------------------------------------------------------------------------------------
AMonster::AMonster()
	: Monster_State(EMonster_State::Missing), Eye_State(EEye_State::Closed), X_Pos(0.0), Y_Pos(0.0), Cornea_Height(Max_Cornea_Height), Start_Blink_Timeout(0),
	Total_Animation_Timeout(0), Speed(0.0), Direction(0.0), Next_Direction_Switch_Tick(0), Alive_Timer_Tick(0), Monster_Rect{}, Blink_Ticks{}
{
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Begin_Movement()
{
	// �� ������������
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Finish_Movement()
{
	if (!Is_Active())
		return;

	Redraw_Monster();
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Advance(double max_speed)
{
	double next_step;

	next_step = Speed / max_speed * AsConfig::Moving_Step_Size;

	X_Pos += next_step * cos(Direction);
	Y_Pos -= next_step * sin(Direction);

	// ������������ ����������� ������� ������� ������
	if (Monster_State == EMonster_State::Alive)
	{
		if (X_Pos < (double)AsConfig::Level_X_Offset)
			X_Pos = (double)AsConfig::Level_X_Offset;

		if (X_Pos + (double)Width > (double)AsConfig::Max_X_Pos)
			X_Pos = (double)AsConfig::Max_X_Pos - Width;

		if (Y_Pos < (double)AsConfig::Level_Y_Offset)
			Y_Pos = (double)AsConfig::Level_Y_Offset;

		if (Y_Pos + (double)Height > (double)AsConfig::Max_Y_Pos)
			Y_Pos = (double)AsConfig::Max_Y_Pos - Height;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
double AMonster::Get_Speed()
{
	return 0.0;
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Act()
{
	switch (Monster_State)
	{
	case EMonster_State::Missing:
		return;

	case EMonster_State::Emitting:
		if (AsConfig::Current_Timer_Tick >= Alive_Timer_Tick)
			Monster_State = EMonster_State::Alive;
		break;

	case EMonster_State::Alive:
		Act_Alive();
		break;

	case EMonster_State::Destroing:
		Act_Destroing();
		break;

	default:
		break;
	}

	AsTools::Invalidate_Rect(Monster_Rect);
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Clear(HDC hdc, RECT& paint_area)
{
	RECT itersection_rect;

	if (!IntersectRect(&itersection_rect, &paint_area, &Monster_Rect))
		return;

	AsTools::Ellipse(hdc, Prev_Monster_Rect, AsConfig::BG_Color);
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Draw(HDC hdc, RECT& paint_area)
{
	RECT itersection_rect;

	if (!IntersectRect(&itersection_rect, &paint_area, &Monster_Rect))
		return;

	switch (Monster_State)
	{
	case EMonster_State::Missing:
		break;

	case EMonster_State::Emitting:
	case EMonster_State::Alive:
		Draw_Alive(hdc);
		break;

	case EMonster_State::Destroing:
		Draw_Destroing(hdc, paint_area);
		break;

	default:
		AsConfig::Throw();
		break;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Finished()
{
	return false;
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Activate(int x_pos, int y_pos, bool moving_right)
{
	int i;
	int tick_offset;
	int rand_speed;
	int emitting_time_offset;
	double curr_timeout = 0.0;
	const int scale = AsConfig::Global_Scale;

	Monster_State = EMonster_State::Emitting;

	X_Pos = x_pos;
	Y_Pos = y_pos;

	rand_speed = AsTools::Rand(5) + 1;
	Speed = (double)rand_speed / 10.0;

	emitting_time_offset = (int)((double)AGate::Gate_Width / Speed);
	Alive_Timer_Tick = AsConfig::Current_Timer_Tick + emitting_time_offset;

	if (moving_right)
		Direction = 0.0;
	else
		Direction = M_PI;

	// ������������ ���� ��������
	Start_Blink_Timeout = AsConfig::Current_Timer_Tick;

	for (i = 0; i < Blink_Stages_Count; i++)
	{
		curr_timeout += Blinks_Timeouts[i];
		tick_offset = (int)((double)AsConfig::FPS * curr_timeout);
		Blink_Ticks[i] = tick_offset;
	}

	Total_Animation_Timeout = tick_offset;

	Redraw_Monster();
}
//-------------------------------------------------------------------------------------------------------------------------
bool AMonster::Is_Active()
{
	if (Monster_State == EMonster_State::Missing)
		return false;
	else
		return true;
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Destroy()
{
	const int scale = AsConfig::Global_Scale;
	int i;
	int half_width = Width * scale / 2;
	int half_height = Height * scale / 2;
	int x_pos = (int)(X_Pos * AsConfig::D_Global_Scale) + half_width;
	int y_pos = (int)(Y_Pos * AsConfig::D_Global_Scale) + half_height;
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
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Alive(HDC hdc)
{
	HRGN region;
	RECT rect, cornea_rect;
	const int scale = AsConfig::Global_Scale;
	const int half_scale = scale / 2;

	if (Monster_State == EMonster_State::Missing)
		return;

	rect = Monster_Rect;

	++rect.right;
	++rect.bottom;

	region = CreateEllipticRgnIndirect(&Monster_Rect);
	SelectClipRgn(hdc, region);

	// ����� ���
	AsTools::Ellipse(hdc, Monster_Rect, AsConfig::Monster_Dark_Orange);

	// ������� ���
	rect = Monster_Rect;

	rect.left -= 2 * scale;
	rect.top -= 3 * scale;
	rect.right -= 2 * scale;
	rect.bottom -= 3 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Orange_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	if (Eye_State == EEye_State::Closed)
		return;

	// ��������
	cornea_rect = Monster_Rect;

	cornea_rect.left += scale + half_scale;
	cornea_rect.top += 2 * scale + (int)((Max_Cornea_Height / 2.0 - Cornea_Height / 2.0) * AsConfig::D_Global_Scale);
	cornea_rect.right -= scale + half_scale;
	cornea_rect.bottom = cornea_rect.top + (int)(Cornea_Height * AsConfig::D_Global_Scale);

	region = CreateEllipticRgnIndirect(&cornea_rect);
	SelectClipRgn(hdc, region);

	AsTools::Ellipse(hdc, cornea_rect, AsConfig::Monster_Cornea_Color);

	// �������
	rect = Monster_Rect;

	rect.left += 4 * scale + half_scale;
	rect.top += 4 * scale;
	rect.right -= 4 * scale + half_scale;
	rect.bottom -= 5 * scale;

	AsTools::Ellipse(hdc, rect, AsConfig::Monster_Iris_Color);

	// ������
	rect = Monster_Rect;

	rect.left += 7 * scale;
	rect.top += 6 * scale;
	rect.right = rect.left + 2 * scale;
	rect.bottom = rect.top + 3 * scale;

	AsTools::Rect(hdc, rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);

	// ������� ��������
	AsConfig::BG_Outline_Color.Select(hdc);
	Arc(hdc, cornea_rect.left, cornea_rect.top, cornea_rect.right - 1, cornea_rect.bottom - 1, 0, 0, 0, 0);
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Draw_Destroing(HDC hdc, RECT &paint_area)
{
	int i;

	for (i = 0; i < Explosive_Balls_Count; i++)
	{
		Explosive_Balls[i].Draw(hdc, paint_area);
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Act_Alive()
{
	int i;
	int curr_tick_offset;
	int prev_tick;
	double ratio;
	double direction_delta;

	if (Monster_State == EMonster_State::Missing)
		return;

	curr_tick_offset = (AsConfig::Current_Timer_Tick - Start_Blink_Timeout) % Total_Animation_Timeout;

	for (i = 0; i < Blink_Stages_Count; i++)
	{
		if (curr_tick_offset < Blink_Ticks[i])
		{
			Eye_State =  Blink_States[i];
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

	case EEye_State::Openning:
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

	if (AsConfig::Current_Timer_Tick > Next_Direction_Switch_Tick)
	{//������ �������� ��������� ��������

		Next_Direction_Switch_Tick += AsTools::Rand(AsConfig::FPS); 

		direction_delta = (double)(AsTools::Rand(90) - 45) * M_PI / 180.0;
		Direction += direction_delta;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Act_Destroing()
{
	int i;

	for (i = 0; i < Explosive_Balls_Count; i++)
	{
		Explosive_Balls[i].Act();
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void AMonster::Redraw_Monster()
{
	Prev_Monster_Rect = Monster_Rect;

	int scale = AsConfig::Global_Scale;

	Monster_Rect.left = (int)(X_Pos * AsConfig::D_Global_Scale);
	Monster_Rect.top = (int)(Y_Pos * AsConfig::D_Global_Scale);
	Monster_Rect.right = Monster_Rect.left + Width * scale;
	Monster_Rect.bottom = Monster_Rect.top + Height * scale;

	AsTools::Invalidate_Rect(Monster_Rect);
	AsTools::Invalidate_Rect(Prev_Monster_Rect);
}
//-------------------------------------------------------------------------------------------------------------------------




// AsMonster_Set
//------------------------------------------------------------------------------------------------------------
AsMonster_Set::AsMonster_Set()
: Monster_Set_State(EMonster_Set_State::Idle), Border(0), Current_Gate_Index(-1), Max_Alive_Monsters_Count(0)
{
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Act()
{
	int i;
	int current_alive_count;

	switch (Monster_Set_State)
	{
	case EMonster_Set_State::Idle:
		break;

	case EMonster_Set_State::Select_Next_Gate:
		// ������� ���-�� ����� ��������
		current_alive_count = 0;

		for (i = 0; i < Max_Monsters_Count; i++)
		{
			if (Monsters[i].Is_Active())
				++current_alive_count;
		}

		if (current_alive_count < Max_Alive_Monsters_Count)
		{
			//Current_Gate_Index = AsTools::Rand(AsConfig::Gate_Count);
			Current_Gate_Index = Border->Long_Open_Gate();
			Monster_Set_State = EMonster_Set_State::Waiting_Gate_Opening;
		}
		break;

	case EMonster_Set_State::Waiting_Gate_Opening:
		if (Border->Is_Gate_Opened(Current_Gate_Index))
		{
			Emit_At_Gate(Current_Gate_Index);
			Monster_Set_State = EMonster_Set_State::Waiting_Gate_Closing;
		}
		break;

	case EMonster_Set_State::Waiting_Gate_Closing:
		if (Border->Is_Gate_Closed(Current_Gate_Index))
			Monster_Set_State = EMonster_Set_State::Select_Next_Gate;
		break;

	default:
		AsConfig::Throw();
		break;
	}

	AGame_Objects_Set::Act();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Init(AsBorder* border)
{
	Border = border;
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Emit_At_Gate(int gate_index)
{
	int i;
	int gate_x_pos, gate_y_pos;
	bool gate_is_left;
	AMonster *monster = 0;

	if (gate_index < 0 || gate_index >= AsConfig::Gate_Count)
		AsConfig::Throw();

	for (i = 0; i < Max_Monsters_Count; i++)
	{
		if (! Monsters[i].Is_Active())
		{
			monster = &Monsters[i];
			break;
		}
	}
	if (monster == 0)
		return;

	Border->Get_Gate_Pos(gate_index, gate_x_pos, gate_y_pos);

	if (gate_index % 2 == 0)
		gate_is_left = true;
	else
		gate_is_left = false;

	if (!gate_is_left)
		gate_x_pos -= monster->Width - AGate::Gate_Width;

	monster->Activate(gate_x_pos, gate_y_pos + 1, gate_is_left);

	//monster->Destroy();
}
//------------------------------------------------------------------------------------------------------------
void AsMonster_Set::Activate(int max_alive_monsters_count)
{
	Monster_Set_State = EMonster_Set_State::Select_Next_Gate;

	Max_Alive_Monsters_Count = max_alive_monsters_count;
}
//------------------------------------------------------------------------------------------------------------
bool AsMonster_Set::Get_Next_Game_Object(int &index, AGame_Object **game_object)
{
	if (index < 0 || index >= AsConfig::Max_Ball_Count)
		return false;

	*game_object = &Monsters[index++];

	return true;
}
//------------------------------------------------------------------------------------------------------------
