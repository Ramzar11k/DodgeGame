#include "stdafx.h"


void GameData::AddEnemy(float eSpeed)
{
	Enemy* newEnemy = new Enemy(m_pPlayer, eSpeed);
	newEnemy->m_position.z = -9 + rand() % 19;
	if (m_projCount == 0)
	{
		m_pProjEnemyRoot = newEnemy;
	}
	else
	{
		Enemy* pNode = m_pProjEnemyRoot;
		while (pNode != nullptr)
		{
			if (pNode->next == nullptr)
			{
				pNode->next = newEnemy;
				break;
			}
			pNode = pNode->next;
		}
	}
	m_projCount++;
}

void GameData::RemoveEnemy(Enemy* pEnemy)
{
	if (pEnemy == m_pProjEnemyRoot)
	{
		m_pProjEnemyRoot = m_pProjEnemyRoot->next;
	}
	else
	{
		Enemy* pNode = m_pProjEnemyRoot;
		while (pNode != nullptr)
		{
			if (pNode->next == pEnemy)
			{
				pNode->next = pNode->next->next;
				break;
			}
			pNode = pNode->next;
		}
	}
	delete pEnemy;
	m_projCount--;
}


////A ball starts in the middle button of the screen 
//var lineY = mouseX;
//var ballXPosition = 200;
//var ballYPosition = 380;
//var clicked = 0;
//
//draw = function() {
//	background(255, 255, 255);
//	fill(0, 0, 0);
//	ellipse(ballXPosition, ballYPosition, 30, 30);
//
//	if (mouseX > 200 && clicked == = 0) {
//		lineY = 0.9 * mouseX;
//		line(200, 380, mouseX, lineY);
//	}
//	else if (clicked == = 0) {
//		lineY = 400 - 0.9 * mouseX;
//		line(200, 380, mouseX, lineY);
//	}
//
//	if (clicked == = 1) {
//		ballXPosition += (lineY / 0.9 - 200) / 400;
//		ballYPosition += -lineY / 400;
//	}
//
//
//
//};
//
//mousePressed = function() {
//	clicked = 1;
//};
//
////From it, a line goes out which points towards your mouse
////when you click, the ball starts moving in that direction
////once it exits the screen, you restart