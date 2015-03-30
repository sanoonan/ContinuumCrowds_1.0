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
#include <cmath>
#include <vector> // STL dynamic memory.
#include <string>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <AntTweakBar.h>

#include "Cell.h"

using namespace std;

#define INFINITY std::numeric_limits<float>::max()

class Grid
{
public:
	
	int m_width, m_height;

//	std::vector<std::vector<Cell>> m_cells;

	Grid();
	Grid(int width, int height);
	
	void setupGridCells(int width, int height);


	virtual void setupGridCells() = 0;

	bool checkExists(glm::vec2 cell_pos);

	glm::vec2 findClosestCellPos(glm::vec2 cell_pos);

	void getNeighbours(glm::vec2 cell, glm::vec2 *cells);

	void checkMinMax(float value, float &min_value, float &max_value);

	void drawOutline(float scale, GLuint spID);
};

class SharedGrid : public Grid
{
public:
	std::vector<std::vector<SharedCell>> m_cells;

	float max_slope, min_slope;

	SharedGrid();
	SharedGrid(int width, int height);

	void setupGridCells();
	SharedCell &findCellByPos(glm::vec2 pos);
	
//	void setupGridCells(int width, int height);

	void drawDensities(float scale, GLuint spIDlin, GLuint spIDdensity);
	void drawHeights(float scale, GLuint spIDlin, GLuint spIDheight);
	void drawDiscomfort(float scale, GLuint spIDlin, GLuint spIDdiscomfort);

	void assignRandomHeights(int max_height);
	void makeMiddleMountain(int max_height, float radius);
	void setHeightGrads();

	void assignDiscomfortToBottom(float dis);

	void checkMinMaxSlopes(float value);

};

class GroupGrid : public Grid
{

public:
	std::vector<std::vector<GroupCell>> m_cells;


	GroupGrid();
	GroupGrid(int width, int height);

	void setupGridCells();
	GroupCell &findCellByPos(glm::vec2 pos);


	void copyGridSize(SharedGrid &grid);

	void drawPotentials(float scale, GLuint spIDline, GLuint spIDpotential);



};

