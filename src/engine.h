#pragma once

#include <cstdint>

#include <include\GL\glew.h>
#include <include\GL\freeglut.h>
#include <include\GLFW\glfw3.h>
#include <include\glm\common.hpp>
#include <include\glm\glm.hpp>
#include <include\glm\ext.hpp>

#include <src\resource\bsa_manager.h>
#include <src\config\config.h>
#include <src\renderer\camera.h>
#include <src\renderer\shader.h>
#include <src\resource\texture_manager.h>

class engine {

public:

	GLFWwindow *window;

	camera *cam;

	config *cfg;

	uint32_t mouse_x;
	uint32_t mouse_y;

	bool init();
	bool init_opengl();
	bool run();


};
