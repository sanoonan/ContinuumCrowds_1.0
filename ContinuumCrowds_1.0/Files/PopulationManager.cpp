#include "PopulationManager.h"

using namespace std;

PopulationManager :: PopulationManager()
{
	m_num_groups = 0;
}

void PopulationManager :: addGroup(Group &group)
{
	m_num_groups++;
	m_groups.push_back(&group);
}


void PopulationManager :: draw(float scale, GLuint spID)
{
	for(int i=0; i<m_num_groups; i++)
		m_groups[i]->draw(scale, spID);
}

void PopulationManager :: setupGroupGrids(SharedGrid &shared_grid)
{
	for(int i=0; i<m_num_groups; i++)
		m_groups[i]->setupGrid(shared_grid);
}

void PopulationManager :: assignRandomLocs()
{
	for(int i=0; i<m_num_groups; i++)
		m_groups[i]->assignRandomLocs();
}

void PopulationManager :: addTBar(TwBar *bar)
{
	for(int i=0; i<m_num_groups; i++)
		m_groups[i]->addTBar(bar);
}

void PopulationManager :: move(float dt)
{
	for(int i=0; i<m_num_groups; i++)
		m_groups[i]->move(dt);
}

