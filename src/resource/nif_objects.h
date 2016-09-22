#pragma once

#include <cstdint>

#include <include\glm\glm.hpp>

struct texture_description {
	// TODO: confirm that all types are here
	uint32_t source;
	uint32_t clamp_mode;
	uint32_t filter_mode;
	uint32_t uv_set;
	uint8_t has_texture_transform;
	glm::vec2 translation; 
	glm::vec2 tiling;
};

struct shader_texture_description {
	// TODO:
};

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
	uint8_t *data;
};

#define BCVS_UNKNOWN_FLOAT_SIZE 6
struct bhk_convex_vertices_shape {
	uint32_t material;
	float radius;
	float unknown[BCVS_UNKNOWN_FLOAT_SIZE];
	uint32_t num_vertices;
	glm::vec4 *vertices;
	uint32_t num_normals;
	glm::vec4 *normals;
};

struct bhk_rigid_body {
	uint32_t shape;
	uint8_t layer;
	uint8_t flags_part_number;
	uint16_t unknown_short;
	int32_t unknown_int_1;
	int32_t unknown_int_2;
	int32_t unknown_3_ints[3];
	uint8_t collision_response;
	uint8_t unknown_byte;
	uint16_t process_contact_callback;
	uint16_t unknown_2_shorts[2];
	uint8_t layer_copy;
	uint8_t flags_part_number_copy;
	uint16_t unknown_short_copy;
	uint16_t unknown_6_shorts[6];
	glm::vec4 translation;
	glm::vec4 rotation;
	glm::vec4 linear_velocity;
	glm::vec4 angular_velocity;
	glm::mat3x4 inertia;
	glm::vec4 center;
	float mass;
	float linear_damping;
	float angular_damping;
	float friction;
	float restitution;
	float max_linear_velocity;
	float max_angular_velocity;
	float penetration_depth;
	uint8_t motion_system;
	uint8_t deactivator_type;
	uint8_t solver_deactivation;
	uint8_t motion_quality;
	uint32_t unknown_int_6;
	uint32_t unknown_int_7;
	uint32_t unknown_int_8;
	uint32_t num_constraints;
	uint32_t *constraints;
	uint32_t unknown_int_9;
};

struct bhk_collision_object {
	uint32_t target;
	uint16_t flags;
	uint32_t body;
};

struct ni_tri_strips {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	uint32_t *extra_data;
	uint32_t controller;
	uint16_t flags;
	glm::vec3 translation;
	glm::mat3 rotation;
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
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 emissive;
	float glossiness;
	float alpha;
};

struct ni_texturing_property {
	uint32_t name_len;
	char *name;
	uint32_t num_extra_data;
	uint32_t *extra_data_list;
	uint32_t controller;
	uint32_t apply_mode;
	uint32_t texture_count;
	uint8_t has_base_texture;
	struct texture_description *base_texture;
	uint8_t has_dark_texture;
	struct texture_description *dark_texture;
	uint8_t has_detail_texture;
	struct texture_description *detail_texture;
	uint8_t has_gloss_texture;
	struct texture_description *gloss_texture;
	uint8_t has_glow_texture;
	struct texture_description *glow_texture;
	uint8_t has_bump_map_texture;
	struct texture_description *bump_map_texture;
	uint8_t has_decal_0_texture;
	struct texture_description *decal_0_texture;
	uint32_t num_shader_textures;
	struct shader_texture_description shader_textures;
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

struct ni_tri_strips_data {
	int32_t unknown_int;
	uint16_t num_vertices;
	uint8_t keep_flags;
	uint8_t compress_flags;
	uint8_t has_vertices;
	glm::vec3 *vertices;
	uint16_t vector_flags;
	uint8_t has_normals;
	glm::vec3 *normals;
	glm::vec3 center;
	float radius;
	uint8_t has_vertex_colours;
	glm::vec4 *vertex_colours;
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
	glm::vec3 translation;
	glm::mat3 rotation;
	float scale;
	uint32_t num_properties;
	int32_t *properties;
	int32_t collision_object;
	uint32_t num_children;
	int32_t *children;
	uint32_t num_effects;
	int32_t *effects;
};
