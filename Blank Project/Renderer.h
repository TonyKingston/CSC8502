#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/Light.h"
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
	SceneNode* root;
	Mesh* triangle;
	Mesh* quad;
	Mesh* tree;
	Shader* shader;
	Shader* skyboxShader;
	Shader* terrainShader;
	Shader* sceneShader;
	MeshMaterial* material;
	Camera* camera;
	GLuint texture;
	GLuint terrainTexs[3];
	GLuint terrainBumps[3];
	GLuint skybox;
	GLuint font;
	vector<Shader*> sceneShaders;
	vector<GLuint> sceneTextures;
	vector<Mesh*> sceneMeshes;
	bool filtering;
	bool repeating;
	float fov;
	Vector3 position;
	Frustum frameFrustum;
	WaterFBO* waterBuffer;
	HeightMap* heightMap;
	Light* light;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};