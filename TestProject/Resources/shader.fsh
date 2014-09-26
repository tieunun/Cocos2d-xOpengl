#ifdef GL_ES
#define hvec2 highp vec2
#define hvec3 highp vec3
#define hvec4 highp vec4
#define hfloat highp float
#else
#define hvec2 vec2
#define hvec3 vec3
#define hvec4 vec4
#define hfloat float
#endif

varying hvec3 var_normal;
varying hvec2 var_texCoord;

uniform sampler2D texture1;
uniform sampler2D texture;
uniform hvec3 diffuse;
uniform hvec3 ambient;

void main(void)
{
	hvec3 lightDir = normalize(vec3(1.0,-1.0,1.0));	
	hfloat bright = dot(lightDir,var_normal);
	bright = clamp(bright,0.0,1.0);
	
	hvec4 texColor = texture2D(texture, var_texCoord);
	gl_FragColor.xyz = (diffuse*texColor.xyz) * bright + ambient;
	gl_FragColor.w = 1.0;
}