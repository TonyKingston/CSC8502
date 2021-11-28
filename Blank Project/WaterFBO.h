#pragma once
#include "../nclgl/OGLRenderer.h"

class WaterFBO
{
public:
	WaterFBO();
	~WaterFBO();

	void BindReflectBuffer();
	void BindRefractBuffer();
	void UnbindCurrentBuffer();

	GLuint GetReflectTexture();
	GLuint GetRefractTexture();
	GLuint GetRefractDepthTexture();

	void CreateReflectBuffer();
	void CreateRefractBuffer();

protected:
	GLuint reflectFBO;
	GLuint refractFBO;
	GLuint reflectTex;
	GLuint reflectDepthBuffer;
	GLuint refractTex;
	GLuint refractDepthTex;
};

