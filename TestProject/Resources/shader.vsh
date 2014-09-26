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

attribute hvec3 in_pos;
attribute hvec2 in_uv;
attribute hvec3 in_normal;
attribute hvec2 in_bone;
attribute hvec2 in_weight;

uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;
uniform hvec3 bone_translate[100];
uniform hvec4 bone_quaternion[100];
uniform sampler2D bone_tex;

hvec4 QuaConj(hvec4 q)
{
	hvec4 conj = vec4(-q.x,-q.y,-q.z,q.w);
	return conj;
}

hvec4 QQMul(hvec4 q1,hvec4 q2)
{
	return vec4(q2.xyz*q1.w + q1.xyz*q2.w + cross(q1.xyz,q2.xyz), q1.w*q2.w - dot(q1.xyz,q2.xyz));
}

hvec3 QVMul(hvec3 v,hvec4 q)
{
	hvec4 quaVec = vec4(v,0.0);
	hvec4 result = QQMul( QuaConj(q),quaVec);
	result = QQMul(result,q);
	return result.xyz;
}

void main(void)
{

	//スキニング.
	hvec3 pos = QVMul(in_pos,bone_quaternion[int(in_bone.x)]) * in_weight.x;
	pos += QVMul(in_pos,bone_quaternion[int(in_bone.y)]) * in_weight.y;
	pos += bone_translate[int(in_bone.x)] * in_weight.x;
	pos += bone_translate[int(in_bone.y)] * in_weight.y;
	
	//pos += texture2D(bone_tex,vec2((in_bone.x*2.0) / 1024.0,0.0)).xyz;
	//ワールドビュー変換.
	gl_Position = vec4(pos,1.0) * world * view * proj;
	
	
	var_normal = (vec4(in_normal,1.0) * world).xyz;
	var_texCoord = in_uv;
	in_bone;
	in_weight;
}