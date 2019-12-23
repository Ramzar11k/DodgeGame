#pragma once

class Player;

class Enemy
{
public:
	int m_health = 100;
	float m_speed = 0.25f;
	Direction m_direction = { 0 , 0 };
	Player* m_p;
	Enemy* next = nullptr;
	Point m_position = { -23.0f , 0.0f };
	D3DXMATRIX enemyMat;
	void Move();
	Enemy(Player* p, float speed);
	bool CollideWithPlayer();
};