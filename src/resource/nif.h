#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <src/resource/nif_objects.h>

#define NIF_HEADER_STRING_LENGTH 0x27
#define NIF_HEADER_EXPORT_INFO_START 0x38

struct nif_header {
	char header_str[NIF_HEADER_STRING_LENGTH];
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
	void *blocks;
	ni_node *root;
	void load_header();
	void load_export_info();
	void load_block_data();
	void load_root();
	void free_header();
	void load_bsx_flags();
	void load_ni_string_extra_data();
	void load_bhk_convex_vertices_shape();
	void load_bhk_rigid_body();
	void load_bhk_collision_object();
	void load_ni_tri_strips();
};
