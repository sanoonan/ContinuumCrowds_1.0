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
	glm::vec2 cell_pos;


	int width = shared_grid->m_width;
	int height = shared_grid->m_height;

	int num_groups = popManager->m_num_groups;

	int i, j, k;

	for(i=0; i<num_groups; ++i)
	{
		curr_group = popManager->m_groups[i];	

		for(j=0; j<width; ++j)
			for(k=0; k<height; ++k)
			{
				cell_pos = glm::vec2(j, k);

				getCellSpeeds(curr_group, cell_pos);	
			}
	}
}


float SpeedField :: getTopoSpeed(float fmax, float fmin, float smax, float smin, glm::vec2 grad_height, glm::vec2 offset)
{
	float h_dot_n = glm::dot(grad_height, offset);

	float diff_slope = smax - smin;
	if(diff_slope == 0)
		return fmax;

	float topo_speed = fmax + ((h_dot_n - smin) / (diff_slope)) * (fmin - fmax);
	topo_speed = max(topo_speed, 0.0f);
	return topo_speed;
}

float SpeedField :: getFlowSpeed(glm::vec2 avg_vel, glm::vec2 offset)
{
	float flow_speed = glm::dot(avg_vel, offset);
	flow_speed = max(flow_speed, 0.0f);
	return flow_speed;
}

float SpeedField :: getFlowSpeedCell(glm::vec2 cell_pos, glm::vec2 offset)
{
	glm::vec2 offset_pos = cell_pos + offset;

	glm::vec2 offset_cell_pos = shared_grid->findClosestCellPos(offset_pos);

	if(!shared_grid->checkExists(offset_cell_pos))
		return 0.0f;

	SharedCell *offset_cell = &shared_grid->findCellByPos(offset_cell_pos);

	glm::vec2 avg_vel = offset_cell->m_avg_velocity;


	return getFlowSpeed(avg_vel, offset);
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
	float topo_speed, flow_speed, speed;

	GroupCellFace *face;

	int i;

	for(i=0; i<4; ++i)
	{
		face = &cell->m_faces[i];
		offset = face->m_offset;

		topo_speed = getTopoSpeed(max_speed, min_speed, max_slope, min_slope, height_vector, offset);
		flow_speed = getFlowSpeedCell(cell_pos, offset);



//		speed = topo_speed;
		speed = interpolateSpeedCell(cell_pos, offset, topo_speed, flow_speed);



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
	float flow_speed = getFlowSpeedCell(cell_pos, offset);

	
	float speed = interpolateSpeedCell(cell_pos, offset, topo_speed, flow_speed);

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

				speed = -getCellDirSpeed(curr_group, cell_pos);
			//	speed = -0.5f;
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


	float cellAdist, cellBdist, cellCdist, cellDdist;
	float cellAcontrib, cellBcontrib, cellCcontrib, cellDcontrib;

	glm::vec2 cellAvel, cellBvel, cellCvel, cellDvel;

	float total_dist;

	glm::vec2 velocity;

	int num_existing_cells;

	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		num_people = curr_group->m_num_people;

		grid = &curr_group->m_grid;

		for(int j=0; j<num_people; j++)
		{
			num_existing_cells = 0;
			total_dist = 0.0f;
			cellAdist = cellBdist = cellCdist = cellDdist = -1.0f;
			cellAvel = cellBvel = cellCvel = cellDvel = glm::vec2(0.0f);

			left = bot = top = right = false;

			curr_person = &curr_group->m_people[j];
			person_pos = curr_person->m_position;


			cellApos = glm::vec2(floor(person_pos.x), floor(person_pos.y));
			cellBpos = glm::vec2(ceil(person_pos.x), floor(person_pos.y));
			cellCpos = glm::vec2(ceil(person_pos.x), ceil(person_pos.y));
			cellDpos = glm::vec2(floor(person_pos.x), ceil(person_pos.y));


			if(shared_grid->checkExists(cellApos))
			{
				cellAdist = glm::distance(cellApos, person_pos);
				
				cell = &grid->findCellByPos(cellApos);
				cellAvel = cell->m_velocity;
				
				total_dist += cellAdist;

				num_existing_cells++;
			}

			if(shared_grid->checkExists(cellBpos))
			{
				cellBdist = glm::distance(cellBpos, person_pos);
				
				cell = &grid->findCellByPos(cellBpos);
				cellBvel = cell->m_velocity;

				total_dist += cellBdist;

				num_existing_cells++;
			}

			if(shared_grid->checkExists(cellCpos))
			{
				cellCdist = glm::distance(cellCpos, person_pos);
				
				cell = &grid->findCellByPos(cellCpos);
				cellCvel = cell->m_velocity;

				total_dist += cellCdist;

				num_existing_cells++;
			}

			if(shared_grid->checkExists(cellDpos))
			{
				cellDdist = glm::distance(cellDpos, person_pos);
	
				cell = &grid->findCellByPos(cellDpos);
				cellDvel = cell->m_velocity;

				total_dist += cellDdist;

				num_existing_cells++;
			}

			if(num_existing_cells == 1)
				cellAcontrib = cellBcontrib = cellCcontrib = cellDcontrib = 1.0f;
			else
			{
				cellAcontrib = calcContribution(cellAdist, total_dist);
				cellBcontrib = calcContribution(cellBdist, total_dist);
				cellCcontrib = calcContribution(cellCdist, total_dist);
				cellDcontrib = calcContribution(cellDdist, total_dist);
			}

			velocity = cellAcontrib * cellAvel + cellBcontrib * cellBvel + cellCcontrib * cellCvel + cellDcontrib * cellDvel;
		//	if(glm::length(velocity) != 0)
		//		velocity = glm::normalize(velocity);
			curr_person->m_velocity = velocity;
		}
	}		
	
}


float SpeedField :: calcContribution(float dist, float tot_dist)
{
	if(dist < 0.0f)
		return 0.0f;

	float rest_dist = tot_dist - dist;
	float contrib = rest_dist/tot_dist;

	return contrib;
}



float SpeedField :: interpolateSpeed(float ft, float fv, float p, float pmin, float pmax)
{
	float p_diff = pmax - pmin;

	if(p_diff == 0)
		return ft;

	if(p > pmax)
		return fv;

	if(p < pmin)
		return ft;

	float speed = ft + ((p - pmin)/p_diff) * (fv - ft);
	return speed;
}


float SpeedField :: interpolateSpeedCell(glm::vec2 cell_pos, glm::vec2 offset, float topo_speed, float flow_speed)
{
	glm::vec2 offset_pos = cell_pos + offset;

	glm::vec2 offset_cell_pos = shared_grid->findClosestCellPos(offset_pos);

	if(!shared_grid->checkExists(offset_cell_pos))
		return 0.0f;

	SharedCell *offset_cell = &shared_grid->findCellByPos(offset_cell_pos);

	float density = offset_cell->m_density;
	

	return interpolateSpeed(topo_speed, flow_speed, density, *min_density, *max_density);
}

void SpeedField :: assignDensityField(DensityField *df)
{
	densityField = df;

	min_density = &df->m_min_density;
	max_density = &df->m_max_density;
}