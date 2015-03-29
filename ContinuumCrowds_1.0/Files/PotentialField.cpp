#include "PotentialField.h"

using namespace std;


PotentialField :: PotentialField()
{
}

void PotentialField :: assignCosts()
{
	Group *curr_group;
	GroupGrid *curr_grid;
	GroupCell *curr_group_cell, *neighbour_cell;
	glm::vec2 cell_pos, neighbour_pos;
	GroupCellFace *curr_cellface;

	float cost, time_coeff, distance_coeff, discomfort_coeff, speed, discomfort;

	int width = shared_grid->m_width;
	int height = shared_grid->m_height;

	int num_groups = popManager->m_num_groups;
	for(int i=0; i<num_groups; i++)
	{
		curr_group = popManager->m_groups[i];
		curr_grid = &curr_group->m_grid;

		distance_coeff = curr_group->m_distance_coeff;
		time_coeff = curr_group->m_time_coeff;
		discomfort_coeff = curr_group->m_discomfort_coeff;

		for(int j=0; j<width; j++)
			for(int k=0; k<height; k++)
			{
				cell_pos = glm::vec2(j, k);
				curr_group_cell = &curr_grid->findCellByPos(cell_pos);

				for(int l=0; l<4; l++)
				{
					curr_cellface = &curr_group_cell->m_faces[l];
					speed = curr_cellface->m_speed;

					neighbour_pos = curr_group_cell->getNeighbourPos(*curr_cellface);

					if((curr_grid->checkExists(neighbour_pos))&&(speed != 0.0f))
					{ 
						neighbour_cell = &curr_grid->findCellByPos(neighbour_pos);
						discomfort = neighbour_cell->m_discomfort;

						speed = curr_cellface->m_speed;

						cost = (distance_coeff * speed + time_coeff + discomfort_coeff * discomfort) / speed;
					}
					else
						cost = INFINITY;


					curr_cellface->m_cost = cost;
				}
			}
	}
}

void PotentialField :: update()
{
	assignCosts();
	assignPotentials();
}

void PotentialField :: assignPotentials()
{
	int num_groups = popManager->m_num_groups;

	for(int i=0; i<num_groups; i++)
	{
		getGroupPotential(popManager->m_groups[i]);
		setGroupPotentialGrads(popManager->m_groups[i]);
	}
}

bool checkForCellByPos(std::vector<GroupCell*> &group_cells, glm::vec2 pos)
{
	int num_cells = group_cells.size();
	for(int i=0; i<num_cells; i++)
		if(pos == group_cells[i]->m_position)
			return true;

	return false;
}

int checkForLowestTempPotential(std::vector<GroupCell*> &group_cells)
{
	int num_cells = group_cells.size();
	float min_pot = group_cells[0]->m_temp_potential;
	int min_pot_num = 0;
	float pot;

	for(int i=1; i<num_cells; i++)
	{
		pot = group_cells[i]->m_temp_potential;
		if(pot < min_pot)
		{
			min_pot = pot;
			min_pot_num = i;
		}
	}
	return min_pot_num;
}

void PotentialField :: getGroupPotential(Group *group)
{
	resetGroupPotentials(group);

	GroupGrid *grid = &group->m_grid;

	int grid_width = grid->m_width;
	int grid_height = grid->m_height;

	std::vector<GroupCell*> known, candidate;

	int num_candidate = 0;

	std::vector<glm::vec2> *goal = &group->m_goal;
	int num_known = goal->size();

	known.resize(num_known);

	std::vector<std::vector<bool>> isKnown(grid_width);
	for(int i=0; i<grid_width; i++)
	{
		isKnown[i].resize(grid_height);
		
		for(int j=0; j<grid_height; j++)
			isKnown[i][j] = false;
	}

	
	std::vector<std::vector<bool>> isCandidate = isKnown;

	int posx, posy;

	for(int i=0; i<num_known; i++)
	{
		known[i] = &grid->findCellByPos((*goal)[i]);
		known[i]->m_potential = 0.0f;

		posx = (*goal)[i].x;
		posy = (*goal)[i].y;

		isKnown[posx][posy] = true;
	}

	int tot_num_cells = grid->m_height * grid->m_width;
	
	glm::vec2 neighbours_pos[4];
	glm::vec2 curr_neighbour_pos;
	GroupCell *candidate_cell, *curr_known;;
	int best_candidate_num;
	float temp_potential;

	

	


	

	while(num_known < tot_num_cells)
	{
		for(int i=0; i<num_known; i++)
		{
			curr_known = known[i];
			grid->getNeighbours(curr_known->m_position, neighbours_pos);
			for(int j=0; j<4; j++)
			{
				curr_neighbour_pos = neighbours_pos[j];
				if(grid->checkExists(curr_neighbour_pos))
				{
					posx = curr_neighbour_pos.x;
					posy = curr_neighbour_pos.y;

//					if(!checkForCellByPos(known, curr_neighbour_pos))
					if(!isKnown[posx][posy])
//						if(!checkForCellByPos(candidate, curr_neighbour_pos))
						if(!isCandidate[posx][posy])
						{
							candidate_cell = &grid->findCellByPos(curr_neighbour_pos);
							candidate.push_back(candidate_cell);

							isCandidate[curr_neighbour_pos.x][curr_neighbour_pos.y] = true;

							num_candidate++;
						}
				}
			}
		}

		for(int i=0; i<num_candidate; i++)
		{
			temp_potential = getCellPotential(candidate[i], grid);
			candidate[i]->m_temp_potential = temp_potential;
		}

		best_candidate_num = checkForLowestTempPotential(candidate);
		candidate_cell = candidate[best_candidate_num];
		candidate_cell->unTempPotential();
		candidate.erase (candidate.begin()+best_candidate_num);
		num_candidate--;

		known.push_back(candidate_cell);

		glm::vec2 cand_pos = candidate_cell->m_position;

		isKnown[cand_pos.x][cand_pos.y] = true;
		isCandidate[cand_pos.x][cand_pos.y] = false;

		num_known++;


	}

}



float PotentialField :: getCellPotential(GroupCell *cell, GroupGrid *grid)
{
	float final_potential;

	bool nox, noy;
	nox = noy = false;

	glm::vec2 cell_pos = cell->m_position;
	glm::vec2 faces[4];
	grid->getNeighbours(cell_pos, faces);

	glm::vec2 mx_pos, my_pos;

	nox = getArgMin(grid, *cell, faces[0], faces[2], mx_pos);
	noy = getArgMin(grid, *cell, faces[1], faces[3], my_pos);

	float costx, costy, potentialx, potentialy;
	GroupCell mx, my;
	GroupCellFace face_mx, face_my;


	if(!nox)
	{
		if(!noy)
			return 0.0f;


		final_potential = getFiniteDifference(grid, cell, my_pos);
	}
	else if(!noy)
		final_potential = getFiniteDifference(grid, cell, mx_pos);
	else
		final_potential = getFiniteDifference(grid, cell, mx_pos, my_pos);

	return final_potential;
}

float PotentialField :: getFiniteDifference(GroupGrid *grid, GroupCell *cell, glm::vec2 neigh_pos)
{
	float cost, potential;
	GroupCell m;
	GroupCellFace face_m;

	face_m = cell->getFaceByNeighbour(neigh_pos);
	m = grid->findCellByPos(neigh_pos);

	cost = face_m.m_cost;
	potential = m.m_potential;

	float arg1 = potential + cost;
	float arg2 = potential - cost;

//	return (cost * cost) + potential;

	return max(arg1, arg2);
}

float PotentialField :: getFiniteDifference(GroupGrid *grid, GroupCell *cell, glm::vec2 neigh_pos_x, glm::vec2 neigh_pos_y)
{
	float costx, costy, potentialx, potentialy;
	GroupCell mx, my;
	GroupCellFace face_mx, face_my;


	face_mx  = cell->getFaceByNeighbour(neigh_pos_x);
	mx = grid->findCellByPos(neigh_pos_x);
		
	costx = face_mx.m_cost;
	potentialx = mx.m_potential;

	face_my = cell->getFaceByNeighbour(neigh_pos_y);
	my = grid->findCellByPos(neigh_pos_y);

	costy = face_my.m_cost;
	potentialy = my.m_potential;

	float a, b, c;

	a = (costx * costx) + (costy * costy);

	b = -2 * ((costx * costx * potentialy) + (costy * costy * potentialx));

	c = (costx * costx * potentialy * potentialy) + (costy * costy * potentialx * potentialx) - (costx * costx * costy * costy);

	float under_sqrt = (b * b) - (4 * a * c);

	if(under_sqrt >= 0)
	{
		float const_part = -b;

		float change_part = sqrt(under_sqrt);

		float mul_part = 1/(2*a);

		float arg1 = (const_part + change_part)*mul_part;
		float arg2 = (const_part - change_part)*mul_part;

		float potential = max(arg1, arg2);

		if((potential >= potentialx)&&(potential >= potentialy))
			return potential;
		

	}

	if(potentialx < potentialy)
		return getFiniteDifference(grid, cell, neigh_pos_x);
	else
		return getFiniteDifference(grid, cell, neigh_pos_y);
	
}



void PotentialField :: setGroupPotentialGrads(Group *group)
{
	GroupGrid *grid = &group->m_grid;

	int curr_angle;
	GroupCell *curr_cell;
	GroupCellFace *curr_face;
	bool left, right, top, bot;
	glm::vec2 neighbours[4];
	GroupCell *neighbour_cell;
	glm::vec2 cell_pos;
	float pot_grad, curr_pot, neighbour_pot;
	glm::vec2 tot_pot_grad;

	int width = grid->m_width;
	int height = grid->m_height;

	float speed;

	for(int i=0; i<width; i++)
		for(int j=0; j<height; j++)
		{
			tot_pot_grad = glm::vec2(0.0f);
			curr_cell = &grid->m_cells[i][j];
			cell_pos = glm::vec2(i, j); 
			curr_pot = curr_cell->m_potential;
			grid->getNeighbours(cell_pos, neighbours);
			for(int k=0; k<4; k++)
			{
				curr_face = &curr_cell->m_faces[k];
				if(grid->checkExists(neighbours[k]))
				{
					neighbour_cell = &grid->findCellByPos(neighbours[k]);
					neighbour_pot = neighbour_cell->m_potential;
					pot_grad = neighbour_pot - curr_pot;

					
					if(k > 1)
						pot_grad = -pot_grad;


					if(k==0)
						tot_pot_grad.x = pot_grad;
					else if(k==1)
						tot_pot_grad.y = pot_grad;

					else if(k==2)
					{
						if(tot_pot_grad.x == INFINITY)
							if(pot_grad < 0.0f)
								tot_pot_grad.x = 0.0f;
							else
								tot_pot_grad.x = pot_grad;
					}
					else if(k==3)
					{
						if(tot_pot_grad.y == INFINITY)
							if(pot_grad < 0.0f)
								tot_pot_grad.y = 0.0f;
							else
								tot_pot_grad.y = pot_grad;
					}
				}
				else
				{
					pot_grad = INFINITY;

					if(k==0)
						tot_pot_grad.x = INFINITY;
					else if(k==1)
						tot_pot_grad.y = INFINITY;

					else if(k==2)
					{
						if(tot_pot_grad.x > 0.0f)
							tot_pot_grad.x = 0.0f;
					}
					else if(k==3)
						if(tot_pot_grad.y > 0.0f)
							tot_pot_grad.y = 0.0f;
				}

				curr_face->m_grad_potential = pot_grad;
			}
			if(tot_pot_grad != glm::vec2(0.0f))
				tot_pot_grad = glm::normalize(tot_pot_grad);

			curr_cell->m_tot_grad_potential = tot_pot_grad;

	//		glm::vec2 speed = curr_cell->m_tot_speed;
	//		float dir_speed = glm::dot(speed, tot_pot_grad);
	//		glm::vec2 vel = -dir_speed * tot_pot_grad;
//			glm::vec2 vel;

			/*
			float speedx, speedy;

			if(tot_pot_grad.x > 0)
				speedx = curr_cell->m_faces[0].m_speed;
			else if (tot_pot_grad.x < 0)
				speedx = curr_cell->m_faces[2].m_speed;
			else
				speedx = 0.0f;

			if(tot_pot_grad.y > 0)
				speedy = curr_cell->m_faces[1].m_speed;
			else if (tot_pot_grad.y < 0)
				speedy = curr_cell->m_faces[3].m_speed;
			else
				speedy = 0.0f;

			vel.x = -speedx * tot_pot_grad.x;
			vel.y = -speedy * tot_pot_grad.y;

			*/


	//		glm::vec2 vel = -tot_pot_grad;
	//		curr_cell->m_velocity = vel;
		}
}

float angleUnitCircle(float angle)
{
	while(angle < 0)
		angle += 360;

	while(angle >= 360)
		angle -= 360;

	return angle;
}

/*
void PotentialField :: setCellVelocity(GroupCell *cell)
{
	std::vector<float> face_grad_pot(4);
	GroupCellFace *curr_face;
	for(int i=0; i<4; i++)
	{
		curr_face = &cell->m_faces[i];
		face_grad_pot[i] = curr_face->m_grad_potential;
	}

	float final_x, final_y;

	if(face_grad_pot[0] == INFINITY)
		if(face_grad_pot[2] == INFINITY)
			final_x = 0.0f;
		else
			final_x = -face_grad_pot[2];
	else if(face_grad_pot[2] == INFINITY)
		final_x = face_grad_pot[0];
	else
		final_x = face_grad_pot[0] - face_grad_pot[2];

	if(face_grad_pot[1] == INFINITY)
		if(face_grad_pot[3] == INFINITY)
			final_y = 0.0f;
		else
			final_y = -face_grad_pot[3];
	else if(face_grad_pot[3] == INFINITY)
		final_y = face_grad_pot[1];
	else
		final_y = face_grad_pot[1] - face_grad_pot[3];

	glm::vec2 tot_grad_pot(final_x, final_y);
	tot_grad_pot = glm::normalize(tot_grad_pot);

	cell->m_tot_grad_potential = tot_grad_pot;

	float angle = atan(tot_grad_pot.y / tot_grad_pot.x);
	angle = glm::degrees(angle);
	angle = angleUnitCircle(angle);

	int face1, face2;

	if(angle < 90)
	{
		face1 = 0;
		face2 = 1;
	}
	else if (angle < 180)
	{
		face1 = 1;
		face2 = 2;
	}
	else if (angle < 270)
	{
		face1 = 2;
		face2 = 3;
	}
	else
	{
		face1 = 3;
		face2 = 0;
	}

	while(angle >= 90)
		angle -= 90;
	angle /= 90;

	float speed1 = cell->m_faces[face1].m_speed;
	float speed2 = cell->m_faces[face2].m_speed;

	float speed = speed1 + angle * (speed2 - speed1);

	glm::vec2 vel = -speed * tot_grad_pot;
	cell->m_velocity = vel;

}
*/



bool PotentialField :: getArgMin(GroupGrid *grid, GroupCell curr_cell, glm::vec2 cell1pos, glm::vec2 cell2pos, glm::vec2 &min_cell_pos)
{
	bool not1, not2;

	not1 = not2 = false;

	GroupCell cell;
	float potential;
	GroupCellFace cellface;
	float cost;
	float arg1, arg2;

	if(grid->checkExists(cell1pos))
	{
		cell = grid->findCellByPos(cell1pos);
		potential = cell.m_potential;
		if(potential != INFINITY)
		{
			cellface = curr_cell.getFaceByNeighbour(cell1pos);
			cost = cellface.m_cost;
			if(cost != INFINITY)
				arg1 = potential + cost;
			else
				not1 = true;
		}
		else
			not1 = true;
	}
	else
		not1 = true;


	if(grid->checkExists(cell2pos))
	{
		cell = grid->findCellByPos(cell2pos);
		potential = cell.m_potential;
		if(potential != INFINITY)
		{
			cellface = curr_cell.getFaceByNeighbour(cell2pos);
			cost = cellface.m_cost;
			if(cost != INFINITY)
				arg2 = potential + cost;
			else
				not2 = true;
		}
		else
			not2 = true;
	}
	else
		not2 = true;

	
	if(not1)
		if(not2)
			return false;
		else
		{
			min_cell_pos = cell2pos;
			return true;
		}

	if(not2)
	{
		min_cell_pos = cell1pos;
		return true;
	}

	if(arg1 <= arg2)
		min_cell_pos = cell1pos;
	else
		min_cell_pos = cell2pos;

	return true;
}

void PotentialField :: resetGroupPotentials(Group *group)
{
	GroupGrid *grid = &group->m_grid;
	GroupCell *curr_cell;

	int width = grid->m_width;
	int height = grid->m_height;

	for(int i=0; i<width; i++)
		for(int j=0; j<height; j++)
		{
			curr_cell = &grid->m_cells[i][j];
			curr_cell->m_potential = INFINITY;
		}
}

