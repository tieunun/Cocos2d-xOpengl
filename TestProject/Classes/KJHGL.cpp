#include "KJHGL.h"


namespace KJH
{
	GLStatus::GLStatus()
		: m_maxVertexAttibs(0)
		, m_maxVaryingVectors(0)
		, m_maxVertexUniformVectors(0)
		, m_maxFragmentUniFormVectors(0)
		, m_maxVertexTextureImageUnits(0)
		, m_maxTextureImageUnits(0)
		, m_maxTextureSize(0)
		, m_maxCubeMapTextureSize(0)
		, m_maxRenderBufferSize(0)
		, m_maxViewPortDims(0)
	{


	}

	void GLStatus::Init()
	{
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&m_maxVertexAttibs);
		glGetIntegerv(GL_MAX_VARYING_VECTORS,&m_maxVaryingVectors);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS,&m_maxVertexUniformVectors);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS,&m_maxFragmentUniFormVectors);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,&m_maxVertexTextureImageUnits);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_maxTextureImageUnits);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE,&m_maxTextureSize);
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,&m_maxCubeMapTextureSize);
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&m_maxRenderBufferSize);
		//glGetIntegerv(GL_MAX_VIEWPORT_DIMS,&m_maxViewPortDims);
	}


	void GLStatus::Log()
	{
		CCLOG("GL_STATUS------------------------------------------");
		CCLOG("GL_MAX_VERTEX_ATTRIBS:%d",m_maxVertexAttibs);
		CCLOG("GL_MAX_VARYING_VECTORS:%d",m_maxVaryingVectors);
		CCLOG("GL_MAX_VERTEX_UNIFORM_VECTORS:%d",m_maxVertexUniformVectors);
		CCLOG("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:%d",m_maxVertexTextureImageUnits);
		CCLOG("GL_MAX_FRAGMENT_UNIFORM_VECTORS:%d",m_maxFragmentUniFormVectors);
		CCLOG("GL_MAX_TEXTURE_IMAGE_UNITS:%d",m_maxTextureImageUnits);
		CCLOG("GL_MAX_TEXTURE_SIZE:%d",m_maxTextureSize);
		CCLOG("GL_MAX_CUBE_MAP_TEXTURE_SIZE:%d",m_maxCubeMapTextureSize);
		CCLOG("GL_MAX_RENDERBUFFER_SIZE:%d",m_maxRenderBufferSize);
		CCLOG("GL_MAX_VIEWPORT_DIMS:%d",m_maxViewPortDims);
		CCLOG("---------------------------------------------------");

	}
















}