#include "KJHTexture.h"

namespace KJH
{
	Texture2D::Texture2D()
	{

	}
	Texture2D::~Texture2D()
	{
		if(m_texture) glDeleteTextures(1,&m_texture);
	}
	Texture2D* Texture2D::Create(const std::string& filePath)
	{
		auto image = new cocos2d::Image();
		if(!image->initWithImageFile(filePath)){
			CCLOG("%s not open.\n",filePath.c_str());
			delete image;
			return nullptr;
		}
		
		KJH::Texture2D* tex = new KJH::Texture2D();

		//テクスチャ生成.
		glGenTextures(1,&tex->m_texture);

		glBindTexture(GL_TEXTURE_2D,tex->m_texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		GLuint format = 0;
		switch(image->getRenderFormat())
		{
		case cocos2d::Texture2D::PixelFormat::RGB888:
			format = GL_RGB;
			break;
		case cocos2d::Texture2D::PixelFormat::RGBA8888:
			format = GL_RGBA;
			break;
		}
		glTexImage2D(GL_TEXTURE_2D,0,format,image->getWidth(),image->getHeight(),0,format,GL_UNSIGNED_BYTE,image->getData());

		GLenum error = glGetError();

		delete image;
		return tex;
	}


	Texture2D* Texture2D::CreateWhite()
	{
		KJH::Texture2D* tex = new KJH::Texture2D();
		unsigned char textureByte[][4] = {
			{255,255,255,255},{255,255,255,255},
			{255,255,255,255},{255,255,255,255},
		};
		glGenTextures(1,&tex->m_texture);
		glBindTexture(GL_TEXTURE_2D,tex->m_texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,textureByte);
	
		return tex;
	}

}