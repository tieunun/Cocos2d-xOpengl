#ifndef __MESH_H_
#define __MESH_H_

#include "cocos2d.h"

namespace KJH
{
	struct MeshVertex
	{
		cocos2d::Vec3 vertex;
	};
	typedef unsigned short MeshIndex;
	
	class Mesh
	{
		std::vector<MeshVertex> m_vertex;
		std::vector<MeshIndex> m_index;
	public:
		GLuint Load(const char* filepath);
	};
}

#endif // __MESH_H_