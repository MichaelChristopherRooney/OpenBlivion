#include <src\resource\texture_manager.h>

std::unordered_map<std::string, GLuint> texture_manager::textures = {};

GLuint texture_manager::load_dds(const std::string file_path) {

	// check if texture is already loaded in VRAM
	if (textures.find(file_path) != textures.end()) {
		return textures[file_path]; 
	}

	uint8_t * data = bsa_manager::load_asset(file_path);

	unsigned int height = *(unsigned int*)&(data[12]);
	unsigned int width = *(unsigned int*)&(data[16]);
	unsigned int linearSize = *(unsigned int*)&(data[20]);
	unsigned int mipMapCount = *(unsigned int*)&(data[28]);
	unsigned int fourCC = *(unsigned int*)&(data[84]);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	}

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// TODO: change this for textures with mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// TODO: mipmap loading code

	unsigned int block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, size, &data[128]); // raw data starts at location 128

	textures.emplace(file_path, texture_id);

	// now that the texture is in VRAM we can unload the raw data
	bsa_manager::unload_asset_data(file_path);

	return texture_id;

}

void texture_manager::unload_dds(const std::string file_path) {

	// check if texture is already loaded in VRAM
	if (textures.find(file_path) == textures.end()) {
		return;
	}

	GLuint texture_id = textures[file_path];
	textures.erase(file_path);
	glDeleteTextures(1, &texture_id); // TODO: is there more that needs to be done here to ensure the texture is unloaded from VRAM?

}
