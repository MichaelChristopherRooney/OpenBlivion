#pragma once

#include <cstdint>

struct bsx_flags {
	uint32_t name_len; // TODO: is this always 3?
	char * name;
	uint32_t flags;
};

struct ni_string_extra_data {
	uint32_t name_len; // TODO: is this always 3?
	char *name;
	uint32_t data_len;
	char *data;
};

struct ni_binary_extra_data {
	uint32_t name_len; // TODO: is this always 3?
	char *name;
	uint32_t data_len;
	uint8_t data;
};

# define BCVS_VEC_SIZE 4
struct bhk_convex_vertices_shape {
	uint32_t material;
	float radius;
	float unknown[6];
	uint32_t num_vertices;
	float **vertices; // TODO: calloc this
	uint32_t num_normals;
	float **normals;
};

struct bhk_rigid_body {
	// TODO
};

struct bhk_collision_object {
	uint32_t target;
	uint16_t unknown_short;
	uint32_t body;
};

struct ni_tri_strips {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	uint32_t *extra_data;
	uint32_t controller;
	uint32_t flags;
	float translation[3];
	float rotation[9];
	float scale;
	uint32_t num_properties;
	uint32_t *properties;
	uint32_t collision_object;
	uint32_t data;
	uint32_t skin_instance;
	uint8_t has_shader;
};

struct ni_material_property {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	uint32_t *extra_data_list;
	uint32_t controller;
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float emissive[3];
	float glossiness;
	float alpha;
};

// TODO: confirm bool sizes
struct ni_texturing_property {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	uint32_t *extra_data_list;
	uint32_t apply_mode;
	uint32_t texture_count;
	uint8_t has_base_texture;
	uint32_t source;
	uint32_t clamp_mode;
	uint32_t filter_mode;
	uint32_t uv_set;
	uint8_t has_texture_transform;
	uint8_t has_dark_texture;
	uint32_t dark_texture;
	uint8_t has_detail_texutre;
	uint32_t detail_texture;
	uint8_t has_gloss_texture;
	uint32_t gloss_texture;
	uint8_t has_glow_texture;
	uint32_t glow_texture;
	uint8_t has_bump_map_texture;
	uint32_t bump_map_texture;
	uint8_t has_decal_0_texture;
	uint32_t decal_0_texture;
};

struct ni_source_texture {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	uint32_t *extra_data_list;
	uint32_t controller;
	uint8_t use_external;
	uint32_t file_name_len;
	char *file_name;
	uint32_t unknown_link;
	uint32_t pixel_layout;
	uint32_t use_mipmaps;
	uint32_t alpha_format;
	uint8_t is_static;
	uint8_t direct_render;
};

#define TSD_VERT_VEC_SIZE 3
struct ni_tri_strips_data {
	uint32_t unknown_int;
	uint16_t num_vertices;
	uint8_t keep_flags;
	uint8_t compress_flags;
	uint8_t has_vertices;
	float *vertices;
	uint8_t has_normals;
	float *normals;
	float center[3];
	float radius;
	uint8_t has_vertex_colours;
	float *vertex_colours;
	// TODO: uv sets
};

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
