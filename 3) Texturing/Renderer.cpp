#include "Renderer.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();

	texture = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
	);
	texture2 = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!texture) return;

	//shader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");
	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	camera = new Camera();

	if (!shader->LoadSuccess()) {
		return;
	}

	filtering = true;
	repeating = false;
	init = true;
	//SwitchToOrthographic();
	//fov = 45.0f;
	//SwitchToPerspective();
}

Renderer::~Renderer(void) {
	delete triangle;
	delete shader;
	delete camera;
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
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

void Renderer::ToggleRepeating()
{
	repeating = !repeating;
	SetTextureRepeating(texture, repeating);
}

void Renderer::ToggleFiltering()
{
	filtering = !filtering;
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

	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex2"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram()
	, "projMatrix"), 1, false, projMatrix.values);

	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram()
	, "viewMatrix"), 1, false, viewMatrix.values);
	

	for (int i = 0; i < 1; ++i) {
		Vector3 tempPos = position;
		tempPos.z += (i * 500.0f);
		tempPos.x -= (i * 100.0f);
		tempPos.y -= (i * 100.0f);

		modelMatrix = Matrix4::Translation(tempPos) *
			Matrix4::Rotation(rotation, Vector3(0, 1, 0)) *
			Matrix4::Scale(Vector3(scale, scale, scale));

		glUniformMatrix4fv(glGetUniformLocation(
			shader->GetProgram(), "modelMatrix"),
			1, false, modelMatrix.values);
		triangle->Draw();
	}
	//triangle->Draw();

}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}