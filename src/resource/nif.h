#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#define NIF_HEADER_EXPORT_INFO_START	0x38

class nif_header {

public:

	char header_str[0x27]; // "Gamebryo File Format, Version 20.0.0.5\n"
	uint8_t ver[4];
	uint8_t endian;
	uint32_t user_ver;
	uint32_t num_blocks;
	uint32_t user_ver_2;
	uint32_t unknown_3;
	uint8_t export_info_unknown;
	uint8_t creator_len;
	char *creator;
	uint8_t export_info_1_len;
	char *export_info_1;
	uint8_t export_info_2_len;
	char *export_info_2;
	uint16_t num_block_types;
	char **block_types;
	uint16_t *block_type_indexes;
	uint32_t unknown_2;

};

class nif {

public:

	FILE *fp;

	bool load(const char *path);

private:

	nif_header *h;

	void load_export_info();
	void load_block_types();

};