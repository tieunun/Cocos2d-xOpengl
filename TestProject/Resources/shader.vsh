#ifdef GL_ES
#define hvec2 highp vec2
#define hvec3 highp vec3
#define hvec4 highp vec4
#define hfloat highp float
#define hint highp int
#else
#define hvec2 vec2
#define hvec3 vec3
#define hvec4 vec4
#define hfloat float
#define hint int
#endif

//送る変数.
varying hvec3 var_normal;
varying hvec2 var_texCoord;

//頂点の肩.
attribute hvec3 in_pos;
attribute hvec2 in_uv;
attribute hvec3 in_normal;
attribute hvec2 in_bone;
attribute hvec2 in_weight;

//ユニフォーム変数.
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
#if 01
	hvec3 pos = QVMul(in_pos,bone_quaternion[int(in_bone.x)]) * in_weight.x;
	pos      += QVMul(in_pos,bone_quaternion[int(in_bone.y)]) * in_weight.y;
	pos      += bone_translate[int(in_bone.x)] * in_weight.x;
	pos      += bone_translate[int(in_bone.y)] * in_weight.y;

#else
	const hint BONE_NUM = 2;	//ボーン数.
	hvec2 boneQua[BONE_NUM];	//クォータニオンのUV.
	hvec2 boneTrans[BONE_NUM];	//移動のUV.

	const hfloat TEX_SIZE = 32.0;	//ボーンのテクスチャサイズ.
	hfloat pixelSize = 1.0 / TEX_SIZE;	//ピクセル数.

	//UVを計算する.
	for(hint i = 0; i < BONE_NUM; ++i)
	{
		boneQua[i].x = mod(int(in_bone[i]) * BONE_NUM,TEX_SIZE) / TEX_SIZE;
		boneQua[i].y = int(in_bone[i]) * BONE_NUM / TEX_SIZE / TEX_SIZE;
		boneTrans[i] = boneQua[i] + vec2(pixelSize,0.0);
	}
	hvec3 pos = vec3(0.0,0.0,0.0);
	//回転.
	for(hint i = 0; i < BONE_NUM; ++i)
	{
		pos += QVMul(in_pos,texture2D(bone_tex,boneQua[i])) * in_weight[i];
	}
	//移動.
	//pos += in_pos;
	for(hint i = 0; i < BONE_NUM; ++i)
	{
		pos += texture2D(bone_tex,boneTrans[i]).xyz * in_weight[i];
	}
#endif
	
	//ワールドビュー変換.
	gl_Position = vec4(pos,1.0) * world * view * proj;
	var_normal = (vec4(in_normal,1.0) * world).xyz;
	var_texCoord = in_uv;
	
}