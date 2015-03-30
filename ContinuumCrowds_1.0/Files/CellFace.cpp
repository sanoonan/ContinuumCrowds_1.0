#include "CellFace.h"

using namespace std;

#pragma region BASE CLASS
CellFace :: CellFace()
{
	m_angle = 0.0f;
}

void CellFace :: setAngle(float angle, glm::vec2 offset)
{
	m_angle = angle;
	m_offset = offset;
}


#pragma endregion

#pragma region SHARED
SharedCellFace :: SharedCellFace() : CellFace()
{
	m_grad_height = 0.0f;
}
#pragma endregion


#pragma region GROUP
GroupCellFace :: GroupCellFace() : CellFace()
{
	m_speed = m_cost = m_grad_potential = 0.0f;
	m_velocity = 0.0f;
}
#pragma endregion