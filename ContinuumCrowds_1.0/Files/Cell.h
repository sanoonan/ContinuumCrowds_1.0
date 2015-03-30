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

#include "CellFace.h"

class Cell
{
public:
	
	glm::vec2 m_position;
	
	

	glm::vec2 grid_size;


	Cell();

	void draw(float scale, GLuint spID);

	glm::vec2 getNeighbourPos(CellFace face);

	

	virtual void setFaces() = 0;
};

class SharedCell : public Cell
{
public:

	float m_density, m_height;
	glm::vec2 m_avg_velocity;
	float m_discomfort;


	SharedCellFace m_faces[4];

	SharedCell();

	void drawDensities(float scale, GLuint spID);
	void drawHeights(float scale, GLuint spID);
	void drawDiscomfort(float scale, GLuint spID);

	void setFaces();
};

class GroupCell : public Cell
{
public:

	float m_potential;
	float m_temp_potential;

	glm::vec2 m_tot_velocity;


	GroupCellFace m_faces[4];

	GroupCell();

	void setFaces();

	GroupCellFace getFaceByNeighbour(glm::vec2 neighbour_pos);

	void drawPotentials(float scale, GLuint spID);


	void unTempPotential();
};

