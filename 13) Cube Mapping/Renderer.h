#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void DrawHeightMap();
	void DrawWater();
	void DrawSkybox();

	HeightMap* heightMap;
	Mesh* quad;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Camera* camera;
	Light* light;

	GLuint cubeMap;
	GLuint waterTex;
	GLuint earthTex;
	GLuint earthBump;

	float waterRotate;
	float waterCycle;
};
