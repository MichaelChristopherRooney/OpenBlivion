#include <src\resource\nif.h>

bool nif::load(uint8_t *data) {

	base_data = data;

	load_header();

	// TODO: figure out a good way of loading all ni_objects in the file
	blocks = (void **)malloc(sizeof(void *) * h->num_blocks);

	// TODO: implement this without using a lot of if/else
	for (uint32_t i = 0; i < h->num_blocks; i++) {

		int type_index = h->block_type_indexes[i];
		char *type = h->block_types[type_index];

		if (strcmp(type, "NiNode") == 0) {
			blocks[i] = load_ni_node();
		} else if (strcmp(type, "BSXFlags") == 0) {
			blocks[i] = load_bsx_flags();
		} else if (strcmp(type, "NiStringExtraData") == 0) {
			blocks[i] = load_ni_string_extra_data();
		} else if (strcmp(type, "bhkConvexVerticesShape") == 0) {
			blocks[i] = load_bhk_convex_vertices_shape();
		} else if (strcmp(type, "bhkRigidBody") == 0) {
			blocks[i] = load_bhk_rigid_body();
		} else if (strcmp(type, "bhkCollisionObject") == 0) {
			blocks[i] = load_bhk_collision_object();
		} else if (strcmp(type, "NiTriStrips") == 0) {
			blocks[i] = load_ni_tri_strips();
		} else if (strcmp(type, "NiBinaryExtraData") == 0) {
			blocks[i] = load_ni_binary_extra_data();
		} else if (strcmp(type, "NiMaterialProperty") == 0) {
			blocks[i] = load_ni_material_property();
		} else if (strcmp(type, "NiTexturingProperty") == 0) {
			blocks[i] = load_ni_texturing_property();
		} else if (strcmp(type, "NiSourceTexture") == 0) {
			blocks[i] = load_ni_source_texture();
		} else if (strcmp(type, "NiTriStripsData") == 0) {
			blocks[i] = load_ni_tri_strips_data();
		}
		
	}
	
	free_header();
	return true;
}

void nif::load_header() {

	h = (struct nif_header*)malloc(sizeof(struct nif_header));
	copy_data(h, NIF_HEADER_EXPORT_INFO_START, 1);
	h->header_str[NIF_HEADER_STRING_LENGTH - 1] = '\0'; // replace '\n' with '\0'

	load_export_info();
	load_block_data();

	copy_data(&h->unknown_2, sizeof(uint32_t), 1);

}

void nif::free_header() {

	free(h->creator);
	free(h->export_info_1);
	free(h->export_info_2);
	if (h->num_block_types > 0) {
		for (uint32_t i = 0; i < h->num_block_types; i++) {
			free(h->block_types[i]);
		}
		free(h->block_types);
	}
	free(h->block_type_indexes);

}

// export info can be of varying lengths
// so we must process data from the file
void nif::load_export_info() {

	copy_data(&h->creator_len, sizeof(uint8_t), 1);
	h->creator = (char *)malloc(h->creator_len);
	copy_data(h->creator, h->creator_len * sizeof(char), 1);

	copy_data(&h->export_info_1_len, sizeof(uint8_t), 1);
	h->export_info_1 = (char *)malloc(h->export_info_1_len);
	copy_data(h->export_info_1, h->export_info_1_len * sizeof(char), 1);

	copy_data(&h->export_info_2_len, sizeof(uint8_t), 1);
	h->export_info_2 = (char *)malloc(h->export_info_2_len);
	copy_data(h->export_info_2, h->export_info_2_len * sizeof(char), 1);

}

// the length of each block type string is stored before the string data
// these lengths and strings are stored sequentially, so we can read them easily
// the strings are not null-terminated so we must terminate them manually
// the block type indexes are also stored sequentially
void nif::load_block_data() {

	copy_data(&h->num_block_types, sizeof(uint16_t), 1);
	h->block_types = (char **)malloc(h->num_block_types * sizeof(uint16_t *));
	for (uint16_t i = 0; i < h->num_block_types; i++) {
		uint32_t str_size = 0;
		copy_data(&str_size, sizeof(uint32_t), 1);
		h->block_types[i] = (char *)malloc(str_size + 1); // +1 for '\0'
		copy_data(h->block_types[i], sizeof(uint8_t), str_size);
		h->block_types[i][str_size] = '\0';
	}
	h->block_type_indexes = (uint16_t *)malloc(h->num_blocks * sizeof(uint16_t));
	copy_data(h->block_type_indexes,  sizeof(uint16_t), h->num_blocks);

}

/*
	TODO:
	for now all of this loading code uses multiple copy_data calls
	these will need to be consolidated into as few calls as possible
	once the correctness of the data structures has been verified 
	(in particular once sizes have been confirmed)
*/

void * nif::load_ni_node() {

	struct ni_node *node = (struct ni_node*)malloc(sizeof(struct ni_node));

	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->num_extra_data, sizeof(uint32_t), 1);
	if (node->num_extra_data > 0) {
		node->extra_data = (int32_t *)malloc(node->num_extra_data);
		copy_data(node->extra_data, sizeof(int32_t), node->num_extra_data);
	}
	copy_data(&node->controller, sizeof(int32_t), 1);
	copy_data(&node->flags, sizeof(uint16_t), 1);
	copy_data(&node->translation, sizeof(glm::vec3), 1);
	copy_data(&node->rotation, sizeof(glm::mat3), 1);
	copy_data(&node->scale, sizeof(uint32_t), 1);
	copy_data(&node->num_properties, sizeof(uint32_t), 1);
	if (node->num_properties != 0) {
		node->properties = (int32_t *)malloc(sizeof(int32_t) * node->num_properties);
		copy_data(node->properties, sizeof(int32_t), node->num_properties);
	}
	copy_data(&node->collision_object, sizeof(int32_t), 1);
	copy_data(&node->num_children, sizeof(uint32_t), 1);
	if (node->num_children != 0) {
		node->children = (int32_t *)malloc(sizeof(int32_t) * node->num_children);
		copy_data(node->children, sizeof(int32_t), node->num_children);
	}
	copy_data(&node->num_effects, sizeof(uint32_t), 1);
	if (node->num_effects != 0) {
		node->effects = (int32_t *)malloc(sizeof(int32_t) * node->num_effects);
		copy_data(node->effects, sizeof(int32_t), node->num_effects);
	}

	return (void *)node;

}

void * nif::load_bsx_flags() {

	struct bsx_flags *node = (struct bsx_flags*)malloc(sizeof(struct bsx_flags));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->flags, sizeof(uint32_t), 1);

	return (void *)node;

}

void * nif::load_ni_string_extra_data() {

	struct ni_string_extra_data *node = (struct ni_string_extra_data*)malloc(sizeof(struct ni_string_extra_data));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->data_len, sizeof(uint32_t), 1);
	node->data = (char *)malloc(node->data_len + 1); //+1 for '\0'
	node->data[node->data_len] = '\0';
	copy_data(node->data, sizeof(uint8_t), node->data_len);

	return (void *)node;

}

void * nif::load_ni_binary_extra_data() {

	struct ni_binary_extra_data *node = (struct ni_binary_extra_data*)malloc(sizeof(struct ni_binary_extra_data));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->data_len, sizeof(uint32_t), 1);
	node->data = (uint8_t *)malloc(node->data_len);
	copy_data(node->data, sizeof(uint8_t), node->data_len);

	return (void *)node;

}

void * nif::load_bhk_convex_vertices_shape() {

	struct bhk_convex_vertices_shape *node = (struct bhk_convex_vertices_shape*)malloc(sizeof(struct bhk_convex_vertices_shape));
	copy_data(&node->material, sizeof(uint32_t), 1);
	copy_data(&node->radius, sizeof(float), 1);
	copy_data(node->unknown, sizeof(float), BCVS_UNKNOWN_FLOAT_SIZE);
	copy_data(&node->num_vertices, sizeof(uint32_t), 1);
	node->vertices = (glm::vec4 *)malloc(node->num_vertices * sizeof(glm::vec4));
	copy_data(node->vertices, sizeof(glm::vec4), node->num_vertices);
	copy_data(&node->num_normals, sizeof(uint32_t), 1);
	node->normals = (glm::vec4 *)malloc(node->num_normals * sizeof(glm::vec4));
	copy_data(node->normals, sizeof(glm::vec4), node->num_normals);

	return (void *)node;

}

void * nif::load_bhk_rigid_body() {
	
	struct bhk_rigid_body *node = (struct bhk_rigid_body*)malloc(sizeof(struct bhk_rigid_body));
	copy_data(node, sizeof(struct bhk_rigid_body) - 8, 1); // -8 so we read num constraints then stop
	if (node->num_constraints > 0) {
		node->constraints = (uint32_t *)malloc(node->num_constraints * sizeof(uint32_t));
		copy_data(node->constraints, sizeof(uint32_t), node->num_constraints);
	} else {
		node->constraints = NULL;
	}
	copy_data(&node->unknown_int_9, sizeof(uint32_t), 1);

	return (void *) node;

}

void * nif::load_bhk_collision_object() {

	struct bhk_collision_object *node = (struct bhk_collision_object*)malloc(sizeof(struct bhk_collision_object));
	copy_data(&node->target, sizeof(uint32_t), 1);
	copy_data(&node->flags, sizeof(uint16_t), 1);
	copy_data(&node->body, sizeof(uint32_t), 1);

	return (void *)node;

}

void * nif::load_ni_tri_strips() {

	struct ni_tri_strips *node = (struct ni_tri_strips*)malloc(sizeof(struct ni_tri_strips));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->num_extra_data, sizeof(uint32_t), 1);
	if (node->num_extra_data > 0) {
		node->extra_data = (uint32_t *)malloc(node->num_extra_data * sizeof(uint32_t));
		copy_data(node->extra_data, sizeof(uint32_t), node->num_extra_data);
	} else {
		node->extra_data = NULL;
	}
	copy_data(&node->controller, sizeof(uint32_t), 1);
	copy_data(&node->flags, sizeof(uint16_t), 1);
	copy_data(&node->translation, sizeof(glm::vec3), 1);
	copy_data(&node->rotation, sizeof(glm::mat3), 1);
	copy_data(&node->scale, sizeof(float), 1);
	copy_data(&node->num_properties, sizeof(uint32_t), 1);
	if (node->num_properties > 0) {
		node->properties = (uint32_t *)malloc(node->num_properties * sizeof(uint32_t));
		copy_data(node->properties, sizeof(uint32_t), node->num_properties);
	} else {
		node->properties = NULL;
	}
	copy_data(&node->collision_object, sizeof(uint32_t), 1);
	copy_data(&node->data, sizeof(uint32_t), 1);
	copy_data(&node->skin_instance, sizeof(uint32_t), 1);
	copy_data(&node->has_shader, sizeof(uint8_t), 1);

	return (void *)node;

}

void * nif::load_ni_material_property() {

	struct ni_material_property *node = (struct ni_material_property*)malloc(sizeof(struct ni_material_property));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->num_extra_data, sizeof(uint32_t), 1);
	if (node->num_extra_data > 0) {
		node->extra_data_list = (uint32_t *)malloc(node->num_extra_data * sizeof(uint32_t));
		copy_data(node->extra_data_list, sizeof(uint32_t), node->num_extra_data);
	} else {
		node->extra_data_list = NULL;
	}
	copy_data(&node->controller, sizeof(uint32_t), 1);
	copy_data(&node->ambient, sizeof(glm::vec3), 1);
	copy_data(&node->diffuse, sizeof(glm::vec3), 1);
	copy_data(&node->specular, sizeof(glm::vec3), 1);
	copy_data(&node->emissive, sizeof(glm::vec3), 1);
	copy_data(&node->glossiness, sizeof(float), 1);
	copy_data(&node->alpha, sizeof(float), 1);

	return (void *)node;

}

void * nif::load_ni_texturing_property() {

	struct ni_texturing_property *node = (struct ni_texturing_property*)malloc(sizeof(struct ni_texturing_property));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->num_extra_data, sizeof(uint32_t), 1);
	if (node->num_extra_data > 0) {
		node->extra_data_list = (uint32_t *)malloc(node->num_extra_data * sizeof(uint32_t));
		copy_data(node->extra_data_list, sizeof(uint32_t), node->num_extra_data);
	} else {
		node->extra_data_list = NULL;
	}
	copy_data(&node->controller, sizeof(uint32_t), 1);
	copy_data(&node->apply_mode, sizeof(uint32_t), 1);
	copy_data(&node->texture_count, sizeof(uint32_t), 1);
	copy_data(&node->has_base_texture, sizeof(uint8_t), 1);
	if (node->has_base_texture) {
		node->base_texture = load_texture_description();
	}
	copy_data(&node->has_dark_texture, sizeof(uint8_t), 1);
	if (node->has_dark_texture) {
		node->dark_texture = load_texture_description();
	}
	copy_data(&node->has_detail_texture, sizeof(uint8_t), 1);
	if (node->has_detail_texture) {
		node->detail_texture = load_texture_description();
	}
	copy_data(&node->has_gloss_texture, sizeof(uint8_t), 1);
	if (node->has_gloss_texture) {
		node->gloss_texture = load_texture_description();
	}
	copy_data(&node->has_glow_texture, sizeof(uint8_t), 1);
	if (node->has_glow_texture) {
		node->glow_texture = load_texture_description();
	}
	copy_data(&node->has_bump_map_texture, sizeof(uint8_t), 1);
	if (node->has_bump_map_texture) {
		node->bump_map_texture = load_texture_description();
	}
	copy_data(&node->has_decal_0_texture, sizeof(uint8_t), 1);
	if (node->has_decal_0_texture) {
		node->decal_0_texture = load_texture_description();
	}
	copy_data(&node->num_shader_textures, sizeof(uint32_t), 1);
	if (node->num_shader_textures > 0) {
		// TODO:
	}

	return (void *)node;

}

void * nif::load_ni_source_texture() {

	struct ni_source_texture *node = (struct ni_source_texture*)malloc(sizeof(struct ni_source_texture));
	copy_data(&node->name_len, sizeof(uint32_t), 1);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	copy_data(node->name, sizeof(uint8_t), node->name_len);
	copy_data(&node->num_extra_data, sizeof(uint32_t), 1);
	if (node->num_extra_data > 0) {
		node->extra_data_list = (uint32_t *)malloc(node->num_extra_data * sizeof(uint32_t));
		copy_data(node->extra_data_list, sizeof(uint32_t), node->num_extra_data);
	} else {
		node->extra_data_list = NULL;
	}
	copy_data(&node->controller, sizeof(uint32_t), 1);
	copy_data(&node->use_external, sizeof(uint8_t), 1);
	copy_data(&node->file_name_len, sizeof(uint32_t), 1);
	node->file_name = (char *)malloc(node->file_name_len + 1); //+1 for '\0'
	node->file_name[node->file_name_len] = '\0';
	copy_data(node->file_name, sizeof(uint8_t), node->file_name_len);
	copy_data(&node->unknown_link, sizeof(uint32_t), 1);
	copy_data(&node->pixel_layout, sizeof(uint32_t), 1);
	copy_data(&node->use_mipmaps, sizeof(uint32_t), 1);
	copy_data(&node->alpha_format, sizeof(uint32_t), 1);
	copy_data(&node->is_static, sizeof(uint8_t), 1);
	copy_data(&node->direct_render, sizeof(uint8_t), 1);

	return (void *)node;

}

void * nif::load_ni_tri_strips_data() {

	struct ni_tri_strips_data *node = (struct ni_tri_strips_data*)malloc(sizeof(struct ni_tri_strips_data));
	copy_data(&node->unknown_int, sizeof(int32_t), 1);
	copy_data(&node->num_vertices, sizeof(uint16_t), 1);
	copy_data(&node->keep_flags, sizeof(uint8_t), 1);
	copy_data(&node->compress_flags, sizeof(uint8_t), 1);
	copy_data(&node->has_vertices, sizeof(uint8_t), 1);
	if (node->has_vertices) {
		node->vertices = (glm::vec3 *)malloc(node->num_vertices * sizeof(glm::vec3));
		copy_data(node->vertices, sizeof(glm::vec3), node->num_vertices);
	}
	copy_data(&node->vector_flags, sizeof(uint16_t), 1);
	copy_data(&node->has_normals, sizeof(uint8_t), 1);
	if (node->has_normals) {
		node->normals = (glm::vec3 *)malloc(node->num_vertices * sizeof(glm::vec3));
		copy_data(node->normals, sizeof(glm::vec3), node->num_vertices);
	}
	copy_data(&node->center, sizeof(glm::vec3), 1);
	copy_data(&node->radius, sizeof(float), 1);
	copy_data(&node->has_vertex_colours, sizeof(uint8_t), 1);
	if (node->has_vertex_colours) { // TODO: check if the sizes here are correct
		node->vertex_colours = (glm::vec4 *)malloc(node->num_vertices * sizeof(glm::vec4));
		copy_data(node->vertex_colours, sizeof(glm::vec4), node->num_vertices);
	}
	node->uv_sets = (glm::vec2 **)malloc(1 * sizeof(glm::vec2 *)); // TODO: for now assuming there is only 1 UV set
	node->uv_sets[0] = (glm::vec2 *)malloc(node->num_vertices * sizeof(glm::vec2));
	copy_data(node->uv_sets[0], sizeof(glm::vec2), node->num_vertices);
	copy_data(&node->consistency_flags, sizeof(uint16_t), 1);
	copy_data(&node->additional_data, sizeof(uint32_t), 1);
	copy_data(&node->num_triangles, sizeof(uint16_t), 1);
	copy_data(&node->num_strips, sizeof(uint16_t), 1);
	node->strip_lengths = (uint16_t *)malloc(node->num_strips * sizeof(uint16_t *)); // TODO: for now assuming there is only 1 strip
	copy_data(&node->strip_lengths[0], sizeof(uint16_t), 1);
	copy_data(&node->has_points, sizeof(uint8_t), 1);
	if (node->has_points) { // TODO: for now assuming there is only 1 set of points
		node->points = (uint16_t **)malloc(node->num_strips * sizeof(uint16_t *));
		node->points[0] = (uint16_t *)malloc(node->strip_lengths[0] * sizeof(uint16_t *));
		copy_data(node->points[0], sizeof(uint16_t), node->strip_lengths[0]);
	}
	
	return (void *)node;

}

// a wrapper for memcpy
// loads from base_data + data_offset
// then increments data_offset by the correct amount
void nif::copy_data(void * dest, uint32_t type_size, uint32_t count) {
	uint32_t size = type_size * count;
	memcpy(dest, base_data + data_offset, size);
	data_offset += size;
}

struct texture_description * nif::load_texture_description() {

	struct texture_description *cur = (struct texture_description*)malloc(sizeof(struct texture_description));
	copy_data(&cur->source, sizeof(uint32_t), 1);
	copy_data(&cur->clamp_mode, sizeof(uint32_t), 1);
	copy_data(&cur->filter_mode, sizeof(uint32_t), 1);
	copy_data(&cur->uv_set, sizeof(uint32_t), 1);
	copy_data(&cur->has_texture_transform, sizeof(uint8_t), 1);
	if (cur->has_texture_transform) {
		copy_data(&cur->translation, sizeof(glm::vec2), 1);
		copy_data(&cur->tiling, sizeof(glm::vec2), 1);
	}

	return cur;

}