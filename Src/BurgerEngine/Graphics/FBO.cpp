#include "FBO.h"

#include <iostream>
#include <assert.h>

FBO::FBO(unsigned int iWidth, unsigned int iHeight, FboEnum eType)
	: m_iWidth(iWidth)
	, m_iHeight(iHeight)
	, m_eDefinitionType(eType)
{
	m_bIsActivated = false;
	///If we are creating a a cube map FBO, we specify the texture type
	if (m_eDefinitionType == E_FBO_CUBE)
		m_eTextureType = GL_TEXTURE_CUBE_MAP;
	else
		m_eTextureType = GL_TEXTURE_2D;
}

void FBO::GenerateDepthOnly()
{
	Destroy();

	glGenTextures(1, &m_iTexDepthId);
	glBindTexture(m_eTextureType, m_iTexDepthId);

	//we create a special texture for depth
	glTexImage2D(m_eTextureType, 0, GL_DEPTH_COMPONENT, m_iWidth, m_iHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri (m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(m_eTextureType, 0);

	//Generating ID
	glGenFramebuffersEXT(1, &m_iId);
	Activate();

	//We attach the texture to the depth attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,m_eTextureType, m_iTexDepthId, 0);

	//In order to avoid Color in the depth buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	Deactivate();

	//Check FBO status
	if(!CheckFramebufferStatus())
		std::cerr<<"ERROR : FBO creation Fail "<<std::endl;

	//Depth Only
	m_iType = 1;
}

void FBO::GenerateColorOnly( GLint iInternalFormat, GLint iFormat )
{
	Destroy();

	//Generate the texture
	glGenTextures(1, &m_iTexId[0]);
	glBindTexture(m_eTextureType, m_iTexId[0]);

	glTexParameteri (m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_eTextureType, GL_GENERATE_MIPMAP, GL_TRUE);

	///We need six Face for cube, or one for 2D
	if (m_eTextureType == GL_TEXTURE_CUBE_MAP)
	{
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X , 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y , 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z , 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X , 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y , 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z , 0, GL_RGBA8, m_iWidth, m_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
	else
	{
		glTexImage2D(m_eTextureType, 0, iInternalFormat, m_iWidth, m_iHeight, 0, iFormat, GL_UNSIGNED_BYTE, 0);
	}

	glBindTexture(m_eTextureType, 0);

	//Generate renderbuffer
	glGenRenderbuffersEXT(1, &m_iRenderId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iRenderId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_iWidth, m_iHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	//Generating ID
	glGenFramebuffersEXT(1, &m_iId);
	Activate();

	// attach a texture to FBO color attachement point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_eTextureType, m_iTexId[0], 0);

	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_iRenderId);

	Deactivate();

	//Check FBO status
	if(!CheckFramebufferStatus())
		std::cerr<<"ERROR : FBO creation Fail "<<std::endl;

	//Color Only
	m_iType = 0;
}

void FBO::Generate( GLint iInternalFormat, GLint iFormat )
{
	// create a texture object for the depthmap
	glGenTextures(1, &m_iTexDepthId);
	glBindTexture(m_eTextureType, m_iTexDepthId);
	glTexParameterf(m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(m_eTextureType, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(m_eTextureType, 0, GL_DEPTH_COMPONENT24, m_iWidth, m_iHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glBindTexture(m_eTextureType, 0);

	// create a texture object
	glGenTextures(1, &m_iTexId[0]);
	glBindTexture(m_eTextureType, m_iTexId[0]);
	glTexParameterf(m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(m_eTextureType, 0, iInternalFormat, m_iWidth, m_iHeight, 0, iFormat, GL_UNSIGNED_BYTE, 0);
	glBindTexture(m_eTextureType, 0);

	glGenFramebuffersEXT(1, &m_iId);
	Activate();

	// attach a texture to FBO color attachement point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_eTextureType, m_iTexDepthId, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_eTextureType, m_iTexId[0], 0);

	Deactivate();
	//Check FBO status
	if(!CheckFramebufferStatus())
		std::cerr<<"ERROR : FBO creation Fail "<<std::endl;

	//Color + Depth
	m_iType = 2;
}

void FBO::GenerateFinalHDRBuffer( GLint iInternalFormat0, GLint iFormat0, GLint iInternalFormat1, GLint iFormat1 )
{
	Destroy();

	//Generate the textures
	glGenTextures(1, &m_iTexId[0]);
	glBindTexture(m_eTextureType, m_iTexId[0]);

	glTexParameteri (m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_eTextureType, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(m_eTextureType, 0, iInternalFormat0, m_iWidth, m_iHeight, 0, iFormat0, GL_UNSIGNED_BYTE, 0);

	glGenTextures(1, &m_iTexId[1]);
	glBindTexture(m_eTextureType, m_iTexId[1]);

	glTexParameteri (m_eTextureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (m_eTextureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_eTextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_eTextureType, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexImage2D(m_eTextureType, 0, iInternalFormat1, m_iWidth, m_iHeight, 0, iFormat1, GL_UNSIGNED_BYTE, 0);

	glBindTexture(m_eTextureType, 0);

	//Generate renderbuffer
	glGenRenderbuffersEXT(1, &m_iRenderId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_iRenderId);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_iWidth, m_iHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	//Generating ID
	glGenFramebuffersEXT(1, &m_iId);
	Activate();

	// attach a texture to FBO color attachement point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_eTextureType, m_iTexId[0], 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, m_eTextureType, m_iTexId[1], 0);

	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_iRenderId);

	Deactivate();

	//Check FBO status
	if(!CheckFramebufferStatus())
		std::cerr<<"ERROR : FBO creation Fail "<<std::endl;

	//Color Only
	m_iType = 0;
}

void FBO::ActivateTexture()
{
	if (m_iType == 0 || m_iType == 2)
	{
		glEnable(m_eTextureType);
		glBindTexture(m_eTextureType,m_iTexId[0]);
	}
}

void FBO::ActivateTexture( unsigned int i )
{
	assert( i < 5 );
	if (m_iType == 0 || m_iType == 2)
	{
		glEnable(m_eTextureType);
		glBindTexture(m_eTextureType,m_iTexId[i]);
	}
}

void FBO::DeactivateTexture()
{
	glBindTexture(m_eTextureType,0);
	glDisable(m_eTextureType);
}

void FBO::ActivateDepthTexture()
{
	if (m_iType == 1 || m_iType == 2)
	{
		glEnable(m_eTextureType);
		glBindTexture(m_eTextureType,m_iTexDepthId);
	}
}
void FBO::DeactivateDepthTexture()
{
	DeactivateTexture();
}

void FBO::Activate()
{
	m_bIsActivated = true;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iId);
}

void FBO::Activate(unsigned int iFace)
{
	assert(iFace<6);
	Activate();
	if (m_eDefinitionType == E_FBO_CUBE)
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X+iFace, m_iTexId[0], 0);
}

void FBO::Deactivate()
{
	m_bIsActivated = false;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FBO::Destroy()
{
	glDeleteFramebuffersEXT(1, &m_iId);

	if (m_iType == 0 || m_iType == 2)
		glDeleteTextures(1,&m_iTexId[0]);

	if(m_iType == 0)
		glDeleteRenderbuffersEXT(1, &m_iRenderId);
	else
		glDeleteRenderbuffersEXT(1, &m_iTexDepthId);

	m_bIsActivated = false;
}

bool FBO::CheckFramebufferStatus()
{
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status) {

	case GL_FRAMEBUFFER_COMPLETE_EXT:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << std::endl;
		return false;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_UNSUPPORTED_EXT" << std::endl;
		return false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << std::endl;
		return false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << std::endl;
		return false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << std::endl;
		return false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << std::endl;
		return false;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		std::cerr << "FrameBuffer Status Error : GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << std::endl;
		return false;
		break;

	default:
		std::cerr << "FrameBuffer Status Error : unknown error" << std::endl;
		return false;

	}
}