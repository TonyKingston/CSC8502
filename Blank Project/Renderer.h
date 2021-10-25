#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/Shader.h"
#include "../NCLGL/window.h"

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 virtual void RenderScene()				override;
	 virtual void UpdateScene(float msec)	override;
protected:
	Mesh*	triangle;
	Shader* basicShader;
};
