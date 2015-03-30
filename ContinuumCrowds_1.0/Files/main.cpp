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
#include <stdlib.h>
#include "assert.h"

// Header includes

#include <time.h> 
#include "Camera.h"
#include "Shaders.h"
#include "PopulationManager.h"
#include "DensityField.h"
#include "Grid.h"
#include "SpeedField.h"
#include "PotentialField.h"

#include "AntTweakBar.h"

using namespace std;

int width = 1200;
int height = 900;


#define VS_LINE "../Shaders/lineVS.txt"
#define FS_LINE "../Shaders/lineFS.txt"

#define VS_MINMAX "../Shaders/minMaxVS.txt"
#define FS_MINMAX "../Shaders/minMaxFS.txt"

int oldTime = 0;



Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));


glm::mat4 proj_mat, view_mat;


bool is_paused = false;

float fps = 0.0f;




Shaders line_shader("Line", VS_LINE, FS_LINE);
Shaders density_shader("Density", VS_MINMAX, FS_MINMAX);
Shaders height_shader("Height", VS_MINMAX, FS_MINMAX);
Shaders potential_shader("Potential", VS_MINMAX, FS_MINMAX);
Shaders discomfort_shader("Discomfort", VS_MINMAX, FS_MINMAX);


PopulationManager popManager;

//VORTEX SCENARTIO
SharedGrid mainGrid(32, 32);
Group groupA(10, glm::vec3(1.0f, 0.5f, 0.0f));
Group groupB(10, glm::vec3(0.0f, 1.0f, 1.0f));
Group groupC(10, glm::vec3(1.0f, 0.0f, 0.0f));
Group groupD(10, glm::vec3(1.0f, 1.0f, 0.0f));

DensityField densityField;
SpeedField speedField;
PotentialField potentialField;





float draw_scale = 0.7f;

bool draw_densities = false;
bool draw_heights = true;
bool draw_potentials = false;
bool draw_discomfort = false;


#pragma region DENSITY SHADER STUFF
float min_density, max_density;
glm::vec3 max_density_col, min_density_col;

void updateDensityShaderValues()
{
	glUseProgram(density_shader.id);

	int max_col_location = glGetUniformLocation (density_shader.id, "max_colour");
	int min_col_location = glGetUniformLocation (density_shader.id, "min_colour");
	glUniform3fv (max_col_location, 1,  glm::value_ptr(max_density_col));
	glUniform3fv (min_col_location, 1,  glm::value_ptr(min_density_col));

	int max_dens_location = glGetUniformLocation (density_shader.id, "max_value");
	int min_dens_location = glGetUniformLocation (density_shader.id, "min_value");
	glUniform1f (max_dens_location, max_density);
	glUniform1f (min_dens_location, min_density);
}

void initDensityShaderValues()
{
	max_density = 8.0f;
	min_density = 1.0f;

	max_density_col = glm::vec3(1.0f, 0.0f, 0.0f);
	min_density_col = glm::vec3(0.0f, 1.0f, 0.0f);

	updateDensityShaderValues();
}

#pragma endregion

#pragma region HEIGHT SHADER STUFF
float min_height, max_height;
glm::vec3 max_height_col, min_height_col;

void updateHeightShaderValues()
{
	glUseProgram(height_shader.id);

	int max_col_location = glGetUniformLocation (height_shader.id, "max_colour");
	int min_col_location = glGetUniformLocation (height_shader.id, "min_colour");
	glUniform3fv (max_col_location, 1,  glm::value_ptr(max_height_col));
	glUniform3fv (min_col_location, 1,  glm::value_ptr(min_height_col));

	int max_height_loc = glGetUniformLocation (height_shader.id, "max_value");
	int min_height_loc = glGetUniformLocation (height_shader.id, "min_value");
	glUniform1f (max_height_loc, max_height);
	glUniform1f (min_height_loc, min_height);
}

void initHeightShaderValues()
{
	max_height = 10.0f;
	min_height = 0.0f;

	max_height_col = glm::vec3(0.2f, 0.1f, 0.0f);
	min_height_col = glm::vec3(1.0f, 1.0f, 1.0f);

	updateHeightShaderValues();
}

#pragma endregion


#pragma region POTENTIAL SHADER STUFF
float min_potential, max_potential;
glm::vec3 max_pot_col, min_pot_col;

void updatePotentialShaderValues()
{
	glUseProgram(potential_shader.id);

	int max_col_location = glGetUniformLocation (potential_shader.id, "max_colour");
	int min_col_location = glGetUniformLocation (potential_shader.id, "min_colour");
	glUniform3fv (max_col_location, 1,  glm::value_ptr(max_pot_col));
	glUniform3fv (min_col_location, 1,  glm::value_ptr(min_pot_col));

	int max_pot_loc = glGetUniformLocation (potential_shader.id, "max_value");
	int min_pot_loc = glGetUniformLocation (potential_shader.id, "min_value");
	glUniform1f (max_pot_loc, max_potential);
	glUniform1f (min_pot_loc, min_potential);
}

void initPotentialShaderValues()
{
	max_potential = 5.0f;
	min_potential = 0.0f;

	max_pot_col = glm::vec3(0.0f, 0.0f, 1.0f);
	min_pot_col = glm::vec3(1.0f, 1.0f, 1.0f);

	updatePotentialShaderValues();
}

#pragma endregion

#pragma region DISCOMFORT SHADER STUFF

float min_discomfort, max_discomfort;
glm::vec3 max_disc_col, min_disc_col;

void updateDiscomfortShaderValues()
{
	glUseProgram(discomfort_shader.id);

	int max_col_location = glGetUniformLocation (discomfort_shader.id, "max_colour");
	int min_col_location = glGetUniformLocation (discomfort_shader.id, "min_colour");
	glUniform3fv (max_col_location, 1,  glm::value_ptr(max_disc_col));
	glUniform3fv (min_col_location, 1,  glm::value_ptr(min_disc_col));

	int max_disc_loc = glGetUniformLocation (discomfort_shader.id, "max_value");
	int min_disc_loc = glGetUniformLocation (discomfort_shader.id, "min_value");
	glUniform1f (max_disc_loc, max_discomfort);
	glUniform1f (min_disc_loc, min_discomfort);
}

void initDiscomfortShaderValues()
{
	max_discomfort = 5.0f;
	min_discomfort = 0.0f;

	max_disc_col = glm::vec3(1.0f, 1.0f, 1.0f);
	min_disc_col = glm::vec3(1.0f, 1.0f, 1.0f);

	updateDiscomfortShaderValues();
}

#pragma endregion




#pragma region TWEAK BAR STUFF



void TW_CALL reset(void *)
{
//	popManager.assignRandomLocs();

	groupA.assignRandomLeftLoc();
	groupB.assignRandomRightLoc();
}


void TW_CALL drawDensity(void *)
{
	draw_densities = true;
	draw_heights = false;
	draw_potentials = false;
	draw_discomfort = false;
}



void TW_CALL drawHeight(void *)
{
	draw_densities = false;
	draw_heights = true;
	draw_potentials = false;
	draw_discomfort = false;
}


void TW_CALL drawPotential(void *)
{
	draw_densities = false;
	draw_heights = false;
	draw_potentials = true;
	draw_discomfort = false;
}

void TW_CALL drawDiscomfort(void *)
{
	draw_densities = false;
	draw_heights = false;
	draw_potentials = false;
	draw_discomfort = true;
}



void TW_CALL pause(void *)
{
	is_paused = !is_paused;
}

void init_tweak()
{
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(width, height);

	TwBar *bar;
	bar = TwNewBar("Continuum Crowds");

	
	TwAddVarRO(bar, "FPS", TW_TYPE_FLOAT, &fps, "");
	TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &draw_scale, "");

//	TwAddVarRW(bar, "Draw Direction?", TW_TYPE_BOOLCPP, &draw_teapots, "");
	TwAddButton(bar, "Reset", reset, NULL, "");
//	TwAddVarRW(bar, "Run?", TW_TYPE_BOOLCPP, &are_moving, "");

	
	TwAddButton(bar, "Pause", pause, NULL, "");

	

	TwAddButton(bar, "Show Potentials", drawPotential, NULL, "");
	TwAddVarRW(bar, "Max Potential", TW_TYPE_FLOAT, &max_potential, "");
	TwAddVarRW(bar, "Min Potential", TW_TYPE_FLOAT, &min_potential, "");



	camera.addTBar(bar);
	TwAddButton(bar, "Show Densities", drawDensity, NULL, "");
	TwAddVarRW(bar, "Max Density", TW_TYPE_FLOAT, &max_density, "");
	TwAddVarRW(bar, "Min Density", TW_TYPE_FLOAT, &min_density, "");
//	TwAddVarRW(bar, "Density Coefficient", TW_TYPE_FLOAT, &density_coeff, "");
	TwAddButton(bar, "Show Heights", drawHeight, NULL, "");
//	TwAddVarRW(bar, "Max Height", TW_TYPE_FLOAT, &max_height, "");
//	TwAddVarRW(bar, "Min Height", TW_TYPE_FLOAT, &min_height, "");
//	TwAddVarRW(bar, "Max Density Colour", TW_TYPE_COLOR3F, &max_density_col, "");
//	TwAddVarRW(bar, "Min Denisty Colour", TW_TYPE_COLOR3F, &min_density_col, "");

	camera.addTBar(bar);
//	TwAddButton(bar, "Show Discomfort", drawDiscomfort, NULL, "");
//	TwAddVarRW(bar, "Max Discomfort", TW_TYPE_FLOAT, &max_discomfort, "");
//	TwAddVarRW(bar, "Min Discomfort", TW_TYPE_FLOAT, &min_discomfort, "");

	popManager.addTBar(bar);


}

void draw_tweak()
{
	TwDraw();
}



#pragma endregion 



void init()
{
	line_shader.CompileShaders();

	density_shader.CompileShaders();
	height_shader.CompileShaders();
	potential_shader.CompileShaders();
	discomfort_shader.CompileShaders();
	

	initDensityShaderValues();
	initHeightShaderValues();
	initPotentialShaderValues();
	initDiscomfortShaderValues();
	
	mainGrid.setupGridCells();
//	mainGrid.assignRandomHeights(max_height);
//	mainGrid.makeMiddleMountain(max_height, 0.4f);
	mainGrid.assignDiscomfortToBottom(2.0f);

	popManager.addGroup(groupA);
	popManager.addGroup(groupB);

//	popManager.addGroup(groupC);
//	popManager.addGroup(groupD);




	popManager.setupGroupGrids(mainGrid);
//	popManager.assignRandomLocs();

	

	groupA.assignRandomLeftLoc();
//	groupA.assignRandomBottomLeftLoc();
//	groupA.assignRandomTopLeftLoc();

	groupB.assignRandomRightLoc();
//	groupB.assignRandomTopRightLoc();
//	groupB.assignRandomBottomRightLoc();

	groupC.assignRandomBottomLeftLoc();
	groupD.assignRandomTopRightLoc();





	groupA.setSpeeds(0.01f, 2.0f);
	groupB.setSpeeds(0.01f, 2.0f);
	groupC.setSpeeds(0.1f, 0.2f);
	groupD.setSpeeds(0.1f, 0.2f);





	groupA.setRightSideGoal();
//	groupA.setBottomRightCornerGoal();
	
	groupB.setLeftSideGoal();
//	groupB.setTopLeftCornerGoal();


	groupC.setTopRightCornerGoal();
	groupD.setBottomLeftCornerGoal();






	densityField.m_grid = &mainGrid;
	densityField.popManager = &popManager;

	speedField.shared_grid = &mainGrid;
	speedField.popManager = &popManager;
	speedField.assignDensityField(&densityField);
//	speedField.assignTopoSpeeds();

	potentialField.shared_grid = &mainGrid;
	potentialField.popManager = &popManager;
//	potentialField.assignCosts();

	

	init_tweak();
}

void display()
{
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	proj_mat = glm::perspective(45.0f, (float)width/(float)height, 0.1f, 200.0f);
	view_mat = camera.getRotationMat();

	
	line_shader.setViewProj(view_mat, proj_mat);

	density_shader.setViewProj(view_mat, proj_mat);
	height_shader.setViewProj(view_mat, proj_mat);
	potential_shader.setViewProj(view_mat, proj_mat);
	

	glUseProgram(line_shader.id);

	int colour_location = glGetUniformLocation (line_shader.id, "colour");
	glUniform3fv (colour_location, 1,  glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));

	if(draw_densities)
		mainGrid.drawDensities(draw_scale, line_shader.id, density_shader.id);
	else if (draw_heights)
		mainGrid.drawHeights(draw_scale, line_shader.id, height_shader.id);
	else if (draw_discomfort)
		mainGrid.drawDiscomfort(draw_scale, line_shader.id, discomfort_shader.id);
	else if (draw_potentials)
		groupA.m_grid.drawPotentials(draw_scale, line_shader.id, potential_shader.id);
	



	glUseProgram(line_shader.id);
	glUniform3fv (colour_location, 1,  glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.0f)));
	popManager.draw(draw_scale, line_shader.id);


	
//	int model_mat_location = glGetUniformLocation (line_shader.id, "model");

//	glUniform3fv (colour_location, 1,  glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
//	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f))));
//	glutSolidSphere(0.1f, 5, 5);

//	glUniformMatrix4fv (model_mat_location, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(), glm::vec3(10.5f, 0.0f, 10.5f))));
//	glutSolidSphere(0.1f, 5, 5);

	draw_tweak();



	glutSwapBuffers();
}

void updateScene()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	int delta_time = time-oldTime;
	oldTime = time;

	//time since last frame in seconds
	double elapsed_seconds = (double)delta_time/1000;

	if(is_paused)
		elapsed_seconds = 0;



	fps = 1/elapsed_seconds;

	
	densityField.assignMinMax(min_density, max_density);
	densityField.update();


	speedField.update();

	potentialField.update();

	speedField.assignDirSpeeds();


	speedField.assignPeopleVels();

	popManager.move(elapsed_seconds);
	

	

	updateDensityShaderValues();
	updateHeightShaderValues();
	updatePotentialShaderValues();
	updateDiscomfortShaderValues();


	glutPostRedisplay();
}




int main(int argc, char** argv)
{
	srand(time(NULL));
	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Continuum Crowds");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);


	 glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	 glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	 glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	 glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	 glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
 
	  // send the ''glutGetModifers'' function pointer to AntTweakBar
	 TwGLUTModifiersFunc(glutGetModifiers);
 


	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}
