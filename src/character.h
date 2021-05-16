#pragma once
#ifndef __CHARACTER_H__
#define __CHARACTER_H__

struct character_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	size = 1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update();
};

inline std::vector<character_t> create_characters()
{
	std::vector<character_t> characters;
	character_t t;

	t = { vec3(0,0,0.1f),0.5f };
	characters.emplace_back(t);
	return characters;
}

inline void character_t::update()
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
