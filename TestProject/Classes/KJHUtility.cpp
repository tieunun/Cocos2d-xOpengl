#include "KJHUtility.h"

namespace KJH
{

static GLuint CreateShader(const std::string& filePath,const GLuint SHADER_TYPE)
{
	cocos2d::CCFileUtils *fileUtils = cocos2d::CCFileUtils::sharedFileUtils();
	std::string path = fileUtils->fullPathForFilename(filePath);
	unsigned long size = 0;
	unsigned char *data = fileUtils->getFileData(path.c_str(), "r", (ssize_t*)&size);
	assert(data);
	std::string d = (char*)data;
	GLuint shader = glCreateShader(SHADER_TYPE);
	glShaderSource(shader,1,(const GLchar**)&data,(const GLint*)&size);
	glCompileShader(shader);
	int cvs=0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &cvs);
	if(cvs == 0){
		GLsizei bufSize, length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
		char info[2048*2];
		glGetShaderInfoLog(shader, bufSize, &length, info);
		CCLOGERROR("compiled: %s",info);
	}

	delete[] data;

	return shader;
}

GLuint VSCreateShader(const std::string& filePath)
{
	return CreateShader(filePath,GL_VERTEX_SHADER);
}
GLuint FSCreateShader(const std::string& filePath)
{
	return CreateShader(filePath,GL_FRAGMENT_SHADER);
}


}