# pragma once

#include "../nclgl/OGLRenderer.h"
#include "Camera.h"
#include "SceneNode.h"
#include "CubeRobot.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void UpdateScene(float dt) override;
	void RenderScene() override;
	
protected:
	void DrawNode(SceneNode * n);
	
	SceneNode* root;
	Camera* camera;
	Mesh* cube;
	Shader* shader;
};