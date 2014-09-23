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
		Float2 weight;
		int16_t boneIdx[2];
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
		std::vector<Mat> m_bone;
		GLuint m_vertexNum;
		GLuint m_vertexBuffer;
		GLuint m_indexBuffer;
	};
}

#endif // __MESH_H_