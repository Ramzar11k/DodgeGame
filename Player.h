#pragma once

class Player
{
public:
	int m_health = 100;
	float m_speed = 0.2f;
	Point m_position = { 0 , 0 };
	Direction m_direction = { 0 , 0 };
	void Move();
};