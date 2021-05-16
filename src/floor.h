#pragma once
#pragma once
#ifndef __FLOOR_H__
#define __FLOOR_H__

struct floor_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	size = 1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update();
};

inline std::vector<floor_t> create_floors()
{
	std::vector<floor_t> floors;
	floor_t f;

	f = { vec3(0),20.0f };
	floors.emplace_back(f);
	return floors;
}

inline void floor_t::update()
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		size, 0, 0, 0,
		0, size, 0, 0,
		0, 0, size, 0,
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
