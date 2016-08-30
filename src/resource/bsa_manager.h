#pragma once

#include <cstdint>
#include <unordered_map>
#include <Windows.h>

#include <src\resource\bsa.h>

class bsa_manager {

public:

	static bool init();
	static uint8_t * load_asset(const std::string file_path);
	static void unload_asset_data(const std::string file_path);

private:

	
	static std::unordered_map<std::string, struct bsa_asset *> assets;
	static std::unordered_map<std::string, bsa *> bsas;

	static bool bsa_manager::load_all_archives(const char *path);
	static bool load_archive(const char *base_path);
	static bool is_bsa(const char *str);

};