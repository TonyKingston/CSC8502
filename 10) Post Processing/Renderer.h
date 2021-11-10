#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();

	HeightMap* heightMap;
	Shader* sceneShader;
	Shader* processShader;
	Camera* camera;

	Mesh* quad;
	GLuint heightTexture;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
};
