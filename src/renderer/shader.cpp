#include <src\renderer\shader.h>

bool shader::init(char * vert_file, char *frag_file){

	if (!load_and_compile(vert_file, GL_VERTEX_SHADER)) {
		return false;
	}

	if (!load_and_compile(frag_file, GL_FRAGMENT_SHADER)) {
		return false;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	return true;

}

bool shader::load_and_compile(char *file, GLenum type) {

	GLuint *prog;
	if (type == GL_VERTEX_SHADER) {
		prog = &vert_shader;
	} else {
		prog = &frag_shader;
	}

	FILE *fp = fopen(file, "r");
	if (fp == NULL) {
		printf("Error opening file: %s\n", file);
		return false;
	}

	uint32_t file_size;
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *program_string = (char *) calloc(1, file_size + 1);
	fread(program_string, file_size, 1, fp);
	fclose(fp);

	*prog = glCreateShader(type);
	glShaderSource(*prog, 1, &program_string, NULL);
	glCompileShader(*prog);

	GLint status;

	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		char buffer[512];
		glGetShaderInfoLog(vert_shader, 512, NULL, buffer);
		printf("Error compiling shader from file %s\nError details: %s", file, buffer);
		return false;
	}

	// TODO: can program_string be safely freed here?

	return true;

}

void shader::set_active() {

	glUseProgram(shader_program);

}
