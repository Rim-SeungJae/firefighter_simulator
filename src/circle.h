#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "cgmath.h"
#include "fire.h"

struct circle_t
{
	vec3	center=vec3(0);		// 2D position for translation
	float	radius=0.5f;		// radius
	vec3	velocity;
	float	gravity = 9.8f;

	bool	alive = true;

	mat4	model_matrix;		// modeling transformation
	
	// public functions
	circle_t(int look_at, vec3 c_center) 
	{
		reset(look_at,c_center);
	}
	void	reset(int look_at,vec3 c_center);
	void	update( float t, std::vector<fire_t> * fires, std::vector<wall_t> walls, int * sound);
};

inline void circle_t::reset(int look_at, vec3 c_center)
{
	center = c_center;
	center.z = 0.6f;
	switch (look_at)
	{
		case 0:
			velocity = vec3(5.0f, 0.0f, 3.0f);
			break;
		case 1:
			velocity = vec3(-5.0f, 0.0f, 3.0f);
			break;
		case 2:
			velocity = vec3(0.0f, -5.0f, 3.0f);
			break;
		case 3:
			velocity = vec3(0.0f, 5.0f, 3.0f);
			break;
	}
}

inline void circle_t::update( float dt, std::vector<fire_t> * fires, std::vector<wall_t> walls, int * sound)
{
	bool b_colide = false;
	center += velocity * dt;
	velocity.z -= gravity * dt;
	for (auto& w : walls)
	{
		if (w.center.x - w.width<center.x + radius && w.center.x + w.width > center.x - radius && w.center.y + w.length > center.y - radius && w.center.y - w.length < center.y + radius)
		{
			b_colide = true;
		}
	}
	if (center.z < radius||b_colide)
	{
		alive = false;
		*sound = 1;
		for (std::vector<fire_t>::iterator it = (*fires).begin(); it != (*fires).end();)
		{
			if (distance((*it).center,center)<1.0f)
			{
				it = (*fires).erase(it);
				*sound = 2;
			}
			else
			{
				it++;
			}
		}
	}
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix*scale_matrix;
}


#endif
