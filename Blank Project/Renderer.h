#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/HeightMap.h"
#include "WaterFBO.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	void UpdateScene(float dt);

	void SwitchToPerspective();
	void SwitchToOrthographic();

	void UpdateTextureMatrix(float rotation);
	void ToggleFiltering();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void DrawSkybox();
	void DrawWater();
	void DrawHeightMap();

	int CreateFrameBuffer();

protected:
	Mesh* triangle;
	Mesh* quad;
	Shader* shader;
	Shader* skyboxShader;
	Shader* terrainShader;
	Camera* camera;
	GLuint texture;
	GLuint terrainTex;
	GLuint skybox;
	GLuint font;
	bool filtering;
	bool repeating;
	float fov;
	Vector3 position;
	Frustum frameFrustum;
	WaterFBO* waterBuffer;
	HeightMap* heightMap;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};