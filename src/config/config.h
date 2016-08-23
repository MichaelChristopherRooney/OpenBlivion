#pragma once

#include <cstdint>

class config {

public: 

	uint32_t x_res = 800;
	uint32_t y_res = 800;
	float aspect_ratio = (float) x_res / y_res;
	uint32_t fov = 90;
	float mouse_speed = 0.25;

};
