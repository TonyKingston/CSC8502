#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Mesh.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
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
	
protected:
	Mesh * triangle;
	Shader * shader;
	Camera* camera;
	GLuint texture;
	bool filtering;
	bool repeating;
	Vector3 position;
	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};