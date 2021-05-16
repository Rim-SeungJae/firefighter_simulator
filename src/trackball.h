#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__
#include "cgmath.h"


//*************************************
// common structures
struct camera
{
	vec3	eye = vec3(0, 0, 10);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 1, 0);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};

struct trackball
{
	bool	b_tracking = false;
	float	scale;			// controls how much rotation is applied
	mat4	view_matrix0;	// initial view matrix
	vec2	m0;				// the last mouse position
	vec3	eye0;
	vec3	at0;
	vec3	up0;

	camera* cam;

	int button;
	int mods;

	trackball( float rot_scale=1.0f ) : scale(rot_scale){}
	bool is_tracking() const { return b_tracking; }
	void begin( camera * cam, vec2 m);
	void end() { b_tracking = false; }
	void update( vec2 m ) const;
	void update_zoom( vec2 m) const;
	void update_pan(vec2 m) const;
};

inline void trackball::begin( camera * cam, vec2 m)
{
	b_tracking = true;			// enable trackball tracking
	m0 = m;						// save current mouse position
	view_matrix0 = cam->view_matrix;	// save current view matrix
	eye0 = cam->eye;
	at0 = cam->at;
	up0 = cam->up;
	this->cam = cam;
}

inline void trackball::update( vec2 m ) const
{
	// project a 2D mouse position to a unit sphere
	static const vec3 p0 = vec3(0,0,1.0f);	// reference position on sphere
	vec3 p1 = vec3(m-m0,0);					// displacement
	if( !b_tracking || length(p1)<0.0001f ) return;		// ignore subtle movement
	p1 *= scale;														// apply rotation scale
	p1 = vec3(p1.x,p1.y,sqrtf(std::max(0.0f,1.0f-length2(p1)))).normalize();	// back-project z=0 onto the unit sphere

	// find rotation axis and angle in world space
	// - trackball self-rotation should be done at first in the world space
	// - mat3(view_matrix0): rotation-only view matrix
	// - mat3(view_matrix0).transpose(): inverse view-to-world matrix
	vec3 v = mat3(view_matrix0).transpose()*p0.cross(p1);
	float theta = asin( std::min(v.length(),1.0f) );

	// resulting view matrix, which first applies
	// trackball rotation in the world space
	cam->view_matrix = view_matrix0*mat4::rotate(v.normalize(),theta);
	cam->eye = eye0*mat4::rotate(v.normalize(), theta);
	cam->at = at0 * mat4::rotate(v.normalize(), theta);
	cam->up = up0 * mat4::rotate(v.normalize(), theta);
}

inline void trackball::update_zoom(vec2 m) const
{
	float zoom_scale = 10.0f;
	vec3 n = (eye0 - at0).normalize();
	if (!b_tracking || abs(m.y-m0.y) < 0.0001f) return;		// ignore subtle movement

	cam->eye = eye0 + n * (m.y - m0.y) * zoom_scale;
	cam->view_matrix = mat4::look_at(cam->eye, cam->at, cam->up);
}

inline void trackball::update_pan(vec2 m) const
{
	float pan_scale = 5.0f+length2(eye0)/100;
	vec3 n = (eye0 - at0).normalize();
	vec3 u = up0.cross(n).normalize();
	vec3 v = n.cross(u).normalize();

	vec2 p1 = m - m0;
	if (!b_tracking || length(p1) < 0.0001f) return;		// ignore subtle movement
	p1 *= pan_scale;

	cam->eye = eye0 - u * p1.x - v * p1.y;
	cam->at = at0 - u * p1.x - v * p1.y;
	cam->view_matrix = mat4::look_at(cam->eye, cam->at, cam->up);
}

// utility function
inline vec2 cursor_to_ndc( dvec2 cursor, ivec2 window_size )
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2( float(cursor.x)/float(window_size.x-1),
					  float(cursor.y)/float(window_size.y-1) );
	
	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x*2.0f-1.0f,1.0f-npos.y*2.0f);
}

#endif // __TRACKBALL_H__
