#include "Group.h"

using namespace std;

Group :: Group()
{
	m_num_people = 0;
}

Group :: Group(int num_people)
{
	m_num_people = num_people;
	m_people.resize(num_people);
	m_distance_coeff = m_time_coeff = m_discomfort_coeff = 1.0f;
}

void Group :: draw(float scale, GLuint spID)
{
	for(int i=0; i<m_num_people; i++)
		m_people[i].draw(m_grid.m_width, m_grid.m_height, scale, spID);
}

void Group :: setupGrid(SharedGrid &shared_grid)
{
	m_grid.copyGridSize(shared_grid);
}

void Group :: assignRandomLocs()
{
	for(int i=0; i<m_num_people; i++)
		m_people[i].assignRandomLoc(m_grid.m_width, m_grid.m_height);
}

void Group :: addTBar(TwBar *bar)
{
	TwAddVarRW(bar, "Max Speed", TW_TYPE_FLOAT, &max_speed, "");
	TwAddVarRW(bar, "Min Speed", TW_TYPE_FLOAT, &min_speed, "");

	TwAddVarRW(bar, "Distance Coefficient", TW_TYPE_FLOAT, &m_distance_coeff, "");
	TwAddVarRW(bar, "Time Coefficient", TW_TYPE_FLOAT, &m_time_coeff, "");
	TwAddVarRW(bar, "Discomfort Coefficient", TW_TYPE_FLOAT, &m_discomfort_coeff, "");
}

void Group :: setSpeeds(float min, float max)
{
	min_speed = min;
	max_speed = max;
}

void Group :: setRightSideGoal()
{
	int height = m_grid.m_height;
	int right = m_grid.m_width - 1;
	
	m_goal.resize(height);

	for(int i=0; i<height; i++)
		m_goal[i] = glm::vec2(right, i);
}


void Group :: move(float dt)
{
	for(int i=0; i<m_num_people; i++)
		m_people[i].move(dt);
}