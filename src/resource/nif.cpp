#include <src\resource\nif.h>

bool nif::load(const char *path) {

	fp = fopen(path, "rb");
	if (!fp) {
		printf("Error opening nif file\n");
		return false;
	}

	h = new nif_header();
	fread(h, 1, NIF_HEADER_EXPORT_INFO_START, fp);

	load_export_info();
	load_block_types();

	h->block_type_indexes = (uint16_t *)malloc(h->num_blocks);
	fread(h->block_type_indexes, sizeof(uint16_t), h->num_blocks, fp);

	fread(&h->unknown_2, sizeof(uint32_t), 1, fp);

	return true;
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
// the length and string is stored sequentially, so we can read them easily
// the strings are not null-terminated so we must terminate them manually
void nif::load_block_types() {

	fread(&h->num_block_types, sizeof(uint16_t), 1, fp); // get num block types
	h->block_types = (char **)malloc(h->num_block_types);

	for (uint16_t i = 0; i < h->num_block_types; i++) {
		uint32_t str_size = 0;
		fread(&str_size, sizeof(uint32_t), 1, fp);
		h->block_types[i] = (char *)malloc(str_size + 1); // +1 for \0
		fread(h->block_types[i], sizeof(uint8_t), str_size, fp);
		h->block_types[i][str_size] = '\0';
		printf("%s\n", h->block_types[i]);
	}

}