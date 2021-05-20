#pragma once
#ifndef __FIRE_H__
#define __FIRE_H__

struct fire_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	size = 1.0f;		// radius

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update();
};

inline std::vector<fire_t> create_fires()
{
	std::vector<fire_t> fires;
	fire_t t;

	t = { vec3(0,0,0.05f),0.5f };
	fires.emplace_back(t);
	return fires;
}

inline void fire_t::update()
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
