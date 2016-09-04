#include <src\resource\nif.h>

bool nif::load(const char *path) {

	fp = fopen(path, "rb");
	if (!fp) {
		printf("Error opening nif file\n");
		return false;
	}

	load_header();

	// TODO: figure out a good way of loading all ni_objects in the file
	blocks = malloc(sizeof(void *) * h->num_blocks);
	load_root();
	
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
	free(h->header_str);
}

// export info can be of varying lengths
// so we must process data from the file
void nif::load_export_info() {

	// get creator string length and load the string
	fread(&h->creator_len, sizeof(uint8_t), 1, fp);
	h->creator = (char *)malloc(h->creator_len);
	fread(h->creator, sizeof(uint8_t), h->creator_len, fp);

	// get export info 1 length and load the string
	fread(&h->export_info_1_len, sizeof(uint8_t), 1, fp);
	h->export_info_1 = (char *)malloc(h->export_info_1_len);
	fread(h->export_info_1, sizeof(uint8_t), h->export_info_1_len, fp);

	// get export info 2 length and load the string
	fread(&h->export_info_2_len, sizeof(uint8_t), 1, fp);
	h->export_info_2 = (char *)malloc(h->export_info_2_len);
	fread(h->export_info_2, sizeof(uint8_t), h->export_info_2_len, fp);

}

// the length of each block type string is stored before the string data
// these lengths and strings are stored sequentially, so we can read them easily
// the strings are not null-terminated so we must terminate them manually
// the block type indexes are also stored sequentially
void nif::load_block_data() {

	fread(&h->num_block_types, sizeof(uint16_t), 1, fp); // get num block types
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

void nif::load_root() {

	root = (struct ni_node*)malloc(sizeof(struct ni_node));

	fread(&root->name_len, sizeof(uint32_t), 1, fp);
	root->name = (char *)malloc(root->name_len + 1); //+1 for '\0'
	root->name[root->name_len] = '\0';
	fread(root->name, sizeof(uint8_t), root->name_len, fp);

	fread(&root->num_extra_data, sizeof(uint32_t), 1, fp);
	if (root->num_extra_data > 0) {
		root->extra_data = (int32_t *)malloc(root->num_extra_data);
		fread(root->extra_data, sizeof(int32_t), root->num_extra_data, fp);
	}
	
	// TODO: merge this into a single fread call
	fread(&root->controller, sizeof(int32_t), 1, fp);
	fread(&root->flags, sizeof(uint16_t), 1, fp);
	fread(root->translation, sizeof(uint32_t), 3, fp);
	fread(root->rotation, sizeof(uint32_t), 9, fp);
	fread(&root->scale, sizeof(uint32_t), 1, fp);

	fread(&root->num_properties, sizeof(uint32_t), 1, fp);
	if (root->num_properties != 0) {
		root->properties = (int32_t *)malloc(sizeof(int32_t) * root->num_properties);
		fread(root->properties, sizeof(int32_t), root->num_properties, fp);
	}

	fread(&root->collision_object, sizeof(int32_t), 1, fp);

	fread(&root->num_children, sizeof(uint32_t), 1, fp);
	if (root->num_children != 0) {
		root->children = (int32_t *)malloc(sizeof(int32_t) * root->num_children);
		fread(root->children, sizeof(int32_t), root->num_children, fp);
	}

	fread(&root->num_effects, sizeof(uint32_t), 1, fp);
	if (root->num_effects != 0) {
		root->effects = (int32_t *)malloc(sizeof(int32_t) * root->num_effects);
		fread(root->effects, sizeof(int32_t), root->num_effects, fp);
	}

	int a = 0;

}
