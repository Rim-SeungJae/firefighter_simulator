#pragma once
#ifndef __WALL_H__
#define __WALL_H__

struct wall_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	width = 1.0f;
	float	length = 1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update();
};

inline std::vector<wall_t> create_walls()
{
	std::vector<wall_t> walls;
	wall_t w;

	w = { vec3(0.0f,2.0f,1.0f),1.0f, 1.0f };
	walls.emplace_back(w);
	return walls;
}

inline void wall_t::update()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		width, 0, 0, 0,
		0, length, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix * scale_matrix;
}


#endif
