#include <src\renderer\camera.h>

void camera::init(config *cfg) {

	fov = 90.0f;

	projection = glm::perspective(45.0f, cfg->aspect_ratio, 0.1f, 100.0f);

	view = glm::lookAt(
		glm::vec3(0, 0, -45),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

}
