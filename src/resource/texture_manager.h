#include <include\GL\glew.h>

#include <unordered_map>

#include <src\resource\bsa_manager.h>

#define FOURCC_DXT1 0x31545844 // DXT1
#define FOURCC_DXT3 0x33545844 // DXT3
#define FOURCC_DXT5 0x35545844 // DXT5

class texture_manager {

public:

	static std::unordered_map<std::string, GLuint> textures;

	static GLuint load_dds(const char * file_path);
	static void unload_dds(const char * file_path);

};
