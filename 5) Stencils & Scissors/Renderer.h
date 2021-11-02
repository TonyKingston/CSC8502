#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Mesh.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void ToggleScissor();
	void ToggleStencil();
protected:
	Shader* shader;
	Mesh* meshes[2];
	GLuint textures[2];

	bool usingScissor;
	bool usingStencil;
};