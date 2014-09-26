#ifndef __KJHTEXTURE_MANAGER_H__
#define __KJHTEXTURE_MANAGER_H__

#include "KJHTexture.h"
#include <map>


namespace KJH
{
	class TextureManager
	{
		std::map<std::string,KJH::Texture2D*> m_texMap;
	};
}

#endif // __KJHTEXTURE_MANAGER_H__