#pragma once

#include <stdio.h>

#include <include\GL\glew.h>
#include <include\GL\freeglut.h>
#include <include\GLFW\glfw3.h>

class shader {

public:

	GLuint vert_shader;
	GLuint frag_shader;
	GLuint shader_program;

	bool init(char *vert_file, char *frag_file);
	void set_active();

private:

	bool load_and_compile(char *file, GLenum type);

};

