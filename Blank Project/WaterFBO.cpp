#include "WaterFBO.h"
#include "../nclgl/Window.h"

WaterFBO::WaterFBO(GLuint reflectT, GLuint refractT)
{
	reflectTex = reflectT;
	refractTex = refractT;

	if (!reflectTex || !refractTex) {
		return;
	}

	CreateReflectBuffer();
	CreateRefractBuffer();
	
}

WaterFBO::~WaterFBO()
{
	glDeleteFramebuffers(1, &reflectFBO);
	glDeleteFramebuffers(1, &refractFBO);
	glDeleteTextures(1, &reflectTex);
	glDeleteTextures(1, &refractTex);
}

void WaterFBO::BindRelfectBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, reflectFBO);
}

void WaterFBO::BindRefractBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, refractFBO);
}

void WaterFBO::UnbindCurrentBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint WaterFBO::GetReflectTexture()
{
	return reflectTex;
}

GLuint WaterFBO::GetRefractTexture()
{
	return refractTex;
}

GLuint WaterFBO::GetRefractDepthTexture()
{
	return refractDepthTex;
}


void WaterFBO::CreateReflectBuffer()
{
	glGenFramebuffers(1, &reflectFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindTexture(GL_TEXTURE_2D, reflectTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, reflectTex, 0);

	glGenRenderbuffers(1, &reflectDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, reflectDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
	glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectDepthBuffer);
	UnbindCurrentBuffer();
}

void WaterFBO::CreateRefractBuffer()
{
	glGenFramebuffers(1, &refractFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refractFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);


	glBindTexture(GL_TEXTURE_2D, refractTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, refractTex, 0);

	glBindTexture(GL_TEXTURE_2D, refractDepthTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, refractDepthTex, 0);
	UnbindCurrentBuffer();

}
