#include "SpeedField.h"

using namespace std;


SpeedField :: SpeedField()
{
}
/*
void SpeedField :: assignTopoSpeeds()
{
	Group *curr_group;
	GroupGrid *curr_grid;
	SharedCell curr_shared_cell;
	GroupCell *curr_group_cell;
	glm::vec2 cell_pos;
	float min_speed, max_speed, height_grad;
	SharedCellFace shared_cellface;
	GroupCellFace *cellface;

	float max_slope = shared_grid->max_slope;
	float min_slope = shared_grid->min_slope;
	int width = shared_grid->m_width;
	int height = shared_grid->m_height;

	int num_groups = popManager->m_num_groups;
	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		curr_grid = &curr_group->m_grid;
		min_speed = curr_group->min_speed;
		max_speed = curr_group->max_speed;

		for(int j=0; j<width; j++)
			for(int k=0; k<height; k++)
			{
				cell_pos = glm::vec2(j, k);
				curr_shared_cell = shared_grid->findCellByPos(cell_pos);
				curr_group_cell = &curr_grid->findCellByPos(cell_pos);

				for(int l=0; l<4; l++)
				{
					shared_cellface = curr_shared_cell.m_faces[l];
					cellface = &curr_group_cell->m_faces[l];

					height_grad = shared_cellface.m_grad_height;

					cellface->m_speed_topo = max_speed + ((height_grad - min_slope)/(max_slope - min_slope)) * (min_speed - max_speed);
					cellface->m_speed = cellface->m_speed_topo;
				}
			}
	}
}
*/
void SpeedField :: assignSpeeds()
{
	Group *curr_group;
	GroupGrid *curr_grid;
	GroupCell *curr_group_cell;
	glm::vec2 cell_pos;
	GroupCellFace *cellface;

	float topo_speed, flow_speed, speed;
	


	int width = shared_grid->m_width;
	int height = shared_grid->m_height;

	int num_groups = popManager->m_num_groups;
	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		curr_grid = &curr_group->m_grid;

		for(int j=0; j<width; j++)
			for(int k=0; k<height; k++)
			{
				cell_pos = glm::vec2(j, k);
				curr_group_cell = &curr_grid->findCellByPos(cell_pos);

				getCellSpeeds(curr_group, cell_pos);
				
			}
	}
}


float getTopoSpeed(float fmax, float fmin, float smax, float smin, glm::vec2 grad_height, glm::vec2 offset)
{
	float h_dot_n = glm::dot(grad_height, offset);
	float topo_speed = fmax + ((h_dot_n - smin) / (smax - smin)) * (fmin - fmax);
	return topo_speed;
}

void SpeedField :: getCellSpeeds(Group *group, glm::vec2 cell_pos)
{
	glm::vec2 tot_speed(0.0f);

	GroupGrid *grid = &group->m_grid;
	GroupCell *cell = &grid->findCellByPos(cell_pos);
	SharedCell *shared_cell = &shared_grid->findCellByPos(cell_pos);

	glm::vec2 height_vector = shared_cell->m_tot_grad_height;

	float max_speed = group->max_speed;
	float min_speed = group->min_speed;

	float max_slope = shared_grid->max_slope;
	float min_slope = shared_grid->min_slope;

	glm::vec2 offset;
	float topo_speed, speed;

	GroupCellFace *face;

	for(int i=0; i<4; i++)
	{
		face = &cell->m_faces[i];
		offset = face->m_offset;

		topo_speed = getTopoSpeed(max_speed, min_speed, max_slope, min_slope, height_vector, offset);
		speed = topo_speed;
		face->m_speed = speed;

		if(i>1)
			speed = -speed;

		if(i%2 == 0)
			tot_speed.x += speed;
		else
			tot_speed.y += speed;
	}

	cell->m_tot_speed = tot_speed;

}

float SpeedField :: getCellDirSpeed(Group *group, glm::vec2 cell_pos)
{

	GroupGrid *grid = &group->m_grid;
	GroupCell *cell = &grid->findCellByPos(cell_pos);
	SharedCell *shared_cell = &shared_grid->findCellByPos(cell_pos);

	glm::vec2 height_vector = shared_cell->m_tot_grad_height;

	float max_speed = group->max_speed;
	float min_speed = group->min_speed;

	float max_slope = shared_grid->max_slope;
	float min_slope = shared_grid->min_slope;

	glm::vec2 offset = cell->m_tot_grad_potential;

	float topo_speed = getTopoSpeed(max_speed, min_speed, max_slope, min_slope, height_vector, offset);
	
	float speed = topo_speed;

	return speed;
}

void SpeedField :: assignDirSpeeds()
{
	Group *curr_group;
	GroupGrid *curr_grid;
	GroupCell *curr_group_cell;
	glm::vec2 cell_pos;
	GroupCellFace *cellface;

	float speed;
	glm::vec2 tot_grad_potential, velocity;
	
	int width = shared_grid->m_width;
	int height = shared_grid->m_height;

	int num_groups = popManager->m_num_groups;
	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		curr_grid = &curr_group->m_grid;

		for(int j=0; j<width; j++)
			for(int k=0; k<height; k++)
			{
				cell_pos = glm::vec2(j, k);
				curr_group_cell = &curr_grid->findCellByPos(cell_pos);

				speed = getCellDirSpeed(curr_group, cell_pos);
				speed = -0.5f;
				tot_grad_potential = curr_group_cell->m_tot_grad_potential;

				velocity = speed * tot_grad_potential;

				curr_group_cell->m_velocity = velocity;
			}
	}
}


void SpeedField :: update()
{
	assignSpeeds();
}


void SpeedField :: assignPeopleVels()
{
	int num_groups = popManager->m_num_groups;
	int num_people;

	float deltaX, deltaY;
	GroupCell *cell = NULL;
	Person *curr_person;
	glm::vec2 cellApos, cellBpos, cellCpos, cellDpos, person_pos;

	bool left, bot, top, right;

	Group *curr_group;
	GroupGrid *grid;

	float min_dist;
	float dist;

	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		num_people = curr_group->m_num_people;

		grid = &curr_group->m_grid;

		for(int j=0; j<num_people; j++)
		{
			min_dist = 9999999;


			left = bot = top = right = false;

			curr_person = &curr_group->m_people[j];
			person_pos = curr_person->m_position;


			cellApos = glm::vec2(floor(person_pos.x), floor(person_pos.y));
			cellBpos = glm::vec2(ceil(person_pos.x), floor(person_pos.y));
			cellCpos = glm::vec2(ceil(person_pos.x), ceil(person_pos.y));
			cellDpos = glm::vec2(floor(person_pos.x), ceil(person_pos.y));


			if(shared_grid->checkExists(cellApos))
			{
				dist = glm::distance(cellApos, person_pos);
				if(dist < min_dist)
				{
					min_dist = dist;
					cell = &grid->findCellByPos(cellApos);
				}
			}

			if(shared_grid->checkExists(cellBpos))
			{
				dist = glm::distance(cellBpos, person_pos);
				if(dist < min_dist)
				{
					min_dist = dist;
					cell = &grid->findCellByPos(cellBpos);
				}
			}

			if(shared_grid->checkExists(cellCpos))
			{
				dist = glm::distance(cellCpos, person_pos);
				if(dist < min_dist)
				{
					min_dist = dist;
					cell = &grid->findCellByPos(cellCpos);
				}
			}

			if(shared_grid->checkExists(cellDpos))
			{
				dist = glm::distance(cellDpos, person_pos);
				if(dist < min_dist)
				{
					min_dist = dist;
					cell = &grid->findCellByPos(cellDpos);
				}
			}

			curr_person->m_velocity = cell->m_velocity;


		}
	}		
	
}