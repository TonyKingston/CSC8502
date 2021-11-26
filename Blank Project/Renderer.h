#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/Light.h"
#include <string>
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
	void DrawShadowScene();
	void CreateTrees();
	void DrawScene(bool drawWater);

	int CreateFrameBuffer();

protected:
	std::map<string, Mesh*> meshes;
	std::map<string, MeshMaterial*> materials;
	SceneNode* root;
	Mesh* triangle;
	Mesh* quad;
	Shader* shader;
	Shader* skyboxShader;
	Shader* terrainShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* waterShader;
	Camera* camera;
	GLuint terrainTexs[3];
	GLuint terrainBumps[3];
	GLuint waterDudv;
	GLuint skybox;
	GLuint font;
	vector<Shader*> sceneShaders;
	vector<GLuint> sceneTextures;
	vector<Mesh*> sceneMeshes;
	Plane clippingPlane;
	bool filtering;
	bool repeating;
	float fov;
	// For dudvu map sampling
	float waterMove;
	float waveSpeed;

	Vector3 position;
	Frustum frameFrustum;
	WaterFBO* waterBuffer;
	HeightMap* heightMap;
	Light* light;
	GLuint shadowFBO;
	GLuint shadowTex;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};