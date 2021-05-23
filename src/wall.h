#pragma once
#ifndef __WALL_H__
#define __WALL_H__

struct wall_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	width = 0.5f;
	float	length = 0.5f;		// radius
	float	height = 1.0f;
	vec4	color;				// RGBA color in [0,1]

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update();
};

inline std::vector<wall_t> create_walls()
{
	std::vector<wall_t> walls;
	wall_t w;

	for (float i = -19.5f; i <= 19.5f; i++)
	{
		w = { vec3(19.5f,i,1.0f)};
		walls.emplace_back(w);
		w = { vec3(-19.5f,i,1.0f) };
		walls.emplace_back(w);
		w = { vec3(i,19.5f,1.0f) };
		walls.emplace_back(w);
		w = { vec3(i,-19.5f,1.0f) };
		walls.emplace_back(w);
	}
	for (float i = -18.5f; i <= 10.0f; i++)
	{
		if (i<-1.0f && i>-4.0f) continue;
		w = { vec3(0.5f,i,1.0f) };
		walls.emplace_back(w);
	}
	for (float i = -18.5f; i <= 0.0f; i++)
	{
		if (i>-11 && i<-9) continue;
		w = { vec3(i,-0.5f,1.0f) };
		walls.emplace_back(w);
	}
	for (float i = -1.5; i > -14.0f; i--)
	{
		w = { vec3(-8.5f,i,1.0f) };
		walls.emplace_back(w);
	}
	for (float i = 18.5f; i > 5.0f; i--)
	{
		w = { vec3(-9.5f,i,1.0f) };
		walls.emplace_back(w);
	}
	for (float i = -18.5f; i <= -4.0f; i++)
	{
		w = { vec3(9.5f,i,1.0f) };
		walls.emplace_back(w);
	}
	for (float i = 0.5f; i < 19.0f; i++)
	{
		if (i > 6.0f && i < 13.0f) continue;
		w = { vec3(i,-4.5f,1.0f) };
		walls.emplace_back(w);
	}
	return walls;
}

inline void wall_t::update()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		width, 0, 0, 0,
		0, length, 0, 0,
		0, 0, height, 0,
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
