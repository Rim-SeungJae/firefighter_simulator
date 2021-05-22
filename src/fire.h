#pragma once
#ifndef __FIRE_H__
#define __FIRE_H__

#include "wall.h"
#include "cgut.h"
#include "cgmath.h"
#include "particle.h"

inline vec3 random_pos()
{
	float x = rand() % 40 - 19.5f;
	float y = rand() % 40 - 19.5f;
	return vec3(x, y, 0.05f);
}

struct fire_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	size = 0.5f;

	std::vector<particle_t> particles;

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update();
};

inline std::vector<fire_t> create_fires(int N, std::vector<wall_t> walls)
{
	std::vector<fire_t> fires;
	fire_t t;

	for (int i = 0; i < N; i++)
	{
		vec3 pos = random_pos();
		float size = 0.5f;
		bool cont_flag = false;
		for (auto& w : walls)
		{
			if (w.center.x - w.width<pos.x + size && w.center.x + w.width > pos.x - size && w.center.y + w.length > pos.y - size && w.center.y - w.length < pos.y + size)
			{
				i--;
				cont_flag = true;
				break;
			}
		}
		if (cont_flag) continue;
		t = { pos };
		t.particles.resize(particle_t::MAX_PARTICLES);
		fires.emplace_back(t);
	}
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
