#pragma once
#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "wall.h"
#include "cgmath.h"
#include "cgut.h"
#include "fire.h"
#include "particle.h"

struct character_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	size = 0.5f;		// radius
	float	velocity = 5.0f;
	int		look_at = 0;
	bool	move_up=false;
	bool	move_left=false;
	bool	move_right=false;
	bool	move_down=false;

	float	elapsed_time;
	float	time_interval;

	bool	saved = false;

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float dt, std::vector<wall_t> walls);
	void	update_npc(float dt, std::vector<wall_t> walls, std::vector<character_t> characters, std::vector<fire_t> fires);
};

inline std::vector<character_t> create_characters()
{
	std::vector<character_t> characters;
	character_t t;

	t = { vec3(2.0f,2.0f,0.1f),0.5f };
	characters.emplace_back(t);
	return characters;
}

inline std::vector<character_t> create_npcs(int N, std::vector<wall_t> walls)
{
	std::vector<character_t> npcs;
	character_t t;
	
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
		t = { pos, size };
		if (rand()%2 == 1)
		{
			t.move_up = true;
			t.move_down = false;
		}
		else
		{
			t.move_up = false;
			t.move_down = true;
		}
		if (rand() % 2 == 1)
		{
			t.move_left = true;
			t.move_right = false;
		}
		else
		{
			t.move_left = false;
			t.move_right = true;
		}
		t.velocity = random_range(1.0f, 3.0f);
		t.elapsed_time = 0.0f;
		t.time_interval = random_range(1.0f, 3.0f);
		npcs.emplace_back(t);
	}
	return npcs;
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
		if (w.center.x - w.width<center0.x + size && w.center.x + w.width > center0.x - size && w.center.y + w.length > center.y - size && w.center.y - w.length < center.y + size) center.y = center0.y;
		if (w.center.x - w.width<center.x + size && w.center.x + w.width > center.x - size && w.center.y + w.length > center0.y - size && w.center.y - w.length < center0.y + size) center.x = center0.x;
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

inline void character_t::update_npc(float dt, std::vector<wall_t> walls, std::vector<character_t> characters, std::vector<fire_t> fires)
{
	vec3 center0 = center;
	elapsed_time += dt;
	if (elapsed_time > time_interval)
	{
		if (rand() % 2 == 1)
		{
			move_up = true;
			move_down = false;
		}
		else
		{
			move_up = false;
			move_down = true;
		}
		if (rand() % 2 == 1)
		{
			move_left = true;
			move_right = false;
		}
		else
		{
			move_left = false;
			move_right = true;
		}
		velocity = random_range(1.0f, 3.0f);
		elapsed_time = 0.0f;
		time_interval = random_range(1.0f, 3.0f);
	}

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
		if (w.center.x - w.width<center.x + size && w.center.x + w.width > center.x - size && w.center.y + w.length > center0.y - size && w.center.y - w.length < center0.y + size) center.x = center0.x;
		if (w.center.x - w.width<center0.x + size && w.center.x + w.width > center0.x - size && w.center.y + w.length > center.y - size && w.center.y - w.length < center.y + size) center.y = center0.y;
	}
	for (auto& w : fires)
	{
		if (w.center.x - w.size<center.x + size && w.center.x + w.size > center.x - size && w.center.y + w.size > center0.y - size && w.center.y - w.size < center0.y + size) center.x = center0.x;
		if (w.center.x - w.size<center0.x + size && w.center.x + w.size > center0.x - size && w.center.y + w.size > center.y - size && w.center.y - w.size < center.y + size) center.y = center0.y;
	}
	for (auto& w : characters)
	{
		if (w.center.x - w.size<center.x + size && w.center.x + w.size > center.x - size && w.center.y + w.size > center.y - size && w.center.y - w.size < center.y + size) saved=true;
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
	if (move_left)
	{
		model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	}
}


#endif
