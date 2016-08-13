#pragma once

#include <src\bsa\bsa_archive.h>

int main(void) {

	struct bsa_archive *archive = malloc(sizeof(struct bsa_archive));
	bsa_file_load(archive, "C:\\Program Files (x86)\\Bethesda Softworks\\Oblivion\\Data\\Oblivion - Meshes.bsa");

}