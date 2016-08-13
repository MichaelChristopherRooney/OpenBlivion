#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <include\zlib\zlib.h>

struct bsa_asset {
	char *file_path;
	uint32_t original_size;
	uint32_t compressed_size; // -1 if the file is not compressed
	uint32_t offset;
	uint8_t *data; // NULL if the data is not loaded
};

struct bsa_file_record {
	uint64_t name_hash;
	uint32_t size;
	uint32_t offset;
};

struct bsa_file_record_block {
	char *name;
	struct bsa_file_record *file_records;
};

struct bsa_folder_record {
	uint64_t name_hash;
	uint32_t count;
	uint32_t offset;
};

struct bsa_header {
	char file_id[4];
	uint32_t version;
	uint32_t offset;
	struct archive_flags {
		uint32_t names_for_directories : 1;
		uint32_t names_for_files : 1;
		uint32_t compressed_by_default : 1;
		uint32_t unknown_1 : 1;
		uint32_t unknown_2 : 1;
		uint32_t unknown_3 : 1;
		uint32_t xbox_360 : 1;
		uint32_t unknown_4 : 1;
		uint32_t unknown : 1; // embedded names? or is that Skyrim specific?
		uint32_t unknown_6 : 1;
		uint32_t unknown_7 : 1;
		uint32_t archive_flag_remaining : 21; // TODO: ?
	};
	uint32_t folder_count;
	uint32_t file_count;
	uint32_t total_folder_name_length;
	uint32_t total_file_name_length;
	struct file_flags {
		uint32_t meshes : 1;
		uint32_t textures : 1;
		uint32_t menus : 1;
		uint32_t sounds : 1;
		uint32_t voices : 1;
		uint32_t shaders : 1;
		uint32_t trees : 1;
		uint32_t fonts : 1;
		uint32_t miscellaneous : 1;
		uint32_t file_flag_remaining : 23; // TODO: ?
	};
};

struct bsa_archive {
	struct bsa_header *header;
	struct bsa_folder_record *folder_records;
	struct bsa_file_record_block *file_record_blocks;
	struct bsa_asset *assets;
	char **file_names;
};

bool bsa_file_load(struct bsa_archive *archive, char *file_path);
void bsa_archive_load_file_record_blocks(struct bsa_archive *archive, FILE *fp);
void bsa_archive_load_file_names(struct bsa_archive *archive, FILE *fp);
void bsa_archive_organise_assets(struct bsa_archive *archive, FILE *fp);
void bsa_archive_free_unneeded_data(struct bsa_archive *archive);
void bsa_archive_load_asset(struct bsa_asset *asset, FILE *fp); // TODO: move this function to a more suitable location


