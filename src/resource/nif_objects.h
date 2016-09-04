#pragma once

#include <cstdint>

struct ni_node {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	int32_t *extra_data;
	int32_t controller;
	uint16_t flags;
	uint16_t unknown_short;
	float translation[3];
	float rotation[9];
	float scale;
	uint32_t num_properties;
	int32_t *properties;
	int32_t collision_object;
	uint32_t num_children;
	int32_t *children;
	uint32_t num_effects;
	int32_t *effects;
};
