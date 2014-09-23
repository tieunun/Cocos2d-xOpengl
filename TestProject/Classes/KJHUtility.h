#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "cocos2d.h"



namespace KJH
{


GLuint VSCreateShader(const std::string& filePath);
GLuint FSCreateShader(const std::string& filePath);

}

#endif // __UTILITY_H__