#include "Person.h"

using namespace std;

Person :: Person()
{
	m_position = m_velocity = glm::vec2(0.0f);

}


void Person :: draw(float grid_width, float grid_height, float scale, GLuint spID)
{
	glm::mat4 trans_mat = glm::translate(glm::mat4(), glm::vec3((m_position.x - grid_width/2)*scale, (m_position.y - grid_height/2)*scale, 0.1f));
//	glm::mat4 scale_mat = glm::scale(glm::mat4(), glm::vec3(scale, scale, 0.01f));
	glm::mat4 scale_mat = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 0.01f));
	glm::mat4 model_mat = trans_mat * scale_mat;

	int matrix_location = glGetUniformLocation (spID, "model");
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, glm::value_ptr(model_mat));
	glutSolidSphere(0.1f, 4, 4);
}



void Person :: assignRandomLoc(int grid_width, int grid_height)
{
	int t = rand();

	float w = (rand()%((grid_width)*1000));
	w /= 1000;
	w -= 0.5f;

	float h = (rand()%((grid_height)*1000));
	h /= 1000;
	h -= 0.5f;
	

	m_position = glm::vec2(w, h);
}

void Person :: assignRandomLoc(int min_width, int max_width, int min_height, int max_height)
{
	int t = rand();

	float w = (rand()%((max_width - min_width)*1000));
	w /= 1000;
	w -= 0.5f;
	w += min_width;

	float h = (rand()%((max_height - min_height)*1000));
	h /= 1000;
	h -= 0.5f;
	h += min_height;

	m_position = glm::vec2(w, h);
}

void Person :: move(float dt)
{
	m_position += dt * m_velocity;
}