#pragma once

#include <cstdint>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unordered_map>

#include <include\zlib\zlib.h>

// for reference documentation see: http://www.uesp.net/wiki/Tes4Mod:BSA_File_Format (8th November 2015 revision)

struct bsa_asset {
	uint32_t original_size;
	uint32_t compressed_size; // -1 if the file is not compressed
	uint32_t offset;
	uint8_t *data; // NULL if the data is not loaded
	FILE *fp; // not thread safe but only one thread will be loading assets
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
	} flags_1;
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
	} flags_2;
};

class bsa_archive {

public:

	struct bsa_asset *assets;
	uint32_t num_assets;
	FILE *fp;

	bool load(const char *file_path, std::unordered_map<std::string, struct bsa_asset *> *asset_map);

private:

	void load_file_record_blocks();
	void load_file_names();
	void organise_assets(std::unordered_map<std::string, struct bsa_asset *> *asset_map);
	void free_unneeded_data();
	
	struct bsa_header *header;
	struct bsa_folder_record *folder_records;
	struct bsa_file_record_block *file_record_blocks;
	char **file_names;

};





