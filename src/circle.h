#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec3	center=vec3(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	theta=0.0f;			// rotation angle for self rotation
	float	phi = 0.0f;			// rotation angle for revolution
	vec4	color;				// RGBA color in [0,1]
	float	rot_speed = 1.0f;
	float	rev_speed = 1.0f;

	bool	b_dwarf = false;
	float	d_rev = 1.0f;
	vec3	d_center = vec3(1.0f, 0, 0);
	float	rho = 0.0f;

	bool	b_sun = false;
	int		planet = 0;

	bool	b_ring = false;

	mat4	model_matrix;		// modeling transformation
	


	// public functions
	void	update( float t );
};

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	c = {vec3(0),2.0f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f),1/25.3f,0};
	c.b_sun = true;
	circles.emplace_back(c);
	c = {vec3(-4.0f,0,0),0.2f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f),1/5.8f,1/0.24f};
	c.planet = 1;
	circles.emplace_back(c);
	c = { vec3(7.2f,0,0),0.3f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f),1/24.3f,1/0.68f};
	c.planet = 2;
	circles.emplace_back(c);
	c = { vec3(-10.0f,0,0),0.4f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1/1.0f ,1/1.0f};
	c.planet = 3;
	circles.emplace_back(c);
	c = { vec3(-10.0f,0,0),0.1f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1 / 1.0f ,1 / 1.0f , true, 2.0f};
	circles.emplace_back(c);
	c = { vec3(15.0f,0,0),0.42f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1/1.03f ,1/1.9f};
	c.planet = 4;
	circles.emplace_back(c);
	c = { vec3(-20.0f,0,0),1.12f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1/0.41f ,1/3.0f};
	c.planet = 5;
	circles.emplace_back(c);
	c = { vec3(-20.0f,0,0),0.15f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1 / 0.5f ,1 / 3.0f , true, 2.0f,  vec3(1.5f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-20.0f,0,0),0.18f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1 / 0.6f ,1 / 3.0f , true, 2.2f,  vec3(-1.7f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-20.0f,0,0),0.2f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1 / 0.8f ,1 / 3.0f , true, 2.4f,  vec3(2.0f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-20.0f,0,0),0.22f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1 / 1.0f ,1 / 3.0f , true, 2.8f, vec3(-2.2f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(25.0f,0,0),0.95f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1/0.45f,1/5.0f};
	c.planet = 6;
	circles.emplace_back(c);
	c = { vec3(25.0f,0,0),2.0f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f), 1 / 0.45f,1 / 5.0f };
	c.b_ring = true;
	circles.emplace_back(c);
	c = { vec3(-30.0f,0,0),0.6f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1/0.71f,1/8.0f};
	c.planet = 7;
	circles.emplace_back(c);
	c = { vec3(-30.0f,0,0),0.08f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 0.8f,1 / 8.0f , true, 2.0f, vec3(1.5f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-30.0f,0,0),0.1f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 0.5f,1 / 8.0f , true, 2.9f, vec3(-1.8f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-30.0f,0,0),0.11f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 1.0f,1 / 8.0f , true, 2.7f, vec3(2.2f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-30.0f,0,0),0.12f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 1.2f,1 / 8.0f , true, 2.8f, vec3(-2.5f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(-30.0f,0,0),0.14f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 0.6f,1 / 8.0f , true, 0.5f, vec3(3.0f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(35.0f,0,0),0.76f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1/0.74f,1/10.0f};
	c.planet = 8;
	circles.emplace_back(c);
	c = { vec3(35.0f,0,0),0.14f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 1.0f,1 / 10.0f , true, 2.0f, vec3(1.5f, 0, 0) };
	circles.emplace_back(c);
	c = { vec3(35.0f,0,0),0.12f,0.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f) ,1 / 1.7f,1 / 10.0f , true, 5.0f, vec3(2.5f, 0, 0) };
	circles.emplace_back(c);
	return circles;
}

inline void circle_t::update( float dt)
{
	theta += dt*rot_speed*5.0f;
	phi += dt*rev_speed;
	rho += dt * d_rev;
	float c_theta = cos(theta), s_theta = sin(theta);
	float c_phi = cos(phi), s_phi = sin(phi);
	float c_rho = cos(rho), s_rho = sin(rho);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c_theta,-s_theta, 0, 0,
		s_theta, c_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix2 =
	{
		c_phi,-s_phi, 0, 0,
		s_phi, c_phi, 0, 0,
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

	mat4 d_translate_matrix=
	{
		1, 0, 0, d_center.x,
		0, 1, 0, d_center.y,
		0, 0, 1, d_center.z,
		0, 0, 0, 1
	};

	mat4 d_rev_matrix=
	{
		c_rho,-s_rho, 0, 0,
		s_rho, c_rho, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = rotation_matrix2*translate_matrix*rotation_matrix*scale_matrix;
	if(b_dwarf) model_matrix = rotation_matrix2 * translate_matrix * d_rev_matrix * d_translate_matrix * rotation_matrix * scale_matrix;
}


#endif
