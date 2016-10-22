#include <src\resource\bsa_manager.h>
#include <src\resource\model.h>
#include <src\resource\nif.h>

/*
first load the asset data using the bsa manager
then parse the raw data as a nif file
then finally extract and format the nif data into an easier to use format
*/
void model::load_from_asset(const char * file_path) {

	uint8_t *data = bsa_manager::load_asset_data(file_path);
	nif n;
	n.load(data);

}
