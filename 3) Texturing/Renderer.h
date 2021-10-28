#pragma once
#include "../nclgl/OGLRenderer.h"
#include "Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	
	virtual void RenderScene();
	void UpdateScene(float dt);
	
	void SwitchToPerspective();
	void SwitchToOrthographic();

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();
	
	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	inline void SetFieldOfView(float f) { fov = f; }
	
protected:
	Mesh * triangle;
	Shader * shader;
	Camera* camera;
	GLuint texture;
	bool filtering;
	bool repeating;
	float scale;
	float rotation;
	float fov;
	Vector3 position;
};