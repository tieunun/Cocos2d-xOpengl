#ifndef __KJHTEXTURE_H__
#define __KJHTEXTURE_H__

#include "cocos2d.h"



namespace KJH
{
	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();
		static Texture2D* Create(const std::string& filePath);
		static Texture2D* CreateWhite();
		GLuint GetTex()const{return m_texture;}
	private:
		GLuint m_texture;
	};

}

#endif // __KJHTEXTURE_H__