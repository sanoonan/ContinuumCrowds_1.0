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

#include "Grid.h"
#include "PopulationManager.h"
#include "DensityField.h"

class SpeedField
{
public:
	
	SharedGrid *shared_grid;
	PopulationManager *popManager;

	DensityField *densityField;
	
	SpeedField();

	float *min_density, *max_density;

	void assignDensityField(DensityField *df);

	float getTopoSpeed(float fmax, float fmin, float smax, float smin, float grad_height);



	float getFlowSpeed(glm::vec2 cell_pos, glm::vec2 offset);

	float interpolateSpeed(float ft, float fv, float p, float pmin, float pmax);
	float interpolateSpeedCell(glm::vec2 cell_pos, glm::vec2 offset, float topo_speed, float flow_speed);
	
	void assignSpeeds();
	void update();

	void getCellSpeeds(Group *group, glm::vec2 cell_pos);

	void assignDirSpeeds();
	float getCellDirSpeed(Group *group, glm::vec2 cell_pos);

	void assignPeopleVels();
	
	glm::vec2 interpolateBetweenTwo(float x, float x1, float x2, glm::vec2 f1, glm::vec2 f2);
	glm::vec2 interpolateBetweenFour(float x, float y, float x1, float y1, float x2, float y2, glm::vec2 f11, glm::vec2 f12, glm::vec2 f21, glm::vec2 f22);
	
};

