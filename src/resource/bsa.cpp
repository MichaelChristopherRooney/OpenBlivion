#pragma once

// for reference documentation see: http://www.uesp.net/wiki/Tes4Mod:BSA_File_Format (8th November 2015 revision)

#include <src\resource\bsa.h>

bool bsa::load(const char *file_path, std::unordered_map<std::string, struct bsa_asset *> *asset_map) {

	fp = fopen(file_path, "rb");
	if (fp == NULL) {
		printf("Error opening file: %s\n", file_path);
		return false;
	}

	// load header first
	header = (struct bsa_header *) malloc(sizeof(struct bsa_header));
	fread(header, sizeof(struct bsa_header), 1, fp);
	num_assets = header->file_count;

	// now load an array of bsa_folder_records
	uint32_t folder_records_size = header->folder_count * sizeof(struct bsa_folder_record);
	folder_records = (struct bsa_folder_record *) malloc(folder_records_size);
	fread(folder_records, sizeof(struct bsa_folder_record), header->folder_count, fp);

	// load the bsa_file_record_blocks
	load_file_record_blocks();

	// now load all file names
	load_file_names();

	// now construct the full path for each file
	organise_assets(asset_map);

	// free memory that is no longer needed
	free_unneeded_data();

	//fclose(fp);

	return true;

}

void bsa::load_file_record_blocks() {

	uint32_t file_record_blocks_size = header->folder_count * sizeof(struct bsa_file_record_block);
	file_record_blocks = (struct bsa_file_record_block *) malloc(file_record_blocks_size);

	for (int i = 0; i < header->folder_count; i++) {

		struct bsa_folder_record *cur_folder_record = &folder_records[i];
		struct bsa_file_record_block *cur_file_record_block = &file_record_blocks[i];

		// first load the name of the folder
		// the first byte at the start of the bsa_file_record_block is the length of the name
		fseek(fp, cur_folder_record->offset - header->total_file_name_length, SEEK_SET);
		uint32_t folder_name_length = 0;
		fread(&folder_name_length, 1, 1, fp);
		cur_file_record_block->name = (char *) malloc(folder_name_length * sizeof(uint8_t));
		fread(cur_file_record_block->name, folder_name_length, 1, fp);

		// now load the array of bsa_file_records associated with this bsa_file_record_block
		cur_file_record_block->file_records = (struct bsa_file_record *) malloc(cur_folder_record->count * sizeof(struct bsa_file_record));
		fread(cur_file_record_block->file_records, sizeof(struct bsa_file_record), cur_folder_record->count, fp);

	}

}

/*
the bsa archive contains all file names in one block, delimited by '\0'
this function loads that block and extracts each file name from the block
*/
void bsa::load_file_names() {

	uint32_t total_file_name_length = header->total_file_name_length;

	file_names = (char **) malloc(header->file_count * sizeof(char *));

	char *all_file_names = (char *) malloc(total_file_name_length);
	fread(all_file_names, total_file_name_length, 1, fp);

	int file_names_index = 0;
	int size = 0;

	for (int i = 0; i < total_file_name_length; i++) {
		if (all_file_names[i] == '\0') {
			char *cur_pointer = all_file_names + (i - size);
			file_names[file_names_index] = (char *) malloc((size + 1) * sizeof(char)); // +1 for \0
			strcpy(file_names[file_names_index], cur_pointer);
			file_names_index++;
			size = 0;
		} else {
			size++;
		}
	}

	free(all_file_names);

}

/*
now we have all the information we need to work with an asset
however this information is scattered throughout memory and different file structures
here all information for a file is organised into a single asset structure
*/
void bsa::organise_assets(std::unordered_map<std::string, struct bsa_asset *> *asset_map) {

	assets = (struct bsa_asset *) malloc(header->file_count * sizeof(struct bsa_asset));

	int file_index = 0;
	for (int i = 0; i < header->folder_count; i++) {

		char *cur_folder_path = file_record_blocks[i].name;
		for (int n = 0; n < folder_records[i].count; n++) {

			struct bsa_asset *cur_asset = &assets[file_index];
			struct bsa_file_record *cur_file_record = &file_record_blocks[i].file_records[n];
			char *cur_file_name = file_names[file_index];

			char *file_path = (char *) malloc(strlen(cur_folder_path) + strlen(cur_file_name) + 2); // +2 for "\\" and '\0'

			strcpy(file_path, cur_folder_path);
			strcat(file_path, "\\");
			strcat(file_path, cur_file_name);
			
			set_asset_compressed_size(cur_asset, cur_file_record);

			fseek(fp, cur_file_record->offset, SEEK_SET);
			fread(&cur_asset->original_size, sizeof(uint32_t), 1, fp);
			cur_asset->offset = cur_file_record->offset + 4; // +4 so it directly to the raw data
			cur_asset->data = NULL; // mark the data as not loaded
			cur_asset->fp = fp;

			const std::string string_key = file_path;
			asset_map->emplace(string_key, cur_asset);

			free(file_path);

			//printf("%s, %u, %u, %u\n", cur_asset->file_path, cur_asset->compressed_size, cur_asset->original_size, cur_asset->offset);

			file_index++;

		}
	}

}

void bsa::set_asset_compressed_size(struct bsa_asset *cur_asset, struct bsa_file_record *cur_file_record) {

	// see the documentation link at the top of this file 
	// for details about how these sizes are determined

	if ((cur_file_record->size & (1 << 30)) != 0) {
		if (header->flags_1.compressed_by_default) {
			cur_asset->compressed_size = -1; // mark that the file is not compressed
		} else {
			// TODO: investigate why the -4 is needed
			cur_asset->compressed_size = cur_file_record->size - 4;
		}
	} else if (header->flags_1.compressed_by_default) {
		// TODO: investigate why the -4 is needed
		cur_asset->compressed_size = cur_file_record->size - 4;
	} else {
		cur_asset->compressed_size = -1;
	}

}

/*
now that all information for a file is in the asset structure
we can free the memory used by the other structures
*/
void bsa::free_unneeded_data() {

	for (int i = 0; i < header->folder_count; i++) {
		free(file_record_blocks[i].file_records);
		free(file_record_blocks[i].name);
	}

	free(folder_records);

	for (int i = 0; i < header->file_count; i++) {
		char *cur_file_name = file_names[i];
		free(cur_file_name);
	}

	free(header);

}