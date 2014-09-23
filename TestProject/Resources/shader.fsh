varying vec3 var_normal;
varying vec2 var_texCoord;

uniform sampler2D texture;
uniform vec3 diffuse;
uniform vec3 ambient;

void main(void)
{
	vec4 texColor = texture2D(texture, var_texCoord);
	float bright = clamp(dot(normalize(vec3(1,-1,-1)),var_normal),0,1);
	gl_FragColor.xyz = diffuse*texColor.xyz * bright + ambient;
	gl_FragColor.w = 1;
}