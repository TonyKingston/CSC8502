#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    //triangle = Mesh::GenerateTriangle();
	heightMap = new HeightMap(TEXTUREDIR"oblivionResize.png");
	quad = Mesh::GenerateQuad();

	//GLuint reflect = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	//GLuint refract = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	skybox = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	terrainTexs[0] = SOIL_load_OGL_texture(TEXTUREDIR "sand_Diffuse.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTexs[1] = SOIL_load_OGL_texture(TEXTUREDIR "grass_Diffuse.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainTexs[2] = SOIL_load_OGL_texture(TEXTUREDIR "rock_Diffuse.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	//terrainNorm = SOIL_load_OGL_texture(TEXTUREDIR "sand_Normal.tga",
		//SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	//font = SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
	tree = Mesh::LoadFromMeshFile("rock2.msh");
	material = new MeshMaterial("rock2.mat");
	std::cout << tree->GetSubMeshCount() << std::endl;
	vector <GLuint>* matTextures = new vector<GLuint>(0);
	for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures->emplace_back(texID);
	}
	//if (!reflect || !refract || !skybox) return;

	shader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");
	terrainShader = new Shader("TexturedVertex.glsl ", "TerrainFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl ", "skyboxFragment.glsl");
	sceneShader = new Shader("SceneVertex.glsl", "Scenefragment.glsl");
	//shader = new Shader("waterVertex.glsl", "waterFragment.glsl");

	camera = new Camera();
	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5, 2, 0.5));

	root = new SceneNode();
	SceneNode* s = new SceneNode();
	s->SetMesh(tree);
	s->SetMeshMaterial(material);
	s->SetTextures(matTextures);
	s->SetShader(terrainShader);
	s->SetBoundingRadius(10.0f);
	
	root->AddChild(s);

	//camera->SetPosition(s->GetWorldTransform().GetPositionVector());

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	if (!shader->LoadSuccess() || !terrainShader->LoadSuccess() || !skyboxShader->LoadSuccess()) {
		return;
	}
	for (auto tex : terrainTexs) {
		SetTextureRepeating(tex, true);
	}

	//waterBuffer = new WaterFBO(reflect, refract);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
//	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	init = true;
}

Renderer::~Renderer(void) {
	delete triangle;
	delete quad;
	delete tree;
	delete material;
	delete shader;
	delete terrainShader;
	delete sceneShader;
	delete skyboxShader;
	delete camera;
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &skybox);
	glDeleteTextures(3, terrainTexs);
	glDeleteTextures(1, &font);
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
	BuildNodeLists(root);
	SortNodeLists();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSkybox();
	DrawHeightMap();
	DrawWater();
	//BindShader(terrainShader);
	

	UpdateShaderMatrices();
	/*for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		tree->DrawSubMesh(i);
	}*/
	DrawNodes();
	ClearNodeLists();
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
}

void Renderer::DrawWater() {
	/*modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();*/
	BindShader(shader);
	

	Vector3 hSize = heightMap->GetHeightmapSize();

	/*modelMatrix =
		Matrix4::Translation(hSize * 0.5f) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));*/

	UpdateShaderMatrices();

	//Matrix4 model = Matrix4::Translation(Vector3(0, 0, -5)) * Matrix4::Scale(Vector3(10, 10, 10));
	Matrix4 model = Matrix4::Translation(Vector3(hSize.x * 0.5, 25, hSize.z * 0.5 )) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	glUniformMatrix4fv(
		glGetUniformLocation(shader->GetProgram(),
			"modelMatrix"), 1, false, model.values);
	quad->Draw();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	UpdateShaderMatrices();
	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {
	BindShader(terrainShader);
	UpdateShaderMatrices();
	int array[3] = { 0,1,2 };
	glUniform1iv(glGetUniformLocation(terrainShader->GetProgram(),
		"terrainSampler"), 3, array);
	for (int i = 0; i < 3; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, terrainTexs[i]);
	}

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, terrainTex);
	Matrix4 model = modelMatrix * Matrix4::Scale(Vector3(4, 4, 4));
	glUniformMatrix4fv(
		glGetUniformLocation(terrainShader->GetProgram(),
			"modelMatrix"), 1, false, model.values);
	//modelMatrix.ToIdentity(); 
	//textureMatrix.ToIdentity();
	
	heightMap->Draw();
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
		Shader* currentShader = n->GetShader();
		BindShader(currentShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"diffuseTex"), 0);
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(currentShader->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		/*glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());

		texture = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture); // returns 0 if no texture
		glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "useTexture"),
			texture);*/
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