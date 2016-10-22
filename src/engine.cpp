#include <src\engine.h>

bool engine::init() {

	cfg = new config();

	cam = new camera();
	cam->init(cfg);

	if (!init_opengl()) {
		printf("Error in creating OpenGL instance\n");
		return false;
	}

	return true;

}

bool engine::init_opengl() {

	window = NULL;

	const GLubyte* renderer;
	const GLubyte* version;

	if (!glfwInit()) {
		return 1;
	}

	window = glfwCreateWindow(cfg->x_res, cfg->y_res, "OpenBlivion", NULL, NULL);

	if (!window) {
		return false;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	// glEnable(GL_CULL_FACE); // TODO: re-enable this

	return true;

}

bool engine::run() {

	if (!init()) {
		return false;
	}
	
	// TODO:
	// this rendering code will be heavily refactored once data is being loaded from cells
	// for now it is like this to test texture loading code

	shader s;
	if (!s.init("shaders/vert.glsl", "shaders/frag.glsl")) {
		return false;
	}
	s.set_active();

	static const GLfloat g_vertex_buffer_data[] = {0.0f, 0.0f, 0.0f, 0.0f, 10.0f, 0.0f, 10.0f,  0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, 10.0f, 0.0f, 10.0f,  0.0f, 0.0f,};
	static const GLfloat g_uv_buffer_data[] = {0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,};

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint MatrixID = glGetUniformLocation(s.shader_program, "MVP");

	glm::mat4 Model = glm::mat4(1.0f);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	GLuint texture_sampler = glGetUniformLocation(s.shader_program, "texture_sampler");
	GLuint texture_id = -1;

	bsa_manager::init();

	model test_model;
	test_model.load_from_asset("meshes\\armor\\fur\\shield.nif");

	while (!glfwWindowShouldClose(window)) {

		//texture_id = texture_manager::load_dds(file_path);
		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(s.shader_program);

		glm::mat4 MVP = cam->projection * cam->view * Model; // Remember, matrix multiplication is the other way around
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture_id);
		//glUniform1i(texture_sampler, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	return true;

}
