#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

in	vec3 tc;

out	vec4 fragColor;

uniform samplerCube skybox;

void main()
{    
    fragColor = texture(skybox, tc);
}  
