
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

varying vec3 var_normal;
varying vec2 var_texCoord;

attribute vec3 in_pos;
attribute vec2 in_uv;
attribute vec3 in_normal;

void main(void)
{
	gl_Position = vec4(in_pos,1) * world * view * proj;
	var_normal = (vec4(in_normal,1) * world).xyz;
	var_texCoord = in_uv;
}