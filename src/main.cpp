#include "cgmath.h"		// slee's simple math library
#include "trackball.h"	// virtual trackball
#include "circle.h"
#include "floor.h"
#include "character.h"
#include "wall.h"
#define STB_IMAGE_IMPLEMENTATION
#include "cgut.h"		// slee's OpenGL utility

//*************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";
static const char*  character_image_path = "../bin/images/character.jpg";
static const char*	floor_image_path = "../bin/images/floor.jpg";
static const char* wall_image_path = "../bin/images/wall.jpg";

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
GLuint	CHARACTER = 0;
GLuint	FLOOR = 0;
GLuint	WALL = 0;

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
float	t = 0.0f;						// current simulation parameter
float	dt = 0.0f;
float	dx = 0.0f;
float	dy = 0.0f;
dvec2	pos;
bool	b_time = false;
int		color_type = 0;
bool	b_rotate = true;
#ifndef GL_ES_VERSION_2_0
bool	b_wireframe = false;
#endif
auto	circles = std::move(create_circles());
auto	floors = std::move(create_floors());
auto	characters = std::move(create_characters());
auto	walls = std::move(create_walls());
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

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

//*************************************
void update()
{
	dt = float(glfwGetTime()) * 0.4f - t;
	if (!b_time || !b_rotate)
	{
		dt = 0;
		b_time = true;
	}
	// update global simulation parameter
	t = float(glfwGetTime()) * 0.4f;

	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	//uloc = glGetUniformLocation(program, "color_type");				if (uloc > -1) glUniform1i(uloc, color_type);
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	
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
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );
	
	// bind vertex array object
	glBindVertexArray(vertex_array);

	/*
	// render sphere
	for (auto& c : circles)
	{
		// per-circle update
		c.update(dt);

		if (c.b_sun)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SUN);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
		}
		else if (c.b_dwarf)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MOON);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);
		}
		else if (c.b_ring)
		{
			glBindVertexArray(0);
			glBindVertexArray(ring_vertex_array);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, RING);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, RING_a);
			glUniform1i(glGetUniformLocation(program, "alpha"), 0);

			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
			glUniform1i(glGetUniformLocation(program, "b_sun"), c.b_sun);
			glUniform1i(glGetUniformLocation(program, "b_ring"), c.b_ring);
			glDrawElements(GL_TRIANGLES, 72 * 3 * 2 * 2 , GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(0);
			glBindVertexArray(vertex_array);
			continue;
		}
		else {
			switch (c.planet)
			{
				case 1:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p1);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 2:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p2);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 3:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p3);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 4:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p4);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 5:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p5);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 6:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p6);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 7:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p7);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
				case 8:
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, p8);
					glUniform1i(glGetUniformLocation(program, "TEX"), 0);
					break;
			}
		}

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		glUniform1i(glGetUniformLocation(program, "b_sun"), c.b_sun);
		glUniform1i(glGetUniformLocation(program, "b_ring"), c.b_ring);
		// per-circle draw calls
		glDrawElements(GL_TRIANGLES, 35 * 72 * 6, GL_UNSIGNED_INT, nullptr);
		
	}
	*/
	glBindVertexArray(vertex_array_square);
	
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

		cam.eye = characters[0].center;
		cam.eye.z = 10;
		cam.at = characters[0].center;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, CHARACTER);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

		// per-circle draw calls
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
	}

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
	printf( "- press Home to reset camera\n" );
	printf("- press 'd' to change color type\n");
	printf("- press 'w' to toggle wireframe\n");
	printf( "\n" );
}

std::vector<vertex> create_circle_vertices()
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

void update_vertex_buffer(const std::vector<vertex>& vertices)
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
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
}

std::vector<vertex> create_ring_vertices()
{
	std::vector<vertex> v = {}; // origin
	for (uint k = 0; k <= 72; k++)
	{
		float t = PI * 2.0f * k / float(72), c = cos(t), s = sin(t);
		float p = PI * 2.0f * k / float(72) + PI / float(72), cp = 0.6f * cos(p), sp = 0.6f * sin(p);
		v.push_back({ vec3(cp,sp,0), vec3(cp,sp,0), vec2(1.0f,t / 2 / PI) });
		v.push_back({ vec3(c,s,0), vec3(c,s,0), vec2(0.0f,(p - PI / float(72)) / 2 / PI) });
	}
	return v;
}

void update_ring_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffer
	std::vector<uint> indices;
	for (uint k = 0; k < 2 * 72; k += 2)
	{
		indices.push_back(k);	// the origin
		indices.push_back(k + 1);
		indices.push_back(k + 3);

		indices.push_back(k + 3);
		indices.push_back(k + 1);
		indices.push_back(k);

		indices.push_back(k);	// the origin
		indices.push_back(k + 3);
		indices.push_back(k + 2);

		indices.push_back(k + 2);
		indices.push_back(k + 3);
		indices.push_back(k);	// the origin
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
	if (ring_vertex_array) glDeleteVertexArrays(1, &ring_vertex_array);
	ring_vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!ring_vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
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

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_HOME)					cam=camera();
		else if (key == GLFW_KEY_D)
		{
			color_type = (color_type + 1) % 3;
			printf("> using color type: %d \n", color_type);
		}
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_PAUSE)
		{
			b_rotate = !b_rotate;
			printf("> rotate %s\n", b_rotate ? "start" : "stop");
		}
		else if (key == GLFW_KEY_RIGHT)
		{
			characters[0].look_right = true;
			characters[0].move_right = true;
		}
		else if (key == GLFW_KEY_LEFT)
		{
			characters[0].look_right = false;
			characters[0].move_left = true;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			characters[0].move_down = true;
		}
		else if (key == GLFW_KEY_UP)
		{
			characters[0].move_up = true;
		}
	}
	else if (action == GLFW_RELEASE)
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
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	if (tb.button == GLFW_MOUSE_BUTTON_LEFT && tb.mods == 0)
		tb.update(npos);
	else if (tb.button == GLFW_MOUSE_BUTTON_RIGHT || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods & GLFW_MOD_SHIFT)))
		tb.update_zoom(npos);
	else if (tb.button == GLFW_MOUSE_BUTTON_MIDDLE || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods & GLFW_MOD_CONTROL)))
		tb.update_pan(npos);
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_TEXTURE_2D);								// enable texturing
	//glActiveTexture(GL_TEXTURE0);							// notify GL the current texture slot is 0
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// define the position of four corner vertices
	unit_circle_vertices = std::move(create_circle_vertices());
	unit_square_vertices = std::move(create_square_vertices());
	unit_cube_vertices = std::move(create_cube_vertices());

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer(unit_circle_vertices);
	update_vertex_buffer_square(unit_square_vertices);
	update_vertex_buffer_cube(unit_cube_vertices);

	unit_ring_vertices = std::move(create_ring_vertices());

	update_ring_vertex_buffer(unit_ring_vertices);

	// load texture
	FLOOR = cg_create_texture(floor_image_path, true); if (!FLOOR) return false;
	CHARACTER = cg_create_texture(character_image_path, true); if (!CHARACTER) return false;
	WALL = cg_create_texture(wall_image_path, true); if (!WALL) return false;

	return true;
}

void user_finalize()
{
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
