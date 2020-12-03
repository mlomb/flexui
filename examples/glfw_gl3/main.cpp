#include <iostream>
#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#else
#define GLFW_INCLUDE_NONE
#include <gl/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <flexui/Element.hpp>
#include <flexui/Surface.hpp>
#include <flexui/Render/Painter.hpp> // TODO: move structs to another file
#include <flexui/Style/StyleSheet.hpp>
#include <flexui/Style/StyleParse.hpp>

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		// abort();
	}
}

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

GLFWwindow* window = nullptr;
static bool main_loop_running = false;

GLuint vbo, ibo, vao;
GLuint shader;
GLuint loc_matrix;

flexui::Surface* ui_surface = nullptr;

bool check_shader(GLuint handle, const char* desc) {
	GLint status = 0, log_length = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if ((GLboolean)status == GL_FALSE)
		fprintf(stderr, "ERROR: Shader failed to compile %s!\n", desc);
	if (log_length > 1) {
		std::vector<char> buf;
		buf.resize((int)(log_length + 1));
		glGetShaderInfoLog(handle, log_length, NULL, buf.data());
		fprintf(stderr, "%s\n", buf.begin());
	}
	return (GLboolean)status == GL_TRUE;
}

void create_shader() {
	#ifdef __EMSCRIPTEN__ // GLES
	const GLchar* vertex_shader = R"(
		#version 100
		precision mediump float;

		uniform mat4 proj;

		attribute vec2 in_position;
		attribute vec2 in_uv;
		attribute vec4 in_color;

		varying vec2 var_uv;
		varying vec4 var_color;

		void main() {
		    gl_Position = proj * vec4(in_position.xy, 0, 1);
		    var_color = in_color;
		    var_uv = in_uv;
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 100
		precision mediump float;

		uniform sampler2D atlas;

		varying vec2 var_uv;
		varying vec4 var_color;

		void main()
		{
			gl_FragColor = var_color;
		}
	)";
	#else
	const GLchar* vertex_shader = R"(
		#version 130
		uniform mat4 proj;

		in vec2 in_position;
		in vec2 in_uv;
		in vec4 in_color;

		out vec2 var_uv;
		out vec4 var_color;

		void main() {
		    gl_Position = proj * vec4(in_position.xy, 0, 1);
		    var_color = in_color;
		    var_uv = in_uv;
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 130
		uniform sampler2D atlas;

		in vec2 var_uv;
		in vec4 var_color;

		out vec4 out_color;

		void main()
		{
			out_color = var_color;
		}
	)";
	#endif

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertex_shader, NULL);
	glCompileShader(vert);
	check_shader(vert, "Vertex");

	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragment_shader, NULL);
	glCompileShader(frag);
	check_shader(frag, "Fragment");

	shader = glCreateProgram();
	glAttachShader(shader, vert);
	glAttachShader(shader, frag);
	glLinkProgram(shader);

	GL_CHECK(glUseProgram(shader));
	loc_matrix = glGetUniformLocation(shader, "proj");

	using namespace flexui;
	// load css

	std::string css_source = R"(
		* {
			background-color: red;
		}
		.test {
			background-color: orange;
			width: 50px;
			height: 50px;
		}
	)";
	StyleParseResult pr;
	StyleSheet* ss = ParseStyleSheet(css_source, pr);

	for (auto s : pr.warnings) std::cout << "[CSS WARN] " << s << std::endl;
	for (auto s : pr.errors) std::cout << "[CSS ERR] " << s << std::endl;

	// init ui
	ui_surface = new Surface();
	Element* root = ui_surface->getRoot();
	root->setID("root");
	root->addStyleSheet(ss);

	Element* div = new Element();
	div->addClass("test");
	root->addElement(div);
}

void init() {
	create_shader();

	GL_CHECK(glGenVertexArrays(1, &vao));
	GL_CHECK(glGenBuffers(1, &vbo));
	GL_CHECK(glGenBuffers(1, &ibo));

	GL_CHECK(glBindVertexArray(vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLuint vtx_pos = (GLuint)glGetAttribLocation(shader, "in_position");
	GLuint vtx_uv = (GLuint)glGetAttribLocation(shader, "in_uv");
	GLuint vtx_col = (GLuint)glGetAttribLocation(shader, "in_color");
	GL_CHECK(glEnableVertexAttribArray(vtx_pos));
	GL_CHECK(glEnableVertexAttribArray(vtx_uv));
	GL_CHECK(glEnableVertexAttribArray(vtx_col));
	GL_CHECK(glVertexAttribPointer(vtx_pos, 2,         GL_FLOAT, GL_FALSE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, pos)));
	GL_CHECK(glVertexAttribPointer(vtx_uv,  2,         GL_FLOAT, GL_FALSE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, uv)));
	GL_CHECK(glVertexAttribPointer(vtx_col, 4, GL_UNSIGNED_BYTE,  GL_TRUE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, color)));

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glEnable(GL_SCISSOR_TEST);
}

void shutdown() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void main_loop() {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float L = 0.0f;
	float R = 0.0f + width;
	float T = 0.0f;
	float B = 0.0f + height;
	const float ortho_projection[4][4] = {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
	};
	GL_CHECK(glUseProgram(shader));
	GL_CHECK(glUniformMatrix4fv(loc_matrix, 1, GL_FALSE, &ortho_projection[0][0]));

	{
		using namespace flexui;
		
		ui_surface->updateTree();

		Painter* p = ui_surface->getPainter();

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)p->getVertexCount() * (int)sizeof(UIVertex), (const GLvoid*)p->getVertexPtr(), GL_STREAM_DRAW));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)p->getIndexCount() * (int)sizeof(UIIndex), (const GLvoid*)p->getIndexPtr(), GL_STREAM_DRAW));

		GL_CHECK(glDrawElements(GL_TRIANGLES, (GLsizei)p->getIndexCount() * 3, sizeof(UIIndex) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0));
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main(int, char**) {
	if (glfwInit() != GLFW_TRUE) {
		throw new std::runtime_error("GLFW failed to initialize");
	}

	window = glfwCreateWindow(1280, 720, "flexui demo", NULL, NULL);
	if (!window) {
		throw new std::runtime_error("GLFW window creation failed");
	}

	glfwMakeContextCurrent(window);

	#ifndef __EMSCRIPTEN__
	if (glewInit() != GLEW_OK) {
		throw new std::runtime_error("GLEW failed to initialize");
	}
	#endif

	#ifdef __EMSCRIPTEN__
	// dirty hack to apply the initial window size
	EM_ASM({ window.dispatchEvent(new Event('resize')); });
	#endif

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	init();

	main_loop_running = true;

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (main_loop_running) main_loop();
	#endif

	shutdown();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}