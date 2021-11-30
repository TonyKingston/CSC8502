#include "Renderer.h"
#include <algorithm>
#include <cmath>

const int MAX_TEXTURES = 3;
const int waterHeight = 25;
const float nearPlane = 1.0f;
const float farPlane = 15000.0f;
#define SHADOWSIZE 2048

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    //triangle = Mesh::GenerateTriangle();
	heightMap = new HeightMap(TEXTUREDIR"islandSmall.png");
	quad = Mesh::GenerateQuad();
	Vector3 dimensions = heightMap->GetHeightmapSize();
	light = new Light(dimensions * Vector3(0.5f, 2.0f, 0.5f),
		Vector4(1, 1, 1, 1), dimensions.x * 1.5);
	//light->SetLinearCoefficient(0.000007);
	light->SetLinearCoefficient(1.0 / (light->GetRadius() * light->GetRadius() * 0.1));
	light->SetQuadraticCoefficient(0.0000002);
	lights.push_back(light);
	/*light = new Light(Vector3(200,dimensions.y,200),
		Vector4(1, 1, 1, 1), dimensions.x * 1.5);*/

	// Shaders
//	shader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");
	//shader = new Shader("TexturedVertex.glsl ", "TexturedFragment.glsl");
	shader = new Shader("bumpvertex.glsl ", "bumpFragment.glsl");
	//terrainShader = new Shader("TexturedVertex.glsl ", "TexturedFragment.glsl");
	//terrainShader = new Shader("BumpVertex.glsl ", "TerrainFragment.glsl");
	terrainShader = new Shader("basicterrainvertex.glsl ", "basicterrainfrag.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl ", "skyboxFragment.glsl");
	sceneShader = new Shader("MatrixVertex.glsl", "colourFragment.glsl");
	animationShader = new Shader("SkinningVertex.glsl", "bumpFragment.glsl");

	//sceneShader = new Shader("watervertex.glsl", "waterfragment.glsl");
	shadowShader = new Shader("shadowvertex.glsl", "shadowfragment.glsl");
	waterShader = new Shader("watervertex.glsl", "waterfragment.glsl");
	//waterShader = new Shader("TexturedVertex.glsl ", "TexturedFragment.glsl");


	//Shader* lightShader = new Shader("bumpVertex.glsl ", "bumpFragment.glsl");

	sceneShaders.push_back(shader);
	sceneShaders.push_back(terrainShader);
	sceneShaders.push_back(skyboxShader);
	sceneShaders.push_back(sceneShader);
	sceneShaders.push_back(shadowShader);
	sceneShaders.push_back(waterShader);
	sceneShaders.push_back(animationShader);
	//sceneShaders.push_back(lightShader);
	//shader = new Shader("waterVertex.glsl", "waterFragment.glsl");
	for (auto s : sceneShaders) {
		if (!s->LoadSuccess()) {
			return;
		}
	}

	// Textures
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

	terrainBumps[0] = SOIL_load_OGL_texture(TEXTUREDIR "sand_Normal.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainBumps[1] = SOIL_load_OGL_texture(TEXTUREDIR "grass_Normal.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainBumps[2] = SOIL_load_OGL_texture(TEXTUREDIR "rock_Normal.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	waterBump = SOIL_load_OGL_texture(TEXTUREDIR "waterNormal.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	waterDudv = SOIL_load_OGL_texture(TEXTUREDIR "water_dudv.PNG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!waterBump || !waterDudv) return;

	//font = SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT);
	meshes.insert({ "tree", Mesh::LoadFromMeshFile("Coconut_Palm_Tree01.msh") });
	materials.insert({ "tree", new MeshMaterial("Coconut_Palm_Tree01.mat") });
	meshes.insert({ "rock", Mesh::LoadFromMeshFile("Rock_05.msh") });
	materials.insert({ "rock", new MeshMaterial("Rock_05.mat") });
	meshes.insert({ "skeleton",  Mesh::LoadFromMeshFile("Skeleton.msh") });
	materials.insert({ "skeleton", new MeshMaterial("Skeleton.mat") });
	meshes.insert({ "golem",  Mesh::LoadFromMeshFile("Golem.msh") });
	materials.insert({ "golem", new MeshMaterial("Golem.mat") });
	vector<MeshAnimation*> skeletonAnims;
	//skeletonAnims.push_back(new MeshAnimation("Skeleton.anm"));
	skeletonAnims.push_back(new MeshAnimation("Golem.anm"));
	//animations.insert({ "skeleton", vector<MeshAnimation*>(skeletonAnims) });
	animations.insert({ "golem", vector<MeshAnimation*>(skeletonAnims) });

	camera = new Camera();
	camera->SetPosition(Vector3(21, 232, 48));
	camera->SetPitch(-6.8);
	camera->SetYaw(271.2);

	SetWaypoints();

	root = new SceneNode();
	CreateRocks();
	CreateGolem();
	CreateTrees();

	/*auto it = root->GetChildIteratorStart();
	Vector3 pos = (*it)->GetWorldTransform().GetPositionVector();
	camera->SetPosition(root->GetChildIteratorStart()[0]->GetWorldTransform().GetPositionVector());*/
	/*SceneNode* s = new SceneNode(tree, Vector4(1,1,1,0.9));
	s->SetMeshMaterial(material);
	//s->SetTextures(matTextures);
	s->SetShader(shader);
	s->SetBoundingRadius(50.0f);
	s->SetTexture(texID);
	s->SetModelScale(Vector3(5, 5, 5));
	
	root->AddChild(s);

	camera->SetPosition(s->GetWorldTransform().GetPositionVector());*/

	projMatrix = Matrix4::Perspective(nearPlane, farPlane,
		(float)width / (float)height, 45.0f);

	
	for (auto tex : terrainTexs) {
		if (!tex) {
			return;
		}
		SetTextureRepeating(tex, true);
	//	SetTextureFiltering(tex, true);
	}
	for (auto tex : terrainBumps) {
		if (!tex) {
			return;
		}
		SetTextureRepeating(tex, true);
	//	SetTextureFiltering(tex, true);
	}
	//SetTextureRepeating(waterDudv, true);

	clippingPlane = Plane(Vector3(0, 1, 0), -waterHeight);
	glEnable(GL_CLIP_DISTANCE0); // Clipping plane for water

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	waterBuffer = new WaterFBO();

	waterMove = 0.0f;
	waveSpeed = 0.03f;
	inAutomode = true;
	init = true;
}

Renderer::~Renderer(void) {
	delete triangle;
	delete quad;
	delete camera;
	glDeleteTextures(1, &skybox);
	glDeleteTextures(3, terrainTexs);
	glDeleteTextures(3, terrainBumps);
	glDeleteTextures(1, &font);
	glDeleteTextures(1, &waterDudv);
	auto it = root->GetChildIteratorStart();
	it++;
	delete [] (*it)->GetMaterialTextures();
	it = root->GetChildIteratorStart();
	it++;
	delete[](*it)->GetBumpTextures();
	for (auto& shader : sceneShaders)
		delete shader;
	for (auto l : lights)
		delete l;
	for (auto it = meshes.begin(); it != meshes.end(); it++) {
		delete it->second;
	}
	for (auto it = materials.begin(); it != materials.end(); it++) {
		delete it->second;
	}
	for (auto it = animations.begin(); it != animations.end(); it++) {
		for (auto anim : it->second) {
			delete anim;
		}
	}
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
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


void Renderer::RenderScene() {

	BuildNodeLists(root);
	SortNodeLists();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
	
	float distance = 2 * (camera->GetPosition().y - waterHeight);
	Vector3 cameraPos = camera->GetPosition();
	camera->SetPosition(Vector3(cameraPos.x, cameraPos.y - distance, cameraPos.z));
	camera->SetPitch(-camera->GetPitch());
	viewMatrix = camera->BuildViewMatrix();
	waterBuffer->BindReflectBuffer();
	glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	clippingPlane.SetNormal(Vector3(0, 1, 0)); 
	clippingPlane.SetDistance(-waterHeight + 0.5f); // Offset to avoid edge of reflection looking strange
	DrawScene(false);
	waterBuffer->UnbindCurrentBuffer();
	cameraPos = camera->GetPosition();
	camera->SetPosition(Vector3(cameraPos.x, cameraPos.y + distance, cameraPos.z));
	camera->SetPitch(-camera->GetPitch());
	viewMatrix = camera->BuildViewMatrix();

	waterBuffer->BindRefractBuffer();
	glViewport(0, 0, 1280, 720);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	clippingPlane.SetNormal(Vector3(0, -1, 0));
	clippingPlane.SetDistance(waterHeight);
	DrawScene(false);
	waterBuffer->UnbindCurrentBuffer();

	// Draw regular scene
	glDisable(GL_CLIP_DISTANCE0);
	//DrawShadowScene();
	DrawScene(true);
	//BindShader(terrainShader);
	

	//UpdateShaderMatrices();
	/*for (int i = 0; i < tree->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		tree->DrawSubMesh(i);
	}*/
	
	ClearNodeLists();
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L)) {
		inAutomode = !inAutomode;
	}
}

void Renderer::DrawScene(bool drawWater) {
	DrawSkybox();
	DrawHeightMap();
	if (drawWater) {
		DrawWater();
	}
	DrawNodes();
}

void Renderer::SetWaypoints()
{
	Waypoint point;
	point.position = Vector3(272, 232, 255); // Fresnel
	point.pitch = -84;
	point.yaw = 236.5;
	waypoints.push_back(point);
	point.position = Vector3(2054, 256, 1468); // Golem
	point.pitch = -22.5;
	point.yaw = 271.2;
	waypoints.push_back(point);

}

void Renderer::UpdateScene(float dt) {
	if (inAutomode) {
		if (waypointWaitTime <= 0) { // Go to the next waypoint after a certain length of time
			Vector3 newPos = Vector3::Lerp(waypoints[waypointsCleared].position, camera->GetPosition(), dt);
			float newYaw = Lerp(waypoints[waypointsCleared].yaw, camera->GetYaw(), dt);
			float newPitch = Lerp(waypoints[waypointsCleared].pitch, camera->GetPitch(), dt);
			camera->SetPosition(newPos);
			camera->SetYaw(newYaw);
			camera->SetPitch(newPitch);

			if ((camera->GetPosition() - waypoints[waypointsCleared].position).Length() < 10.0f) {
				waypointWaitTime = 10.0f;
				waypointsCleared++;
				waypointsCleared = waypointsCleared == waypoints.size() ? 0 : waypointsCleared;
				std::cout << "Waypoint Reacher" << std::endl;

			}
		}
		else {
			waypointWaitTime -= dt;
		}
	}
	else {
		camera->UpdateCamera(dt);
	}
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
	waterMove += waveSpeed * dt;
	waterMove = fmod(waterMove, 1);
}

void Renderer::DrawWater() {
	//viewMatrix.ToIdentity();
	//projMatrix.ToIdentity();
	BindShader(waterShader);
	UpdateShaderMatrices();

	Vector3 hSize = heightMap->GetHeightmapSize();

	/*modelMatrix =
		Matrix4::Translation(hSize * 0.5f) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));*/


	//Matrix4 model = Matrix4::Translation(Vector3(0, 0, -5)) * Matrix4::Scale(Vector3(10, 10, 10));
	Matrix4 model = Matrix4::Translation(Vector3(hSize.x * 0.5, 25, hSize.z * 0.5)) * Matrix4::Scale(hSize * 0.5f); // Matrix4::Rotation(90, Vector3(1, 0, 0));
//	Matrix4 model = Matrix4::Translation(Vector3(0, 25, 0)) * Matrix4::Scale(Vector3(200,200,200)) * Matrix4::Rotation(90, Vector3(0, 1, 0));
	glUniformMatrix4fv(
		glGetUniformLocation(waterShader->GetProgram(),
			"modelMatrix"), 1, false, model.values);

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(),
		"reflectTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterBuffer->GetReflectTexture());

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(),
		"refractTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBuffer->GetRefractTexture());

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(),
		"bumpTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, waterBump);

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(),
		"dudvTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, waterDudv);

	glUniform1i(glGetUniformLocation(waterShader->GetProgram(),
		"depthTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterBuffer->GetRefractDepthTexture());


	glUniform1f(glGetUniformLocation(waterShader->GetProgram(),
		"waterMove"), waterMove);

	glUniform1f(glGetUniformLocation(waterShader->GetProgram(),
		"near"), nearPlane);

	glUniform1f(glGetUniformLocation(waterShader->GetProgram(),
		"far"), farPlane);

	glUniform3fv(glGetUniformLocation(waterShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	SetShaderLight(light);

	//modelMatrix.ToIdentity();
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
	glDisable(GL_BLEND);
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(terrainShader);
	UpdateShaderMatrices();
	//GLuint texArray[MAX_TEXTURES];
	int texArray[] = { 0,1,2};
	int bumpArray[] = { 3,4,5 };
	int index = 0;

	glUniform1iv(glGetUniformLocation(terrainShader->GetProgram(),
		"terrainSampler"), MAX_TEXTURES, texArray);

	for (int i = 0; i < 3; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, terrainTexs[i]);
		index++;
	}

    GLfloat bounds [] = { 40.0f, 150.0f, heightMap->GetHeightmapSize().y };
	glUniform1fv(glGetUniformLocation(terrainShader->GetProgram(),
		"bound"), 3, bounds);

	
	glUniform1iv(glGetUniformLocation(terrainShader->GetProgram(),
		"terrainBumps"), MAX_TEXTURES, bumpArray);
	for (int i = 0; i < 3; i++) {
		glActiveTexture(GL_TEXTURE0 + index + i);
		glBindTexture(GL_TEXTURE_2D, terrainBumps[i]);
	}
	
	glUniform3fv(glGetUniformLocation(terrainShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	

	SetShaderLight(light);

	Matrix4 model = modelMatrix * Matrix4::Scale(Vector3(1,1,1));
	glUniformMatrix4fv(
		glGetUniformLocation(terrainShader->GetProgram(),
			"modelMatrix"), 1, false, model.values);

	Vector3 norm = clippingPlane.GetNormal();
	glUniform4f(glGetUniformLocation(terrainShader->GetProgram(), "plane"), norm.x , norm.y, norm.z, clippingPlane.GetDistance());
//	modelMatrix.ToIdentity(); 
	//textureMatrix.ToIdentity();
	
	heightMap->Draw();
	glEnable(GL_BLEND);
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(
		light->GetPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix;
	DrawNodes();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		UpdateShaderMatrices();
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"diffuseTex"), 0);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"bumpTex"), 1);

		//UpdateShaderMatrices();
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(currentShader->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		Vector3 norm = clippingPlane.GetNormal();
		glUniform4f(glGetUniformLocation(currentShader->GetProgram(), "plane"), norm.x, norm.y, norm.z, clippingPlane.GetDistance());

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
			"cameraPos"), 1, (float*)&camera->GetPosition());

		if (n->GetAnimation()) {
			Mesh* mesh = n->GetMesh();
			vector <Matrix4> frameMatrices;
			const Matrix4* invBindPose = mesh->GetInverseBindPose();
			const Matrix4* frameData = n->GetAnimation()->GetJointData(n->GetCurrentFrame());

			for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
				frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
			}
			
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "joints"), frameMatrices.size(), false,
				(float*)frameMatrices.data());
		}
		
		SetShaderLight(light);

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

void Renderer::CreateRocks() {
	auto it = materials.find("rock");
	if (it != materials.end()) {
		MeshMaterial* material = it->second;
		Mesh* mesh = meshes.find("rock")->second;
		vector<GLuint>* matTextures = GetTexturesForMesh("rock");
		vector<GLuint>* bumpTextures = GetBumpsForMesh("rock");

		for (int i = 0; i < 10; i++) {
			//SceneNode* s = new SceneNode(treeMesh, Vector4(1, 1, 1, 0.9));
			SceneNode* s = new SceneNode();
			s->SetMesh(mesh);
			s->SetMeshMaterial(material);
			s->SetShader(shader);
			s->SetBoundingRadius(50.0f);
			s->SetTextures(matTextures);
			s->SetBumpTextures(bumpTextures);

			Vector2 pos = Vector2(light->GetPosition().x + (50 * i), (50 * i));
			s->SetTransform(Matrix4::Translation(
				Vector3(pos.x + 50, heightMap->GetHeightForPosition(pos), pos.y)));
			s->SetModelScale(Vector3(10, 10, 10));
			
			root->AddChild(s);
		}
	//	textureMatrix.ToIdentity();
	}
	else {
		return;
	}
	
}

void Renderer::CreateTrees()
{
	auto it = materials.find("tree");
	if (it != materials.end()) {
		vector<GLuint>* matTextures = GetTexturesForMesh("tree");
		vector<GLuint>* bumpTextures = new vector<GLuint>(); // The mesh extractor doesn't pick up the bump map for the tree so I have to do it manually.
		MeshMaterial* material = it->second;
		Mesh* mesh = meshes.find("tree")->second;
		string path = TEXTUREDIR"Coconut Palm Tree Pack/Textures/Coconut_Palm_Tree_normalspec_bark.psd";
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		bumpTextures->emplace_back(0);
		bumpTextures->emplace_back(texID);

		for (int i = 0; i < 10; i++) {
			SceneNode* s = new SceneNode();
			s->SetMesh(mesh);
			s->SetMeshMaterial(material);
			s->SetShader(shader);
			s->SetBoundingRadius(45.0f);
			s->SetTextures(matTextures);
			s->SetBumpTextures(bumpTextures);
			s->SetColour(Vector4(1, 1, 1, 0.9));
			/*s->SetTransform(Matrix4::Translation(
				Vector3(100 * i, 25, 100 * i + (i*20))));*/
			Vector2 pos = Vector2((50 * i), light->GetPosition().z + (50 * i));
			s->SetTransform(Matrix4::Translation(
				Vector3(pos.x - 50, heightMap->GetHeightForPosition(pos),pos.y)));
			s->SetModelScale(Vector3(10, 10, 10));

			root->AddChild(s);
		}
	}
}

void Renderer::CreateGolem()
{
	Mesh* mesh = meshes.find("golem")->second;
	MeshMaterial* material = materials.find("golem")->second;

	vector <GLuint>* matTextures = GetTexturesForMesh("golem");
	vector <GLuint>* bumpTextures = GetBumpsForMesh("golem");

	SceneNode* s = new SceneNode();
	s->SetMesh(mesh);
	s->SetTextures(matTextures);
	s->SetBumpTextures(bumpTextures);
	Vector2 pos = Vector2(2200, 1414);
	//Vector2 pos = Vector2(light->GetPosition().x, light->GetPosition().z);
	s->SetTransform(Matrix4::Translation(
		Vector3(pos.x, heightMap->GetHeightForPosition(pos), pos.y)) * Matrix4::Rotation(20, Vector3(0, 1, 0)));
	s->SetAnimation(animations.find("golem")->second[0]);
	s->SetShader(animationShader);
	s->SetBoundingRadius(50.0f);
	s->SetModelScale(Vector3(10, 10, 10));
	root->AddChild(s);

}

vector<GLuint>* Renderer::GetTexturesForMesh(string obj)
{
	auto meshIt = meshes.find(obj);
	auto matIt = materials.find(obj);
	if (meshIt == meshes.end() || matIt == materials.end()) {
		return NULL;
	}
	Mesh* mesh = meshIt->second;
	MeshMaterial* material = matIt->second;
	vector<GLuint>* matTextures = new vector<GLuint>(0);
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures->emplace_back(texID);
	}
	return matTextures;
}

vector<GLuint>* Renderer::GetBumpsForMesh(string obj)
{
	auto meshIt = meshes.find(obj);
	auto matIt = materials.find(obj);
	if (meshIt == meshes.end() || matIt == materials.end()) {
		return NULL;
	}
	Mesh* mesh = meshIt->second;
	MeshMaterial* material = matIt->second;
	vector<GLuint>* bumpTextures = new vector<GLuint>(0);
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Bump", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		bumpTextures->emplace_back(texID);
	}
	return bumpTextures;
}
