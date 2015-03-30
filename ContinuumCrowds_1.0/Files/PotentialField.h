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
#include <limits>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <AntTweakBar.h>

#include "Grid.h"
#include "PopulationManager.h"

#define INFINITY std::numeric_limits<float>::max()

class PotentialField
{
public:
	
	SharedGrid *shared_grid;
	PopulationManager *popManager;
	
	PotentialField();

	void assignCosts();
	void update();


	void assignPotentials();

	void getGroupPotential(Group *group);
	float getCellPotential(GroupCell *cell, GroupGrid *grid);
	
	float getFiniteDifference(GroupGrid *grid, GroupCell *cell, glm::vec2 neigh_pos);
	float getFiniteDifference(GroupGrid *grid, GroupCell *cell, glm::vec2 neigh_pos_x, glm::vec2 neigh_pos_y);

	int checkForLowestTempPotential(std::vector<GroupCell*> &group_cells);
	
	void setGroupPotentialGrads(Group *group);

	void normaliseCellPotetialGrads(GroupCell *cell);

	bool getArgMin(GroupGrid *grid, GroupCell curr_cell, glm::vec2 cell1pos, glm::vec2 cell2pos, glm::vec2 &min_cell_pos);

	void resetGroupPotentials(Group *group);
};

