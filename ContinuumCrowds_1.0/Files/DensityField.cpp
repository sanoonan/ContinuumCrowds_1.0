#include "DensityField.h"

using namespace std;

DensityField :: DensityField()
{
}

void DensityField :: clearDensities()
{
	int num_cellsX = m_grid->m_width;
	int num_cellsY = m_grid->m_height;
	SharedCell *curr_cell;
	glm::vec2 curr_cell_pos;

	for(int i=0; i<num_cellsX; i++)
		for(int j=0; j<num_cellsY; j++)
		{
			curr_cell_pos = glm::vec2(i, j);
			curr_cell = &m_grid->findCellByPos(curr_cell_pos);
			curr_cell->m_density = 0;
			curr_cell->m_avg_velocity = glm::vec2(0.0f);
		}
}

void DensityField :: assignDensities()
{
	int num_groups = popManager->m_num_groups;
	int num_people;

	float deltaX, deltaY, densityA, densityB, densityC, densityD;
	SharedCell *cellA, *cellB, *cellC, *cellD;
	Person *curr_person;
	glm::vec2 cellApos, cellBpos, cellCpos, cellDpos, person_pos;

	bool left, bot, top, right;

	Group *curr_group;

	float density_contrib, density_exponent;

	float log_half = glm::log(0.5);
	log_half = 1 / log_half;

	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		num_people = curr_group->m_num_people;
		for(int j=0; j<num_people; j++)
		{
			left = bot = top = right = false;

			curr_person = &curr_group->m_people[j];
			person_pos = curr_person->m_position;

			density_contrib = curr_person->m_density_contrib;
			density_exponent = log_half * glm::log(density_contrib);


			cellCpos = glm::vec2(ceil(person_pos.x), ceil(person_pos.y));

			if(cellCpos.x <= 0)
				left = true;
			else if(cellCpos.x >= m_grid->m_width-1)
				right = true;

			if(cellCpos.y <= 0)
				bot = true;
			else if(cellCpos.y >= m_grid->m_height-1)
				top = true;
			
		
			cellApos = glm::vec2(floor(person_pos.x), floor(person_pos.y));
			cellBpos = glm::vec2(ceil(person_pos.x), floor(person_pos.y));
			cellDpos = glm::vec2(floor(person_pos.x), ceil(person_pos.y));

			deltaX = person_pos.x - cellApos.x;
			deltaY = person_pos.y - cellApos.y;

			if((!left)&&(!top))
			{
				cellD = &m_grid->findCellByPos(cellDpos);
				densityD = pow(min(1 - deltaX, deltaY), density_exponent);
				cellD->m_density += densityD;
				cellD->m_avg_velocity += densityD * curr_person->m_velocity;
			}

			if((!bot)&&(!right))
			{
				cellB = &m_grid->findCellByPos(cellBpos);
				densityB = pow(min(deltaX, 1 - deltaY), density_exponent);
				cellB->m_density += densityB;
				cellB->m_avg_velocity += densityB * curr_person->m_velocity;
			}

			if((!left)&&(!bot))
			{
				cellA = &m_grid->findCellByPos(cellApos);
				densityA = pow(min(1 - deltaX, 1 - deltaY), density_exponent);
				cellA->m_density += densityA;
				cellA->m_avg_velocity += densityA * curr_person->m_velocity;
			}

			if((!right)&&(!top))
			{
				cellC = &m_grid->findCellByPos(cellCpos);
				densityC = pow(min(deltaX, deltaY), density_exponent);
				cellC->m_density += densityC;
				cellC->m_avg_velocity += densityC * curr_person->m_velocity;
			}
		}
	}

	divideVelocities();
	
}

void DensityField :: divideVelocities()
{
	int num_cellsX = m_grid->m_width;
	int num_cellsY = m_grid->m_height;
	SharedCell *curr_cell;
	glm::vec2 curr_cell_pos;

	for(int i=0; i<num_cellsX; i++)
		for(int j=0; j<num_cellsY; j++)
		{
			curr_cell_pos = glm::vec2(i, j);
			curr_cell = &m_grid->findCellByPos(curr_cell_pos);
			curr_cell->m_avg_velocity /= curr_cell->m_density;
		}
}

void DensityField :: update()
{
	clearDensities();
	assignDensities();
}