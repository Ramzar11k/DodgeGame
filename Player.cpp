
#include "stdafx.h"

void Player::Move()
{
	if (m_direction.x > 0 && m_position.x < 19.5f)
		m_position.x += m_speed * m_direction.x;
	else if (m_direction.x < 0 && m_position.x > -19.5f)
		m_position.x += m_speed * m_direction.x;



	if (m_direction.z > 0 && m_position.z < 9.5f)
		m_position.z += m_speed * m_direction.z;
	else if (m_direction.z < 0 && m_position.z > -9.5f)
		m_position.z += m_speed * m_direction.z;

}