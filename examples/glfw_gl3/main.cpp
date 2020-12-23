#include <iostream>
#include <cassert>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#else
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <flexui/Element.hpp>
#include <flexui/Surface.hpp>
#include <flexui/Render/Painter.hpp> // TODO: move structs to another file
#include <flexui/Providers/TextureProvider.hpp>
#include <flexui/Providers/ResourceProvider.hpp>
#include <flexui/Style/StyleSheet.hpp>
#include <flexui/Style/StyleParse.hpp>
#include <flexui/Elements/Text.hpp>
#include <flexui/Elements/Button.hpp>
#include <flexui/Events/EventsController.hpp>
#include <flexui/Structure/XMLParse.hpp>

class TextureProviderImpl : public flexui::TextureProvider {
public:

	flexui::TextureID create(unsigned int width, unsigned int height) override {
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // empty texture
		return (flexui::TextureID)tex;
	}

	flexui::TextureID resize(flexui::TextureID src_tex, unsigned int oldWidth, unsigned int oldHeight, unsigned int newWidth, unsigned int newHeight) override {
		flexui::TextureID dst_tex = create(newWidth, newHeight);

		// TODO: find a better/faster way to do this?

		GLuint fboIds[2];
		glGenFramebuffers(2, fboIds);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboIds[0]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboIds[1]);

		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)src_tex, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)dst_tex, 0);

		glBlitFramebuffer(
			0, 0, oldWidth, oldHeight,
			0, 0, oldWidth, oldHeight,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST
		);

		glDeleteFramebuffers(2, fboIds);

		// destroy old texture, no longer needed
		dispose(src_tex);

		return dst_tex;
	}

	void store(flexui::TextureID texture, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char* image) override {
		GLuint tex = (GLuint)texture;

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}

	void dispose(flexui::TextureID texture) override {
		GLuint tex = (GLuint)texture;
		glDeleteTextures(1, &tex);
	}
};

class ResourceProviderImpl : public flexui::ResourceProvider {
public:

	bool loadFont(const std::string& familyName, unsigned char*& data, size_t& size) override {
		std::cout << "Loading font \"" << familyName << "\"" << std::endl;

		// TODO: change, copied from somewhere during testing
		FILE* fileptr;
		char* buffer;
		long filelen;

		fileptr = fopen(familyName.c_str(), "rb");
		if (!fileptr) return false;
		fseek(fileptr, 0, SEEK_END);
		filelen = ftell(fileptr);
		rewind(fileptr);

		buffer = (char*)malloc(filelen * sizeof(char)); // Enough memory for the file
		fread(buffer, filelen, 1, fileptr); // Read in the entire file
		fclose(fileptr); // Close the file

		data = (unsigned char*)buffer;
		size = filelen;

		return true;
	}

};

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
GLFWcursor* cursor_pointer;
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
		fprintf(stderr, "%s\n", std::string(buf.begin(), buf.end()).c_str());
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
		attribute float in_flags;

		varying vec2 var_uv;
		varying vec4 var_color;
		varying float var_flags;

		void main() {
		    gl_Position = proj * vec4(in_position.xy, 0, 1);
		    var_color = in_color;
		    var_uv = in_uv;
			var_flags = in_flags;
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 100
		precision mediump float;

		uniform sampler2D atlas;

		varying vec2 var_uv;
		varying vec4 var_color;
		varying float var_flags;

		void main()
		{
			gl_FragColor = var_color; // flags = 0 (only color)

			if(var_flags > 1.5) { // flags = 2 (full texture)
				gl_FragColor = texture2D(atlas, var_uv);
			} else if(var_flags > 0.5) { // flags = 1 (text)
				gl_FragColor = vec4(var_color.rgb, texture2D(atlas, var_uv).r * var_color.a);
			}
		}
	)";
	#else
	const GLchar* vertex_shader = R"(
		#version 130
		uniform mat4 proj;

		in vec2 in_position;
		in vec2 in_uv;
		in vec4 in_color;
		in float in_flags;

		out vec2 var_uv;
		out vec4 var_color;
		out float var_flags;

		void main() {
		    gl_Position = proj * vec4(in_position.xy, 0, 1);
		    var_color = in_color;
		    var_uv = in_uv;
			var_flags = in_flags;
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 130
		uniform sampler2D atlas;

		in vec2 var_uv;
		in vec4 var_color;
		in float var_flags;

		out vec4 out_color;

		void main()
		{
			out_color = var_color; // flags = 0 (only color)

			if(var_flags > 1.5) { // flags = 2 (full texture)
				out_color = texture(atlas, var_uv);
			} else if(var_flags > 0.5) { // flags = 1 (text)
				out_color = vec4(var_color.rgb, texture(atlas, var_uv).r * var_color.a);
			}
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
}

static int generated = 0;
void generate_random_ui(flexui::Element* parent, int depth = 0) {
	using namespace flexui;

	int num_childs = 0;
	switch (depth) {
	case 0: num_childs = 10; break;
	case 1: num_childs = 3;  break;
	case 2: num_childs = 5;  break;
	}

	for (int i = 0; i < num_childs; i++) {
		Element* child = new Element(); generated++;
		child->addClass("d" + std::to_string(depth));

		generate_random_ui(child, depth + 1);

		parent->addElement(child, 0);
	}
}

void init_ui() {
	using namespace flexui;

	// load css
	std::string css_source = R"(
		* {
			font-family: "default";
			font-size: 24px;
		}
		.test {
			background-color: orange;
			width: 50px;
			height: 50px;
		}

		.d0, .d1, .d2, .d3 {
			align-self: center;
			flex-direction: row;
			padding: 8px;
		}
		.d0, .d2, {
			flex-direction: row;
		}
		.d1, .d3, {
			flex-direction: column;
		}

		.d0 { background-color: rgba(255,0,255,0.3); }
		.d1 { background-color: rgba(255,0,255,0.5); }
		.d2 { background-color: rgba(255,0,255,0.7); }

		#root {
			background-color: transparent;
			align-items: center;
			flex-wrap: wrap;
		}
		.aaa {
			flex-wrap: wrap;
		}

		Text {
			/*width: 150px;
			height: 150px;*/
			/*min-width: 150px;
			background-color: rgba(255, 128, 128, 0.3);*/
		}
		.emoji-font {
			/*font-family: "emojis";*/
			font-size: 24px;
			background-color: rgba(255, 128, 128, 0.5);
		}
		/**:hover {
			background-color: rgba(255, 0, 255, 0.05);
		}*/

		Button {
			color: #D9D9D9;
			background-color: #656565;
			padding-left: 3px;
			padding-top: 0;
			padding-right: 3px;
			padding-bottom: 1px;
			border-color: #242424;
			border-width: 1px;
			border-radius: 3px;
			cursor: pointer;
		}
		Button:hover {
			background-color: #828282;
		}
		Button:active {
			background-color: orange;
		}

	)";

	css_source = R"(
		* {
			font-family: "default";
			font-size: 24px;
		}
		.container {
			background-color: white;
			color: black;

			height: 90px;
			padding: 25px;
			align-items: center;
		}
		#some {
			margin-right: 5px;
		}
		Button {
			background-color: #1266f1;
			color: white;

			align-items: center;
			padding: 5px;

			border-color: #0d6efd;
			border-width: 1px;
			border-radius: 4px;

			cursor: pointer;
		}
	)";

	StyleParseResult pr;
	StyleSheet* ss = ParseStyleSheet(css_source, pr);

	for (auto s : pr.warnings) std::cout << "[CSS WARN] " << s << std::endl;
	for (auto s : pr.errors) std::cout << "[CSS ERR] " << s << std::endl;

    /*std::string xml_source = u8R"(
		<Element class="aaa">
			<Button><Text>Hello world</Text></Button>
			<Button>Alone</Button>
			<Button>Outer<Text>Inner</Text></Button>
			<Text>Hello world1</Text>
			<Text>Hello world2</Text>
			<Text>Hello world3</Text>
		</Element>
	)";*/
	std::string xml_source = u8R"(
		<Element class="container">
			<Text id="some">Some text:</Text>
			<Button><Text>A button</Text></Button>
		</Element>
	)";

	XMLParseResult pr2;
    Element* loaded = ParseXML(xml_source, pr2);

	// init ui
	ui_surface = new Surface(new ResourceProviderImpl(), new TextureProviderImpl());

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		ui_surface->getEventsController()->sendMouseMove({ (float)xpos, (float)ypos });
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        ui_surface->getEventsController()->sendMouseButton(button, action == GLFW_PRESS, { (float)xpos, (float)ypos });
	});

	Element* root = ui_surface->getRoot();
	root->setID("root");
	root->addStyleSheet(ss);


	Element* div = new Element();
	div->addClass("cont");
	generate_random_ui(div, 0);

	Text* text = new Text();
	//text->setText("El veloz murciélago comía feliz cardillo y quiwi. 1234567890");
	div->addElement(text);

    Button* btn1 = new Button();
    Text* btn1_txt = new Text();
    btn1_txt->setText("Press me =)");
    btn1->addElement(btn1_txt);
    div->addElement(btn1);

	Text* text2 = new Text();
	text2->m_FontNameTEST = "TwemojiMozilla.ttf";
	text2->addClass("emoji-font");
	div->addElement(text2);

	Text* text3 = new Text();
	text3->m_FontNameTEST = "seguiemj.ttf";
	text3->addClass("emoji-font");
	div->addElement(text3);

    root->addElement(loaded);
	//root->addElement(div);

	//text->setTextToAllGlyphsTEST();
	//text2->setTextToAllGlyphsTEST();
	//text3->setTextToAllGlyphsTEST();
}

void init() {
	create_shader();

	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// glPixelStorei(GL_PACK_ALIGNMENT, 1);

	GL_CHECK(glGenVertexArrays(1, &vao));
	GL_CHECK(glGenBuffers(1, &vbo));
	GL_CHECK(glGenBuffers(1, &ibo));

	GL_CHECK(glBindVertexArray(vao));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLuint vtx_pos = (GLuint)glGetAttribLocation(shader, "in_position");
	GLuint vtx_uv = (GLuint)glGetAttribLocation(shader, "in_uv");
	GLuint vtx_col = (GLuint)glGetAttribLocation(shader, "in_color");
	GLuint vtx_flags = (GLuint)glGetAttribLocation(shader, "in_flags");
	GL_CHECK(glEnableVertexAttribArray(vtx_pos));
	GL_CHECK(glEnableVertexAttribArray(vtx_uv));
	GL_CHECK(glEnableVertexAttribArray(vtx_col));
	GL_CHECK(glEnableVertexAttribArray(vtx_flags));
	GL_CHECK(glVertexAttribPointer(vtx_pos, 2,         GL_FLOAT, GL_FALSE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, pos)));
	GL_CHECK(glVertexAttribPointer(vtx_uv,  2,         GL_FLOAT, GL_FALSE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, uv)));
	GL_CHECK(glVertexAttribPointer(vtx_col, 4, GL_UNSIGNED_BYTE,  GL_TRUE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, color)));
	GL_CHECK(glVertexAttribPointer(vtx_flags, 1,       GL_FLOAT, GL_FALSE, sizeof(flexui::UIVertex), (GLvoid*)offsetof(flexui::UIVertex, flags)));

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_SCISSOR_TEST);

	init_ui();
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

		ui_surface->setSize({ (float)width, (float)height });
		ui_surface->process();

		auto cursor = ui_surface->getCurrentCusor();
		#ifndef __EMSCRIPTEN__
        switch (cursor) {
        default:
        case StyleCursor::AUTO:
        case StyleCursor::DEFAULT:
            glfwSetCursor(window, NULL);
            break;
        case StyleCursor::POINTER:
            glfwSetCursor(window, cursor_pointer);
            break;
        }
		#else
		// can't use switch or else, makes the linker crash
		// see https://github.com/emscripten-core/emscripten/issues/11539
		if (cursor == StyleCursor::POINTER) {
			EM_ASM({ document['body']['style']['cursor'] = 'pointer'; });
        }
        if (cursor != StyleCursor::POINTER) {
			EM_ASM({ document['body']['style']['cursor'] = ''; });
        }
		#endif

		Painter* p = ui_surface->getPainter();

		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)p->getVertexCount() * (int)sizeof(UIVertex), (const GLvoid*)p->getVertexPtr(), GL_STREAM_DRAW));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)p->getIndexCount() * (int)sizeof(UIIndex), (const GLvoid*)p->getIndexPtr(), GL_STREAM_DRAW));

		GL_CHECK(glDrawElements(GL_TRIANGLES, (GLsizei)p->getIndexCount(), sizeof(UIIndex) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0));
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

    cursor_pointer = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

	#ifndef __EMSCRIPTEN__
	if (glewInit() != GLEW_OK) {
		throw new std::runtime_error("GLEW failed to initialize");
	}
	#endif

	#ifdef __EMSCRIPTEN__
	// dirty hack to apply the initial window size
	EM_ASM(window.dispatchEvent(new Event('resize')));
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
