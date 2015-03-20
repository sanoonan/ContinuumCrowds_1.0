#include "Grid.h"

using namespace std;

#pragma region BASE
Grid :: Grid()
{
	m_width = m_height = 0;
}

Grid :: Grid(int width, int height)
{
	m_width = width;
	m_height = height;
}

bool Grid :: checkExists(glm::vec2 cell_pos)
{
	int x = cell_pos.x;
	int y = cell_pos.y;

	if(x < 0)
		return false;
	if(x >= m_width)
		return false;

	if(y < 0)
		return false;
	if(y >= m_height)
		return false;

	return true;
}

std::vector<glm::vec2> Grid :: getNeighbours(glm::vec2 cell)
{
	int x = cell.x;
	int y = cell.y;

	std::vector<glm::vec2> cells(4);

	cells[0] = glm::vec2(x+1, y);
	cells[1] = glm::vec2(x, y+1);
	cells[2] = glm::vec2(x-1, y);
	cells[3] = glm::vec2(x, y-1);

	return cells;
}

void Grid :: checkMinMax(float value, float &min_value, float &max_value)
{
	if(value < min_value)
		min_value = value;

	if(value > max_value)
		max_value = value;
}

void Grid :: drawOutline(float scale, GLuint spID)
{

	glm::mat4 trans_mat = glm::translate(glm::mat4(), glm::vec3(-0.5f * scale, -0.5f * scale, 0.0f));
	glm::mat4 scale_mat = glm::scale(glm::mat4(), glm::vec3(m_width, m_height, 0.01f));
	glm::mat4 model_mat = trans_mat * scale_mat;

	int matrix_location = glGetUniformLocation (spID, "model");
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));
	glutWireCube(scale);
}

#pragma endregion

#pragma region SHARED
SharedGrid :: SharedGrid() : Grid()
{
	max_slope = min_slope = 0.0f;
}

SharedGrid :: SharedGrid(int width, int height) : Grid(width, height)
{
	min_slope = 0.0f;
	max_slope = 0.0f;
}

void SharedGrid :: setupGridCells()
{
	m_cells.resize(m_width);
	for(int i=0; i<m_height; i++)
	{
		m_cells[i].resize(m_height);
		for(int j=0; j<m_height; j++)
		{
			m_cells[i][j].m_position = glm::vec2(i, j);
			m_cells[i][j].grid_size = glm::vec2(m_width, m_height);
			m_cells[i][j].setFaces();
		}
	}


}
void SharedGrid :: checkMinMaxSlopes(float value)
{
	checkMinMax(value, min_slope, max_slope);
}


/*
void SharedGrid :: setupGridCells(int width, int height)
{
	m_width = width;
	m_height = height;

	setupGridCells();
}
*/

SharedCell& SharedGrid :: findCellByPos(glm::vec2 pos)
{
	return m_cells[pos.x][pos.y];
}




void SharedGrid :: drawDensities(float scale, GLuint spIDline, GLuint spIDdensity)
{
	drawOutline(scale, spIDline);

	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
			m_cells[i][j].drawDensities(scale, spIDdensity);
}

void SharedGrid :: drawHeights(float scale, GLuint spIDline, GLuint spIDheight)
{
	drawOutline(scale, spIDline);

	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
			m_cells[i][j].drawHeights(scale, spIDheight);
}

void SharedGrid :: assignRandomHeights(int max_height)
{
	int h;
	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
		{
			h = rand()%max_height;
			m_cells[i][j].m_height = h;
		}

	setHeightGrads();
}

void SharedGrid :: setHeightGrads()
{
	int curr_angle;
	SharedCell *curr_cell;
	SharedCellFace *curr_face;
	bool left, right, top, bot;
	std::vector<glm::vec2> neighbours;
	SharedCell *neighbour_cell;
	glm::vec2 cell_pos;
	float height_grad, curr_height, neighbour_height;

	glm::vec2 tot_height_grad;

	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
		{
			curr_cell = &m_cells[i][j];
			cell_pos = glm::vec2(i, j); 
			curr_height = curr_cell->m_height;
			neighbours = getNeighbours(cell_pos);

			tot_height_grad = glm::vec2(0.0f);

			for(int k=0; k<4; k++)
			{
				curr_face = &curr_cell->m_faces[k];
				if(checkExists(neighbours[k]))
				{
					neighbour_cell = &findCellByPos(neighbours[k]);
					neighbour_height = neighbour_cell->m_height;
					height_grad = neighbour_height - curr_height;

					if(k>1)
						height_grad = -height_grad;

					if(k%2 == 0)
						tot_height_grad.x += height_grad;
					else
						tot_height_grad.y += height_grad;
				}
				else
					height_grad = 0.0f;

				checkMinMaxSlopes(height_grad);

				curr_face = &curr_cell->m_faces[k];
				curr_face->m_grad_height = height_grad;

			}

			curr_cell->m_tot_grad_height = tot_height_grad;
		}
}

void SharedGrid :: makeMiddleMountain(int max_height, float radius)
{
	int x_spread = (m_width * radius) / 2;
	int y_spread = (m_height * radius) / 2;

	float max_dist = glm::length(glm::vec2(x_spread, y_spread));

	int x_mid = m_width/2;
	int y_mid = m_height/2;

	glm::vec2 mid(x_mid, y_mid);
	

	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
		{
			glm::vec2 curr_pos(i, j);
			float dist = glm::distance(mid, curr_pos);
			float h;
			if(dist > max_dist)
				h = 0.0f;
			else
				h = (max_height * (max_dist - dist)) / max_dist;

			m_cells[i][j].m_height = h;
		}

	setHeightGrads();
}

#pragma endregion


#pragma region GROUP
GroupGrid :: GroupGrid() : Grid()
{
}

GroupGrid :: GroupGrid(int width, int height) : Grid(width, height)
{
}

void GroupGrid :: setupGridCells()
{
	m_cells.resize(m_width);
	for(int i=0; i<m_height; i++)
	{
		m_cells[i].resize(m_height);
		for(int j=0; j<m_height; j++)
		{
			m_cells[i][j].m_position = glm::vec2(i, j);
			m_cells[i][j].grid_size = glm::vec2(m_width, m_height);
			m_cells[i][j].setFaces();
		}
	}
}
/*
void GroupGrid :: setupGridCells(int width, int height)
{
	m_width = width;
	m_height = height;

	setupGridCells();
}
*/

GroupCell& GroupGrid :: findCellByPos(glm::vec2 pos)
{
	return m_cells[pos.x][pos.y];
}

void GroupGrid :: copyGridSize(SharedGrid &grid)
{
	m_width = grid.m_width;
	m_height = grid.m_height;
	setupGridCells();
}

void GroupGrid :: drawPotentials(float scale, GLuint spIDline, GLuint spIDpotential)
{
	drawOutline(scale, spIDline);

	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
			m_cells[i][j].drawPotentials(scale, spIDpotential);
			
}

void GroupGrid :: drawTeapots(float scale, GLuint spID)
{
	for(int i=0; i<m_width; i++)
		for(int j=0; j<m_height; j++)
			m_cells[i][j].drawTeapots(scale, spID);
}


#pragma endregion