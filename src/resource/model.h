#pragma once

#include <include\glm\glm.hpp>

class model {

public:
	void load_from_asset(const char * file_path);

private:
	glm::vec3 *vertices;
	glm::vec3 *normals;
	glm::vec3 *uvs;

};
