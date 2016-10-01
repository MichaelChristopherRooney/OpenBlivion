#include <src\resource\bsa_manager.h>

std::unordered_map<std::string, struct bsa_asset *> bsa_manager::assets = {};
std::unordered_map<std::string, bsa *> bsa_manager::bsas = {};

bool bsa_manager::init() {

	// TODO: eventually there will be code here to find and load all .bsa files under the current directory
	// for now just load the .bsa with texture data for testing
	if (!load_archive("C:\\Program Files (x86)\\Bethesda Softworks\\Oblivion\\Data\\Oblivion - Meshes.bsa")) {
		return false;
	}

	//if (!load_all_archives("C:\\Program Files(x86)\\Bethesda Softworks\\Oblivion\\Data\\*.bsa")) {
	//	return false;
	//}

	return true;

}
/*
// TODO: use wchar_t instead
bool bsa_manager::load_all_archives(const char *base_path) {

	WIN32_FIND_DATAA cur_entry;
	HANDLE cur_handle = FindFirstFileA(base_path, &cur_entry);

	do {
		// ignore "." and ".." directories
		if (strcmp(cur_entry.cFileName, ".") != 0 && strcmp(cur_entry.cFileName, "..") != 0) {
			// found a directory, recursively search through it
			if (cur_entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				char dir_path[2048];
				sprintf(dir_path, "%s\%s", base_path, cur_entry.cFileName);
				printf("Entering directory %s\n", dir_path);
				load_all_archives(dir_path);
			} else if (is_bsa(cur_entry.cFileName)) { // don't list backups
				char file_path[2048];
				sprintf(file_path, "%s%s", base_path, cur_entry.cFileName);
				printf("Loading archive %s\n", file_path);
				if (!load_archive(file_path)) {
					return false;
				}
			}
		}
	} while (FindNextFile(cur_handle, &cur_entry));

	return true;

}

bool bsa_manager::is_bsa(const char *str) {

	uint32_t len = strlen(str);
	const char * testString = str + len - 5;
	if (strcmp(testString, ".bsa") == 0) {
		return true;
	}
	return false;

}
*/

bool bsa_manager::load_archive(const char *file_path) {

	const std::string string_key = file_path;

	bsa *arch = new bsa;
	if (!arch->load(file_path, &assets)) {
		return false;
	}
	
	//bsas.emplace(file_path, arch);

	return true;

}

uint8_t * bsa_manager::load_asset(const std::string file_path) {

	// TODO: handle error when asset does not exist
	struct bsa_asset *asset = assets[file_path];

	if (asset->data != NULL) { // data already loaded
		return asset->data;
	}

	asset->data = (uint8_t *)malloc(asset->original_size);
	fseek(asset->fp, asset->offset, SEEK_SET);

	if (asset->compressed_size == -1) {
		fread(asset->data, asset->original_size, 1, asset->fp);
	} else {
		uint8_t *compressed_buffer = (uint8_t *)malloc(asset->compressed_size);
		fread(compressed_buffer, asset->compressed_size, 1, asset->fp);
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

	return asset->data;

}

void bsa_manager::unload_asset_data(const std::string file_path) {

	// TODO: handle error when asset does not exist
	struct bsa_asset *cur_asset = assets[file_path];
	free(cur_asset->data);
	cur_asset->data = NULL;

}
