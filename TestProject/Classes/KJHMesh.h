#ifndef __MESH_H_
#define __MESH_H_

#include "cocos2d.h"
#include "KJHMath.h"
#include "KJHTexture.h"

namespace KJH
{
	struct MeshVertex
	{
		Float3 pos;
		Float3 normal;
		Float2 uv;
		Float2 boneIdx;
		Float2 weight;
	};
	typedef unsigned short MeshIndex;
	
	struct MeshMaterial
	{
		Float3 diffuse;
		Float3 ambient;
		float alpha;
		int32_t indexNum;
		KJH::Texture2D* tex;

		MeshMaterial() : diffuse()
			, ambient()
			, alpha()
			, indexNum(0)
			, tex(nullptr){}
	};

	struct MeshBone
	{
		MeshBone* parent;	//êeÇ™Ç¢Ç»ÇØÇÍÇŒNULL.
		Float3 offset;
		Float3 translate;
		Quaternion rotate;

		MeshBone()
			: parent(nullptr)
			, translate()
			, rotate()
		{
		}

		Quaternion LocalRotate();
		Float3 LocalTranslate();
	};

	struct QuaTrans
	{
		Quaternion rotate;
		Float4 translate;
	};

	class Mesh
	{
	public:
		GLuint Load(const char* filepath);
		void Draw(unsigned int shader);
		Mesh();
		~Mesh();
	private:
		std::vector<MeshVertex> m_vertex;
		std::vector<MeshIndex> m_index;
		std::vector<MeshMaterial> m_material;
		std::vector<MeshBone> m_bone;
		std::vector<Quaternion> m_bone_rotate;
		std::vector<Float3> m_boneTranslate;
		std::vector<QuaTrans> m_boneQuaTrans;

		GLuint m_vertexNum;
		GLuint m_vertexBuffer;
		GLuint m_indexBuffer;
		KJH::Texture2D* m_white;
		
		GLuint m_boneTex;

	};
}

#endif // __MESH_H_