#pragma once
#include "stdafx.h"

class GameData
{
public:
	int m_score = 0;

	int m_projCount = 0;
	Enemy* m_pProjEnemyRoot = nullptr;


	Player* m_pPlayer = nullptr;

	void AddEnemy(float eSpeed);
	void RemoveEnemy(Enemy* pEnemy);
};