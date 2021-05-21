#ifndef __PARTICLE_H__
#define __PARTICLE_H__
#pragma once

#include "cgmath.h"
#include "cgut.h"

inline float random_range(float min, float max) { return mix(min, max, rand() / float(RAND_MAX)); }

struct particle_t
{
	static constexpr int MAX_PARTICLES = 10;

	vec2 pos;
	vec4 color;
	vec2 velocity;
	float scale;
	float life;
	float alpha_val = 1;
	GLuint program;

	//optional
	float elapsed_time;
	float time_interval;

	particle_t() { reset(); }
	void reset();
	void update(GLuint prg,float dt);
};

inline void particle_t::reset()
{
	pos = vec2(random_range(-0.4f, 0.4f), random_range(-0.2f, 0.0f));
	scale = random_range(0.02f, 0.04f);
	life = random_range(0.01f, 1.0f);
	velocity = vec2(0.0f, random_range(1.0f, 2.0f));
	elapsed_time = 0.0f;
	time_interval = random_range(1.0f, 3.0f);
	alpha_val=1.0f;
}

inline void particle_t::update(GLuint prg, float dt)
{
	program = prg;
	const float dwTime = dt;
	elapsed_time += dwTime;
	/*
	if (elapsed_time > time_interval)
	{
		const float theta = random_range(0, 1.0f) * PI * 2.0f;
		velocity = vec2(cos(theta), sin(theta));

		elapsed_time = 0.0f;
	}
	*/
	pos += velocity*dwTime;

	life -= 5.0f * dwTime;
	// disappear
	if (life < 0.0f)
	{
		constexpr float alpha_factor = 0.001f;
		alpha_val -= 5.0f * dwTime;
		glUniform1f(glGetUniformLocation(program, "alpha_val"), alpha_val);
	}
	// dead
	if (alpha_val < 0.0f)
	{
		reset();
	}
}

#endif