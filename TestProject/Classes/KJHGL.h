#ifndef __KJHGL_H_
#define __KJHGL_H_


#include "cocos2d.h"


namespace KJH
{
class GLStatus
{
public:
	GLStatus();
	void Init();
	void Log();
private:
	GLint m_maxVertexAttibs;
	GLint m_maxVaryingVectors;
	GLint m_maxVertexUniformVectors;
	GLint m_maxFragmentUniFormVectors;
	GLint m_maxVertexTextureImageUnits;
	GLint m_maxTextureImageUnits;
	GLint m_maxTextureSize;
	GLint m_maxCubeMapTextureSize;
	GLint m_maxRenderBufferSize;
	GLint m_maxViewPortDims;

};
}


#endif //__KJHGL_H_