#pragma once

#include <include\glm\common.hpp>
#include <include\glm\glm.hpp>
#include <include\glm\ext.hpp>

#include <src\config\config.h>

class camera {

public:

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 projection;
	glm::mat4 view;

	void init(config *cfg);

	float fov;

};