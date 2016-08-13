// for reference documentation see: http://www.uesp.net/wiki/Tes4Mod:BSA_File_Format (8th November 2015 revision)

#pragma once

#include <src\bsa\bsa_archive.h>

bool bsa_file_load(struct bsa_archive *archive, char *file_path) {

	FILE *fp = fopen(file_path, "rb");
	if (fp == NULL) {
		printf("Error opening file: %s\n", file_path);
		return false;
	}

	// load header first
	archive->header = malloc(sizeof(struct bsa_header));
	fread(archive->header, sizeof(struct bsa_header), 1, fp);

	// now load an array of bsa_folder_records
	archive->folder_records = malloc(archive->header->folder_count * sizeof(struct bsa_folder_record));
	fread(archive->folder_records, sizeof(struct bsa_folder_record), archive->header->folder_count, fp);

	// load the bsa_file_record_blocks
	bsa_archive_load_file_record_blocks(archive, fp);

	// now load all file names
	bsa_archive_load_file_names(archive, fp);

	// now construct the full path for each file
	bsa_archive_organise_assets(archive, fp);

	// free memory that is no longer needed
	bsa_archive_free_unneeded_data(archive);

	fclose(fp);

	return true;

}

void bsa_archive_load_file_record_blocks(struct bsa_archive *archive, FILE *fp) {

	archive->file_record_blocks = malloc(archive->header->folder_count * sizeof(struct bsa_file_record_block));

	for (int i = 0; i < archive->header->folder_count; i++) {

		struct bsa_folder_record *cur_folder_record = &archive->folder_records[i];
		struct bsa_file_record_block *cur_file_record_block = &archive->file_record_blocks[i];

		// first load the name of the folder
		// the first byte at the start of the bsa_file_record_block is the length of the name
		fseek(fp, cur_folder_record->offset - archive->header->total_file_name_length, SEEK_SET);
		uint32_t folder_name_length = 0;
		fread(&folder_name_length, 1, 1, fp);
		cur_file_record_block->name = malloc(folder_name_length * sizeof(char));
		fread(cur_file_record_block->name, folder_name_length, 1, fp);

		// now load the array of bsa_file_records associated with this bsa_file_record_block
		cur_file_record_block->file_records = malloc(cur_folder_record->count * sizeof(struct bsa_file_record));
		fread(cur_file_record_block->file_records, sizeof(struct bsa_file_record), cur_folder_record->count, fp);

	}

}

/*
the bsa archive contains all file names in one block, delimited by '\0'
this function loads that block and extracts each file name from the block
*/
void bsa_archive_load_file_names(struct bsa_archive *archive, FILE *fp) {

	uint32_t total_file_name_length = archive->header->total_file_name_length;

	archive->file_names = malloc(archive->header->file_count * sizeof(char *));
	char **file_names = archive->file_names;

	char *all_file_names = malloc(total_file_name_length);
	fread(all_file_names, total_file_name_length, 1, fp);

	int file_names_index = 0;
	int size = 0;

	for (int i = 0; i < total_file_name_length; i++) {
		if (all_file_names[i] == '\0') {
			char *cur_pointer = all_file_names + (i - size);
			file_names[file_names_index] = malloc((size + 1) * sizeof(char)); // +1 for \0
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
now we have all the information we need to work with a file
however this information is scattered throughout memory and different file structures
here all information for a file is organised into a single structure
*/
void bsa_archive_organise_assets(struct bsa_archive *archive, FILE *fp) {

	archive->assets = malloc(archive->header->file_count * sizeof(struct bsa_asset));

	int file_index = 0;
	for (int i = 0; i < archive->header->folder_count; i++) {

		char *cur_folder_path = archive->file_record_blocks[i].name;
		for (int n = 0; n < archive->folder_records[i].count; n++) {

			struct bsa_asset *cur_asset = &archive->assets[file_index];
			struct bsa_file_record *cur_file_record = &archive->file_record_blocks[i].file_records[n];
			char *cur_file_name = archive->file_names[file_index];

			cur_asset->file_path = malloc(strlen(cur_folder_path) + strlen(cur_file_name) + 2); // +2 for "\\" and '\0'
			strcpy(cur_asset->file_path, cur_folder_path);
			strcat(cur_asset->file_path, "\\");
			strcat(cur_asset->file_path, cur_file_name);

			// this code block determines if the file is compressed or not
			// see the documentation link at the top of this file for details
			if ((cur_file_record->size & (1 << 30)) != 0) {
				if (archive->header->compressed_by_default) {
					cur_asset->compressed_size = -1; // mark that the file is not compressed
				} else {
					// TODO: investigate why the -4 is needed
					cur_asset->compressed_size = cur_file_record->size - 4;
				}
			} else if (archive->header->compressed_by_default) {
				// TODO: investigate why the -4 is needed
				cur_asset->compressed_size = cur_file_record->size - 4;
			} else {
				cur_asset->compressed_size = -1;
			}

			cur_asset->offset = cur_file_record->offset;
			fseek(fp, cur_asset->offset, SEEK_SET);
			fread(&cur_asset->original_size, sizeof(uint32_t), 1, fp);
			cur_asset->data = NULL; // mark the data as not loaded

			//printf("%s, %u, %u, %u\n", cur_asset->file_path, cur_asset->compressed_size, cur_asset->original_size, cur_asset->offset);

			file_index++;

		}
	}

}

/*
now that all information for a file is in the asset structure
we can free the memory used by the other structures
*/
void bsa_archive_free_unneeded_data(struct bsa_archive *archive) {

	for (int i = 0; i < archive->header->folder_count; i++) {
		free(archive->file_record_blocks[i].file_records);
		free(archive->file_record_blocks[i].name);
	}

	free(archive->folder_records);

	for (int i = 0; i < archive->header->file_count; i++) {
		char *cur_file_name = archive->file_names[i];
		free(cur_file_name);
	}

	free(archive->header);

}

void bsa_archive_load_asset(struct bsa_asset *asset, FILE *fp) {

	asset->data = malloc(asset->original_size);
	
	if (asset->compressed_size == -1) {
		fread(asset->data, asset->original_size, 1, fp);
	} else {
		uint8_t *compressed_buffer = malloc(asset->compressed_size);
		fread(compressed_buffer, asset->compressed_size, 1, fp);
		z_stream infstream;
		infstream.zalloc = Z_NULL;
		infstream.zfree = Z_NULL;
		infstream.opaque = Z_NULL;
		infstream.avail_in = (uInt)asset->compressed_size;
		infstream.next_in = (Bytef *)compressed_buffer;
		infstream.avail_out = (uInt)asset->original_size;
		infstream.next_out = (Bytef *)asset->data;
		inflateInit(&infstream);
		inflate(&infstream, Z_NO_FLUSH);
		inflateEnd(&infstream);
		free(compressed_buffer);
	}
	
	// uncomment to write the data to disk
	/*
	FILE *out = fopen(asset->file_path, "wb");
	fwrite(asset->data, asset->original_size, 1, out);
	fflush(out);
	fclose(out);
	*/

}