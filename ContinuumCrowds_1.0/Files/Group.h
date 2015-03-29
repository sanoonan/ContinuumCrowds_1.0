#pragma once

#define _CRT_SECURE_NO_DEPRECATE
//Some Windows Headers (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>


// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <string>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <AntTweakBar.h>

#include "Person.h"
#include "Grid.h"

using namespace std;

class Group
{
public:
	
	int m_num_people;

	float m_distance_coeff, m_time_coeff, m_discomfort_coeff;

	float max_speed, min_speed;

	glm::vec3 m_colour;

	std::vector<glm::vec2> m_goal;
	std::vector<Person> m_people;

	GroupGrid m_grid;

	Group();
	Group(int num_people);
	Group(int num_people, glm::vec3 colour);

	void draw(float scale, GLuint spID);



	void setRightSideGoal();
	void setLeftSideGoal();

	void setBottomRightCornerGoal();
	void setBottomLeftCornerGoal();
	void setTopRightCornerGoal();
	void setTopLeftCornerGoal();

	void setupGrid(SharedGrid &shared_grid);




	void assignRandomLocs();

	void assignRandomLeftLoc();
	void assignRandomRightLoc();

	void assignRandomTopLeftLoc();
	void assignRandomTopRightLoc();
	void assignRandomBottomLeftLoc();
	void assignRandomBottomRightLoc();

	void addTBar(TwBar *bar);

	void setSpeeds(float min, float max);

	void move(float dt);
};

