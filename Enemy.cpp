#include "stdafx.h"

Enemy::Enemy(Player* p, float speed)
{
	m_p = p;
	m_speed = speed;
}

void Enemy::Move()
{
	m_position.x += m_speed;
}

bool Enemy::CollideWithPlayer()
{
	if (sqrt((m_p->m_position.x - this->m_position.x) * (m_p->m_position.x - this->m_position.x)
		+ (m_p->m_position.z - this->m_position.z) * (m_p->m_position.z - this->m_position.z)) < 2.0f)
	{
		if (m_p->m_position.x + 1.0f > this->m_position.x&&
			m_p->m_position.x - 1.0f < this->m_position.x &&
			m_p->m_position.z + 1.0f > this->m_position.z&&
			m_p->m_position.z - 1.0f < this->m_position.z)
		{
			m_p->m_health -= 7;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}