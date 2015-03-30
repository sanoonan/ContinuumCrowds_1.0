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


float SpeedField :: getTopoSpeed(float fmax, float fmin, float smax, float smin, float grad_height)
{
	float diff_slope = smax - smin;
	if(diff_slope == 0)
		return fmax;

	float topo_speed = fmax + ((grad_height - smin) / (diff_slope)) * (fmin - fmax);
	topo_speed = max(topo_speed, 0.01f);
	return topo_speed;
}



float SpeedField :: getFlowSpeed(glm::vec2 cell_pos, glm::vec2 offset)
{
	glm::vec2 offset_pos = cell_pos + offset;

	if(!shared_grid->checkExists(offset_pos))
		return 0.0f;

	SharedCell *offset_cell = &shared_grid->findCellByPos(offset_pos);

	glm::vec2 avg_vel = offset_cell->m_avg_velocity;

	float flow_speed = glm::dot(avg_vel, offset);

	flow_speed = max(flow_speed, 0.01f);

	return flow_speed;
}

void SpeedField :: getCellSpeeds(Group *group, glm::vec2 cell_pos)
{

	GroupGrid *grid = &group->m_grid;
	GroupCell *cell = &grid->findCellByPos(cell_pos);
	SharedCell *shared_cell = &shared_grid->findCellByPos(cell_pos);


	float max_speed = group->max_speed;
	float min_speed = group->min_speed;

	float max_slope = shared_grid->max_slope;
	float min_slope = shared_grid->min_slope;

	glm::vec2 offset;
	float topo_speed, flow_speed, speed;

	GroupCellFace *face;
	SharedCellFace *shared_face;

	int i;

	for(i=0; i<4; ++i)
	{
		face = &cell->m_faces[i];
		offset = face->m_offset;

		shared_face = &shared_cell->m_faces[i];

		float grad_height = shared_face->m_grad_height;

		topo_speed = getTopoSpeed(max_speed, min_speed, max_slope, min_slope, grad_height);
		flow_speed = getFlowSpeed(cell_pos, offset);

		speed = interpolateSpeedCell(cell_pos, offset, topo_speed, flow_speed);

		face->m_speed = speed;
	}
}


void SpeedField :: assignDirSpeeds()
{
	Group *curr_group;
	GroupGrid *curr_grid;
	GroupCell *curr_cell;
	glm::vec2 cell_pos;
	GroupCellFace *cellface;

	float speed, pot_grad;
	float velocity, east_vel, north_vel, west_vel, south_vel;
	glm::vec2 tot_velocity;
	
	int width = shared_grid->m_width;
	int height = shared_grid->m_height;

	int num_groups = popManager->m_num_groups;
	float potential;

	int i, j, k, l;

	for(i=0; i<num_groups; ++i)
	{
		curr_group = popManager->m_groups[i];
		curr_grid = &curr_group->m_grid;

		for(j=0; j<width; ++j)
			for(k=0; k<height; ++k)
			{
				cell_pos = glm::vec2(j, k);
				curr_cell = &curr_grid->findCellByPos(cell_pos);
				potential = curr_cell->m_potential;

				if(potential == 0.0f)
				{
					curr_cell->m_tot_velocity = glm::vec2(0.0f);
					continue;
				}

				for(l=0; l<4; ++l)
				{
					cellface = &curr_cell->m_faces[l];
					
					speed = cellface->m_speed;
					pot_grad = cellface->m_grad_potential;

					velocity = -speed * pot_grad;
					cellface->m_velocity = velocity;

					if(l==0)
						east_vel = velocity;
					if(l==1)
						north_vel = velocity;
					if(l==2)
						west_vel = velocity;
					if(l==3)
						south_vel = velocity;
				}
				tot_velocity = glm::vec2((east_vel - west_vel), (north_vel - south_vel));

				curr_cell->m_tot_velocity = tot_velocity;
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


	glm::vec2 cellAvel, cellBvel, cellCvel, cellDvel;

	float total_dist = 0.0f;

	glm::vec2 velocity;

	int num_existing_cells;

	int i, j;

	float x1, x2, y1, y2, x, y;

	bool a, b, c, d;

	for(i=0; i<num_groups; ++i)
	{
		curr_group = popManager->m_groups[i];
		num_people = curr_group->m_num_people;

		grid = &curr_group->m_grid;

		for(j=0; j<num_people; ++j)
		{
			num_existing_cells = 0;

			cellAdist = cellBdist = cellCdist = cellDdist = -1.0f;
			cellAvel = cellBvel = cellCvel = cellDvel = glm::vec2(0.0f);


			curr_person = &curr_group->m_people[j];
			person_pos = curr_person->m_position;
		

			cellApos = glm::vec2(floor(person_pos.x), floor(person_pos.y));

			if(person_pos == cellApos)
			{
				cell = &grid->findCellByPos(cellApos);
				velocity = cell->m_tot_velocity;
				curr_person->m_velocity = velocity;
				continue;
			}

			a = b = c = d = false;

			cellBpos = glm::vec2(ceil(person_pos.x), floor(person_pos.y));
			cellCpos = glm::vec2(ceil(person_pos.x), ceil(person_pos.y));
			cellDpos = glm::vec2(floor(person_pos.x), ceil(person_pos.y));


			if(shared_grid->checkExists(cellApos))
			{
				
				cell = &grid->findCellByPos(cellApos);
				cellAvel = cell->m_tot_velocity;
				
				a = true;
			}

			if(shared_grid->checkExists(cellBpos))
			{
		
				
				cell = &grid->findCellByPos(cellBpos);
				cellBvel = cell->m_tot_velocity;

				b = true;
			}

			if(shared_grid->checkExists(cellCpos))
			{

				
				cell = &grid->findCellByPos(cellCpos);
				cellCvel = cell->m_tot_velocity;

				c = true;
			}

			if(shared_grid->checkExists(cellDpos))
			{
				cell = &grid->findCellByPos(cellDpos);
				cellDvel = cell->m_tot_velocity;

				d = true;
			}

	
			x1 = cellApos.x;
			x2 = cellCpos.x;
			y1 = cellApos.y;
			y2 = cellCpos.y;
			x = person_pos.x;
			y = person_pos.y;

			float diffx = x2 - x1;
			float diffy = y2 - y1;

			if(diffx == 0.0f)
				velocity = interpolateBetweenTwo(y, y1, y2, cellBvel, cellCvel);
			else if((!a)&&(!d))
			{
				velocity = interpolateBetweenTwo(y, y1, y2, cellBvel, cellCvel);
				velocity.x = max(velocity.x, 0.0f);
			}
			else if ((!c)&&(!b))
			{
				velocity = interpolateBetweenTwo(y, y1, y2, cellAvel, cellDvel);
				velocity.x = min(velocity.x, 0.0f);
			}
			else if (diffy == 0.0f)
				velocity = interpolateBetweenTwo(x, x1, x2, cellDvel, cellCvel);
			else if ((!a)&&(!b))
			{
				velocity = interpolateBetweenTwo(x, x1, x2, cellDvel, cellCvel);
				velocity.y = max(velocity.y, 0.0f);
			}
			else if ((!c)&&(!d))
			{
				velocity = interpolateBetweenTwo(x, x1, x2, cellAvel, cellBvel);
				velocity.y = min(velocity.y, 0.0f);
			}
			else
				velocity = interpolateBetweenFour(x, y, x1, y1, x2, y2, cellAvel, cellBvel, cellDvel, cellCvel);

			curr_person->m_velocity = velocity;
		}
	}		
	
}

glm::vec2 SpeedField :: interpolateBetweenTwo(float x, float x1, float x2, glm::vec2 f1, glm::vec2 f2)
{
	glm::vec2 fP = f1 * ((x2 - x) / (x2 - x1)) + f2 * ((x - x1) / (x2 - x1));

	return fP;
}

glm::vec2 SpeedField :: interpolateBetweenFour(float x, float y, float x1, float y1, float x2, float y2, glm::vec2 f11, glm::vec2 f12, glm::vec2 f21, glm::vec2 f22)
{
	float first_x_mul = (x2 - x) / (x2 - x1);
	float second_x_mul = (x - x1) / (x2 - x1);

	glm::vec2 fR1 = f11 * first_x_mul + f21 * second_x_mul;

	glm::vec2 fR2 = f12 * first_x_mul + f22 * second_x_mul;

	glm::vec2 fP = fR1 * ((y2 - y) / (y2 - y1)) + fR2 * ((y - y1) / (y2 - y1));

	return fP;
}



float SpeedField :: interpolateSpeed(float ft, float fv, float p, float pmin, float pmax)
{
	float p_diff = pmax - pmin;

	if(p_diff == 0)
		return ft;

	if(p >= pmax)
		return fv;

	if(p <= pmin)
		return ft;

	float speed = ft + ((p - pmin)/p_diff) * (fv - ft);
	return speed;
}


float SpeedField :: interpolateSpeedCell(glm::vec2 cell_pos, glm::vec2 offset, float topo_speed, float flow_speed)
{
	glm::vec2 offset_pos = cell_pos + offset;


	if(!shared_grid->checkExists(offset_pos))
		return 0.0f;

	SharedCell *offset_cell = &shared_grid->findCellByPos(offset_pos);

	float density = offset_cell->m_density;


	
	return interpolateSpeed(topo_speed, flow_speed, density, *min_density, *max_density);
}

void SpeedField :: assignDensityField(DensityField *df)
{
	densityField = df;

	min_density = &df->m_min_density;
	max_density = &df->m_max_density;
}