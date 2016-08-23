#pragma once

#include <cstdint>
#include <unordered_map>

#include <src\bsa\bsa_archive.h>

class bsa_manager {

public:

	static bool init();
	static uint8_t * load_asset(const std::string file_path);
	static void unload_asset_data(const std::string file_path);

private:

	static std::unordered_map<std::string, struct bsa_asset *> assets;

	static bool load_archive(const char *file_path);

};