#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    //triangle = Mesh::GenerateTriangle();
	quad = Mesh::GenerateQuad();

	GLuint reflect = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	GLuint refract = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!texture) return;

	shader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");
	//shader = new Shader("waterVertex.glsl", "waterFragment.glsl");
	camera = new Camera();
	camera->SetPosition(Vector3(0, 5, 20));

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	if (!shader->LoadSuccess()) {
		return;
	}

	waterBuffer = new WaterFBO(reflect, refract);

	init = true;
}

Renderer::~Renderer(void) {
	delete triangle;
	delete quad;
	delete shader;
	delete camera;
	glDeleteTextures(1, &texture);
}

void Renderer::SwitchToPerspective()
{
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, float(width) / float(height), fov);

}

void Renderer::SwitchToOrthographic()
{
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f,
		height / 2.0f, -height / 2.0f);
}

void Renderer::UpdateTextureMatrix(float value)
{
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}


void Renderer::ToggleFiltering()
{
	filtering != filtering;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();

	Matrix4 model = Matrix4::Translation(Vector3(0,0,-5)) * Matrix4::Scale(Vector3(10,10,10));
	glUniformMatrix4fv(
		glGetUniformLocation(shader->GetProgram(),
			"modelMatrix"), 1, false, model.values);
	quad->Draw();
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::DrawWater() {
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (auto i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(), // note the r!
		transparentNodeList.rend(), // note the r!
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(shader->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture); // returns 0 if no texture
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTexture"),
			texture);
		n->Draw(*this);

	}
}



/*bool Renderer::SetTexture(GLuint texID, GLuint unit, const std::string& uniformName, Shader* s) {
	if (uniformID < 0) {
		std::cout << "Trying to bind invalid 2D texture uniform!\n"; //Put breakpoint on this!
		return false;
	}
	if (shader != s) {
		std::cout << "Trying to set shader uniform on wrong shader!\n"; //Put breakpoint on this!
		return false;
	}
	glActiveTexture(GL_TEXTURE0 + unit); //A neat trick!
	glBindTexture(GL_TEXTURE_2D, texID);

	glUniform1i(uniformID, unit);

	return true;
}*/