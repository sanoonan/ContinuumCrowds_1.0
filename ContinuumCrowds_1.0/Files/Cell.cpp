#include "Cell.h"

using namespace std;

#pragma region BASE
Cell :: Cell()
{
	m_position = glm::vec2(0.0f);

	
}


void Cell :: draw(float scale, GLuint spID)
{
	glm::mat4 trans_mat = glm::translate(glm::mat4(), glm::vec3((m_position.x - grid_size.x/2)*scale, (m_position.y - grid_size.y/2)*scale, 0.0f));
	glm::mat4 scale_mat = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 0.01f));
	glm::mat4 model_mat = trans_mat * scale_mat;

	int matrix_location = glGetUniformLocation (spID, "model");
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));
	glutSolidCube(scale);
}

glm::vec2 Cell :: getNeighbourPos(CellFace face)
{
	return m_position + face.m_offset;
}



#pragma endregion

#pragma region SHARED
SharedCell :: SharedCell() : Cell()
{
	m_density = m_height = 0.0f;
	m_avg_velocity = glm::vec2(0.0f);
}

void SharedCell :: drawDensities(float scale, GLuint spID)
{
	glUseProgram(spID);

	int density_location = glGetUniformLocation (spID, "value");
	glUniform1f (density_location, m_density);

	draw(scale, spID);
}

void SharedCell :: drawHeights(float scale, GLuint spID)
{
	glUseProgram(spID);

	int height_location = glGetUniformLocation (spID, "value");
	glUniform1f (height_location, m_height);

	draw(scale, spID);
}

void SharedCell :: setFaces()
{
	float curr_angle = 0.0f;
	glm::vec2 offset(1.0f, 0.0f);

	for(int i=0; i<4; i++)
	{
		if(curr_angle == 90)
			offset = glm::vec2(0.0f, 1.0f);
		else if (curr_angle == 180)
			offset = glm::vec2(-1.0f, 0.0f);
		else if (curr_angle == 270)
			offset = glm::vec2(0.0f, -1.0f);

		m_faces[i].setAngle(curr_angle, offset);
		curr_angle += 90.0f;
	}
}


#pragma endregion

#pragma region GROUP
GroupCell :: GroupCell() : Cell()
{
	m_position = glm::vec2(0.0f);
	m_discomfort = m_potential = 0.0f;
}

void GroupCell :: setFaces()
{
	float curr_angle = 0.0f;
	glm::vec2 offset(1.0f, 0.0f);

	for(int i=0; i<4; i++)
	{
		if(curr_angle == 90)
			offset = glm::vec2(0.0f, 1.0f);
		else if (curr_angle == 180)
			offset = glm::vec2(-1.0f, 0.0f);
		else if (curr_angle == 270)
			offset = glm::vec2(0.0f, -1.0f);

		m_faces[i].setAngle(curr_angle, offset);
		curr_angle += 90.0f;
	}
}

GroupCellFace GroupCell :: getFaceByNeighbour(glm::vec2 neighbour_pos)
{
	glm::vec2 offset = neighbour_pos - m_position;

	for(int i=0; i<4; i++)
		if(offset == m_faces[i].m_offset)
			return m_faces[i];
}

void GroupCell :: drawPotentials(float scale, GLuint spID)
{
	glUseProgram(spID);

	int potential_location = glGetUniformLocation (spID, "value");
	glUniform1f (potential_location, m_potential);

	draw(scale, spID);

}

void GroupCell :: drawTeapots(float scale,GLuint spID)
{

	glm::vec3 colour(1.0f, 0.0f, 0.0f);

	glm::vec2 pot = m_tot_grad_potential;
	pot = -pot;
	if(pot != glm::vec2(0.0f))
	{
		float angle;

		if(pot.x == 0.0f)
			if(pot.y > 0.0f)
				angle = 90.0f;
			else
				angle = 270.0f;
		else if(pot.y == 0.0f)
			if(pot.x > 0.0f)
				angle = 0.0f;
			else
				angle = 180.0f;
		else
		{
			colour = glm::vec3(0.0f);
			angle = glm::atan(pot.y , pot.x);
			angle = glm::degrees(angle);
		}
	

		float size = 0.1f;

		glUseProgram(spID);

		glm::mat4 trans_mat = glm::translate(glm::mat4(), glm::vec3((m_position.x - grid_size.x/2)*scale, (m_position.y - grid_size.y/2)*scale, 0.08f));
		glm::mat4 scale_mat = glm::scale(glm::mat4(), glm::vec3(size*scale, 1.2*size*scale, 0.001f));
		glm::mat4 rot_mat = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 model_mat = trans_mat * rot_mat * scale_mat;

		int colour_location = glGetUniformLocation (spID, "colour");
		glUniform3fv(colour_location, 1, glm::value_ptr(colour));
		int matrix_location = glGetUniformLocation (spID, "model");
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));
		glutSolidTeapot(1.0f);
	}
}


void GroupCell :: unTempPotential()
{
	m_potential = m_temp_potential;
}

#pragma endregion

