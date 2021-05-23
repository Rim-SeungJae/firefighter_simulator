#include "cgmath.h"		// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "floor.h"
#include "wall.h"
#include "character.h"
#include "particle.h"
#include "fire.h"
#include "circle.h"
#include "irrKlang\irrKlang.h"
#pragma comment(lib, "irrKlang.lib")

struct Srand
{
	Srand()
	{
		srand((unsigned int)time(nullptr));
	}
};

//*******************************************************************
// forward declarations for freetype text
bool init_text();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, GLfloat dpi_scale = 1.0f);

//*************************************
// global constants
static const Srand sr;
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";
static const char*  character_image_path = "../bin/images/character.png";
static const char*  up_image_path = "../bin/images/up.png";
static const char*  down_image_path = "../bin/images/down.png";
static const char*	floor_image_path = "../bin/images/floor.jpg";
static const char*	wall_image_path = "../bin/images/wall.jpg";
static const char*	water_image_path = "../bin/images/water.jpg";
static const char*	npc_image_path = "../bin/images/npc.png";
static const char*	help_image_path = "../bin/images/help.jpg";
static const char*	sky_image_path = "../bin/images/sky.jpg";
static const char*	title_image_path = "../bin/images/title.jpg";
static const char*  over_image_path = "../bin/images/game_over.jpg";
static const char*  clear_image_path = "../bin/images/game_clear.jpg";

static const char* mp3_path = "../bin/sounds/theme.mp3";
static const char* mp3_path_water = "../bin/sounds/water.mp3";
static const char* mp3_path_sizzle = "../bin/sounds/sizzle.mp3";
static const char* mp3_path_saved = "../bin/sounds/saved.mp3";

struct light_t
{
	vec4	position = vec4(0.0f, 0.0f, 10.0f, 1.0f);
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(0.1f, 0.1f, 0.1f, 0.1f);
	float	shininess = 1000.0f;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2(1280, 720); // initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	vertex_array = 0;	// ID holder for vertex array object
GLuint	ring_vertex_array = 0;
GLuint	vertex_array_square = 0;
GLuint	vertex_array_cube = 0;
GLuint	vertex_array_sphere = 0;
GLuint	vertex_array_sky_sphere = 0;
GLuint	CHARACTER = 0;
GLuint	FLOOR = 0;
GLuint	WALL = 0;
GLuint  FLAME = 0;
GLuint	FIRE = 0;
GLuint	UP = 0;
GLuint	DOWN = 0;
GLuint	WATER = 0;
GLuint	NPC = 0;
GLuint	HELP = 0;
GLuint	SKY = 0;
GLuint	TITLE = 0;
GLuint	GAME_OVER = 0;
GLuint	GAME_CLEAR = 0;

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
int		difficulty;
int		n_fire = 30;
int		n_npc = 3;
float	time_limit=10;
float	t0;
float	t = 0.0f;						// current simulation parameter
float	dt = 0.0f;
float	dx = 0.0f;
float	dy = 0.0f;
float	easy_a = 1.0f;
float	normal_a = 1.0f;
float	hard_a = 1.0f;
dvec2	pos;
bool	b_time = false;
int		color_type = 0;
bool	b_rotate = true;
bool	b_help = false;
bool	b_title = true;
bool	b_difficulty = false;
bool	b_game_over = false;
bool	b_game_clear = false;
bool	b_free_cam = false;
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	floors = std::move(create_floors());
auto	walls = std::move(create_walls());
auto	characters = std::move(create_characters(walls));
auto	fires = std::move(create_fires(n_fire,walls));
auto	npcs = std::move(create_npcs(n_npc,walls,fires));
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

float	a = 0.0f;
bool b_particle = false;

//*******************************************************************
// irrKlang objects
irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundSource* mp3_src = nullptr;
irrklang::ISoundSource* mp3_src_water = nullptr;
irrklang::ISoundSource* mp3_src_sizzle = nullptr;
irrklang::ISoundSource* mp3_src_saved = nullptr;

//*************************************
// scene objects
mesh*		pMesh = nullptr;
camera		cam;
trackball	tb;
light_t		light;
material_t	material;

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_circle_vertices;	// host-side vertices
std::vector<vertex> unit_ring_vertices;
std::vector<vertex>	unit_square_vertices;	// host-side vertices
std::vector<vertex>	unit_cube_vertices;	// host-side vertices
std::vector<vertex> unit_sphere_vertices;
std::vector<vertex> unit_sky_sphere_vertices;
std::vector<particle_t> particles;
std::vector<circle_t> circles;

//*************************************
void update()
{
	dt = float(glfwGetTime()) * 0.4f - t;
	if (!b_time || !b_rotate)
	{
		dt = 0;
		b_time = true;
	}
	for (std::vector<circle_t>::iterator it = circles.begin(); it != circles.end();)
	{
		if (!(*it).alive)
		{
			it = circles.erase(it);
		}
		else
		{
			it++;
		}
	}
	for (std::vector<character_t>::iterator it = npcs.begin(); it != npcs.end();)
	{
		if ((*it).saved)
		{
			it = npcs.erase(it);
			engine->play2D(mp3_src_saved, false);
		}
		else
		{
			it++;
		}
	}
	light.position = vec4(cam.eye, 1.0f);
	// update global simulation parameter
	t = float(glfwGetTime()) * 0.4f;
	if (!b_game_clear && !b_game_over &&b_difficulty)
	{
		if (fires.size() == 0 && npcs.size() == 0) b_game_clear = true;
		if (time_limit - glfwGetTime() + t0 < 0) b_game_over = true;
	}

	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	mat4 aspect_matrix =
	{
		std::min(1 / cam.aspect,1.0f), 0, 0, 0,
		0, std::min(cam.aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	//uloc = glGetUniformLocation(program, "color_type");				if (uloc > -1) glUniform1i(uloc, color_type);
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	uloc = glGetUniformLocation(program, "aspect_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix);

	// setup light properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
}

void render()
{
	float dpi_scale = cg_get_dpi_scale();
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray(vertex_array_square);

	if (b_title)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TITLE);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		mat4 scale_matrix =
		{
			1.0f, 0, 0, 0,
			0, 1.0f, 0, 0,
			0, 0, 1.0f, 0,
			0, 0, 0, 1
		};

		glUniform1i(glGetUniformLocation(program, "b_help"), true);

		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, scale_matrix);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

		// =============================
		a = abs(sin(float(glfwGetTime()) * 2.5f));
		render_text("Press Any Key", window_size.x / 3, window_size.y / 4 * 3, 0.5f, vec4(0.5f, 0.8f, 0.2f, a), dpi_scale);
		glUseProgram(program);
	}
	else if (!b_difficulty)
	{
		float dpi_scale = cg_get_dpi_scale();
		render_text("Easy", window_size.x/2, window_size.y/4, 0.5f, vec4(0.5f, 0.8f, 0.2f, easy_a), dpi_scale);
		render_text("Normal", window_size.x / 2, window_size.y / 4*2, 0.5f, vec4(0.5f, 0.8f, 0.2f, normal_a), dpi_scale);
		render_text("Hard", window_size.x / 2, window_size.y / 4*3, 0.5f, vec4(0.5f, 0.8f, 0.2f, hard_a), dpi_scale);
	}
	else if (b_game_over)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GAME_OVER);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		mat4 scale_matrix =
		{
			1.0f, 0, 0, 0,
			0, 1.0f, 0, 0,
			0, 0, 1.0f, 0,
			0, 0, 0, 1
		};

		glUniform1i(glGetUniformLocation(program, "b_help"), true);

		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, scale_matrix);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	else if (b_game_clear)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GAME_CLEAR);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		mat4 scale_matrix =
		{
			1.0f, 0, 0, 0,
			0, 1.0f, 0, 0,
			0, 0, 1.0f, 0,
			0, 0, 0, 1
		};

		glUniform1i(glGetUniformLocation(program, "b_help"), true);

		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, scale_matrix);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		if (b_help)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, HELP);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);

			mat4 scale_matrix =
			{
				1.2f, 0, 0, 0,
				0, 0.8f, 0, 0,
				0, 0, 0.8f, 0,
				0, 0, 0, 1
			};

			glUniform1i(glGetUniformLocation(program, "b_help"), true);

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, scale_matrix);

			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
		}

		glUniform1i(glGetUniformLocation(program, "b_help"), false);

		for (auto& f : floors)
		{
			f.update();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FLOOR);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, f.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
		}

		for (auto& c : characters)
		{
			c.update(dt, walls);
			
			if (!b_free_cam)
			{
				cam.eye.x = characters[0].center.x;
				cam.eye.y = characters[0].center.y;
				cam.at = characters[0].center;
				cam.up = vec3(0, 1, 0);
				cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			}
			
			glUniform1i(glGetUniformLocation(program, "b_character"), true);

			if (c.look_at == 0 || c.look_at == 1)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, CHARACTER);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			}
			else if (c.look_at == 2)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, DOWN);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, UP);
				glUniform1i(glGetUniformLocation(program, "TEX"), 0);
			}

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
		}

		for (auto& c : npcs)
		{
			c.update_npc(dt, walls, characters, fires);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, NPC);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);


			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
		}

		for (auto& f : fires)
		{
			glBindVertexArray(vertex_array_square);
			f.update();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FIRE);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, f.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

			b_particle = true;	glBindVertexArray(vertex_array);

			for (auto& p : f.particles)
			{
				p.update(program, dt);

				mat4 translate_matrix = mat4::translate(vec3(p.pos.x, p.pos.y, 0.06f));
				mat4 scale_matrix = mat4::scale(p.scale);
				mat4 translate_matrix_2 = mat4::translate(vec3(f.center.x, f.center.y, 0));
				mat4 model_matrix = translate_matrix_2 * translate_matrix * scale_matrix;

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, FLAME);
				glUniform1i(glGetUniformLocation(program, "b_particle"), b_particle);

				GLint uloc;
				uloc = glGetUniformLocation(program, "color");			if (uloc > -1) glUniform4fv(uloc, 1, p.color);
				uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, model_matrix);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			}
			b_particle = false;
			glUniform1i(glGetUniformLocation(program, "b_particle"), b_particle);
		}
		glUniform1i(glGetUniformLocation(program, "b_character"), false);
		glBindVertexArray(vertex_array_cube);

		for (auto& w : walls)
		{
			w.update();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, WALL);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, w.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);
		}

		glBindVertexArray(vertex_array_sphere);

		for (auto& c : circles)
		{
			int after_sound = 0;
			c.update(dt, &fires, walls, &after_sound);

			if (after_sound == 1)
			{
				engine->play2D(mp3_src_water, false);
			}
			else if (after_sound == 2)
			{
				engine->play2D(mp3_src_sizzle, false);
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, WATER);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			// per-circle draw calls
			glDrawElements(GL_TRIANGLES, 35 * 72 * 6, GL_UNSIGNED_INT, nullptr);
		}

		//draw skysphere

		glBindVertexArray(vertex_array_sky_sphere);

		mat4 scale_matrix_sky =
		{
			100.0f, 0, 0, 0,
			0, 100.0f, 0, 0,
			0, 0, 100.0f, 0,
			0, 0, 0, 1
		};
		mat4 symmetry_matrix_sky =
		{
			1.0f, 0, 0, 0,
			0, -1.0f, 0, 0,
			0, 0, -1.0f, 0,
			0, 0, 0, 1
		};

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, SKY);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, scale_matrix_sky * symmetry_matrix_sky);
		// per-circle draw calls
		glDrawElements(GL_TRIANGLES, 35 * 72 * 6, GL_UNSIGNED_INT, nullptr);


		// text
		render_text("Remain Time: ", 100, 100, 0.5f, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
		render_text(std::to_string(time_limit-glfwGetTime()+t0), 300, 100, 0.5f, vec4(0.5f, 0.8f, 1.0f, 1.0f), dpi_scale);
		render_text("Remain People: ", 100, 125, 0.5f, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
		render_text(std::to_string(npcs.size()), 300, 125, 0.5f, vec4(0.5f, 0.8f, 1.0f, 1.0f), dpi_scale);
		render_text("Remain Fire: ", 100, 150, 0.5f, vec4(0.5f, 0.8f, 0.2f, 1.0f), dpi_scale);
		render_text(std::to_string(fires.size()), 300, 150, 0.5f, vec4(0.5f, 0.8f, 1.0f, 1.0f), dpi_scale);

		glUseProgram(program);
	}
	

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
	
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf("- use arrow keys to move\n");
	printf("- press space bar to throw water bomb\n");
	printf("- camera follows your character. However, press 'f' to activate/deactivate free camera mode.\n-In free camera mode, you can rotate, pan, zoom the camera using mouse left, middle, right button\n");
	printf("- you can only zoom when you are not in free camera mode.\n");
	//printf("- press 'w' to toggle wireframe\n");
	printf( "\n" );
}

std::vector<vertex> create_square_vertices()
{
	std::vector<vertex> v = {};
	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f) });
	return v;
}

void update_vertex_buffer_square(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers

	std::vector<uint> indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(4);
	indices.push_back(7);
	indices.push_back(6);

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_square) glDeleteVertexArrays(1, &vertex_array_square);
	vertex_array_square = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_square) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_cube_vertices()
{
	std::vector<vertex> v = {};
	v.push_back({ vec3(+1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f) });
	v.push_back({ vec3(+1.0f, +1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f) });
	v.push_back({ vec3(-1.0f, +1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f) });
	return v;
}

void update_vertex_buffer_cube(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers

	std::vector<uint> indices;
	for (uint k = 0; k < 6 * 4; k += 4)
	{
		indices.push_back(k);	// the origin
		indices.push_back(k + 1);
		indices.push_back(k + 2);

		indices.push_back(k);
		indices.push_back(k + 2);
		indices.push_back(k + 3);
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_cube) glDeleteVertexArrays(1, &vertex_array_cube);
	vertex_array_cube = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_square) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_sphere_vertices()
{
	std::vector<vertex> v = {};
	for (uint i = 0; i <= 36; i++)
	{
		float theta = PI * i / 36.0f, c_theta = cos(theta), s_theta = sin(theta);
		for (uint j = 0; j <= 72; j++)
		{
			float phi = PI * 2.0f * j / 72.0f, c_phi = cos(phi), s_phi = sin(phi);
			v.push_back({ vec3(s_theta * c_phi,s_theta * s_phi,c_theta), vec3(s_theta * c_phi,s_theta * s_phi,c_theta), vec2(phi / 2.0f / PI,1.0f - theta / PI) });
		}
		//float t=PI*2.0f*k/float(N), c=cos(t), s=sin(t);
	}
	return v;
}

void update_vertex_buffer_sphere(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers

	std::vector<uint> indices;
	/*
	for( uint k=0; k < N; k++ )
	{
		indices.push_back(0);	// the origin
		indices.push_back(k+1);
		indices.push_back(k+2);
	}
	*/
	for (uint i = 0; i < 36; i++)
	{
		for (uint j = 0; j < 72; j++)
		{
			if (i != 0)
			{
				indices.push_back(i * (72 + 1) + j);
				indices.push_back(i * (72 + 1) + (72 + 1) + j);
				indices.push_back(i * (72 + 1) + 1 + j);
			}
			if (i != 35)
			{
				indices.push_back(i * (72 + 1) + 1 + j);
				indices.push_back(i * (72 + 1) + (72 + 1) + j);
				indices.push_back(i * (72 + 1) + (72 + 1) + 1 + j);
			}
		}
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_sphere) glDeleteVertexArrays(1, &vertex_array_sphere);
	vertex_array_sphere = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_sphere) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_sky_sphere_vertices()
{
	std::vector<vertex> v = {};
	for (uint i = 0; i <= 36; i++)
	{
		float theta = PI * i / 36.0f, c_theta = cos(theta), s_theta = sin(theta);
		for (uint j = 0; j <= 72; j++)
		{
			float phi = PI * 2.0f * j / 72.0f, c_phi = cos(phi), s_phi = sin(phi);
			v.push_back({ vec3(s_theta * c_phi,s_theta * s_phi,c_theta), vec3(-(s_theta * c_phi),-(s_theta * s_phi),-c_theta), vec2(phi / 2.0f / PI,1.0f - theta / PI) });
		}
		//float t=PI*2.0f*k/float(N), c=cos(t), s=sin(t);
	}
	return v;
}

void update_vertex_buffer_sky_sphere(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers

	std::vector<uint> indices;
	/*
	for( uint k=0; k < N; k++ )
	{
		indices.push_back(0);	// the origin
		indices.push_back(k+1);
		indices.push_back(k+2);
	}
	*/
	for (uint i = 0; i < 36; i++)
	{
		for (uint j = 0; j < 72; j++)
		{
			if (i != 0)
			{
				indices.push_back(i * (72 + 1) + j);
				indices.push_back(i * (72 + 1) + 1 + j);
				indices.push_back(i * (72 + 1) + (72 + 1) + j);
			}
			if (i != 35)
			{
				indices.push_back(i * (72 + 1) + 1 + j);
				indices.push_back(i * (72 + 1) + (72 + 1) + 1 + j);
				indices.push_back(i * (72 + 1) + (72 + 1) + j);
			}
		}
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array_sky_sphere) glDeleteVertexArrays(1, &vertex_array_sky_sphere);
	vertex_array_sky_sphere = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array_sky_sphere) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

void reset()
{
	cam = camera();
	b_game_over = false;
	b_game_clear = false;
	t0 = (float)glfwGetTime();
	floors = std::move(create_floors());
	walls = std::move(create_walls());
	characters = std::move(create_characters(walls));
	fires = std::move(create_fires(n_fire, walls));
	npcs = std::move(create_npcs(n_npc, walls,fires));
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (b_title) b_title = false;
		else
		{
			if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
			else if (key == GLFW_KEY_HOME)					cam = camera();
			/*
			else if (key == GLFW_KEY_W)
			{
				b_wireframe = !b_wireframe;
				glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
				printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
			}
			*/
			else if (key == GLFW_KEY_RIGHT)
			{
				characters[0].look_at = 0;
				characters[0].move_right = true;
			}
			else if (key == GLFW_KEY_LEFT)
			{
				characters[0].look_at = 1;
				characters[0].move_left = true;
			}
			else if (key == GLFW_KEY_DOWN)
			{
				characters[0].look_at = 2;
				characters[0].move_down = true;
			}
			else if (key == GLFW_KEY_UP)
			{
				characters[0].look_at = 3;
				characters[0].move_up = true;
			}
			else if (key == GLFW_KEY_SPACE)
			{
				circle_t* c = new circle_t(characters[0].look_at, characters[0].center);
				circles.push_back(*c);
			}
			else if (key == GLFW_KEY_F1)
			{
				b_help = !b_help;
			}
			else if (key == GLFW_KEY_R)
			{
				b_title = true;
				b_difficulty = false;
				easy_a = 1.0f;
				normal_a = 1.0f;
				hard_a = 1.0f;
			}
			else if (key == GLFW_KEY_F)
			{
				b_free_cam = !b_free_cam;
			}
		}
	}
	else if (action == GLFW_RELEASE&&!b_title)
	{
		if (key == GLFW_KEY_RIGHT)
		{
			characters[0].move_right = false;
		}
		else if (key == GLFW_KEY_LEFT)
		{
			characters[0].move_left = false;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			characters[0].move_down = false;
		}
		else if (key == GLFW_KEY_UP)
		{
			characters[0].move_up = false;
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	
	if(button==GLFW_MOUSE_BUTTON_LEFT|| button == GLFW_MOUSE_BUTTON_RIGHT||button==GLFW_MOUSE_BUTTON_MIDDLE)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if(action==GLFW_PRESS)			tb.begin( &cam, npos);
		else if(action==GLFW_RELEASE)	tb.end();
	}
	tb.button = button;
	tb.mods = mods;
	
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (!b_title&&!b_difficulty)
		{
			bool b_selected = false;
			dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
			vec2 npos = cursor_to_ndc(pos, window_size);

			if (pos.y > window_size.y / 5 && pos.y < window_size.y / 5 * 2 && action==GLFW_PRESS)
			{
				difficulty = 0;
				n_fire = 30;
				n_npc = 1;
				b_selected = true;
				easy_a = 0.5f;
				circles.clear();
				time_limit = 240.0f;
				reset();
			}
			if (pos.y > window_size.y / 5*2 && pos.y < window_size.y / 5 * 3 && action == GLFW_PRESS)
			{
				difficulty = 1;
				n_fire = 50;
				n_npc = 3;
				b_selected = true;
				normal_a = 0.5f;
				circles.clear();
				time_limit = 210.0f;
				reset();
			}
			if (pos.y > window_size.y / 5*3 && pos.y < window_size.y / 5 * 4&& action==GLFW_PRESS)
			{
				difficulty = 2;
				n_fire = 70;
				n_npc = 5;
				b_selected = true;
				hard_a = 0.5f;
				circles.clear();
				time_limit = 180.0f;
				reset();
			}
			if (action == GLFW_RELEASE)
			{
				b_difficulty = true;
			}
		}
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	if (tb.button == GLFW_MOUSE_BUTTON_LEFT && tb.mods == 0 && b_free_cam)
		tb.update(npos);
	else if (tb.button == GLFW_MOUSE_BUTTON_RIGHT || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods & GLFW_MOD_SHIFT)))
		tb.update_zoom(npos);
	else if ((tb.button == GLFW_MOUSE_BUTTON_MIDDLE || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods & GLFW_MOD_CONTROL)))&&b_free_cam)
		tb.update_pan(npos);
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	//glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_TEXTURE_2D);								// enable texturing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// define the position of four corner vertices
	unit_square_vertices = std::move(create_square_vertices());
	unit_cube_vertices = std::move(create_cube_vertices());
	unit_sphere_vertices = std::move(create_sphere_vertices());
	unit_sky_sphere_vertices = std::move(create_sky_sphere_vertices());

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer_square(unit_square_vertices);
	update_vertex_buffer_cube(unit_cube_vertices);
	update_vertex_buffer_sphere(unit_sphere_vertices);
	update_vertex_buffer_sky_sphere(unit_sky_sphere_vertices);

	// load texture
	FLOOR = cg_create_texture(floor_image_path, true); if (!FLOOR) return false;
	CHARACTER = cg_create_texture(character_image_path, true); if (!CHARACTER) return false;
	WALL = cg_create_texture(wall_image_path, true); if (!WALL) return false;
	FIRE = cg_create_texture("../bin/images/fire.png", true); if (!FIRE) return false;
	UP = cg_create_texture(up_image_path, true); if (!UP) return false;
	DOWN = cg_create_texture(down_image_path, true); if (!DOWN) return false;
	WATER = cg_create_texture(water_image_path, true); if (!WATER) return false;
	NPC = cg_create_texture(npc_image_path, true); if (!NPC) return false;
	HELP = cg_create_texture(help_image_path, true); if (!HELP) return false;
	SKY = cg_create_texture(sky_image_path, true); if (!SKY) return false;
	TITLE = cg_create_texture(title_image_path, true); if (!TITLE) return false;
	GAME_CLEAR = cg_create_texture(clear_image_path, true); if (!GAME_CLEAR) return false;
	GAME_OVER = cg_create_texture(over_image_path, true); if (!GAME_OVER) return false;

	static vertex vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec2(0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec2(1,0)}, {vec3(-1,1,0),vec3(0,0,1),vec2(0,1)}, {vec3(1,1,0),vec3(0,0,1),vec2(1,1)} }; // strip ordering [0, 1, 3, 2]

	// generation of vertex buffer: use vertices as it is
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return false; }

	FLAME = cg_create_texture("../bin/images/flame_particle.png", false); if (!FLAME) return false;
	particles.resize(particle_t::MAX_PARTICLES);

	engine = irrklang::createIrrKlangDevice();
	if (!engine) return false;
	//add sound source from the sound file
	mp3_src = engine->addSoundSourceFromFile(mp3_path);
	mp3_src_water = engine->addSoundSourceFromFile(mp3_path_water);
	mp3_src_sizzle = engine->addSoundSourceFromFile(mp3_path_sizzle);
	mp3_src_saved = engine->addSoundSourceFromFile(mp3_path_saved);
	//set default volume
	mp3_src->setDefaultVolume(0.5f);
	mp3_src_water->setDefaultVolume(0.2f);
	mp3_src_sizzle->setDefaultVolume(0.2f);
	mp3_src_saved->setDefaultVolume(0.5f);
	//play the sound file
	engine->play2D(mp3_src, true);

	// setup freetype
	if (!init_text()) return false;

	return true;
}

void user_finalize()
{
	engine->drop();
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if (!(program = cg_create_program(vert_shader_path,frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
