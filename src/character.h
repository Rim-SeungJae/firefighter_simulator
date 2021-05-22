#pragma once
#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "wall.h"

struct character_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	size = 0.5f;		// radius
	vec4	color;				// RGBA color in [0,1]
	float	velocity = 5.0f;
	int		look_at = 0;
	bool	move_up=false;
	bool	move_left=false;
	bool	move_right=false;
	bool	move_down=false;

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float dt, std::vector<wall_t> walls);
};

inline std::vector<character_t> create_characters()
{
	std::vector<character_t> characters;
	character_t t;

	t = { vec3(2.0f,2.0f,0.1f),0.5f };
	characters.emplace_back(t);
	return characters;
}

inline void character_t::update(float dt, std::vector<wall_t> walls)
{
	vec3 center0 = center;
	if (move_up)
	{
		center.y += velocity * dt;
	}
	if (move_left)
	{
		center.x -= velocity * dt;
	}
	if (move_right)
	{
		center.x += velocity * dt;
	}
	if (move_down)
	{
		center.y -= velocity * dt;
	}
	for (auto& w : walls)
	{
		if (w.center.x - w.width<center.x + size && w.center.x + w.width > center.x - size && w.center.y + w.length > center.y - size && w.center.y - w.length < center.y + size)
		{
			center = center0;
		}
	}
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		size, 0, 0, 0,
		0, size, 0, 0,
		0, 0, size, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
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
	if (look_at == 1)
	{
		model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	}
}


#endif
