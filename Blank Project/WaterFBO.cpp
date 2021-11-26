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

void WaterFBO::BindReflectBuffer()
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	/*glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	/*glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, reflectTex, 0);*/
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	if (!reflectTex) {
		return;
	}

	glGenRenderbuffers(1, &reflectDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, reflectDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectDepthBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE) {
		return;
	}


	UnbindCurrentBuffer();
}

void WaterFBO::CreateRefractBuffer()
{
	glGenFramebuffers(1, &refractFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refractFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindTexture(GL_TEXTURE_2D, refractTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refractTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	if (!refractTex) {
		return;
	}

	glGenTextures(1, &refractDepthTex);
	glBindTexture(GL_TEXTURE_2D, refractDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1280, 720, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,refractDepthTex, 0);
	UnbindCurrentBuffer();

}
