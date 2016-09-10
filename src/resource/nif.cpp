#include <src\resource\nif.h>

bool nif::load(const char *path) {

	fp = fopen(path, "rb");
	if (!fp) {
		printf("Error opening nif file\n");
		return false;
	}

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
		}
		
	}
	
	free_header();
	return true;
}

void nif::load_header() {

	h = (struct nif_header*)malloc(sizeof(struct nif_header));
	fread(h, 1, NIF_HEADER_EXPORT_INFO_START, fp);
	h->header_str[NIF_HEADER_STRING_LENGTH - 1] = '\0'; // replace '\n' with '\0'

	load_export_info();
	load_block_data();

	fread(&h->unknown_2, sizeof(uint32_t), 1, fp);

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

	fread(&h->creator_len, sizeof(uint8_t), 1, fp);
	h->creator = (char *)malloc(h->creator_len);
	fread(h->creator, sizeof(uint8_t), h->creator_len, fp);

	fread(&h->export_info_1_len, sizeof(uint8_t), 1, fp);
	h->export_info_1 = (char *)malloc(h->export_info_1_len);
	fread(h->export_info_1, sizeof(uint8_t), h->export_info_1_len, fp);

	fread(&h->export_info_2_len, sizeof(uint8_t), 1, fp);
	h->export_info_2 = (char *)malloc(h->export_info_2_len);
	fread(h->export_info_2, sizeof(uint8_t), h->export_info_2_len, fp);

}

// the length of each block type string is stored before the string data
// these lengths and strings are stored sequentially, so we can read them easily
// the strings are not null-terminated so we must terminate them manually
// the block type indexes are also stored sequentially
void nif::load_block_data() {

	fread(&h->num_block_types, sizeof(uint16_t), 1, fp);
	h->block_types = (char **)malloc(h->num_block_types * sizeof(uint16_t *));
	for (uint16_t i = 0; i < h->num_block_types; i++) {
		uint32_t str_size = 0;
		fread(&str_size, sizeof(uint32_t), 1, fp);
		h->block_types[i] = (char *)malloc(str_size + 1); // +1 for '\0'
		fread(h->block_types[i], sizeof(uint8_t), str_size, fp);
		h->block_types[i][str_size] = '\0';
		printf("%s\n", h->block_types[i]);
	}
	h->block_type_indexes = (uint16_t *)malloc(h->num_blocks * sizeof(uint16_t));
	fread(h->block_type_indexes, sizeof(uint16_t), h->num_blocks, fp);

}

/*
	TODO:
	for now all of this loading code uses multiple fread calls
	these will need to be consolidated into as few calls as possible
	once the correctness of the data structures has been verified 
	(in particular once sizes have been confirmed)
	then they will need to be converted to memcpy calls 
	once data is coming from the bsa manager
*/

void * nif::load_ni_node() {

	struct ni_node *node = (struct ni_node*)malloc(sizeof(struct ni_node));

	fread(&node->name_len, sizeof(uint32_t), 1, fp);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	fread(node->name, sizeof(uint8_t), node->name_len, fp);

	fread(&node->num_extra_data, sizeof(uint32_t), 1, fp);
	if (node->num_extra_data > 0) {
		node->extra_data = (int32_t *)malloc(node->num_extra_data);
		fread(node->extra_data, sizeof(int32_t), node->num_extra_data, fp);
	}

	// TODO: merge this into a single fread call
	fread(&node->controller, sizeof(int32_t), 1, fp);
	fread(&node->flags, sizeof(uint16_t), 1, fp);
	fread(&node->translation, sizeof(glm::vec3), 1, fp);
	fread(&node->rotation, sizeof(glm::mat3), 1, fp);
	fread(&node->scale, sizeof(uint32_t), 1, fp);

	fread(&node->num_properties, sizeof(uint32_t), 1, fp);
	if (node->num_properties != 0) {
		node->properties = (int32_t *)malloc(sizeof(int32_t) * node->num_properties);
		fread(node->properties, sizeof(int32_t), node->num_properties, fp);
	}

	fread(&node->collision_object, sizeof(int32_t), 1, fp);

	fread(&node->num_children, sizeof(uint32_t), 1, fp);
	if (node->num_children != 0) {
		node->children = (int32_t *)malloc(sizeof(int32_t) * node->num_children);
		fread(node->children, sizeof(int32_t), node->num_children, fp);
	}

	fread(&node->num_effects, sizeof(uint32_t), 1, fp);
	if (node->num_effects != 0) {
		node->effects = (int32_t *)malloc(sizeof(int32_t) * node->num_effects);
		fread(node->effects, sizeof(int32_t), node->num_effects, fp);
	}

	return (void *)node;

}

void * nif::load_bsx_flags() {

	struct bsx_flags *node = (struct bsx_flags*)malloc(sizeof(struct bsx_flags));
	fread(&node->name_len, sizeof(uint32_t), 1, fp);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	fread(node->name, sizeof(uint8_t), node->name_len, fp);
	fread(&node->flags, sizeof(uint32_t), 1, fp);

	return (void *)node;

}

void * nif::load_ni_string_extra_data() {

	struct ni_string_extra_data *node = (struct ni_string_extra_data*)malloc(sizeof(struct ni_string_extra_data));
	fread(&node->name_len, sizeof(uint32_t), 1, fp);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	fread(node->name, sizeof(uint8_t), node->name_len, fp);
	fread(&node->data_len, sizeof(uint32_t), 1, fp);
	node->data = (char *)malloc(node->data_len + 1); //+1 for '\0'
	node->data[node->data_len] = '\0';
	fread(node->data, sizeof(uint8_t), node->data_len, fp);

	return (void *)node;

}

void * nif::load_bhk_convex_vertices_shape() {

	struct bhk_convex_vertices_shape *node = (struct bhk_convex_vertices_shape*)malloc(sizeof(struct bhk_convex_vertices_shape));
	fread(&node->material, sizeof(uint32_t), 1, fp);
	fread(&node->radius, sizeof(float), 1, fp);
	fread(node->unknown, sizeof(float), BCVS_UNKNOWN_FLOAT_SIZE, fp);
	fread(&node->num_vertices, sizeof(uint32_t), 1, fp);
	node->vertices = (glm::vec4 *)malloc(node->num_vertices * sizeof(glm::vec4));
	fread(node->vertices, sizeof(glm::vec4), node->num_vertices, fp);
	fread(&node->num_normals, sizeof(uint32_t), 1, fp);
	node->normals = (glm::vec4 *)malloc(node->num_normals * sizeof(glm::vec4));
	fread(node->normals, sizeof(glm::vec4), node->num_normals, fp);

	return (void *)node;

}

void * nif::load_bhk_rigid_body() {
	// TODO

	return (void *)0;
}

void * nif::load_bhk_collision_object() {

	struct bhk_collision_object *node = (struct bhk_collision_object*)malloc(sizeof(struct bhk_collision_object));
	fread(&node->target, sizeof(uint32_t), 1, fp);
	fread(&node->unknown_short, sizeof(uint16_t), 1, fp);
	fread(&node->body, sizeof(uint32_t), 1, fp);

	return (void *)node;

}

void * nif::load_ni_tri_strips() {

	struct ni_tri_strips *node = (struct ni_tri_strips*)malloc(sizeof(struct ni_tri_strips));
	fread(&node->name_len, sizeof(uint32_t), 1, fp);
	node->name = (char *)malloc(node->name_len + 1); //+1 for '\0'
	node->name[node->name_len] = '\0';
	fread(node->name, sizeof(uint8_t), node->name_len, fp);
	fread(&node->num_extra_data, sizeof(uint32_t), 1, fp);
	if (node->num_extra_data > 0) {
		node->extra_data = (uint32_t *)malloc(node->num_extra_data * sizeof(uint32_t));
		fread(node->extra_data, sizeof(uint32_t), node->num_extra_data, fp);
	} else {
		node->extra_data = NULL;
	}
	fread(&node->controller, sizeof(uint32_t), 1, fp);
	fread(&node->flags, sizeof(uint32_t), 1, fp);
	fread(&node->translation, sizeof(glm::vec3), 1, fp);
	fread(&node->rotation, sizeof(glm::mat3), 1, fp);
	fread(&node->scale, sizeof(float), 1, fp);
	fread(&node->num_properties, sizeof(uint32_t), 1, fp);
	if (node->num_properties > 0) {
		node->properties = (uint32_t *)malloc(node->num_properties * sizeof(uint32_t));
		fread(node->properties, sizeof(uint32_t), node->num_properties, fp);
	} else {
		node->properties = NULL;
	}
	fread(&node->collision_object, sizeof(uint32_t), 1, fp);
	fread(&node->data, sizeof(uint32_t), 1, fp);
	fread(&node->skin_instance, sizeof(uint32_t), 1, fp);
	fread(&node->has_shader, sizeof(uint8_t), 1, fp);

	return (void *)node;

}